/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#include "los_sys.h"
#include "los_tick.h"
#include "los_task.ph"
#include "los_config.h"

  #include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#pragma data_alignment=8
  
#define UART_QUEUE_LEN    64  
  
UINT8 *m_aucSysMem0;
UINT32 g_sys_mem_addr_end = 0;
EVENT_CB_S  mx_event;
UINT32      g_uwMsgQueue;

extern UINT32 Uart_TaskEntry(VOID);
extern UINT32 Motor_TaskEntry(VOID);
extern UINT32 IR_TaskEntry(VOID);
extern UINT8 g_ucMemStart[];
extern UINT32 osTickInit(UINT32 uwSystemClock, UINT32 uwTickPerSecond);
extern UINT32   g_uwTskMaxNum;

extern void SystemInit();
void osEnableFPU(void)
{
  
    *(volatile UINT32 *)0xE000ED88 |= ((3UL << 10*2)|(3UL << 11*2));
    //SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));
}
/*****************************************************************************
 Function    : osRegister
 Description : Configuring the maximum number of tasks
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT VOID osRegister(VOID)
{
    g_uwTskMaxNum = LOSCFG_BASE_CORE_TSK_LIMIT + 1; /* Reserved 1 for IDLE */
    g_sys_mem_addr_end = (UINT32)g_ucMemStart + OS_SYS_MEM_SIZE;
    return;
}

/*****************************************************************************
 Function    : LOS_Start
 Description : Task start function
 Input       : None
 Output      : None
 Return      : LOS_OK
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT UINT32 LOS_Start()
{
    UINT32 uwRet;
#if (LOSCFG_BASE_CORE_TICK_HW_TIME == NO)
    uwRet = osTickStart();

    if (uwRet != LOS_OK)
    {
        PRINT_ERR("osTickStart error\n");
        return uwRet;
    }
#else
    os_timer_init();
#endif
    LOS_StartToRun();

    return uwRet;
}

/*****************************************************************************
 Function    : osMain
 Description : System kernel initialization function, configure all system modules
 Input       : None
 Output      : None
 Return      : LOS_OK
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT int osMain(void)
{
    UINT32 uwRet;

    osRegister();

    uwRet = osMemSystemInit();
    if (uwRet != LOS_OK)
    {
        PRINT_ERR("osMemSystemInit error %d\n", uwRet);
        return uwRet;
    }

#if (LOSCFG_PLATFORM_HWI == YES)
    {
        osHwiInit();
    }
#endif

    uwRet =osTaskInit();
    if (uwRet != LOS_OK)
    {
        PRINT_ERR("osTaskInit error\n");
        return uwRet;
    }

#if (LOSCFG_BASE_IPC_SEM == YES)
    {
        uwRet = osSemInit();
        if (uwRet != LOS_OK)
        {
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_IPC_MUX == YES)
    {
        uwRet = osMuxInit();
        if (uwRet != LOS_OK)
        {
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_IPC_QUEUE == YES)
    {
        uwRet = osQueueInit();
        if (uwRet != LOS_OK)
        {
            PRINT_ERR("osQueueInit error\n");
            return uwRet;
        }
    }
#endif

#if (LOSCFG_BASE_CORE_SWTMR == YES)
    {
        uwRet = osSwTmrInit();
        if (uwRet != LOS_OK)
        {
            PRINT_ERR("osSwTmrInit error\n");
            return uwRet;
        }
    }
#endif

    #if(LOSCFG_BASE_CORE_TIMESLICE == YES)
    osTimesliceInit();
    #endif

    uwRet = osIdleTaskCreate();
    if (uwRet != LOS_OK) {
        return uwRet;
    }

    return LOS_OK;
}


void Clock_Config(void){ 
 
 ErrorStatus  State; 
 uint32_t   PLL_M;  
 uint32_t   PLL_N; 
 uint32_t   PLL_P; 
 uint32_t   PLL_Q; 
 
 /*配置前将所有RCC重置为初始值*/ 
 RCC_DeInit(); 
 
 /*这里选择 外部晶振（HSE）作为 时钟源，因此首先打开外部晶振*/ 
 RCC_HSEConfig(RCC_HSE_ON); 
 /*等待外部晶振进入稳定状态*/ 
 while( RCC_WaitForHSEStartUp() != SUCCESS ); 
 
 /* 
 **我们要选择PLL时钟作为系统时钟，因此这里先要对PLL时钟进行配置 
 */ 
 
 /*选择外部晶振作为PLL的时钟源*/ 
 
 /* 到这一步为止，已有 HSE_VALUE = 8 MHz. 
  PLL_VCO input clock = (HSE_VALUE or HSI_VALUE / PLL_M)， 
  根据文档，这个值被建议在 1~2MHz，因此我们令 PLL_M = 8， 
  即 PLL_VCO input clock = 1MHz */ 
 PLL_M  =  4; 
 
 /* 到这一步为止，已有 PLL_VCO input clock = 1 MHz. 
  PLL_VCO output clock = (PLL_VCO input clock) * PLL_N, 
  这个值要用来计算系统时钟，我们 令 PLL_N = 336, 
  即 PLL_VCO output clock = 336 MHz.*/  
 PLL_N  =  100; 
 
 /* 到这一步为止，已有 PLL_VCO output clock = 336 MHz. 
  System Clock = (PLL_VCO output clock)/PLL_P , 
  因为我们要 SystemClock = 168 Mhz，因此令 PLL_P = 2. 
  */ 
 PLL_P   = 2; 
 
 /*这个系数用来配置SD卡读写，USB等功能，暂时不用，根据文档，暂时先设为7*/ 
 PLL_Q   =  4; 
 
 /* 配置PLL并将其使能，获得 168Mhz 的 System Clock 时钟*/ 
 RCC_PLLConfig(RCC_PLLSource_HSE, PLL_M, PLL_N, PLL_P, PLL_Q); 
 RCC_PLLCmd(ENABLE); 
 
 /*到了这一步，我们已经配置好了PLL时钟。下面我们配置Syetem Clock*/ 
 /*选择PLL时钟作为系统时钟源*/ 
 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); 
 
 
 
 
