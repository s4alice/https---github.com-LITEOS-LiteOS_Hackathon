#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"

typedef UINT32 TSK_HANDLE_T;

TSK_HANDLE_T g_mxTskHiID;
#define TSK_PRIOR_HI 4
#define OS_TSK_DETACHED 0

UINT32 Mx_Task()
{
    UINT32      uwRet;
    UINT32      uwCurrentID;
    TSK_INFO_S  stTaskInfo;

    printf("Enter Mx_Task Handler.\r\n");

    /*��ʱ2��Tick����ʱ�����������ִ��ʣ������*/
    uwRet = LOS_TaskDelay(2);
    if (uwRet != LOS_OK)
    {
        printf("Delay Mx_Task Failed.\r\n");
        return LOS_NOK;
    }

    /*2��tickʱ�䵽�˺󣬸�����ָ�������ִ��*/
    printf("Mx_Task LOS_TaskDelay Done.\r\n");

    /*������������*/
    uwRet = LOS_TaskSuspend(g_mxTskHiID);
    if (uwRet != LOS_OK)
    {
        printf("Suspend Mx_Task Failed.\r\n");
        return LOS_NOK;
    }
    printf("Mx_Task LOS_TaskResume Success.\r\n");
}

/*������ں����������洴������*/
UINT32 Mx_Hello(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    LEDGpio_Init();
    AllLedLow();    
    //Uart_init(115200);

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)Mx_Task;
    stInitParam.usTaskPrio = TSK_PRIOR_HI;
    stInitParam.pcName = "MX.Info";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;
    
    /*��������*/
    uwRet = LOS_TaskCreate(&g_mxTskHiID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Mx_Task create Failed!\r\n");
        return LOS_NOK;
    }
    /*�����ɹ����ӡ hello world*/
    printf("Mx_Task create Success!\r\n");
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */