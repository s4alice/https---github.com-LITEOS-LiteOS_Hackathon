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

    /*��ʱ2��Tick����ʱ�����������ִ��ʣ������*/
    uwRet = LOS_TaskDelay(2);
    if (uwRet != LOS_OK)
    {
        printf("Delay Task Failed.\r\n");
        return LOS_NOK;
    }

    /*2��tickʱ�䵽�˺󣬸�����ָ�������ִ��*/
    printf("Task LOS_TaskDelay Done.\r\n");

    /*������������*/
    uwRet = LOS_TaskSuspend(g_uwTskHiID);
    if (uwRet != LOS_OK)
    {
        printf("Suspend Task Failed.\r\n");
        return LOS_NOK;
    }
    printf("Task LOS_TaskResume Success.\r\n");
}

/*������ں����������洴������*/
UINT32 user_myhello(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Example_Task;
    stInitParam.usTaskPrio = TSK_PRIOR_HI;
    stInitParam.pcName = "MR.wang";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*��������*/
    uwRet = LOS_TaskCreate(&g_uwTskHiID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Task create Failed!\r\n");
        return LOS_NOK;
    }
    /*�����ɹ����ӡ hello world*/
    printf("Task create Success!\r\n");
    printf("hello world!\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */