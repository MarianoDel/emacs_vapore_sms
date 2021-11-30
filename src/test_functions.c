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


void TF_Led_Alarm_Input (void)
{
    while (1)
    {
        if (ALARM_INPUT)
            LED_ON;
        else
            LED_OFF;

        Wait_ms(300);
    }
}


void TF_Led_Alarm_Input_Filtered (void)
{
    while (1)
    {
        if (Check_Alarm_Input())
            LED_ON;
        else
            LED_OFF;

    }
}


void TF_Act_12V (void)
{
    while (1)
    {
        if (ACT_12V)
            ACT_12V_OFF;
        else
            ACT_12V_ON;

        Wait_ms(300);
    }
}


void TF_Led_Blinking (void)
{
    ChangeLed(LED_GSM_NETWORK_LOW_RSSI);
    while (1)
        UpdateLed();

}


void TF_Usart2_TxRx (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    char s_to_send [100] = { 0 };
    Usart2Send("Ready to test...\n");
    while (1)
    {
        if (usart2_have_data)
        {
            usart2_have_data = 0;
            
            if (LED)
                LED_OFF;
            else
                LED_ON;
            
            Usart2ReadBuffer((unsigned char *) s_to_send, 100);
            Wait_ms(1000);
            Usart2Send(s_to_send);
        }
    }
}


void TF_Usart2_NetLight_and_Status (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();
    Usart2Send("Ready to test NetLight and Status...\n");
    
    unsigned char netlight_state = 0;
    unsigned char status_state = 0;
    while (1)
    {
        if (NETLIGHT)
        {
            if (netlight_state != NETLIGHT)
            {
                netlight_state = NETLIGHT;
                Usart2Send("NetLight is ON\n");
            }
        }
        else
        {
            if (netlight_state != NETLIGHT)
            {
                netlight_state = NETLIGHT;
                Usart2Send("NetLight is OFF\n");
            }
        }

        if (STATUS)
        {
            if (status_state != STATUS)
            {
                status_state = STATUS;
                Usart2Send("Status is ON\n");
            }
        }
        else
        {
            if (status_state != STATUS)
            {
                status_state = STATUS;
                Usart2Send("Status is OFF\n");
            }
        }
    }
}


void TF_Usart2_Adc_Dma (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    //-- ADC Init
    AdcConfig();

    //-- DMA configuration and Init
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->CR |= ADC_CR_ADSTART;

    unsigned short cntr = 0;
    char s_to_send [100] = { 0 };
    Usart2Send("\nTesting ADC with dma transfers...\n");

    while (1)
    {
        if (sequence_ready)
        {
            sequence_ready_reset;
            if (cntr < 10000)
                cntr++;
            else
            {
                sprintf(s_to_send, "V_Sense_4V: %d V_Sense_12V: %d\n",
                        V_Sense_4V,
                        V_Sense_12V);
                
                Usart2Send(s_to_send);
                cntr = 0;
            }
        }            
    }
}


void TF_Usart2_Flash_Empty_Page (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    char s_to_send [100] = { 0 };
    unsigned char * p;
    p = (unsigned char *) PAGE15_ADDR;
    
    Usart2Send("\nReading Flash Data...\n");

    for (unsigned char i = 0; i < 64; i+=8)
    {
        sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
                (unsigned int) (p + i),
                *(p + i + 0),
                *(p + i + 1),
                *(p + i + 2),
                *(p + i + 3),
                *(p + i + 4),
                *(p + i + 5),
                *(p + i + 6),
                *(p + i + 7));
        
        Usart2Send(s_to_send);
        Wait_ms(20);
    }

    Usart2Send("\nBlanking flash...\n");
    Wait_ms(500);
    if (Flash_ErasePage(FLASH_PAGE_FOR_BKP, 1) == FLASH_COMPLETE)
    {
        Usart2Send("Blank OK\n");
        Wait_ms(100);
    }
    else
    {
        Usart2Send("Blank NOK\n");
        Wait_ms(100);
    }

    Usart2Send("\nReading Flash Data...\n");

    for (unsigned char i = 0; i < 64; i+=8)
    {
        sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
                (unsigned int) (p + i),
                *(p + i + 0),
                *(p + i + 1),
                *(p + i + 2),
                *(p + i + 3),
                *(p + i + 4),
                *(p + i + 5),
                *(p + i + 6),
                *(p + i + 7));
        
        Usart2Send(s_to_send);
        Wait_ms(20);
    }
    
    while (1)
    {
        Wait_ms(300);
        if (LED)
            LED_OFF;
        else
            LED_ON;

    }
}


