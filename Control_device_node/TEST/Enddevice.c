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

#define SOMEONE         0x01    //有人
#define NOONE           0x02    //无人
#define HAVESMOKE       0x03    //有烟雾
#define NOSMOKE         0x04    //无烟雾
#define B_LIGHT	        0x05	//强光
#define L_LIGHT	        0x06	//弱光
#define HAVEKEY	        0x07	//按下
#define NOKEY	        0x08	//没按
#define HAVEFIRE	0x09	//有火焰
#define NOFIRE	        0x0A	//无火焰
char cnt = 1;

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] = 
{
  GENERICAPP_CLUSTERID,
  GENERICAPP_CLUSTERID1,
  GENERICAPP_CLUSTERID2,
  GENERICAPP_CLUSTERID3,
  GENERICAPP_CLUSTERID4,
  GENERICAPP_CLUSTERID5,
  GENERICAPP_CLUSTERID6,
  GENERICAPP_CLUSTERID7
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
unsigned char uartbuf[128];

void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt);
void GenericApp_SendTheMessage(void);
//static void rxCB(uint8 port, uint8 event);

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
  uartConfig.callBackFunc       = NULL;
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
        }
        break;
      case AF_INCOMING_MSG_CMD:
        GenericApp_MessageMSGCB(MSGpkt);
        break;
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
    GenericApp_SendTheMessage();
    osal_start_timerEx(GenericApp_TaskID, SEND_DATA_EVENT, 500);
    return (events ^ SEND_DATA_EVENT);
  }
  */
  return 0;
}
/*
void GenericApp_SendTheMessage( void )
{
  HalUARTWrite(0, uartbuf, 128);
  //unsigned char theMessageData[4] = "LED";
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode= (afAddrMode_t) Addr16Bit;
  my_DstAddr.endPoint= GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr= 0x0000;
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID,
                       osal_strlen(uartbuf)+1,
                       uartbuf,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, 
                       AF_DEFAULT_RADIUS ) ;
  
  osal_memset(uartbuf, 0, osal_strlen(uartbuf));
  
  //HalLedSet(HAL_LED_1, HAL_LED_MODE_ON);
  //HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
}
*/

//用于观察协调器发送的数据
void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt)
{
  unsigned char recvbuf[16] ;
  osal_memcpy(recvbuf, pckt->cmd.Data,pckt->cmd.DataLength);
  HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
  /*
  switch (pckt->clusterId)
  {
    case GENERICAPP_CLUSTERID1:
      //人体检测
      if(recvbuf[0] == SOMEONE)//有人开灯
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      else if(recvbuf[0] == NOONE)//无人关灯
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      break;
      
    case GENERICAPP_CLUSTERID2:
      //HalUARTWrite(0, recvbuf, osal_strlen(recvbuf)+1);
      //烟雾检测
      if(recvbuf[0] == HAVESMOKE)//有烟雾
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      else if(recvbuf[0] == NOSMOKE)//无烟雾
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      break;
      
     case GENERICAPP_CLUSTERID3:
        //火焰检测
      if(recvbuf[0] == HAVEFIRE)//应急按下
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      else if(recvbuf[0] == NOFIRE)//应急按下无
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      break;
     
    case GENERICAPP_CLUSTERID4:
        //光敏检测
      if(recvbuf[0] == B_LIGHT)//强光
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }else if(recvbuf[0] == L_LIGHT)//弱光
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      break;
      
    case GENERICAPP_CLUSTERID5:
      break;
      
    case GENERICAPP_CLUSTERID6:
      //应急按钮
      if(recvbuf[0] == HAVEKEY)//应急按下
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }
      else if(recvbuf[0] == NOKEY)//应急按下无
      {
        HalUARTWrite(0, recvbuf, pckt->cmd.DataLength);
      }   
      break;
     
      
      
      
      //LED2指示收到协调器数据
      cnt++;
      if(cnt%2 == 0) HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
      else HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
      if(cnt == 255) cnt=1;
      osal_memset(recvbuf, 0, 16);
      break;
  }
*/
  osal_memset(recvbuf, 0, pckt->cmd.DataLength);
}
/*
char cnt1 = 1;
static void rxCB(uint8 port, uint8 event)
{
  cnt1++;
  if(cnt1%2 == 0) HalLedSet(HAL_LED_2, HAL_LED_MODE_ON);
  else HalLedSet(HAL_LED_2, HAL_LED_MODE_OFF);
  if(cnt1 == 255) cnt1=1;
  //HalUARTPollDMA()
  HalUARTRead(0, uartbuf, 128); 
}
*/