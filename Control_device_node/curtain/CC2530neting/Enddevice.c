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

  return 0;
}


void GenericApp_MessageMSGCB(afIncomingMSGPacket_t *pckt)
{ 
  switch (pckt->clusterId)
  {
	  //直接将收到的数据通过串口发送给设备
    case GENERICAPP_CLUSTERID4:
      
      HalUARTWrite(0, pckt->cmd.Data, pckt->cmd.DataLength);
      
      //LED2指示收到协调器数据
      HalLedSet(HAL_LED_2, HAL_LED_MODE_TOGGLE);

      break;
  }
}
