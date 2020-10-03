/**
************************************************************
* @file         gizwits_product.c
* @brief        Gizwits control protocol processing, and platform-related       hardware initialization 
* @author       Gizwits
* @date         2017-07-19
* @version      V03030000
* @copyright    Gizwits
* 
* @note         机智云.只为智能硬件而生
*               Gizwits Smart Cloud  for Smart Products
*               链接|增值ֵ|开放|中立|安全|自有|自由|生态
*               www.gizwits.com
*
***********************************************************/

#include <stdio.h>
#include <string.h>
#include "hal_key.h"
#include "gizwits_product.h"
#include "common.h"

#define OPENCURTAIN			'1' 	 //开窗帘
#define CLOSECURTAIN		'2' 	 //关窗帘
#define OPENLAMP				'4' 	 //开灯
#define CLOSELAMP 			'3' 	 //关灯
#define OPENFAN	      	'5' 	 //开风扇
#define CLOSEFAN 				'6' 	 //关风扇
#define AUTOMATIC     	'7'   //自动模式
#define MANUAL        	'8'   //手动模式
#define OPENALARM     	'9'   //开警报
#define CLOSEALARM    	'a'   //关警报
#define RXBUFFERSIZE 1

static uint32_t timerMsCount;
uint8_t aRxBuffer;
uint8_t aRxBuffer2;			//接收中断缓冲
uint8_t Uart2_RxBuff[256];		//接收缓冲
uint8_t Uart2_Rx_Cnt = 0;		//接收缓冲计数
uint8_t	cAlmStr[] = "数据溢出(大于256)\r\n";
char Auto[1], Lamp[1], Curtain[1], Fan[1], Alarm[1];
char AM2312_data[4];
int temp, humi, PM25;
float humi1, temp1;
/** User area the current device state structure*/
dataPoint_t currentDataPoint;
extern keysTypedef_t keys;

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

/**@} */
/**@name Gizwits User Interface
* @{
*/

/**
* @brief Event handling interface

* Description:

* 1. Users can customize the changes in WiFi module status

* 2. Users can add data points in the function of event processing logic, such as calling the relevant hardware peripherals operating interface

* @param [in] info: event queue
* @param [in] data: protocol data
* @param [in] len: protocol data length
* @return NULL
* @ref gizwits_protocol.h
*/
int8_t gizwitsEventProcess(eventInfo_t *info, uint8_t *gizdata, uint32_t len)
{
  uint8_t i = 0;
  dataPoint_t *dataPointPtr = (dataPoint_t *)gizdata;
  moduleStatusInfo_t *wifiData = (moduleStatusInfo_t *)gizdata;
  protocolTime_t *ptime = (protocolTime_t *)gizdata;
  
#if MODULE_TYPE
  gprsInfo_t *gprsInfoData = (gprsInfo_t *)gizdata;
#else
  moduleInfo_t *ptModuleInfo = (moduleInfo_t *)gizdata;
#endif

  if((NULL == info) || (NULL == gizdata))
  {
    return -1;
  }

  for(i=0; i<info->num; i++)
  {
    switch(info->event[i])
    {
      case EVENT_Auto:
        currentDataPoint.valueAuto = dataPointPtr->valueAuto;
        ////GIZWITS_LOG("Evt: EVENT_Auto %d \n", currentDataPoint.valueAuto);
        if(0x01 == currentDataPoint.valueAuto)
        {
          //user handle
					Auto[0] = AUTOMATIC;
        }
        else
        {
          //user handle    
					Auto[0] = MANUAL;
        }
				printf("%c", Auto[0]);
				HAL_UART_Transmit(&huart2, (uint8_t *)&Auto, 1,0xFFFF); 
				//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
        break;
      case EVENT_lamp:
        currentDataPoint.valuelamp = dataPointPtr->valuelamp;
        ////GIZWITS_LOG("Evt: EVENT_lamp %d \n", currentDataPoint.valuelamp);
        if(0x01 == currentDataPoint.valuelamp)
        {
          //user handle
					Lamp[0] = OPENLAMP;
        }
        else
        {
          //user handle    
					Lamp[0] = CLOSELAMP;
        }
				printf("%c", Lamp[0]);
				HAL_UART_Transmit(&huart2, (uint8_t *)&Lamp, 1,0xFFFF); 
				//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
        break;
      case EVENT_Curtain:
        currentDataPoint.valueCurtain = dataPointPtr->valueCurtain;
        ////GIZWITS_LOG("Evt: EVENT_Curtain %d \n", currentDataPoint.valueCurtain);
        if(0x01 == currentDataPoint.valueCurtain)
        {
          //user handle
					Curtain[0] = OPENCURTAIN;
        }
        else
        {
          //user handle  
					Curtain[0] = CLOSECURTAIN;						
        }
				printf("%c", Curtain[0]);
				HAL_UART_Transmit(&huart2, (uint8_t *)&Curtain, 1,0xFFFF); 
				//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
        break;
      case EVENT_Fan:
        currentDataPoint.valueFan = dataPointPtr->valueFan;
        ////GIZWITS_LOG("Evt: EVENT_Fan %d \n", currentDataPoint.valueFan);
        if(0x01 == currentDataPoint.valueFan)
        {
          //user handle
					Fan[0] = OPENFAN;
        }
        else
        {
          //user handle    
					Fan[0] = CLOSEFAN;		
        }
				printf("%c", Fan[0]);
				HAL_UART_Transmit(&huart2, (uint8_t *)&Fan, 1,0xFFFF);
				//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
        break;
      case EVENT_Alarm:
        currentDataPoint.valueAlarm = dataPointPtr->valueAlarm;
        ////GIZWITS_LOG("Evt: EVENT_Alarm %d \n", currentDataPoint.valueAlarm);
        if(0x01 == currentDataPoint.valueAlarm)
        {
          //user handle
					Alarm[0] = OPENALARM;
        }
        else
        {
          //user handle    
					Alarm[0] = CLOSEALARM;
        }
				printf("%c", Alarm[0]);
				HAL_UART_Transmit(&huart2, (uint8_t *)&Alarm, 1,0xFFFF);
				//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
        break;




      case WIFI_SOFTAP:
        break;
      case WIFI_AIRLINK:
        break;
      case WIFI_STATION:
        break;
      case WIFI_CON_ROUTER:
 
        break;
      case WIFI_DISCON_ROUTER:
 
        break;
      case WIFI_CON_M2M:
 
        break;
      case WIFI_DISCON_M2M:
        break;
      case WIFI_RSSI:
        ////GIZWITS_LOG("RSSI %d\n", wifiData->rssi);
        break;
      case TRANSPARENT_DATA:
        ////GIZWITS_LOG("TRANSPARENT_DATA \n");
        //user handle , Fetch data from [data] , size is [len]
        break;
      case WIFI_NTP:
        ////GIZWITS_LOG("WIFI_NTP : [%d-%d-%d %02d:%02d:%02d][%d] \n",ptime->year,ptime->month,ptime->day,ptime->hour,ptime->minute,ptime->second,ptime->ntp);
        break;
      case MODULE_INFO:
            ////GIZWITS_LOG("MODULE INFO ...\n");
      #if MODULE_TYPE
            ////GIZWITS_LOG("GPRS MODULE ...\n");
            //Format By gprsInfo_t
      #else
            ////GIZWITS_LOG("WIF MODULE ...\n");
            //Format By moduleInfo_t
            ////GIZWITS_LOG("moduleType : [%d] \n",ptModuleInfo->moduleType);
      #endif
    break;
      default:
        break;
    }
  }

  return 0;
}

