#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#include "time.h"
#include "los_task.h"
#include "los_event.h"
#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

#include "los_queue.h"

#define OPEN_CMD   'a'  //0x01
#define CLOSE_CMD  0x02

UINT32      g_uartTskID;
extern UINT32      g_uwMsgQueue;
extern EVENT_CB_S  mx_event;


void Uart_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟
  
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1
  
  //USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9，PA10
  
  //USART1 初始化设置
  USART_InitStructure.USART_BaudRate = bound;//波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  
  USART_Cmd(USART1, ENABLE);//使能串口1 
  
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
#if 0
  NVIC_InitTypeDef NVIC_InitStructure;
    
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化VIC寄存器、
#endif
}




/*任务入口函数*/
UINT32 Uart_Task()
{
    UINT32 uwRet;

    printf("Enter TaskLo Handler.\r\n");

    for(;;)
    {
      if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
      {
          /* Read one byte from the receive data register */
          UINT8 cmd = USART_ReceiveData(USART1);
          
         // if ('c' <= cmd)
         // {
            //LOS_TaskLock();
            
         //   while('#' != cmd)
         //   {
        //    USART_GetFlagStatus(USART1, USART_FLAG_RXNE);                
        //    cmd = USART_ReceiveData(USART1);
            USART_SendData(USART1, cmd);
            //printf("while cmd:%c\n",cmd);
        //    }
        //  }
          if ('#' == cmd){
            
            
            //LOS_TaskUnlock();          
          }
          
          uwRet = LOS_QueueWrite(g_uwMsgQueue, (void *)&cmd, sizeof(cmd), 0);
          //if(uwRet != LOS_OK)
          {
              printf("send message failure,error:%x\n",uwRet);
              printf("send message failure,cmd:%c\n",cmd);
          }
          
       }
    }
}

/* 串口任务入口函数 */
UINT32 Uart_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;    
   
    Uart_init(115200);     

    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");    

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Uart_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.UART";
    stInitParam.uwStackSize = 0x400;
	stInitParam.uwResved   = LOS_TASK_STATUS_DETACHED;
    
    /*创建目标任务，由于锁任务调度，任务创建成功后不会马上执行*/
    uwRet = LOS_TaskCreate(&g_uartTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        LOS_TaskUnlock();

        printf("Uart_TaskLo create Failed!\r\n");
        return LOS_NOK;
    }

    printf("Uart_TaskLo create Success!\r\n");

    /*解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务*/
    LOS_TaskUnlock();


}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
