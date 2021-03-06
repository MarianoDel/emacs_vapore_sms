//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.H #########################
//------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEST_FUNTIONS_H_
#define _TEST_FUNTIONS_H_



// Exported Types --------------------------------------------------------------


// Exported Constants or Macros ------------------------------------------------


// Exported Functions ----------------------------------------------------------
void TF_Led (void);
void TF_Led_Alarm_Input (void);
void TF_Led_Alarm_Input_Filtered (void);
void TF_Alarm_Input_As_Output (void);
void TF_Act_12V (void);
void TF_Led_Blinking (void);
void TF_Usart2_TxRx (void);
void TF_Usart2_NetLight_and_Status (void);
void TF_Usart2_Adc_Dma (void);
void TF_Usart2_Flash_Empty_Page (void);
void TF_Usart2_Flash_Write_Data (void);

void TF_Gpio_Input (void);
void TF_Gpio_Share_Outputs (void);
void TF_Usart1_Single (void);
void TF_Usart1_Multiple (void);
void TF_Usart1_TxRx (void);
void TF_Usart1_Adc (void);
void TF_Usart1_Adc_Int (void);
void TF_Usart1_Adc_Dma (void);
void TF_Usart2_Single (void);
void TF_Usart2_Multiple (void);
void TF_Tim3_Pwm (void);
void TF_Usart1_Flash_Empty_Page (void);
void TF_Usart1_Flash_Write_Data (void);
void TF_ReadMemory (void);

#endif    /* _TEST_FUNTIONS_H_ */

//--- end of file ---//

