#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


#include "time.h"
#include "los_task.h"
#include "los_event.h"
#include "stm32f4xx_usart.h"
#include "user_function.h"
  
UINT32 g_uwTskHiID;
UINT32 g_uwTskLoID;
#define TSK_PRIOR_HI 4
#define TSK_PRIOR_LO 5


#define open_motor  0x00000001
#define close_motor 0x00000010

#define OPEN_CMD   'a'  //0x01
#define CLOSE_CMD  0x02


UINT32 Motor_Task()
{
    UINT32 uwRet;
    UINT32 uwEvent;

    while(1)
    {
      printf("Enter TaskHi Handler.\r\n");
      uwEvent = LOS_EventRead(&mx_event, open_motor, LOS_WAITMODE_AND, 100);
      if(uwEvent == open_motor)
      {
          printf("Mx_Event,read event :0x%x\n",uwEvent);
          CW_MotorTurning(32);
          LOS_EventClear(&mx_event, ~mx_event.uwEventID);
          
#if 1
          uwRet = LOS_TaskDelay( LOS_MS2Tick(5000));
          if (uwRet != LOS_OK)
          {
              printf("Delay Task Failed.\r\n");
              return LOS_NOK;
          }
          //5000ms时间到了后，该任务恢复，继续执行
          printf("TaskHi LOS_TaskDelay Done.\r\n");
          CCW_MotorTurning(32);          
#endif
      }
      else
      {
          printf("Example_Event,read event timeout\n");
      }
      
      uwRet = LOS_TaskDelay( LOS_MS2Tick(5000));
      if (uwRet != LOS_OK)
      {
          printf("Delay Task Failed.\r\n");
          return LOS_NOK;
      }
      
     }
}

/*低优先级任务入口函数*/
UINT32 Uart_TaskLo()
{
    UINT32 uwRet;

    printf("Enter TaskLo Handler.\r\n");

    for(;;)
    {
      if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
      {
        /* Read one byte from the receive data register */
        UINT8 cmd =(USART_ReceiveData(USART1));
        switch(cmd)
        {
            case (OPEN_CMD):
              uwRet = LOS_EventWrite(&mx_event, open_motor);
              if(uwRet != LOS_OK)
              {
                  printf("event write failed .\n");
                  return LOS_NOK;
              } 
              break;
          //case ('b'):
            //CCW_MotorTurning(16); break;
          default:
            break;
        }
        USART_SendData(USART1, cmd);
        printf("in[%c].\r\n",cmd);
      }
    }
}

/*任务测试入口函数，在里面创建优先级不一样的两个任务*/
UINT32 Two_Tasks(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    LEDGpio_Init();
    AllLedLow();    
    Uart_init(115200); 

    /*锁任务调度*/
    LOS_TaskLock();

    printf("LOS_TaskLock() Success!\r\n");
    
    /*事件初始化*/
    uwRet = LOS_EventInit(&mx_event);
    if(uwRet != LOS_OK)
    {
        printf("Init event failed .\n");
        return -1;
    }

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Motor_Task;
    stInitParam.usTaskPrio = TSK_PRIOR_HI;
    stInitParam.pcName = "HIGH_Motor";
    stInitParam.uwStackSize = 0x400;
	stInitParam.uwResved   = LOS_TASK_STATUS_DETACHED;
    /*创建高优先级任务，由于锁任务调度，任务创建成功后不会马上执行*/
    uwRet = LOS_TaskCreate(&g_uwTskHiID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        LOS_TaskUnlock();

        printf("Motor_TaskHi create Failed!\r\n");
        return LOS_NOK;
    }

    printf("Motor_TaskHi create Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Uart_TaskLo;
    stInitParam.usTaskPrio = TSK_PRIOR_LO;
    stInitParam.pcName = "LOW_UART";
    stInitParam.uwStackSize = 0x400;
	stInitParam.uwResved   = LOS_TASK_STATUS_DETACHED;
    /*创建低优先级任务，由于锁任务调度，任务创建成功后不会马上执行*/
    uwRet = LOS_TaskCreate(&g_uwTskLoID, &stInitParam);
    if (uwRet != LOS_OK)
    {
        LOS_TaskUnlock();

        printf("Uart_TaskLo create Failed!\r\n");
        return LOS_NOK;
    }

    printf("Uart_TaskLo create Success!\r\n");

    /*解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务*/
    LOS_TaskUnlock();

    //while(1){};

}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
