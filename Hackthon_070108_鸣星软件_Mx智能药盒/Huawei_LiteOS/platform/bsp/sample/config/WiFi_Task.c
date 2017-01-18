#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

typedef UINT32 TSK_HANDLE_T;

#define WAITATMAX        100
#define WIFINAME 
#define WIFINAMELEN
#define WIFINPASSWORD 
#define WIFINPASSWORDLEN
#define WIFIBUFSIZE      128

#define AT_OTHER      0x01
#define AT_OK         0x02
#define OS_TSK_DETACHED 0

UINT32      g_WiFiTskID;

char wifiBuf[WIFIBUFSIZE];

extern EVENT_CB_S  mx_event;


  //AT+CWMODE=1
  //AT+RST
  //AT+CWJAP=\"ALIENTEK\",\"15902020353\" 
  //AT+CIPMUX=0
  //AT+CIPSTART="TCP","192.168.8.8",8086
  //AT+CIPMODE=1
  //AT+CIPSEND
char * ATCWMODE         = "AT+CWMODE=1\r\n" ;
char * ATRST            = "AT+RST\r\n" ;
char * ATCWJAP          = "AT+CWJAP=\"AMI\",\"wenhaozhu\"\r\n" ;
//char * ATCWJAP          = "AT+CWJAP=\"TechTemple Guest\",\"kejisiguest\"\r\n" ;
char * ATCIPMUX        = "AT+CIPMUX=0\r\n";
char * ATCIPSTART       = "AT+CIPSTART=\"TCP\",\"192.168.43.170\",48569\r\n" ;
char * ATCIPMODE        = "AT+CIPMODE=1\r\n" ;
char * ATCIPSEND        = "AT+CIPSEND\r\n" ;



void Uartx_Send(USART_TypeDef* USARTx, char * data,uint16_t datLen)
{
  for (uint16_t i = 0; i < datLen; i++)
  {
      USART_SendData(USARTx, *(data + i));
      while((USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET));  
  }
}

UINT8 AtWifiCMD(char * cmd, uint16_t cmdLen)
{
  LOS_TaskLock();

  uint16_t waitTimes = WAITATMAX;
  uint16_t receiveLen = 0;
  UINT8  reAt = AT_OTHER;
  
  Uartx_Send(USART2, cmd, cmdLen);  
  
  while(waitTimes--)
  {
      UINT8 ret;
      memset(wifiBuf, 0, sizeof(wifiBuf));
      if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)
      {
          /* Read one byte from the receive data register */
          wifiBuf[ receiveLen++ ] = USART_ReceiveData(USART2);      
      }
  }
  for (UINT16 i = 0; i < receiveLen; i++ )    
  {
   if( ('o '== wifiBuf[i] || 'O '== wifiBuf[i]) && ('k '== wifiBuf[i+1] || 'K'== wifiBuf[i+1]) )
   {
      reAt = AT_OK;
      break;
   } 
  }
  LOS_TaskUnlock();
  return reAt;
}


UINT8 ConnectNet()
{
  UINT8 netRet = 0x00;
  UINT8 retAT;
  
  
  //1: AT+CWMODE=1
  retAT = AtWifiCMD(ATCWMODE, strlen(ATCWMODE));
  if (AT_OK != retAT)
  {
    netRet |= 1;
  }
  LOS_TaskDelay(LOS_MS2Tick(200));
  
  //2: AT+RST
  retAT = AtWifiCMD(ATRST, strlen(ATRST));
  if (AT_OK != retAT)
  {
    netRet |= 1<<1;
  }
  LOS_TaskDelay(LOS_MS2Tick(1000));
    
  //3: T+CWJAP="ALIENTEK","15902020353" 
  retAT = AtWifiCMD(ATCWJAP, strlen(ATCWJAP));
  if (AT_OK != retAT)
  {
    netRet |= 1<<2;
  }
  LOS_TaskDelay(LOS_MS2Tick(2000));
  
  //4: AT+CIPMUX=0
  retAT = AtWifiCMD(ATCIPMUX, strlen(ATCIPMUX));
  if (AT_OK != retAT)
  {
    netRet |= 1<<3;
  }  
  LOS_TaskDelay(LOS_MS2Tick(200));
   
  //5: AT+CIPSTART="TCP","192.168.8.8",8086
  retAT = AtWifiCMD(ATCIPSTART, strlen(ATCIPSTART));
  if (AT_OK != retAT)
  {
    netRet |= 1<<4;
  }
  LOS_TaskDelay(LOS_MS2Tick(1000));
  
  //6: AT+CIPMODE=1
  retAT = AtWifiCMD(ATCIPMODE, strlen(ATCIPMODE));
  if (AT_OK != retAT)
  {
    netRet |= 1<<5;
  }
  LOS_TaskDelay(LOS_MS2Tick(200));
  
  //7: AT+CIPSEND
  retAT = AtWifiCMD(ATCIPSEND, strlen(ATCIPSEND));
  if (AT_OK != retAT)
  {
    netRet |= 1<<6;
  }  
  return netRet;
}

UINT32 WiFi_Task()
{
    UINT32      uwRet;

    printf("Enter Task Handler.\r\n");
    
    UINT8 ret = ConnectNet();
    printf("ConnectNet:%d.\r\n", ret);

    for(;;)
    {
      printf("Connet OK.\r\n");
      USART_SendData(USART2, 'W');
      while((USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET));
    }
}

void WiFi_UartConfig()
{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//使能USART1时钟
  
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA10复用为USART1
  
  //USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9，PA10
  
  //USART1 初始化设置
  USART_InitStructure.USART_BaudRate = 115200;//波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
  USART_Init(USART2, &USART_InitStructure); //初始化串口1
  
  USART_Cmd(USART2, ENABLE);//使能串口1 

}

UINT32 WiFi_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    WiFi_UartConfig();
    
    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)WiFi_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.IR";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*创建目标任务*/
    uwRet = LOS_TaskCreate(&g_WiFiTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Task create Failed!\r\n");
        return LOS_NOK;
    }
    printf("Task create Success!\r\n");
    
    /*解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务*/
    LOS_TaskUnlock();
    
    return LOS_OK;    
}
