#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"

typedef UINT32 TSK_HANDLE_T;
typedef UINT8  retIR_t;


#define OS_TSK_DETACHED 0


UINT32      g_RTCTskID;
  
 
extern EVENT_CB_S  mx_event;



UINT32 RTC_Task()
{
    UINT32      uwRet;
    retIR_t     irRet;
    UINT8       irr = PEOPLE_COME;

    printf("Enter Task Handler.\r\n");
    
    while(1)
    {
   
    }
}

/* IR任务入口函数 */
UINT32 RTC_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    /*锁任务调度*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)RTC_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.IR";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*创建目标任务*/
    uwRet = LOS_TaskCreate(&g_RTCTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Task create Failed!\r\n");
        return LOS_NOK;
    }
    /*创建成功后，挂起自身*/
    printf("Task create Success!\r\n");
    
    /*解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务*/
    LOS_TaskUnlock();
    
    /*挂起自身任务*/
    uwRet = LOS_TaskSuspend(g_RTCTskID);
    if (uwRet != LOS_OK)
    {
        printf("Suspend Task Failed.\r\n");
        return LOS_NOK;
    }
}
