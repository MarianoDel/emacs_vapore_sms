//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### GPIO.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32g0xx.h"
#include "gpio.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
// - Port enables
#define GPIOA_ENABLE
#define GPIOB_ENABLE
//#define GPIOC_ENABLE

// - Ports Clocks
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


//-------------------------------------------//
// @brief  GPIO configure.
// @param  None
// @retval None
//------------------------------------------//
void GPIO_Config (void)
{
    //--- MODER ---//
    //00: Input mode 
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode (reset state)

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved

#ifdef HARDWARE_VER_2_0
    unsigned long temp;

#ifdef GPIOA_ENABLE    
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0xFC030003;    //PA1 analog; PA2 - PA3 alternate; PA4 input; PA5 - PA6 alternative; PA7 output
    temp |= 0x016868AC;    //PA9 - PA10 alternative; PA11 - PA12 output
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;       
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFC3303CF;
    temp |= 0x00000000;		// PA2, PA5 - PA7, PA9, PA11, PA12 low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOA->PUPDR = temp;

#endif    // GPIOA_ENABLE
    
#ifdef GPIOB_ENABLE    
    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;        //2 bits por pin
    temp &= 0xFFFFCFC0;        // PB0 input; PB1 output; PB2 analog; PB6 input
    temp |= 0x00000034;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFD;        // PB1 open drain
    temp |= 0x00000002;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFCFFF;        //PB6 pulldwn
    temp |= 0x00002000;
    GPIOB->PUPDR = temp;

#endif    // GPIOB_ENABLE
#endif    //#ifdef HARDWARE_VER_2_0


#ifdef HARDWARE_VER_1_2
    unsigned long temp;

#ifdef GPIOA_ENABLE    
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0xFC033F03;    //PA1 analog; PA2 - PA3 alternate; PA7 output
    temp |= 0x016840AC;    //PA9 - PA10 alternative; PA11 - PA12 output
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFF7F;
    temp |= 0x00000080;        //PA7 open drain
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFC033F0F;
    temp |= 0x00000000;		//low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOA->PUPDR = temp;

#endif    // GPIOA_ENABLE
    

#ifdef GPIOB_ENABLE    
    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;        //2 bits por pin
    temp &= 0xFFFFC0CF;        //PB2 analog; PB4 - PB6 input
    temp |= 0x00000030;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFCFFF;        //PB6 pulldwn
    temp |= 0x00002000;
    GPIOB->PUPDR = temp;

#endif    // GPIOB_ENABLE
#endif    //#ifdef HARDWARE_VER_1_2
    
#if (defined HARDWARE_VER_1_0) || (defined HARDWARE_VER_1_1)
    unsigned long temp;
    
    //--- GPIO A ---//
#ifdef GPIOA_ENABLE
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;    //2 bits por pin
#ifdef WITH_PA1_V4V_SENSE    
    temp &= 0xFC033F03;    //PA1 analog; PA2 - PA3 alternate; PA7 output
    temp |= 0x016840AC;    //PA9 - PA10 alternative; PA11 - PA12 output
#endif
#ifdef WITH_PA1_TEST1_INPUT
    temp &= 0xFC033F03;    //PA1 input; PA2 - PA3 alternate; PA7 output
    temp |= 0x016840A0;    //PA9 - PA10 alternative; PA11 - PA12 output
#endif
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFF7F;
    temp |= 0x00000080;        //PA7 open drain
    GPIOA->OTYPER = temp;

    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFC033F0F;
    temp |= 0x00000000;		//low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
#ifdef WITH_PA1_V4V_SENSE    
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
#endif
#ifdef WITH_PA1_TEST1_INPUT
    temp &= 0xFFFFFFF3;    //PA1 pulldwn
    temp |= 0x00000008;
#endif
    GPIOA->PUPDR = temp;

#endif    // GPIOA_ENABLE
    

    //--- GPIO B ---//
#ifdef GPIOB_ENABLE
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;        //2 bits por pin
    temp &= 0xFFFFF0CF;        //PB2 analog; PB4 - PB5 input;
    temp |= 0x00000030;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->PUPDR = temp;

#endif    // GPIOB_ENABLE
#endif    //#if (defined HARDWARE_VER_1_0) || (defined HARDWARE_VER_1_1)    
    
    //--- GPIO C ---//
#ifdef GPIOC_ENABLE
    if (!GPIOC_CLK)
        GPIOC_CLK_ON;

    temp = GPIOC->MODER;	//2 bits por pin
    temp &= 0xFFFF0F3C;		//PB0 analog input; PB3 input PB6 PB7 alternative
    temp |= 0x0000A003;
    GPIOC->MODER = temp;

    temp = GPIOC->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOC->OTYPER = temp;

    temp = GPIOC->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFF0FFF;
    temp |= 0x00000000;		//low speed
    GPIOC->OSPEEDR = temp;

    temp = GPIOC->PUPDR;	//2 bits por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOC->PUPDR = temp;

#endif    // GPIOC_ENABLE
    

//	//Interrupt en PB8
//	if (!SYSCFG_CLK)
//		SYSCFG_CLK_ON;
//
//	SYSCFG->EXTICR[2] = 0x00000001; //Select Port B & Pin 8 external interrupt
//	EXTI->IMR |= 0x0100; 			//Corresponding mask bit for interrupts
//	EXTI->EMR |= 0x0000; 			//Corresponding mask bit for events
//	EXTI->RTSR |= 0x0100; 			//Pin 8 Interrupt line on rising edge
//	EXTI->FTSR |= 0x0100; 			//Pin 8 Interrupt line on falling edge
//
//	NVIC_EnableIRQ(EXTI4_15_IRQn);
//	NVIC_SetPriority(EXTI4_15_IRQn, 6);
}

inline void EXTIOff (void)
{
    // EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
    // EXTI->IMR |= 0x00000100;
}

//--- end of file ---//
