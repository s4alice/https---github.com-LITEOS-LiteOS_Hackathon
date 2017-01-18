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
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART1ʱ��
  
  //����1��Ӧ���Ÿ���ӳ��
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9����ΪUSART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10����ΪUSART1
  
  //USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9��PA10
  
  //USART1 ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;//����������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  
  USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1 
  
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
#if 0
  NVIC_InitTypeDef NVIC_InitStructure;
    
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//�����ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
#endif
}




/*������ں���*/
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

/* ����������ں��� */
UINT32 Uart_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;    
   
    Uart_init(115200);     

    /*���������*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");    

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Uart_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.UART";
    stInitParam.uwStackSize = 0x400;
	stInitParam.uwResved   = LOS_TASK_STATUS_DETACHED;
    
    /*����Ŀ������������������ȣ����񴴽��ɹ��󲻻�����ִ��*/
    uwRet = LOS_TaskCreate(&g_uartTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        LOS_TaskUnlock();

        printf("Uart_TaskLo create Failed!\r\n");
        return LOS_NOK;
    }

    printf("Uart_TaskLo create Success!\r\n");

    /*����������ȣ���ʱ�ᷢ��������ȣ�ִ�о����б���������ȼ�����*/
    LOS_TaskUnlock();


}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
