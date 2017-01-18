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
#include "misc.h"
  


#define OPEN_CMD   0x01
#define CLOSE_CMD  0x02
  

UINT32      g_motorTskID;
extern UINT32      g_IrTskID;

extern EVENT_CB_S  mx_event;


#define LED1_RCC           RCC_AHB1Periph_GPIOA
#define LED1_GPIO          GPIOA
#define LED1_GPIO_PIN      GPIO_Pin_4
#define LED1_ONOFF(x)      GPIO_WriteBit(GPIOA,GPIO_Pin_4,x);

#define LED2_RCC           RCC_AHB1Periph_GPIOA
#define LED2_GPIO          GPIOA
#define LED2_GPIO_PIN      GPIO_Pin_6
#define LED2_ONOFF(x)      GPIO_WriteBit(GPIOA,GPIO_Pin_6,x);

#define LED3_RCC           RCC_AHB1Periph_GPIOB
#define LED3_GPIO          GPIOB
#define LED3_GPIO_PIN      GPIO_Pin_6
#define LED3_ONOFF(x)      GPIO_WriteBit(GPIOB,GPIO_Pin_6,x);

#define LED4_RCC           RCC_AHB1Periph_GPIOC
#define LED4_GPIO          GPIOC
#define LED4_GPIO_PIN      GPIO_Pin_6
#define LED4_ONOFF(x)      GPIO_WriteBit(GPIOC,GPIO_Pin_6,x);



typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}Gpio_Info;


void DelayMs(UINT16 ms)
{
  for(; ms--; )
  {
    //UINT16 nCount = 0xa53;
    UINT16 nCount = 0xa60;
    for(; nCount--; );  
  }
}

void Delay10Us(UINT16 us)
{  
  for(; us--; )
  {
    UINT8 nCount = 0x16;
    for(; nCount--; );  
  }
}

void LEDGpio_Init(void)
{
  Gpio_Info Gpio_info[4]={
      {LED1_RCC, LED1_GPIO, LED1_GPIO_PIN},
      {LED2_RCC, LED2_GPIO, LED2_GPIO_PIN},
      {LED3_RCC, LED3_GPIO, LED3_GPIO_PIN},
      {LED4_RCC, LED4_GPIO, LED4_GPIO_PIN}
    };
  
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for(i=0;i<4;i++)
    {
        RCC_AHB1PeriphClockCmd( Gpio_info[i].rcc , ENABLE);
        GPIO_InitStructure.GPIO_Pin = Gpio_info[i].pin;
        GPIO_Init(Gpio_info[i].gpio, &GPIO_InitStructure);
    }
}

void AllLedLow(void)
{
  LED1_ONOFF(Bit_SET);
  LED2_ONOFF(Bit_SET);
  LED3_ONOFF(Bit_SET);
  LED4_ONOFF(Bit_SET);
}

void CW_MotorTurning(UINT16 pat)
{
  LOS_TaskLock();
  for(UINT16 j = 0; j < pat; j++)
  {
      for(UINT8 i = 0; i < 8; i++)      
      {
        /* 0x08 */
        LED4_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x0c */
        LED3_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x04*/
        LED4_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x06 */
        LED2_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x02 */
        LED2_ONOFF(Bit_RESET);
        LED3_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x03 */
        LED1_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x01 */
        LED1_ONOFF(Bit_RESET);
        LED2_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x09 */
        LED4_ONOFF(Bit_RESET);
        DelayMs(1);
        AllLedLow();
      }      
  }
  LOS_TaskUnlock();
}

void CCW_MotorTurning(UINT16 pat)
{   
  LOS_TaskLock();  
  for(UINT16 j = 0; j < pat; j++)
  {  
    for(UINT8 i = 0; i < 8; i++)      
    {
      /* 0x09 */
      LED1_ONOFF(Bit_RESET);
      LED4_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x01 */
      LED4_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x03 */
      LED2_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x02 */
      LED1_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x06 */
      LED3_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x04*/
      LED2_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x0c */
      LED4_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x08 */
      LED3_ONOFF(Bit_SET);
      DelayMs(1);
      AllLedLow();
    }
  }
  LOS_TaskUnlock();
}

UINT32 Motor_Task()
{
    UINT32 uwRet;
    UINT32 uwEvent;

    for(;;)
    {
      printf("Enter TaskHi Handler.\r\n");
      uwEvent = LOS_EventRead(&mx_event, OPEN_MOTOR, LOS_WAITMODE_AND, 100);
      if(uwEvent == OPEN_MOTOR)
      {
          printf("Mx_Event,read event :0x%x\n",uwEvent);
          CW_MotorTurning(32);
          LOS_EventClear(&mx_event, ~mx_event.uwEventID);
          
#if defined(IR_SENSOR)
          uwRet = LOS_TaskResume(g_IrTskID);
          if (uwRet != LOS_OK)
          {
            printf("LOS_TaskResume IR_Task Failed.\r\n");              
          }
          else
          {
            uwEvent = LOS_EventRead(&mx_event, PEOPLE_COME, LOS_WAITMODE_AND, 100);
            if(uwEvent == PEOPLE_COME)
            {
              LOS_EventClear(&mx_event, ~mx_event.uwEventID);
            }
          }
#endif
          
          uwRet = LOS_TaskDelay( LOS_MS2Tick(5000) );
          if (uwRet != LOS_OK)
          {
              printf("Delay Task Failed.\r\n");
              return LOS_NOK;
          }
          //5000ms时间到了后，该任务恢复，继续执行
          printf("TaskHi LOS_TaskDelay Done.\r\n");
          
#if defined(IR_SENSOR)
          uwEvent = LOS_EventRead(&mx_event, PEOPLE_GONE, LOS_WAITMODE_AND, 100);
          if(uwEvent == PEOPLE_GONE)
          {
            LOS_EventClear(&mx_event, ~mx_event.uwEventID);
          }
          uwRet = LOS_TaskSuspend(g_IrTskID);
          if (uwRet != LOS_OK)
          {
              printf("Suspend Task Failed.\r\n");
              return LOS_NOK;
          }
#endif
            
          CCW_MotorTurning(32); 
      }
      else
      {
          printf("Example_Event,read event timeout\n");
      } 
    }
}

/* 任务入口函数 */
UINT32 Motor_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    LEDGpio_Init();
    AllLedLow();    

    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Motor_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.Motor";
    stInitParam.uwStackSize = 0x400;
	stInitParam.uwResved   = LOS_TASK_STATUS_DETACHED;
    
    /*创建目标任务，由于锁任务调度，任务创建成功后不会马上执行*/
    uwRet = LOS_TaskCreate(&g_motorTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        LOS_TaskUnlock();

        printf("Motor_TaskHi create Failed!\r\n");
        return LOS_NOK;
    }
    printf("Motor_TaskHi create Success!\r\n");

    /*解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务*/
    LOS_TaskUnlock();

}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
