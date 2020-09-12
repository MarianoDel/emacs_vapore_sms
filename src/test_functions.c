//------------------------------------------------
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C #########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "stm32g0xx.h"
#include "hard.h"
#include "tim.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "flash_program.h"

#include <stdio.h>
#include <string.h>



// Externals -------------------------------------------------------------------
extern volatile unsigned char usart1_have_data;
extern volatile unsigned char usart2_have_data;
extern volatile unsigned char adc_int_seq_ready;
extern volatile unsigned short adc_ch [];

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TF_Led (void)
{
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms(300);
    }
}


// void TF_Gpio_Input (void)
// {
//     while (1)
//     {
//         if (PIN2_IN)
//             PIN1_ON;
//         else
//             PIN1_OFF;

//         Wait_ms(100);
//     }
// }


// //PB5 & PB6 are in the same pin
// void TF_Gpio_Share_Outputs (void)
// {
//     while (1)
//     {
//         if (LED)
//         {
//             PB5_PB6_ON;
//             LED_OFF;
//         }
//         else
//         {
//             PB5_PB6_OFF;
//             LED_ON;
//         }

//         Wait_ms(300);
//     }
// }


// void TF_Usart1_Single (void)
// {
//     Usart1Config();

//     while (1)
//     {
//         Usart1SendSingle('M');
//         Wait_ms(1000);

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;
//     }
// }


// void TF_Usart1_Multiple (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();

//     while (1)
//     {
//         Usart1Send("Mariano\n");
//         Wait_ms(1000);

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;
//     }
// }


// void TF_Usart1_TxRx (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();

//     char s_to_send [100] = { 0 };
//     Usart1Send("Ready to test...\n");
//     while (1)
//     {
//         if (usart1_have_data)
//         {
//             usart1_have_data = 0;
            
//             if (LED)
//                 LED_OFF;
//             else
//                 LED_ON;
            
//             Usart1ReadBuffer((unsigned char *) s_to_send, 100);
//             Wait_ms(1000);
//             Usart1Send(s_to_send);
//         }
//     }
// }


// void TF_Usart1_Adc (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();
//     AdcConfig();

//     unsigned short adc_sample = 0;
//     unsigned char cntr = 0;
//     char s_to_send [100] = { 0 };
//     Usart1Send("\nTesting ADC...\n");
//     while (1)
//     {
//         Wait_ms(1000);
//         adc_sample = ReadADC1_SameSampleTime (ADC_Channel_8);
//         sprintf(s_to_send, "index: %d sample: %d\n", cntr, adc_sample);
//         Usart1Send(s_to_send);
//         cntr++;

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//     }
// }


// void TF_Usart1_Adc_Int (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();
//     AdcConfig();

//     unsigned short cntr = 0;
//     char s_to_send [100] = { 0 };
//     Usart1Send("\nTesting ADC with ints...\n");

//     // Start the conversion
//     ADC1->CR |= (uint32_t)ADC_CR_ADSTART;

//     while (1)
//     {
//         if (adc_int_seq_ready)
//         {
//             adc_int_seq_ready = 0;
//             if (cntr < 10000)
//                 cntr++;
//             else
//             {
//                 sprintf(s_to_send, "index: %d sample: %d\n", cntr, adc_ch[0]);
//                 Usart1Send(s_to_send);
//                 cntr = 0;
//             }
//         }
//     }
// }


// void TF_Usart1_Adc_Dma (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();

//     //-- ADC Init
//     AdcConfig();

//     //-- DMA configuration and Init
//     DMAConfig();
//     DMA1_Channel1->CCR |= DMA_CCR_EN;

//     ADC1->CR |= ADC_CR_ADSTART;


//     unsigned short cntr = 0;
//     char s_to_send [100] = { 0 };
//     Usart1Send("\nTesting ADC with dma transfers...\n");
//     // DMAEnableInterrupt();

//     while (1)
//     {
//         if (sequence_ready)
//         {
//             sequence_ready_reset;
//             if (cntr < 10000)
//                 cntr++;
//             else
//             {
//                 sprintf(s_to_send, "index: %d sample: %d\n", cntr, adc_ch[0]);
//                 Usart1Send(s_to_send);
//                 cntr = 0;
//             }
//         }            
//     }
// }


// void TF_Usart2_Single (void)
// {
//     Usart2Config();

