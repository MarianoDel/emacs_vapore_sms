//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32G030
// ##
// #### TIM.H ################################
//---------------------------------------------

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TIM_H
#define __TIM_H

//--- Exported types ---//

//--- Exported constants ---//

//--- Exported macro ---//
#define RCC_TIM1_CLK    (RCC->APBENR2 & 0x00000800)
#define RCC_TIM1_CLK_ON    (RCC->APBENR2 |= 0x00000800)
#define RCC_TIM1_CLK_OFF    (RCC->APBENR2 &= ~0x00000800)

#define RCC_TIM3_CLK    (RCC->APBENR1 & 0x00000002)
#define RCC_TIM3_CLK_ON    (RCC->APBENR1 |= 0x00000002)
#define RCC_TIM3_CLK_OFF    (RCC->APBENR1 &= ~0x00000002)

#define RCC_TIM6_CLK    (RCC->APBENR1 & 0x00000010)
#define RCC_TIM6_CLK_ON    (RCC->APBENR1 |= 0x00000010)
#define RCC_TIM6_CLK_OFF    (RCC->APBENR1 &= ~0x00000010)

#define RCC_TIM7_CLK    (RCC->APBENR1 & 0x00000020)
#define RCC_TIM7_CLK_ON    (RCC->APBENR1 |= 0x00000020)
#define RCC_TIM7_CLK_OFF    (RCC->APBENR1 &= ~0x00000020)

#define RCC_TIM14_CLK    (RCC->APBENR2 & 0x00008000)
#define RCC_TIM14_CLK_ON    (RCC->APBENR2 |= 0x00008000)
#define RCC_TIM14_CLK_OFF    (RCC->APBENR2 &= ~0x00008000)

#define RCC_TIM15_CLK    (RCC->APBENR2 & 0x00010000)
#define RCC_TIM15_CLK_ON    (RCC->APBENR2 |= 0x00010000)
#define RCC_TIM15_CLK_OFF    (RCC->APBENR2 &= ~0x00010000)

#define RCC_TIM16_CLK    (RCC->APBENR2 & 0x00020000)
#define RCC_TIM16_CLK_ON    (RCC->APBENR2 |= 0x00020000)
#define RCC_TIM16_CLK_OFF    (RCC->APBENR2 &= ~0x00020000)

#define RCC_TIM17_CLK    (RCC->APBENR2 & 0x00040000)
#define RCC_TIM17_CLK_ON    (RCC->APBENR2 |= 0x00040000)
#define RCC_TIM17_CLK_OFF    (RCC->APBENR2 &= ~0x00040000)

// Module Exported Functions ---------------------------------------------------
// void TIM3_IRQHandler (void);
void TIM_3_Init(void);
void TIM_6_Init (void);
void TIM14_IRQHandler (void);
void TIM_14_Init(void);
void TIM16_IRQHandler (void);
void TIM_16_Init(void);
void TIM17_IRQHandler (void);
void TIM_17_Init(void);
void Update_TIM3_CH1 (unsigned short);
void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);

void OneShootTIM16 (unsigned short);
void TIM16Enable (void);
void TIM16Disable (void);

void Wait_ms (unsigned short);
void TIM_Timeouts (void);


#endif
//--- End ---//


//--- END OF FILE ---//
