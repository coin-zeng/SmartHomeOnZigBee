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

#define SEND_DATA_EVENT  0x01

#define HAVELIGHT       '2' 	 //有光
#define NOLIGHT		'1' 	 //无光

char cnt = 1;

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] = 
{
  GENERICAPP_CLUSTERID4
};

const SimpleDescriptionFormat_t GenericApp_SimpleDesc = 
{
  GENERICAPP_ENDPOINT,
  GENERICAPP_PROFID,
  GENERICAPP_DEVICEID,
  GENERICAPP_DEVICE_VERSION,
  GENERICAPP_FLAGS,
  0,
  (cId_t *)NULL,
  GENERICAPP_MAX_CLUSTERS,
  (cId_t *)GenericApp_ClusterList
};

endPointDesc_t GenericApp_epDesc;
byte GenericApp_TaskID;
byte GenericApp_TransID;
devStates_t GenericApp_NwkState;
unsigned char uartbuf[64];

void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);
void GenericApp_SendTheMessage(void);
static void rxCB(uint8 port, uint8 event);
void Delay_ms(unsigned int Time);//n ms延时

void GenericApp_Init(byte task_id)
{
  halUARTCfg_t uartConfig;
  GenericApp_TaskID             = task_id;
  GenericApp_NwkState           = DEV_INIT;
  GenericApp_TransID            = 0;
  GenericApp_epDesc.endPoint    = GENERICAPP_ENDPOINT;
  GenericApp_epDesc.task_id     = &GenericApp_TaskID;
  GenericApp_epDesc.simpleDesc  = (SimpleDescriptionFormat_t *)&GenericApp_SimpleDesc;
  GenericApp_epDesc.latencyReq  = noLatencyReqs;
  afRegister(&GenericApp_epDesc);
  
  uartConfig.configured         = TRUE;
  uartConfig.baudRate           = HAL_UART_BR_115200;
  uartConfig.flowControl        = FALSE;
  uartConfig.callBackFunc       = rxCB;
  HalUARTOpen(0, &uartConfig);
}

UINT16 GenericApp_ProcessEvent(byte task_id, UINT16 events)
{
  afIncomingMSGPacket_t *MSGpkt;
  if(events & SYS_EVENT_MSG)
  {
    MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);
    while(MSGpkt)
    {
      switch(MSGpkt->hdr.event)
      {
      case ZDO_STATE_CHANGE:
        GenericApp_NwkState = (devStates_t)(MSGpkt->hdr.status);
        if(GenericApp_NwkState == DEV_END_DEVICE)
        {
          HalLedBlink(HAL_LED_1,0,50,300);
          //osal_set_event(GenericApp_TaskID, SEND_DATA_EVENT);
          //GenericApp_SendTheMessage();
        }
        break;
        /*
      case AF_INCOMING_MSG_CMD:
        GenericApp_MessageMSGCB(MSGpkt);
        break;
      */
      default:
        break;
      }
      osal_msg_deallocate((uint8 *)MSGpkt);
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);
    }
    return (events ^ SYS_EVENT_MSG);
  }
  /*
  if(events & SEND_DATA_EVENT)
  {
    //GenericApp_SendTheMessage();
    osal_start_timerEx(GenericApp_TaskID, SEND_DATA_EVENT, 500);
    return (events ^ SEND_DATA_EVENT);
  }
  */
  return 0;
}

void GenericApp_SendTheMessage( void )
{
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode= (afAddrMode_t) Addr16Bit;  //单播
  my_DstAddr.endPoint= GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr= 0x0000;    //协调器地址
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID4,
                       osal_strlen(uartbuf),
                       uartbuf,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, 
                       AF_DEFAULT_RADIUS ) ;
  
  //osal_memset(uartbuf, 0, osal_strlen(uartbuf));
  
}


/*
void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt)
{
  char recvbuf[16] ;
  switch (pckt->clusterId)
  {
    case GENERICAPP_CLUSTERID:
      osal_memcpy(recvbuf, pckt->cmd.Data,16);
      if(osal_memcmp(recvbuf,copen,6))
      {
        HalUARTWrite(0, copen1, 6);
      }
      else if(osal_memcmp(recvbuf,coff,5))
      {
        HalUARTWrite(0, coff1, 5);
      }
      //HalUARTWrite(0, buffer, osal_strlen(buffer)+1);
      cnt++;
      if(cnt%2 == 0) HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
      else HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
      if(cnt == 255) cnt=1;
      osal_memset(recvbuf, 0, 16);
      break;
  }
}
*/

char cnt1 = 1;
char data_flag = 1;//状态改变才发送数据
//串口收到数据马上发送
static void rxCB(uint8 port, uint8 event)
{
  //LED2指示收到串口数据
  HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);

  //HalUARTPollDMA()
  HalUARTRead(0, uartbuf, 1);
  if(uartbuf[0] == NOLIGHT)//光暗
  {
    Delay_ms(200);//延时消除隐患
    if(uartbuf[0] == NOLIGHT)
    {
      if(data_flag == 1){
        GenericApp_SendTheMessage();
        data_flag = 0;
      }
    } 
  }
  
  if(uartbuf[0] == HAVELIGHT)//光亮
  {
    Delay_ms(200);
    if(uartbuf[0] == HAVELIGHT){
      if(data_flag == 0){
      GenericApp_SendTheMessage();
      data_flag = 1;
      }
    }
  }
  osal_memset(uartbuf, 0, osal_strlen(uartbuf));
}

void Delay_ms(unsigned int Time)//n ms延时
{
  unsigned char i;
  while(Time--)
  {
    for(i=0;i<100;i++)
     MicroWait(10);
  }
}