void TF_Usart2_Flash_Write_Data (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart2Config();

    char s_to_send [100] = { 0 };
    unsigned char * p;
    p = (unsigned char *) PAGE15_ADDR;
    
    Usart2Send("\nReading Flash Data...\n");

    for (unsigned char i = 0; i < 64; i+=8)
    {
        sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
                (unsigned int) (p + i),
                *(p + i + 0),
                *(p + i + 1),
                *(p + i + 2),
                *(p + i + 3),
                *(p + i + 4),
                *(p + i + 5),
                *(p + i + 6),
                *(p + i + 7));
        
        Usart2Send(s_to_send);
        Wait_ms(20);
    }

    //write mem conf
    struct mem_conf_st {
        uint32_t d0;
        uint32_t d1;
        uint32_t d2;
        uint32_t d3;
    };

    struct mem_conf_st mem_conf;
    mem_conf.d0 = 0x5555;
    mem_conf.d1 = 0xAAAA;
    mem_conf.d2 = 0x0000;
    mem_conf.d3 = 0x7777;

    Usart2Send("\nWriting Flash...\n");
    Wait_ms(300);
    if (Flash_WriteConfigurations((uint32_t *) &mem_conf, sizeof(mem_conf)) == FLASH_COMPLETE)
        Usart2Send("Seems all good\n");

    Wait_ms(300);
    for (unsigned char i = 0; i < 64; i+=8)
    {
        sprintf(s_to_send, "0x%x %x %x %x %x %x %x %x %x\n",
                (unsigned int) (p + i),
                *(p + i + 0),
                *(p + i + 1),
                *(p + i + 2),
                *(p + i + 3),
                *(p + i + 4),
                *(p + i + 5),
                *(p + i + 6),
                *(p + i + 7));
        
        Usart2Send(s_to_send);
        Wait_ms(20);
    }

    Wait_ms(300);
    Usart2Send("\nVerifing Flash Backuped Data...\n");
    Wait_ms(300);

    struct mem_conf_st mem_backuped;
    memcpy(&mem_backuped, (uint32_t *) FLASH_ADDRESS_FOR_BKP, sizeof(mem_backuped));

    if ((mem_conf.d0 == mem_backuped.d0) &&
        (mem_conf.d1 == mem_backuped.d1) &&
        (mem_conf.d2 == mem_backuped.d2) &&
        (mem_conf.d3 == mem_backuped.d3))
        Usart2Send("Verified OK!!!\n");
    else
        Usart2Send("Verified NOK errors in backuped data\n");
        
    while (1)
    {
        Wait_ms(300);
        if (LED)
            LED_OFF;
        else
            LED_ON;

    }
}


void TF_ReadMemory (void)
{
    char s_to_send [100] = { 0 };
    unsigned char * p;
    p = (unsigned char *) PAGE15_ADDR;
    
    Usart2Send("\nReading Flash Data...\n");

    for (unsigned char i = 0; i < 127; i+=8)
    {
        sprintf(s_to_send, "0x%x %d %d %d %d %d %d %d %d\n",
                (unsigned int) (p + i),
                *(p + i + 0),
                *(p + i + 1),
                *(p + i + 2),
                *(p + i + 3),
                *(p + i + 4),
                *(p + i + 5),
                *(p + i + 6),
                *(p + i + 7));
        
        Usart2Send(s_to_send);
        Wait_ms(20);
    }
}
//--- end of file ---//