/*到了这一步，我们已经配置好了系统时钟，频率为 168MHz. 下面我们可以对 AHB，APB，外设等的 时钟进行配置*/ 
 /*时钟的结构请参考用户手册*/ 
 
 /*首先配置 AHB时钟（HCLK）. 为了获得较高的频率，我们对 SYSCLK 1分频，得到HCLK*/ 
 RCC_HCLKConfig(RCC_HCLK_Div1); 
 
 /*APBx时钟（PCLK）由AHB时钟（HCLK）分频得到，下面我们配置 PCLK*/ 
 
 /*APB1时钟配置. 4分频，即 PCLK1 = 42 MHz*/ 
 RCC_PCLK1Config(RCC_HCLK_Div4); 
 
 /*APB2时钟配置. 2分频，即 PCLK2 = 84 MHz*/ 
 RCC_PCLK2Config(RCC_HCLK_Div2); 
 
/*****函数结束******/ 
}


extern void WiFi_UartConfig();
/*****************************************************************************
 Function    : main
 Description : Main function entry
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT
 int main(void)
{
    UINT32 uwRet;    
    
    uwRet = osMain();    
    if (uwRet != LOS_OK) {
        return LOS_NOK;
    }
    
    uwRet = LOS_EventInit(&mx_event);
    if(uwRet != LOS_OK)
    {
        printf("Init event failed .\n");
        return -1;
    }
        
    uwRet = LOS_QueueCreate("queue", UART_QUEUE_LEN, &g_uwMsgQueue, 0, 1);
    if(uwRet != LOS_OK)
    {
      printf("create queue failure!, error:%x\n",uwRet);
    }

    Uart_TaskEntry();
    Manager_TaskEntry();
    Motor_TaskEntry();
    //IR_TaskEntry();
    //RTC_TaskEntry();
    //WiFi_TaskEntry();
    //KeyBoard_TaskEntry();
    Led_TaskEntry();
    

    LOS_Start();

    for (;;);
    /* Replace the dots (...) with your own code.  */
}

void osBackTrace(){}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
