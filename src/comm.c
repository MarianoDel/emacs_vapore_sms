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
// #include "flash_program.h"
#include "parameters.h"

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


// Module Functions ------------------------------------------------------------
void CommsProcessSMSPayload (char * orig_num, char * payload)
{
    unsigned char index = 0;

    // Configurations
    if (!strncmp(payload, "REPORTAR_OK:1", sizeof ("REPORTAR_OK:1") -1))
    {
        envios_ok = 1;
        envios_ok_change_set;

        enviar_sms = 1;
        strcpy(enviar_sms_num, orig_num);
        strcpy(enviar_sms_msg, "OK");
    }

    if (!strncmp(payload, "REPORTAR_OK:0", sizeof ("REPORTAR_OK:0") -1))
    {
        envios_ok = 0;
        envios_ok_change_set;
    }

    if (!strncmp(payload, "PRENDER_RING:0", sizeof ("PRENDER_RING:0") -1))
    {
        prender_ring = 0;
        prender_ring_change_set;

        if (envios_ok)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }
    }

    if (!strncmp(payload, "PRENDER_RING:1", sizeof ("PRENDER_RING:1") -1))
    {
        prender_ring = 1;
        prender_ring_change_set;

        if (envios_ok)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }        
    }
    
    if (!strncmp(payload, (const char *)"TIMER:", sizeof ("TIMER:") -1))
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

        if (envios_ok)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }
    }

    // if (!strncmp(payload, (const char *)"TIMERD:", sizeof ("TIMERD:") -1))
    // {
    //     index = 0;
    //     index += (*(payload + 7) - 48) * 10;
    //     index += *(payload + 8) - 48;

    //     if ((*(payload + 7) == 'F') && (*(payload + 8) == 'F'))
    //     {
    //         timer_debug = 0;
    //         send_sms_ok_set;
    //     }
    //     else if ((index > 1) && (index <= 60))
    //     {
    //         timer_debug = index;
    //         send_sms_ok_set;
    //     }
    // }

    if (!strncmp(payload, (const char *)"PRENDER:", sizeof ("PRENDER:") -1))
    {
        diag_prender_set;
        if (envios_ok)
        {
            enviar_sms = 1;
            strcpy(enviar_sms_num, orig_num);
            strcpy(enviar_sms_msg, "OK");
        }
    }

    // if (!strncmp(payload, (const char *)"APAGAR:", sizeof ("APAGAR:") -1))
    //     diag_apagar_set;

    // if (!strncmp(payload, (const char *)"ENERGIA:", sizeof ("ENERGIA:") -1))
    //     send_energy_set;

    if (!strncmp(payload, "REPORTAR_NUM:", sizeof ("REPORTAR_NUM") -1))
    {
        char * p_new_number = (payload + 13);
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

    if (!strncmp(payload, "REPORTAR_SITIO:", sizeof ("REPORTAR_SITIO") -1))
    {
        char * p_new_place = (payload + 15);
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
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyNumberString (char * number)
{
    unsigned char len = 0;
    len = strlen(number);

    if ((len > 19) || (len < 3))
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if (*(number + i) != '+')
        {
            if ((*(number + i) > '9') ||
                (*(number + i) < '0'))
                return 0;
        }
    }

    return 1;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifySiteString (char * site)
{
    unsigned char len = 0;
    len = strlen(site);

    if ((len > SITE_MAX_LEN) || (len < 3))
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if ((*(site + i) == 'á') ||
            (*(site + i) == 'é') ||
            (*(site + i) == 'í') ||
            (*(site + i) == 'ó') ||
            (*(site + i) == 'ú'))
        {
            // do nothing here
        }
        else if ((unsigned char) *(site + i) == 193)
            *(site + i) = 'A';
        else if ((unsigned char) *(site + i) == 201)
            *(site + i) = 'E';
        else if ((unsigned char) *(site + i) == 205)
            *(site + i) = 'I';
        else if ((unsigned char) *(site + i) == 211)
            *(site + i) = 'O';
        else if ((unsigned char) *(site + i) == 218)
            *(site + i) = 'U';
        else if ((*(site + i) > '~') ||
                 (*(site + i) < ' '))
            return 0;
    }

    return 1;
    
}


//--- end of file ---//
