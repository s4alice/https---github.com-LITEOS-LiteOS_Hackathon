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
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//ʹ��USART1ʱ��
  
  //����1��Ӧ���Ÿ���ӳ��
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9����ΪUSART1
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10����ΪUSART1
  
  //USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
  GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9��PA10
  
  //USART1 ��ʼ������
  USART_InitStructure.USART_BaudRate = bound;//����������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  
  USART_Cmd(USART1, ENABLE);//ʹ�ܴ���1 
  
  //USART_ClearFlag(USART1, USART_FLAG_TC);
  
#if 0
  NVIC_InitTypeDef NVIC_InitStructure;
    
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;//�����ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
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
