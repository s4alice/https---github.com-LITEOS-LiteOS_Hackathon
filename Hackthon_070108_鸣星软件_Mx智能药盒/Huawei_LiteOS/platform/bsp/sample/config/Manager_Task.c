#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "string.h"
#include "los_task.h"
#include "los_queue.h"


typedef UINT32 TSK_HANDLE_T;
typedef UINT8  retIR_t;


#define OS_TSK_DETACHED 0


// UART CMD
#define OPEN_MOTOR_CMD     'a'
#define CLOSE_MOTOR_CMD    'b'
#define SET_CLOCK_CMD      'c'
#define SET_alarm_1_CMD    'd'
#define SET_alarm_2_CMD    'e'
#define SET_alarm_3_CMD    'f'
#define SET_alarm_4_CMD    'g'
#define END_SYMBOL         '#'



UINT32      g_managerTskID;
  
extern UINT32      g_uwMsgQueue;
extern EVENT_CB_S  mx_event;


UINT32 Manager_Task()
{
    UINT32      uwRet;
    UINT32      uwChar;
    char        cmdChar;

    printf("Manager_Task Handler.\r\n");
    
    for(;;)
    {
      printf("Manager_Task Handler.\r\n");
        uwRet = LOS_QueueRead(g_uwMsgQueue, &uwChar, 1, 100);
        if(uwRet != LOS_OK)
        {
            continue;
        }
        cmdChar = *(char *)uwChar;
        
        switch (cmdChar)
        {
          case (OPEN_MOTOR_CMD):
          {
            uwRet = LOS_EventWrite(&mx_event, OPEN_MOTOR);
            if(uwRet != LOS_OK)
            {
                printf("event write failed .\n");
                return LOS_NOK;
            }
            break;
          }
          
          case (CLOSE_MOTOR_CMD):
          {
            uwRet = LOS_EventWrite(&mx_event, CLOSE_MOTOR);
            if(uwRet != LOS_OK)
            {
                printf("event write failed .\n");
                return LOS_NOK;
            }
            break;
          }
          
          case (SET_CLOCK_CMD):
          {
            char time[16];
            char * pt = time;
            memset(time, 0, sizeof(time));
            //while(END_SYMBOL != cmdChar)
            {
              uwRet = LOS_QueueRead(g_uwMsgQueue, &uwChar, 1, 500);
              if(uwRet != LOS_OK)
              {
                  break;
              }
              cmdChar = *(char *)uwChar;              
              *(pt++)= cmdChar; 
            }
            if (END_SYMBOL == time[14])
            {
              printf("%s\n", time);
            
            
            }
            else
            {
              printf("%s\n", time);
            }
            break;
          }
          
          case (SET_alarm_1_CMD):
          {
            
            break;
          }
          
          case (SET_alarm_2_CMD):
          {
            break;
          }
          
          case (SET_alarm_3_CMD):
          {
            break;
          }
          
          case (SET_alarm_4_CMD):
          {
            break;
          }
          default:
            break;   
        }
    }
}

/* 任务入口函数 */
UINT32 Manager_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Manager_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.Manager";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*创建目标任务*/
    uwRet = LOS_TaskCreate(&g_managerTskID, &stInitParam);
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
