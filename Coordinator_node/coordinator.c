#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"

#if !defined( WIN32 )
#include "OnBoard.h"
#endif

/* HAL */
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
#include "MT_UART.h"

#define SEND_TO_ALL_EVENT  0x01  //���巢����Ϣ�¼�

#define OPENCURTAIN		'1' 	 //������
#define CLOSECURTAIN		'2' 	 //�ش���
#define CLOSELAMP		'3' 	 //�ص�
#define OPENLAMP 		'4' 	 //����
#define OPENFAN	     	  	'5'     //������
#define CLOSEFAN		'6' 	//�ط���
#define AUTOMATIC   	  	'7'     //�Զ�ģʽ
#define MANUAL        		'8'     //�ֶ�ģʽ
#define OPENALARM     		'9'     //������
#define CLOSEALARM    		'a'     //�ؾ���

/* ������->�ƹ�
   ������->����
   �������->����
   ���桢Ӧ����ť->����
*/

#define NOLIGHT	        '1' 	 //�޹�
#define HAVELIGHT	'2' 	 //�й�
#define NOMAN		'3' 	 //����
#define HAVEMAN 	'4' 	 //����
#define NOSMOKE	        '5' 	 //������
#define HAVESMOKE 	'6' 	 //������
#define AUTOMATIC       '7'      //�Զ�ģʽ
#define MANUAL          '8'     //�ֶ�ģʽ
#define NOKEY	        'b'	 //�ް���
#define HAVEKEY 	'e' 	 //�а���
#define NOFIRE	        'd' 	 //�޻���
#define HAVEFIRE 	'e' 	 //�л���



unsigned char rxbuf[16];  //���ڽ��ջ�����
char Auto_flag = 0;       //1:�Զ�ģʽ  0:�ֶ�ģʽ 

//��ID
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] = 
{
  GENERICAPP_CLUSTERID,
  GENERICAPP_CLUSTERID1,
  GENERICAPP_CLUSTERID2,
  GENERICAPP_CLUSTERID3,
  GENERICAPP_CLUSTERID4,
  GENERICAPP_CLUSTERID5,
  GENERICAPP_CLUSTERID6
};

//�ڵ��豸������
const SimpleDescriptionFormat_t GenericApp_SimpleDesc = 
{
  GENERICAPP_ENDPOINT,
  GENERICAPP_PROFID,
  GENERICAPP_DEVICEID,
  GENERICAPP_DEVICE_VERSION,
  GENERICAPP_FLAGS,
  GENERICAPP_MAX_CLUSTERS,
  (cId_t *)GenericApp_ClusterList,
  0,
  (cId_t *)NULL
};

endPointDesc_t GenericApp_epDesc;//�ڵ�������
byte GenericApp_TaskID;          //�������ȼ�
byte GenericApp_TransID;        //���ݷ������к�
devStates_t GenericApp_NwkState; //���ݷ������к�

void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);//��Ϣ������
void GenericApp_SendTheMessage( unsigned char *uartbuf, uint16 len ,const int clusterID );//��Ϣ���ͺ���
static void rxCB(uint8 port,uint8 event);


//�����ʼ������
void GenericApp_Init(byte task_id)
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID             = task_id;      //��ʼ���������ȼ�
  GenericApp_TransID            = 0;           //���������ݰ�����ų�ʼ��Ϊ0,�����������㶪����
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;//�Խڵ����������г�ʼ��,�˴�ֵӦ����Э�����е�ֵ����һ�£��������ڽ���Э��������Ϣ��
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;
  afRegister(&GenericApp_epDesc);       //���ڵ����ע�ᣬ����ʹ��OSAL�ṩ����

    
  uartConfig.configured         = TRUE;
  uartConfig.baudRate           = HAL_UART_BR_9600;
  uartConfig.flowControl        = FALSE;
  uartConfig.callBackFunc       = rxCB;
  HalUARTOpen(0, &uartConfig);           //���ڳ�ʼ��

  //HalUARTWrite(0,"hello world!\n\r",strlen("hello world!\n\r"));

}


UINT16 GenericApp_ProcessEvent(byte task_id, UINT16 events)
{
  afIncomingMSGPacket_t *MSGpkt;//������Ϣ�ṹ���ָ��
  if(events & SYS_EVENT_MSG)
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);//����Ϣ�����Ͻ�����Ϣ
    while(MSGpkt)
    {
      switch(MSGpkt->hdr.event)//�����������ݰ�
      {
      case AF_INCOMING_MSG_CMD: //�յ�����Ϣ
        GenericApp_MessageMSGCB(MSGpkt);
        break;
      case ZDO_STATE_CHANGE://����״̬�仯
        GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);//��ȡ�ڵ���豸����
        if(GenericApp_NwkState == DEV_ZB_COORD)
        {
          osal_set_event(GenericApp_TaskID, SEND_TO_ALL_EVENT);//�����¼�
        }
        break;
      default:
        break;
      }
      osal_msg_deallocate((uint8 *)MSGpkt);//�ͷ��ڴ�
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);//��������
    }
    return (events ^ SYS_EVENT_MSG);
  }
  if(events & SEND_TO_ALL_EVENT)//���ݷ����¼�����
  {
    osal_start_timerEx(GenericApp_TaskID, SEND_TO_ALL_EVENT, 200);//200ms����һ������
    return (events ^ SEND_TO_ALL_EVENT);
  }
  return 0;
}

