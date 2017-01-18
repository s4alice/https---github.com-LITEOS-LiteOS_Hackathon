#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"

typedef UINT32 TSK_HANDLE_T;

TSK_HANDLE_T g_uwTskHiID;
#define TSK_PRIOR_HI 4
#define OS_TSK_DETACHED 0

UINT32 Example_Task()
{
    UINT32      uwRet;
    UINT32      uwCurrentID;
    TSK_INFO_S  stTaskInfo;

     printf("Enter Task Handler.\r\n");

    /*延时2个Tick，延时后该任务会挂起，执行剩余任务*/
    uwRet = LOS_TaskDelay(2);
    if (uwRet != LOS_OK)
    {
        printf("Delay Task Failed.\r\n");
        return LOS_NOK;
    }

    /*2个tick时间到了后，该任务恢复，继续执行*/
    printf("Task LOS_TaskDelay Done.\r\n");

    /*挂起自身任务*/
    uwRet = LOS_TaskSuspend(g_uwTskHiID);
    if (uwRet != LOS_OK)
    {
        printf("Suspend Task Failed.\r\n");
        return LOS_NOK;
    }
    printf("Task LOS_TaskResume Success.\r\n");
}

/*任务入口函数，在里面创建任务*/
UINT32 user_myhello(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_Task;
    stInitParam.usTaskPrio = TSK_PRIOR_HI;
    stInitParam.pcName = "MR.wang";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*创建任务*/
    uwRet = LOS_TaskCreate(&g_uwTskHiID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Task create Failed!\r\n");
        return LOS_NOK;
    }
    /*创建成功后打印 hello world*/
    printf("Task create Success!\r\n");
    printf("hello world!\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */