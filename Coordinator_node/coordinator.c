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

#define SEND_TO_ALL_EVENT  0x01  //定义发送消息事件

#define OPENCURTAIN		'1' 	 //开窗帘
#define CLOSECURTAIN		'2' 	 //关窗帘
#define CLOSELAMP		'3' 	 //关灯
#define OPENLAMP 		'4' 	 //开灯
#define OPENFAN	     	  	'5'     //开风扇
#define CLOSEFAN		'6' 	//关风扇
#define AUTOMATIC   	  	'7'     //自动模式
#define MANUAL        		'8'     //手动模式
#define OPENALARM     		'9'     //开警报
#define CLOSEALARM    		'a'     //关警报

/* 人体检测->灯光
   烟雾检测->风扇
   光敏检测->窗帘
   火焰、应急按钮->警报
*/

#define NOLIGHT	        '1' 	 //无光
#define HAVELIGHT	'2' 	 //有光
#define NOMAN		'3' 	 //无人
#define HAVEMAN 	'4' 	 //有人
#define NOSMOKE	        '5' 	 //无烟雾
#define HAVESMOKE 	'6' 	 //有烟雾
#define AUTOMATIC       '7'      //自动模式
#define MANUAL          '8'     //手动模式
#define NOKEY	        'b'	 //无按键
#define HAVEKEY 	'e' 	 //有按键
#define NOFIRE	        'd' 	 //无火焰
#define HAVEFIRE 	'e' 	 //有火焰



unsigned char rxbuf[16];  //串口接收缓冲区
char Auto_flag = 0;       //1:自动模式  0:手动模式 

//簇ID
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

//节点设备描述符
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

endPointDesc_t GenericApp_epDesc;//节点描述符
byte GenericApp_TaskID;          //任务优先级
byte GenericApp_TransID;        //数据发送序列号
devStates_t GenericApp_NwkState; //数据发送序列号

void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);//消息处理函数
void GenericApp_SendTheMessage( unsigned char *uartbuf, uint16 len ,const int clusterID );//消息发送函数
static void rxCB(uint8 port,uint8 event);


//任务初始化函数
void GenericApp_Init(byte task_id)
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID             = task_id;      //初始化任务优先级
  GenericApp_TransID            = 0;           //将发送数据包的序号初始化为0,可以用来计算丢包率
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;//对节点描述符进行初始化,此处值应该与协调器中的值保持一致，作用用于接收协调器的信息。
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;
  afRegister(&GenericApp_epDesc);       //将节点进行注册，才能使用OSAL提供服务

    
  uartConfig.configured         = TRUE;
  uartConfig.baudRate           = HAL_UART_BR_9600;
  uartConfig.flowControl        = FALSE;
  uartConfig.callBackFunc       = rxCB;
  HalUARTOpen(0, &uartConfig);           //串口初始化

  //HalUARTWrite(0,"hello world!\n\r",strlen("hello world!\n\r"));

}