unsigned char cnt1 = 1;
int humi, temp,type1;
unsigned char tem[4], hum[4],typ[4];
unsigned char AM2312_head[1] = {0xff};
unsigned char AM2312_tail[1] = {0xfe};
unsigned char PM25_head[1] = {0xfd};
unsigned char PM25_tail[1] = {0xfc};
void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt)
{
  unsigned char buffer[64] ;
  
  switch (pckt->clusterId)       
  {
    case GENERICAPP_CLUSTERID1:
         
    case GENERICAPP_CLUSTERID2:
      
    case GENERICAPP_CLUSTERID3:
      
    case GENERICAPP_CLUSTERID4:
      
      if(Auto_flag == 1)//�Զ�ģʽ
      {
        //osal_memcpy(buffer, pckt->cmd.Data,1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);
        GenericApp_SendTheMessage(pckt->cmd.Data,pckt->cmd.DataLength, pckt->clusterId);/* �յ����������·�*/
      }    
      osal_memset(buffer, 0, 1);   
      break;
         
    case GENERICAPP_CLUSTERID5:  //��ʪ��
    
        osal_memcpy(buffer, pckt->cmd.Data,pckt->cmd.DataLength);
        HalUARTWrite(0, AM2312_head, 1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);//���յ������ݴ������
        HalUARTWrite(0, AM2312_tail, 1);
        //HalUARTWrite(1, pckt->cmd.Data, pckt->cmd.DataLength);
        //osal_memset(pckt->cmd.Data, 0, pckt->cmd.DataLength); 
        break;
    
     case GENERICAPP_CLUSTERID6: //PM2.5
        osal_memcpy(buffer, pckt->cmd.Data,pckt->cmd.DataLength);
        HalUARTWrite(0, PM25_head, 1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);//���յ������ݴ������
        HalUARTWrite(0, PM25_tail, 1);
       
        break;
  }
  //LED1ָʾ�յ�����
     HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);
      
}
char cnt = 1;
void GenericApp_SendTheMessage( unsigned char *uartbuf, uint16 len ,const int clusterID )
{
  //HalUARTWrite(0, uartbuf, len);
  afAddrType_t my_DstAddr;      
  my_DstAddr.addrMode= (afAddrMode_t) AddrBroadcast;//�㲥
  my_DstAddr.endPoint= GENERICAPP_ENDPOINT;     //��ʼ���˿ں�
  my_DstAddr.addr.shortAddr= 0xFFFF;    //ȫ���㲥
  while(len){
  //���ݷ��ͺ���
  AF_DataRequest( &my_DstAddr,          //Ŀ�Ľڵ�������ַ�Լ��������ݵĸ�ʽ
                 &GenericApp_epDesc,    //�ڵ�������
                       clusterID,       //��ID,�����
                       len,             //�������ݵ� ����
                       uartbuf,         //ָ�������ݻ�������ָ��
                       &GenericApp_TransID,//ָ�������е�ָ��
                       AF_DISCV_ROUTE, 
                       AF_DEFAULT_RADIUS ) ;
   len = 0;
  }
   
  
}

int clusterID;
unsigned char flagstate[1] = "1";
static void rxCB(uint8 port, uint8 event)
{
  
  //HalUARTPollDMA()
  HalUARTRead(0, rxbuf, 1);
  if(rxbuf[0] == MANUAL)//�ֶ�ģʽ
  {
    Auto_flag = 0; 
    //HalUARTWrite(0, rxbuf, 1);
    GenericApp_SendTheMessage(rxbuf, 1, GENERICAPP_CLUSTERID1);
  }
  if(rxbuf[0] == AUTOMATIC)//�Զ�ģʽ
  {
    //HalUARTWrite(0, rxbuf, 1);
    Auto_flag = 1; 
    GenericApp_SendTheMessage(rxbuf, 1, GENERICAPP_CLUSTERID1);
  }
  if(Auto_flag == 0)//�ֶ�ģʽ���ɽ���λ������͵��ն˽ڵ�
  {
    switch(rxbuf[0]){
      case OPENLAMP:
      case CLOSELAMP:       clusterID = GENERICAPP_CLUSTERID1; break;//�������ݿ��Ƶƹ�ڵ�
      case OPENFAN:
      case CLOSEFAN:        clusterID = GENERICAPP_CLUSTERID2; break;//�������ݿ��Ʒ��Ƚڵ�    
      case OPENALARM:
      case CLOSEALARM:      clusterID = GENERICAPP_CLUSTERID3; break;//�������ݿ��ƾ����ڵ�  
      case OPENCURTAIN:
      case CLOSECURTAIN:    clusterID = GENERICAPP_CLUSTERID4; break;//�������ݿ��ƴ����ڵ�  
    }
    GenericApp_SendTheMessage(rxbuf, 1, clusterID);
  }
  if(Auto_flag == 1) //HalUARTWrite(0, flagstate, 1);

  osal_memset(rxbuf, 0, 1);
  //LED2ָʾ�յ�������������
 
  HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);

}

/*
static void rxCB(uint8 port,uint8 event)
{
	uint8  ch;  
	uint8  rxBuffer[128];  
	uint8 len = 0;  

        while (Hal_UART_RxBufLen(port))  
        {
            HalUARTRead (port, &ch, 1);
            rxBuffer[len]  = ch;      
            len++;  
        }  
        if(len)  
        {      
                HalUARTWrite(port,rxBuffer,len); 
                len = 0;
        }
         

}
*/
