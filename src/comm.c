//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM.C #####################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comm.h"
#include "parameters.h"
#include "battery.h"
#include "sms_gprs_data.h"
#include "usart.h"
#include "hard.h"    

#include <string.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------
extern unsigned char enviar_sms;
extern char enviar_sms_num [];
extern char enviar_sms_msg [];
extern parameters_typedef mem_conf;
extern unsigned short comms_global_flag;
extern volatile unsigned short keepalive_cnt;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void CommsCheckSendOK (char * orig_num);


// Module Functions ------------------------------------------------------------
void CommsProcessSMSPayload (char * orig_num, char * payload)
{
    unsigned char index = 0;

    // Configurations
    if (!strncmp(payload, "REPORTAR_OK:", sizeof ("REPORTAR_OK:") - 1))
    {
        char * p_new = (payload + sizeof ("REPORTAR_OK:") - 1);
        if (*p_new == '1')
        {
            envios_ok_conf = 1;
            comms_memory_save_flag_set;

            CommsCheckSendOK (orig_num);
        }
        else if (*p_new == '0')
        {
            envios_ok_conf = 0;
            comms_memory_save_flag_set;            
        }
    }

    else if (!strncmp(payload, "PRENDER_RING:", sizeof ("PRENDER_RING:") - 1))
    {
        char * p_new = (payload + sizeof ("PRENDER_RING:") - 1);
        if (*p_new == '1')
        {
            prender_ring_conf = 1;
            comms_memory_save_flag_set;
            CommsCheckSendOK (orig_num);
        }
        else if (*p_new == '0')
        {
            prender_ring_conf = 0;
            comms_memory_save_flag_set;            
            CommsCheckSendOK (orig_num);
        }
    }

    else if (!strncmp(payload, (const char *)"TIMER:", sizeof ("TIMER:") -1))
    {
        index = 0;
        index += (*(payload + 6) - 48) * 10;
        index += *(payload + 7) - 48;

        if ((*(payload + 6) == 'F') && (*(payload + 7) == 'F'))
        {
            timer_rep_conf = 0;
        }
        else if ((index > 1) && (index <= 60))
        {
            char debug [60] = {'\0'};
            sprintf(debug, "nuevo timer %d", index);
            Usart2Debug(debug, 2);

            timer_rep_conf = index;
            comms_memory_save_flag_set;
        }
        
        CommsCheckSendOK (orig_num);
    }

    else if (!strncmp(payload, "REPORTAR_NUM:", sizeof ("REPORTAR_NUM:") - 1))
    {
        char * p_new_number = (payload + sizeof ("REPORTAR_NUM:") - 1);
        unsigned char report_ok = 0;

        unsigned char len = strlen(p_new_number);
        if (len < (20 + 2))
        {
            char new_number [20] = { 0 };
            strncpy(new_number, p_new_number, (len - 2));

            char debug [60] = {'\0'};
            sprintf(debug, "nuevo numero a reportar %s\n", new_number);
            Usart2Debug(debug, 2);

            if (VerifyNumberString(new_number) == 1)
            {
                comms_memory_save_flag_set;
                strcpy(num_tel_rep, new_number);
                report_ok = 1;
            }
        }
        
        if (envios_ok_conf)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            if (report_ok)
                strcpy(enviar_sms_msg, "OK");
            else
                strcpy(enviar_sms_msg, "NOK");
            
        }
    }

    else if (!strncmp(payload, "REPORTAR_SITIO:", sizeof ("REPORTAR_SITIO:") -1))
    {
        char * p_new_place = (payload + sizeof ("REPORTAR_SITIO:") - 1);
        unsigned char report_ok = 0;

        unsigned char len = strlen(p_new_place);
        if (len < (SITE_MAX_LEN + 2))
        {
            char new_site [SITE_MAX_LEN + 1] = { 0 };
            strncpy(new_site, p_new_place, len - 2);    //quito el trailing OK

            char debug [SITE_MAX_LEN + 20] = {'\0'};
            sprintf(debug, "nuevo lugar %s\n", new_site);
            Usart2Debug(debug, 2);

            if (VerifySiteString(new_site) == 1)
            {
                comms_memory_save_flag_set;
                strcpy(sitio_prop, new_site);
                report_ok = 1;
            }
        }
        
        if (envios_ok_conf)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);

            if (report_ok)
                strcpy(enviar_sms_msg, "OK");
            else
                strcpy(enviar_sms_msg, "NOK");
        }
    }

    else if (!strncmp(payload, "REPORTAR_BAT:", sizeof ("REPORTAR_BAT:") - 1))
    {
        char * p_new_conf = (payload + sizeof ("REPORTAR_BAT:") - 1);
        if (*p_new_conf == '0')
        {
            battery_check_conf = 0;
            comms_memory_save_flag_set;
            CommsCheckSendOK (orig_num);
        }
        else if (*p_new_conf == '1')
        {
            battery_check_conf = 1;
            comms_memory_save_flag_set;
            CommsCheckSendOK (orig_num);
        }
    }

    else if (!strncmp(payload, "IP1:", sizeof ("IP1:") - 1))
    {
        // GPRS config, call the correspondig module function
        // cut the trailing OK
        unsigned char len = strlen(payload);
        *(payload + len - 2) = '\0';
        if (VerifyGPRSConfig_IP1(payload))
        {
            Usart2Debug("socket config ok\n", 1);
            // update the keepalive timer & enable socket use
            keepalive_cnt = mem_conf.keepalive;
            socket_use_enable_conf = 1;

            comms_memory_save_flag_set;

            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }
        else
            Usart2Debug("bad socket data!\n", 1);

    }
    else if (!strncmp(payload, "IP2:", sizeof ("IP2:") - 1))
    {
        // GPRS config, call the correspondig module function
        // cut the trailing OK
        unsigned char len = strlen(payload);
        *(payload + len - 2) = '\0';
        if (VerifyGPRSConfig_IP2(payload))
        {
            Usart2Debug("socket config ok\n", 1);
            socket_use_enable_conf = 2;
            comms_memory_save_flag_set;

            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }
        else
            Usart2Debug("bad socket data!\n", 1);

    }
    // else if (!strncmp(payload, "IPDN:", sizeof ("IPDN:") - 1))
    // {
    //     // GPRS config, call the correspondig module function
    //     // cut the trailing OK
    //     unsigned char len = strlen(payload);
    //     *(payload + len - 2) = '\0';
    //     if (GPRS_Config(payload, 1))
    //     {
    //         Usart2Debug("socket config ok\n");
    //         socket_use_enable = 1;            
    //         socket_conf_change_set;

    //         enviar_sms = 1;
    //         strcpy(enviar_sms_num, orig_num);
    //         strcpy(enviar_sms_msg, "OK");
    //     }
    //     else
    //         Usart2Debug("bad socket data!\n");

    // }
    else if (!strncmp(payload, "IPNO:", sizeof ("IPNO:") - 1))
    {
        socket_use_enable_conf = 0;
        comms_memory_save_flag_set;
        
        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        strcpy(enviar_sms_msg, "OK");
    }

    // Diagnostics and Activations    
    else if (!strncmp(payload, (const char *)"PRENDER:", sizeof ("PRENDER:") - 1))
    {
        comms_activate_sms_flag_set;
        CommsCheckSendOK (orig_num);
    }

    else if (!strncmp(payload, "BATERIA:", sizeof ("BATERIA:") - 1))
    {
        // diag_battery_set;
        unsigned char volts_int = 0;
        unsigned char volts_dec = 0;
        Battery_Voltage(&volts_int, &volts_dec);

        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        sprintf(enviar_sms_msg, "BAT: %02d.%02dV", volts_int, volts_dec);
    }

    else if (!strncmp(payload, "VERSION:", sizeof ("VERSION:") - 1))
    {
        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        sprintf(enviar_sms_msg, "%s\r\n%s", HARD, SOFT);
    }
}


void CommsCheckSendOK (char * orig_num)
{
    if (envios_ok_conf)
    {
        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        strcpy(enviar_sms_msg, "OK");
    }
}


//--- end of file ---//