/**
* User data acquisition

* Here users need to achieve in addition to data points other than the collection of data collection, can be self-defined acquisition frequency and design data filtering algorithm

* @param none
* @return none
*/
void userHandle(void)
{
 
    currentDataPoint.valuePM25 = PM25;//Add Sensor Data Collection
    currentDataPoint.valueTemperature = temp1;//Add Sensor Data Collection
    currentDataPoint.valueHumidity = humi1;//Add Sensor Data Collection
}

/* USART2 init function */


/**
* Data point initialization function

* In the function to complete the initial user-related data
* @param none
* @return none
* @note The developer can add a data point state initialization value within this function
*/
void userInit(void)
{
    memset((uint8_t*)&currentDataPoint, 0, sizeof(dataPoint_t));
    
    /** Warning !!! DataPoint Variables Init , Must Within The Data Range **/ 
    /*
      currentDataPoint.valueAuto = 1;
      currentDataPoint.valuelamp = ;
      currentDataPoint.valueCurtain = ;
      currentDataPoint.valueFan = ;
      currentDataPoint.valueAlarm = ;
      currentDataPoint.valuePM25 = ;
      currentDataPoint.valueTemperature = ;
      currentDataPoint.valueHumidity = ;
    */

}


/**
* @brief Millisecond timing maintenance function, milliseconds increment, overflow to zero

* @param none
* @return none
*/
void gizTimerMs(void)
{
    timerMsCount++;
}

/**
* @brief Read millisecond count

* @param none
* @return millisecond count
*/
uint32_t gizGetTimerCount(void)
{
    return timerMsCount;
}

/**
* @brief MCU reset function

* @param none
* @return none
*/
void mcuRestart(void)
{
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}

/**@} */

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
 
  return ch;
}

/**
  * @brief  Period elapsed callback in non blocking mode 
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim2)
	{
			keyHandle((keysTypedef_t *)&keys);
			gizTimerMs();
	}
}

/**
* @brief Timer TIM3 init function

* @param none
* @return none
*/
void timerInit(void)
{
	HAL_TIM_Base_Start_IT(&htim2);
}