UINT16 GenericApp_ProcessEvent(byte task_id, UINT16 events)
{
  afIncomingMSGPacket_t *MSGpkt;//接收消息结构体的指针
  if(events & SYS_EVENT_MSG)
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);//从消息队列上接收消息
    while(MSGpkt)
    {
      switch(MSGpkt->hdr.event)//处理无线数据包
      {
      case AF_INCOMING_MSG_CMD: //收到新消息
        GenericApp_MessageMSGCB(MSGpkt);
        break;
      case ZDO_STATE_CHANGE://网络状态变化
        GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);//读取节点的设备类型
        if(GenericApp_NwkState == DEV_ZB_COORD)
        {
          osal_set_event(GenericApp_TaskID, SEND_TO_ALL_EVENT);//设置事件
        }
        break;
      default:
        break;
      }
      osal_msg_deallocate((uint8 *)MSGpkt);//释放内存
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);//继续接收
    }
    return (events ^ SYS_EVENT_MSG);
  }
  if(events & SEND_TO_ALL_EVENT)//数据发送事件处理
  {
    osal_start_timerEx(GenericApp_TaskID, SEND_TO_ALL_EVENT, 200);//200ms发送一次数据
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
      
      if(Auto_flag == 1)//自动模式
      {
        //osal_memcpy(buffer, pckt->cmd.Data,1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);
        GenericApp_SendTheMessage(pckt->cmd.Data,pckt->cmd.DataLength, pckt->clusterId);/* 收到数据马上下发*/
      }    
      osal_memset(buffer, 0, 1);   
      break;
         
    case GENERICAPP_CLUSTERID5:  //温湿度
    
        osal_memcpy(buffer, pckt->cmd.Data,pckt->cmd.DataLength);
        HalUARTWrite(0, AM2312_head, 1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);//接收到的数据串口输出
        HalUARTWrite(0, AM2312_tail, 1);
        //HalUARTWrite(1, pckt->cmd.Data, pckt->cmd.DataLength);
        //osal_memset(pckt->cmd.Data, 0, pckt->cmd.DataLength); 
        break;
    
     case GENERICAPP_CLUSTERID6: //PM2.5
        osal_memcpy(buffer, pckt->cmd.Data,pckt->cmd.DataLength);
        HalUARTWrite(0, PM25_head, 1);
        HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);//接收到的数据串口输出
        HalUARTWrite(0, PM25_tail, 1);
       
        break;
  }
  //LED1指示收到数据
     HalLedSet(HAL_LED_1, HAL_LED_MODE_TOGGLE);
      
}
char cnt = 1;
void GenericApp_SendTheMessage( unsigned char *uartbuf, uint16 len ,const int clusterID )
{
  //HalUARTWrite(0, uartbuf, len);
  afAddrType_t my_DstAddr;      
  my_DstAddr.addrMode= (afAddrMode_t) AddrBroadcast;//广播
  my_DstAddr.endPoint= GENERICAPP_ENDPOINT;     //初始化端口号
  my_DstAddr.addr.shortAddr= 0xFFFF;    //全网广播
  while(len){
  //数据发送函数
  AF_DataRequest( &my_DstAddr,          //目的节点的网络地址以及发送数据的格式
                 &GenericApp_epDesc,    //节点描述符
                       clusterID,       //簇ID,命令号
                       len,             //发送数据的 长度
                       uartbuf,         //指向发送数据缓冲区的指针
                       &GenericApp_TransID,//指向发送序列的指针
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
  if(rxbuf[0] == MANUAL)//手动模式
  {
    Auto_flag = 0; 
    //HalUARTWrite(0, rxbuf, 1);
    GenericApp_SendTheMessage(rxbuf, 1, GENERICAPP_CLUSTERID1);
  }
  if(rxbuf[0] == AUTOMATIC)//自动模式
  {
    //HalUARTWrite(0, rxbuf, 1);
    Auto_flag = 1; 
    GenericApp_SendTheMessage(rxbuf, 1, GENERICAPP_CLUSTERID1);
  }
  if(Auto_flag == 0)//手动模式方可将上位机命令发送到终端节点
  {
    switch(rxbuf[0]){
      case OPENLAMP:
      case CLOSELAMP:       clusterID = GENERICAPP_CLUSTERID1; break;//发送数据控制灯光节点
      case OPENFAN:
      case CLOSEFAN:        clusterID = GENERICAPP_CLUSTERID2; break;//发送数据控制风扇节点    
      case OPENALARM:
      case CLOSEALARM:      clusterID = GENERICAPP_CLUSTERID3; break;//发送数据控制警报节点  
      case OPENCURTAIN:
      case CLOSECURTAIN:    clusterID = GENERICAPP_CLUSTERID4; break;//发送数据控制窗帘节点  
    }
    GenericApp_SendTheMessage(rxbuf, 1, clusterID);
  }
  if(Auto_flag == 1) //HalUARTWrite(0, flagstate, 1);

  osal_memset(rxbuf, 0, 1);
  //LED2指示收到串口数据数据
 
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
