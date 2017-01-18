#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"
  #include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"

typedef UINT32 TSK_HANDLE_T;



#define OS_TSK_DETACHED 0



UINT32      g_LedTskID;
  
  
extern EVENT_CB_S  mx_event;


void Led_Init()
{ 
  GPIO_InitTypeDef  GPIO_InitStructure;

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  
  RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
    
UINT32 Led_Task()
{
    printf("Enter Task Handler.\r\n");
    for(;;)
    {      
      GPIO_WriteBit(GPIOA,GPIO_Pin_5, Bit_RESET);
      printf("PPPPPPPPAAAAAA5555555---Bit_RESET\n");
      LOS_TaskDelay(20);
      GPIO_WriteBit(GPIOA,GPIO_Pin_5, Bit_SET);
      printf("PPPPPPPPAAAAAA5555555---Bit_SET\n");
      LOS_TaskDelay(20);        
    }
}

UINT32 Led_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
   
    Led_Init();
    
    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Led_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.Led";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*创建目标任务*/
    uwRet = LOS_TaskCreate(&g_LedTskID, &stInitParam);
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
