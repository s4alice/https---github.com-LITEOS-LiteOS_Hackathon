#ifndef _USER_FUNCTION_H
#define _USER_FUNCTION_H



extern void DelayMs(UINT16 ms);
extern void Delay10Us(UINT16 us);
extern void LEDGpio_Init(void);
extern void AllLedLow(void);
extern void Uart_init(u32 bound);
extern void CW_MotorTurning(UINT16 pat);
extern void CCW_MotorTurning(UINT16 pat);


#endif
