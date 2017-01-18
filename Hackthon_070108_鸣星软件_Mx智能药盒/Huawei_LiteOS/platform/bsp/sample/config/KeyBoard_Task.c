#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"

typedef UINT32 TSK_HANDLE_T;



#define OS_TSK_DETACHED 0



UINT32      g_KeyBoardTskID;
  
  
extern EVENT_CB_S  mx_event;



UINT32 KeyBoard_Task()
{
    UINT32      uwRet;

    printf("Enter Task Handler.\r\n");
    
    for(;;)
    {
    }
}

UINT32 KeyBoard_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    /*���������*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)KeyBoard_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.IR";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*����Ŀ������*/
    uwRet = LOS_TaskCreate(&g_KeyBoardTskID, &stInitParam);
    if (uwRet != LOS_OK)
    {
     //   LOS_TaskUnlock();

        printf("Task create Failed!\r\n");
        return LOS_NOK;
    }
    /*�����ɹ��󣬹�������*/
    printf("Task create Success!\r\n");
    
    /*����������ȣ���ʱ�ᷢ��������ȣ�ִ�о����б���������ȼ�����*/
    LOS_TaskUnlock();
    
    return LOS_OK;    
}
