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
#include "sms_data.h"

#include <string.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
#define COMM_DEBUG_ON

#ifdef COMM_DEBUG_ON
#include "usart.h"
#endif
// Externals -------------------------------------------------------------------
extern unsigned char enviar_sms;
extern char enviar_sms_num [];
extern char enviar_sms_msg [];
extern parameters_typedef mem_conf;


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
            envios_ok = 1;
            envios_ok_change_set;

            enviar_sms = 1;
            CommsCheckSendOK (orig_num);
        }
        else if (*p_new == '0')
        {
            envios_ok = 0;
            envios_ok_change_set;
        }
    }

    else if (!strncmp(payload, "PRENDER_RING:", sizeof ("PRENDER_RING:") - 1))
    {
        char * p_new = (payload + sizeof ("PRENDER_RING:") - 1);
        if (*p_new == '1')
        {
            prender_ring = 1;
            prender_ring_change_set;
            CommsCheckSendOK (orig_num);
        }
        else if (*p_new == '0')
        {
            prender_ring = 0;
            prender_ring_change_set;
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
            timer_rep = 0;
        }
        else if ((index > 1) && (index <= 60))
        {
#ifdef COMM_DEBUG_ON
            char debug [60] = {'\0'};
            sprintf(debug, "nuevo timer %d", index);
            Usart2Send(debug);
#endif
            timer_rep = index;
            timer_rep_change_set;
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

#ifdef COMM_DEBUG_ON
            char debug [60] = {'\0'};
            sprintf(debug, "nuevo numero a reportar %s\n", new_number);
            Usart2Send(debug);
#endif
            if (VerifyNumberString(new_number) == 1)
            {
                num_tel_rep_change_set;
                strcpy(num_tel_rep, new_number);
                report_ok = 1;
            }
        }
        
        if (envios_ok)
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

#ifdef COMM_DEBUG_ON
            char debug [SITE_MAX_LEN + 20] = {'\0'};
            sprintf(debug, "nuevo lugar %s\n", new_site);
            Usart2Send(debug);
#endif
            if (VerifySiteString(new_site) == 1)
            {
                sitio_prop_change_set;
                strcpy(sitio_prop, new_site);
                report_ok = 1;
            }
        }
        
        if (envios_ok)
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
            battery_check = 0;
            battery_check_change_set;
            CommsCheckSendOK (orig_num);
        }
        else if (*p_new_conf == '1')
        {
            battery_check = 1;
            battery_check_change_set;
            CommsCheckSendOK (orig_num);
        }
    }

    // else if (!strncmp(payload, "IP:", sizeof ("IP:") - 1))
    // {
    //     // GPRS config, call the correspondig module function
    //     GPRS_Config(payload);
    // }

    // Diagnostics and Activations    
    else if (!strncmp(payload, (const char *)"PRENDER:", sizeof ("PRENDER:") - 1))
    {
        diag_prender_set;
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
}


void CommsCheckSendOK (char * orig_num)
{
    if (envios_ok)
    {
        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        strcpy(enviar_sms_msg, "OK");
    }
}


//--- end of file ---//
