#include "los_sys.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"
#include "user_function.h"

#define LED1_RCC           RCC_AHB1Periph_GPIOA
#define LED1_GPIO          GPIOA
#define LED1_GPIO_PIN      GPIO_Pin_4
#define LED1_ONOFF(x)      GPIO_WriteBit(GPIOA,GPIO_Pin_4,x);

#define LED2_RCC           RCC_AHB1Periph_GPIOA
#define LED2_GPIO          GPIOA
#define LED2_GPIO_PIN      GPIO_Pin_6
#define LED2_ONOFF(x)      GPIO_WriteBit(GPIOA,GPIO_Pin_6,x);

#define LED3_RCC           RCC_AHB1Periph_GPIOB
#define LED3_GPIO          GPIOB
#define LED3_GPIO_PIN      GPIO_Pin_6
#define LED3_ONOFF(x)      GPIO_WriteBit(GPIOB,GPIO_Pin_6,x);

#define LED4_RCC           RCC_AHB1Periph_GPIOC
#define LED4_GPIO          GPIOC
#define LED4_GPIO_PIN      GPIO_Pin_6
#define LED4_ONOFF(x)      GPIO_WriteBit(GPIOC,GPIO_Pin_6,x);



typedef struct{
    uint32_t     rcc;
    GPIO_TypeDef *gpio;
    uint16_t     pin;
}Gpio_Info;


void DelayMs(UINT16 ms)
{
  for(; ms--; )
  {
    //UINT16 nCount = 0xa53;
    UINT16 nCount = 0xa60;
    for(; nCount--; );  
  }
}

void Delay10Us(UINT16 us)
{  
  for(; us--; )
  {
    UINT8 nCount = 0x16;
    for(; nCount--; );  
  }
}

void LEDGpio_Init(void)
{
  Gpio_Info Gpio_info[4]={
      {LED1_RCC, LED1_GPIO, LED1_GPIO_PIN},
      {LED2_RCC, LED2_GPIO, LED2_GPIO_PIN},
      {LED3_RCC, LED3_GPIO, LED3_GPIO_PIN},
      {LED4_RCC, LED4_GPIO, LED4_GPIO_PIN}
    };
  
    uint8_t i;
    GPIO_InitTypeDef  GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    for(i=0;i<4;i++)
    {
        RCC_AHB1PeriphClockCmd( Gpio_info[i].rcc , ENABLE);
        GPIO_InitStructure.GPIO_Pin = Gpio_info[i].pin;
        GPIO_Init(Gpio_info[i].gpio, &GPIO_InitStructure);
    }
}

void AllLedLow(void)
{
  LED1_ONOFF(Bit_SET);
  LED2_ONOFF(Bit_SET);
  LED3_ONOFF(Bit_SET);
  LED4_ONOFF(Bit_SET);
}

void CW_MotorTurning(UINT16 pat)
{
  for(UINT16 j = 0; j < pat; j++)
  {
      for(UINT8 i = 0; i < 8; i++)      
      {
        /* 0x08 */
        LED4_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x0c */
        LED3_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x04*/
        LED4_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x06 */
        LED2_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x02 */
        LED2_ONOFF(Bit_RESET);
        LED3_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x03 */
        LED1_ONOFF(Bit_RESET);
        DelayMs(1);
        
        /* 0x01 */
        LED1_ONOFF(Bit_RESET);
        LED2_ONOFF(Bit_SET);
        DelayMs(1);
        
        /* 0x09 */
        LED4_ONOFF(Bit_RESET);
        DelayMs(1);
        AllLedLow();
      }      
  }
}

void CCW_MotorTurning(UINT16 pat)
{
  for(UINT16 j = 0; j < pat; j++)
  {  
    for(UINT8 i = 0; i < 8; i++)      
    {
      /* 0x09 */
      LED1_ONOFF(Bit_RESET);
      LED4_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x01 */
      LED4_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x03 */
      LED2_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x02 */
      LED1_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x06 */
      LED3_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x04*/
      LED2_ONOFF(Bit_SET);
      DelayMs(1);
      
      /* 0x0c */
      LED4_ONOFF(Bit_RESET);
      DelayMs(1);
      
      /* 0x08 */
      LED3_ONOFF(Bit_SET);
      DelayMs(1);
      AllLedLow();
    }
  }
}


void Uart_init(u32 bound)
{
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟
  
  //串口1对应引脚复用映射
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9复用为USART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1
  
  //USART1端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9，PA10
  
  //USART1 初始化设置
  USART_InitStructure.USART_BaudRate = bound;//波特率设置
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  
  USART_Cmd(USART1, ENABLE);//使能串口1 
  
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
#if 0
  NVIC_InitTypeDef NVIC_InitStructure;
    
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启相关中断
  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);//根据指定的参数初始化VIC寄存器、
#endif
}



void USART1_IRQHandler(void)
{
  unsigned char ch;
  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    ch =(USART_ReceiveData(USART1));
    USART_SendData(USART1, ch);
    printf("in[%c].\r\n",ch);
  }
}










////////////////////////////User Test MAIN//////////////////////
void Test_main(void)
{
    LEDGpio_Init();
    AllLedLow();
    
    Uart_init(115200);    
    uint16_t a = 'A';
    for(;;)
    {
      if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
      {
        /* Read one byte from the receive data register */
        a =(USART_ReceiveData(USART1));
        switch(a)
          {
          case ('a'):
            CW_MotorTurning(16); break;
          case ('b'):
            CCW_MotorTurning(16); break;
          default:
            break;
        }
        USART_SendData(USART1, a);
        printf("in[%c].\r\n",a);
      }
    }
  
  while(1)
  {    
    CW_MotorTurning(32);
    DelayMs(500);
    CCW_MotorTurning(32);
    DelayMs(500);
  }
}
