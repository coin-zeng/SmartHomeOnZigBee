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

const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] = 
{
  GENERICAPP_CLUSTERID5
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
void GenericApp_SendTheMessage(uint8 *PC_CMD, uint8 Rx_Count);
static void rxCB(uint8 port, uint8 event);

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
          osal_set_event(GenericApp_TaskID, SEND_DATA_EVENT);
          //GenericApp_SendTheMessage();
        }
        break;
      default:
        break;
      }
      osal_msg_deallocate((uint8 *)MSGpkt);
      MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(GenericApp_TaskID);
    }
    return (events ^ SYS_EVENT_MSG);
  }
  if(events & SEND_DATA_EVENT)
  {
    //GenericApp_SendTheMessage();
    //osal_start_timerEx(GenericApp_TaskID, SEND_DATA_EVENT, 2000);//2s发送一次数据
    return (events ^ SEND_DATA_EVENT);
  }
  return 0;
}

void GenericApp_SendTheMessage( uint8 *PC_CMD, uint8 Rx_Count )
{
  //HalUARTWrite(0, uartbuf, 128);
  //unsigned char theMessageData[4] = "LED";
  uint8 Temp[128];
  uint8 i;

   for(i=0;i<Rx_Count;i++)
   {
     Temp[i]=PC_CMD[i];
   }
  afAddrType_t my_DstAddr;
  my_DstAddr.addrMode= (afAddrMode_t) Addr16Bit;//单播
  my_DstAddr.endPoint= GENERICAPP_ENDPOINT;
  my_DstAddr.addr.shortAddr= 0x0000;	//协调器地址
  AF_DataRequest( &my_DstAddr, &GenericApp_epDesc,
                       GENERICAPP_CLUSTERID5,
                       Rx_Count,
                       Temp,
                       &GenericApp_TransID,
                       AF_DISCV_ROUTE, 
                       AF_DEFAULT_RADIUS ) ;
  
  osal_memset(Temp, 0, Rx_Count);
}
char cnt = 1;
static void rxCB(uint8 port, uint8 event)
{
  unsigned char From_Pc_Cmd[128];
  uint8 Rx_Count;
  //LED2指示收到串口数据
   HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);
  
  Rx_Count = Hal_UART_RxBufLen ( 0 );
  HalUARTRead(0, From_Pc_Cmd, Rx_Count);
  GenericApp_SendTheMessage(From_Pc_Cmd, Rx_Count);
  osal_memset(From_Pc_Cmd, 0, osal_strlen(From_Pc_Cmd));
  
}