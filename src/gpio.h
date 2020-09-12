//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### GPIO.H ################################
//---------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _GPIO_H_
#define _GPIO_H_

//--- Exported types ---//
//--- Exported constants ---//
//--- Enable of Ports Pins ---//
#define GPIOB_ENABLE
//#define GPIOC_ENABLE

#define GPIOA_CLK (RCC->IOPENR & 0x00000001)
#define GPIOA_CLK_ON (RCC->IOPENR |= 0x00000001)
#define GPIOA_CLK_OFF (RCC->IOPENR &= ~0x00000001)

#define GPIOB_CLK (RCC->IOPENR & 0x00000002)
#define GPIOB_CLK_ON (RCC->IOPENR |= 0x00000002)
#define GPIOB_CLK_OFF (RCC->IOPENR &= ~0x00000002)

#define GPIOC_CLK (RCC->IOPENR & 0x00000004)
#define GPIOC_CLK_ON (RCC->IOPENR |= 0x00000004)
#define GPIOC_CLK_OFF (RCC->IOPENR &= ~0x00000004)

#define GPIOD_CLK (RCC->IOPENR & 0x00000008)
#define GPIOD_CLK_ON (RCC->IOPENR |= 0x00000008)
#define GPIOD_CLK_OFF (RCC->IOPENR &= ~0x00000008)

#define GPIOF_CLK (RCC->IOPENR & 0x00000020)
#define GPIOF_CLK_ON (RCC->IOPENR |= 0x00000020)
#define GPIOF_CLK_OFF (RCC->IOPENR &= ~0x00000020)

// #define SYSCFG_CLK (RCC->APB2ENR & 0x00000001)
// #define SYSCFG_CLK_ON RCC->APB2ENR |= 0x00000001
// #define SYSCFG_CLK_OFF RCC->APB2ENR &= ~0x00000001

//--- Exported functions ---//
void GPIO_Config(void);
void EXTIOff (void);
void EXTIOn (void);

#endif //--- End ---//


//--- END OF FILE ---//
