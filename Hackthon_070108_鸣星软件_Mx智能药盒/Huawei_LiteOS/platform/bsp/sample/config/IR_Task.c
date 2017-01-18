#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#include "los_task.h"

typedef UINT32 TSK_HANDLE_T;
typedef UINT8  retIR_t;


#define OS_TSK_DETACHED 0

#define PEOPLE_COME     0x01
#define PEOPLE_GONE     0x02


UINT32      g_IrTskID;
  
  
extern EVENT_CB_S  mx_event;

retIR_t GetIrResult()
{
    UINT32 uwRet;
    
    return uwRet;    
}


UINT32 IR_Task()
{
    UINT32      uwRet;
    UINT32      uwEvent;
    retIR_t     irRet;

    printf("Enter Task Handler.\r\n");
    
    for(;;)
    {
      uwEvent = LOS_EventRead(&mx_event, OPEN_MOTOR, LOS_WAITMODE_AND, 100);
      if(uwEvent == OPEN_MOTOR)
      {
        /*
        irRet = GetIrResult();
        if (PEOPLE_COME == irRet)
        {
          irRet = PEOPLE_GONE;
          uwRet = LOS_EventWrite(&mx_event, PEOPLE_COME);
          if(uwRet != LOS_OK)
          {
              printf("event write failed .\n");
              return LOS_NOK;
          }
          if (PEOPLE_GONE == irRet)
          {
            irRet = PEOPLE_COME;
            uwRet = LOS_EventWrite(&mx_event, PEOPLE_GONE);
            if(uwRet != LOS_OK)
            {
                printf("event write failed .\n");
                return LOS_NOK;
            } 
          }
        }
        */
        
        LOS_TaskDelay(300);
        LOS_EventClear(&mx_event, ~mx_event.uwEventID);        
      }
      
      uwEvent = LOS_EventRead(&mx_event, CLOSE_MOTOR, LOS_WAITMODE_AND, 100);
      if(uwEvent == CLOSE_MOTOR)
      {
        uwRet = LOS_TaskSuspend(g_IrTskID);
        if (uwRet != LOS_OK)
        {
            printf("Suspend Task Failed.\r\n");
            return LOS_NOK;
        }
        LOS_EventClear(&mx_event, ~mx_event.uwEventID);        
      }
   
    }
}

/* IR������ں��� */
UINT32 IR_TaskEntry(VOID)
{
    UINT32 uwRet;
    TSK_INIT_PARAM_S stInitParam;
    
    /*���������*/
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\r\n");

    stInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)IR_Task;
    stInitParam.usTaskPrio = 4;
    stInitParam.pcName = "MX.IR";
    stInitParam.uwStackSize = 0x400;
    stInitParam.uwResved   = OS_TSK_DETACHED;

    /*����Ŀ������*/
    uwRet = LOS_TaskCreate(&g_IrTskID, &stInitParam);
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
    
    /*������������*/
    uwRet = LOS_TaskSuspend(g_IrTskID);
    if (uwRet != LOS_OK)
    {
        printf("Suspend Task Failed.\r\n");
        return LOS_NOK;
    }
}