/**
  * @brief  This function handles USART IDLE interrupt.
  */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)  
{  
		if(UartHandle->Instance == USART3)  
    {  
				gizPutData((uint8_t *)&aRxBuffer, 1);

        HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);//开启下一次接收中断  
				//printf("%d", aRxBuffer);
				//printf("UASRT3\r\n");
    } 
		
		
		else if(UartHandle->Instance == USART2)
		{
			//printf("UART2\r\n");
			Uart2_RxBuff[Uart2_Rx_Cnt++] = aRxBuffer2;   //接收数据转存
			//HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
			if(Uart2_Rx_Cnt == 3)
			{
				if(Uart2_RxBuff[0] == 0xfd && Uart2_RxBuff[2] == 0xfc)
				{
					PM25 = Uart2_RxBuff[1];
					memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff)); //清空数组
					Uart2_Rx_Cnt = 0;
					printf("PM25=%d\r\n", PM25);
				}
				
			}
			if(Uart2_Rx_Cnt == 6)
			{
				if(Uart2_RxBuff[0] == 0xff && Uart2_RxBuff[5] == 0xfe)
				{
					AM2312_data[0] = Uart2_RxBuff[1];
					AM2312_data[1] = Uart2_RxBuff[2];
					AM2312_data[2] = Uart2_RxBuff[3];
					AM2312_data[3] = Uart2_RxBuff[4];
					humi &= 0;
					temp &= 0;
					humi |= AM2312_data[0];
					humi = humi<<8;
					humi |= AM2312_data[1];					
					temp |= AM2312_data[2];
					temp = temp<<8;
					temp |= AM2312_data[3];
					humi1 = (float)humi/10;
					temp1 = (float)temp/10;			
					printf("humi=%.1f temp=%.1f\r\n", humi1, temp1);			
					//HAL_UART_Transmit(&huart1, (uint8_t *)&AM2312_data, 4,0xffff);
				}
				//HAL_UART_Transmit(&huart1, (uint8_t *)&Uart2_RxBuff, Uart2_Rx_Cnt,0xffff);
				memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff)); //清空数组
				Uart2_Rx_Cnt = 0;
			}
			
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);
			
		}
	
    
	/*	
		if(Uart2_Rx_Cnt >= 255)  //溢出判断
	{
		Uart2_Rx_Cnt = 0;
		memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff));
		HAL_UART_Transmit(&huart1, (uint8_t *)&cAlmStr, sizeof(cAlmStr),0xFFFF);	
	}
	else
	{
		Uart2_RxBuff[Uart2_Rx_Cnt++] = aRxBuffer;   //接收数据转存
	
		if((Uart2_RxBuff[Uart2_Rx_Cnt-1] == 0x0A)&&(Uart2_RxBuff[Uart2_Rx_Cnt-2] == 0x0D)) //判断结束位
		{
			HAL_UART_Transmit(&huart1, (uint8_t *)&Uart2_RxBuff, Uart2_Rx_Cnt,0xFFFF); //将收到的信息发送出去
			Uart2_Rx_Cnt = 0;
			memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff)); //清空数组
		}
	}
*/
} 
/*
void senddata()
{
	if(Uart2_Rx_Cnt)
	{
		HAL_UART_Transmit(&huart2, (uint8_t *)&Uart2_RxBuff, Uart2_Rx_Cnt,0xFFFF); //将收到的信息发送出去
		//HAL_UART_Transmit(&huart1, (uint8_t *)&Uart2_RxBuff, Uart2_Rx_Cnt,0xFFFF); 
		printf("%s", Uart2_RxBuff);
		Uart2_Rx_Cnt = 0;
		memset(Uart2_RxBuff,0x00,sizeof(Uart2_RxBuff)); //清空数组
	}
}
*/
/**
* @brief USART init function

* Serial communication between WiFi modules and device MCU
* @param none
* @return none
*/
void uartInit(void)
{
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&aRxBuffer, 1);//开启下一次接收中断  
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRxBuffer2, 1);//开启下一次接收中断  
}

/**
* @brief Serial port write operation, send data to WiFi module
*
* @param buf      : buf address
* @param len      : buf length
*
* @return : Return effective data length;-1，return failure
*/
int32_t uartWrite(uint8_t *buf, uint32_t len)
{
		uint8_t crc[1] = {0x55};
    uint32_t i = 0;
	
    if(NULL == buf)
    {
        return -1;
    }

    for(i=0; i<len; i++)
    {
        HAL_UART_Transmit_IT(&huart3, (uint8_t *)&buf[i], 1);
				while (huart3.gState != HAL_UART_STATE_READY);//Loop until the end of transmission

        if(i >=2 && buf[i] == 0xFF)
        {
						HAL_UART_Transmit_IT(&huart3, (uint8_t *)&crc, 1);
						while (huart3.gState != HAL_UART_STATE_READY);//Loop until the end of transmission
        }
    }

#ifdef PROTOCOL_DEBUG
    ////GIZWITS_LOG("MCU2WiFi[%4d:%4d]: ", gizGetTimerCount(), len);
    for(i=0; i<len; i++)
    {
        ////GIZWITS_LOG("%02x ", buf[i]);

        if(i >=2 && buf[i] == 0xFF)
        {
            ////GIZWITS_LOG("%02x ", 0x55);
        }
    }
    ////GIZWITS_LOG("\n");
#endif
		
		return len;
}  
