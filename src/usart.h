//--------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### USART.H ##################
//--------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H_
#define __USART_H_

//--- Exported types ---//
//--- Exported constants ---//
//--- Exported macro ---//
#define USART1_CLK    (RCC->APBENR2 & 0x00004000)
#define USART1_CLK_ON    (RCC->APBENR2 |= 0x00004000)
#define USART1_CLK_OFF    (RCC->APBENR2 &= ~0x00004000)

#define USART2_CLK    (RCC->APBENR1 & 0x00020000)
#define USART2_CLK_ON    (RCC->APBENR1 |= 0x00020000)
#define USART2_CLK_OFF    (RCC->APBENR1 &= ~0x00020000)

#define USART_64MHz_9600    6666
#define USART_16MHz_9600    1666
#define USART_115200		416
#define USART_250000		192

#define USARTx_RX_DISA	USARTx->CR1 &= 0xfffffffb
#define USARTx_RX_ENA	USARTx->CR1 |= 0x04

#define TT_GSM_MINI		5

#define SIZEOF_DATA 256
// #define SIZEOF_TXDATA 128

// Module Exported Functions ---------------------------------------------------
void Usart1Config(void);
void Usart1SendSingle (unsigned char);
void Usart1Send (char *);
void Usart1SendUnsigned(unsigned char *, unsigned char);
unsigned char Usart1ReadBuffer (unsigned char *, unsigned short);
void USART1_IRQHandler(void);
unsigned char Usart1HaveData (void);
void Usart1HaveDataReset (void);

void Usart2Config(void);
void Usart2SendSingle (unsigned char);
void Usart2Send (char *);
void Usart2SendUnsigned(unsigned char *, unsigned char);
unsigned char Usart2ReadBuffer (unsigned char *, unsigned short);
void USART2_IRQHandler(void);
unsigned char Usart2HaveData (void);
void Usart2HaveDataReset (void);

void USART_Timeouts (void);

#endif /* __USART.H */


//--- END OF FILE ---//