//     while (1)
//     {
//         Usart2SendSingle('M');
//         Wait_ms(1000);

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;
//     }
// }


// void TF_Usart2_Multiple (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart2Config();

//     while (1)
//     {
//         Usart2Send("Mariano\n");
//         Wait_ms(1000);

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;
//     }
// }


// void TF_Tim3_Pwm (void)
// {
//     TIM_3_Init();

//     while (1)
//     {
//         for (unsigned short i = 0; i < 1000; i++)
//         {
//             Update_TIM3_CH3 (i);
//             Wait_ms(2);            
//         }

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//         for (unsigned short i = 1000; i > 0; i--)
//         {
//             Update_TIM3_CH3 (i);
//             Wait_ms(2);            
//         }

//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;
        
//     }
// }


// void TF_Usart1_Flash_Empty_Page (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();

//     char s_to_send [100] = { 0 };
//     unsigned char * p;
//     p = (unsigned char *) PAGE15_ADDR;
    
//     Usart1Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart1Send(s_to_send);
//         Wait_ms(20);
//     }

//     Usart1Send("\nBlanking flash...\n");
//     Wait_ms(500);
//     if (Flash_ErasePage(PAGE15, 1) == FLASH_COMPLETE)
//     {
//         Usart1Send("Blank OK\n");
//         Wait_ms(100);
//     }
//     else
//     {
//         Usart1Send("Blank NOK\n");
//         Wait_ms(100);
//     }

//     Usart1Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart1Send(s_to_send);
//         Wait_ms(20);
//     }
    
//     while (1)
//     {
//         Wait_ms(300);
//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//     }
// }


// void TF_Usart1_Flash_Write_Data (void)
// {
//     for (unsigned char i = 0; i < 5; i++)
//     {
//         LED_ON;
//         Wait_ms(250);
//         LED_OFF;
//         Wait_ms(250);
//     }
    
//     Usart1Config();

//     char s_to_send [100] = { 0 };
//     unsigned char * p;
//     p = (unsigned char *) PAGE15_ADDR;
    
//     Usart1Send("\nReading Flash Data...\n");

//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart1Send(s_to_send);
//         Wait_ms(20);
//     }

//     //write mem conf
//     struct mem_conf_st {
//         uint32_t d0;
//         uint32_t d1;
//         uint32_t d2;
//         uint32_t d3;
//     };

//     struct mem_conf_st mem_conf;
//     mem_conf.d0 = 0x5555;
//     mem_conf.d1 = 0xAAAA;
//     mem_conf.d2 = 0x0000;
//     mem_conf.d3 = 0x7777;

//     Usart1Send("\nWriting Flash...\n");
//     Wait_ms(300);
//     if (Flash_WriteConfigurations((uint32_t *) &mem_conf, sizeof(mem_conf)) == FLASH_COMPLETE)
//         Usart1Send("Seems all good\n");

//     Wait_ms(300);
//     for (unsigned char i = 0; i < 64; i+=8)
//     {
//         sprintf(s_to_send, "0x%x %x %x %x %x %x %x %x %x\n",
//                 (unsigned int) (p + i),
//                 *(p + i + 0),
//                 *(p + i + 1),
//                 *(p + i + 2),
//                 *(p + i + 3),
//                 *(p + i + 4),
//                 *(p + i + 5),
//                 *(p + i + 6),
//                 *(p + i + 7));
        
//         Usart1Send(s_to_send);
//         Wait_ms(20);
//     }

//     Wait_ms(300);
//     Usart1Send("\nVerifing Flash Backuped Data...\n");
//     Wait_ms(300);

//     struct mem_conf_st mem_backuped;
//     memcpy(&mem_backuped, (uint32_t *) FLASH_ADDRESS_FOR_BKP, sizeof(mem_backuped));

//     if ((mem_conf.d0 == mem_backuped.d0) &&
//         (mem_conf.d1 == mem_backuped.d1) &&
//         (mem_conf.d2 == mem_backuped.d2) &&
//         (mem_conf.d3 == mem_backuped.d3))
//         Usart1Send("Verified OK!!!\n");
//     else
//         Usart1Send("Verified NOK errors in backuped data\n");
        
//     while (1)
//     {
//         Wait_ms(300);
//         if (LED)
//             LED_OFF;
//         else
//             LED_ON;

//     }
// }

//--- end of file ---//
