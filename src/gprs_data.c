//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPRS_DATA.C ################################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "gprs_data.h"
// #include "sms_data.h"
#include "parameters.h"
#include "usart.h"
#include "hard.h"
#include "funcs_gsm.h"


#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------



// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
unsigned char VerifyAPNString (char * apn, unsigned char len);
unsigned char VerifyDNSString (char * dns, unsigned char len);
unsigned char VerifyIPString (char * ip, unsigned char len);
unsigned char VerifyIPProtocol (char * ip_proto, unsigned char len);
unsigned char VerifyPort (char * ip, unsigned char len);
unsigned char VerifyIsANumber (char * pn, unsigned int * number);

// Module Functions ------------------------------------------------------------
unsigned char VerifyAndSendGPRS (gprs_pckt_t * p_gprs)
{
    if (p_gprs->alarm_input)
        Usart2Send("External 12V: ");    // 12V input alarm activate

    if (p_gprs->panel_input)
        Usart2Send("Keypad ACT: ");
                    
    //check sitio_prop before send gprs
    if (!VerifySiteString(sitio_prop))
    {
        Usart2Send("no site saved\n");
        return GPRS_NOT_PROPER_DATA;
    }

    //check apn before send gprs
    if (!VerifyAPNString("gprs.personal.com", 12))
    {
        Usart2Send("no apn data\n");
        return GPRS_NOT_PROPER_DATA;
    }

    //check ip, port, protocol of server before send gprs
    // if (!VerifySocketData())
    // {
    //     Usart2Send("no socket data\n");
    //     return GPRS_NOT_PROPER_DATA;
    // }

    
    // data is good, try to send an sms
    // Activation_12V_On();    // ACT_12V_ON;
    // if (p_gprs->alarm_input)
    //     strcpy(p_gprs->buff, "Activacion en: ");

    // if (p_gprs->panel_input)
    //     sprintf(p_gprs->buff, "Activo %03d en: ", p_gprs->remote_number);
                        
    // strcat(p_gprs->buff, sitio_prop);
    // if (FuncsGSMSendSMS (p_gprs->buff, num_tel_rep) == resp_gsm_ok)
    // {
    //     Usart2Send("OK\n");
    //     return SMS_SENT;
    // }

    // Usart2Send("delayed\n");    
    return SMS_NOT_SEND;
}



//answer 1 -> ok; 0 -> some error
unsigned char VerifyAPNString (char * apn, unsigned char len)
{
    unsigned char cnt = 0;
    
    if (len > 67)
        return 0;

    for (unsigned char i = 0; i < len; i++)
    {
        if ((*(apn+i) == '-') ||
            (*(apn+i) == '.'))
            cnt++;
        else if ((*(apn+i) >= 'a') &&
                 (*(apn+i) <= 'z'))
            cnt++;
        else if ((*(apn+i) >= '0') &&
                 (*(apn+i) <= '9'))
            cnt++;
        
    }

    if (cnt == len)
        return 1;
    
    return 0;
    
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyDNSString (char * dns, unsigned char len)
{
    return VerifyAPNString(dns, len);
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyIPProtocol (char * ip_proto, unsigned char len)
{
    if (len != 3)
        return 0;

    if (!(strncmp(ip_proto, "TCP", sizeof("TCP") - 1)) ||
        !(strncmp(ip_proto, "UDP", sizeof("UDP") - 1)))
        return 1;
    
    return 0;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyIPString (char * ip, unsigned char len)
{
    unsigned char cnt = 0;
    unsigned char points = 0;
    char points_pos [4] = { 0 };
    unsigned char digits = 0;
    unsigned int num = 0;

    if (len > 15)
        return 0;

    points = 1;
    points_pos[0] = -1;
    for (unsigned char i = 0; i < len; i++)
    {
        if (*(ip + i) == '.')
        {
            if (points < 4)
                points_pos[points] = i;

            points++;
            cnt++;
        }
        else if ((*(ip + i) >= '0') &&
                 (*(ip + i) <= '9'))
            cnt++;
    }

    if ((cnt != len) &&
        (points != 3))
        return 0;

    // check octets values
    for (unsigned char i = 0; i < 4; i++)
    {
        digits = VerifyIsANumber ((ip + points_pos[i] + 1), &num);

        if ((digits == 0) ||
            (digits > 3) ||
            (num > 255))
            return 0;
    }
    
    return 1;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyPort (char * port, unsigned char len)
{
    unsigned int port_num = 0;
    unsigned char digits = 0;
    
    if (len > 5)
        return 0;

    digits = VerifyIsANumber (port, &port_num);

    if ((digits <= 5) &&
        (port_num > 1) &&
        (port_num < 65536))
        return 1;

    return 0;
}


// callback with sms payloads
#define CONFIG_FIELDS    4
#define FIELD_IP    0
#define FIELD_PROTO    1
#define FIELD_PORT    2
#define FIELD_APN    3
//answer 1 -> ok; 0 -> some error
unsigned char GPRS_Config (char * payload)
{
    unsigned char len = strlen (payload);
    unsigned char commas[CONFIG_FIELDS] = { 0 };
    unsigned char field_len;
    unsigned char field_start;    

    // check for max len
    if (len > 165)
        return 0;

    // check for config fields
    unsigned char comma_cnt = 0;
    for (int i = 0; i < len; i++)
    {
        if (*(payload + i) == ',')
        {
            commas[comma_cnt] = i;
            if (comma_cnt < (CONFIG_FIELDS - 1))
                comma_cnt++;
            else
            {
                comma_cnt = 255;
                break;
            }
        }        
    }

    if (comma_cnt != (CONFIG_FIELDS - 1))
        return 0;
    
    // check for valid ip
    field_start = sizeof("IP:") - 1;
    field_len = commas[FIELD_IP] - field_start;
    if (!VerifyIPString(payload + field_start, field_len))
        return 0;

    // check for valid ip protocol
    field_start = commas[FIELD_IP] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;
    if (!VerifyIPProtocol(payload + field_start, field_len))
        return 0;

    // check for valid port
    field_start = commas[FIELD_PROTO] + sizeof("PORT:");    // take the comma in account
    field_len = commas[FIELD_PORT] - field_start;
    if (!VerifyPort(payload + field_start, field_len))
        return 0;

    // check for valid apn
    field_start = commas[FIELD_PORT] + sizeof("APN:");    // take the comma in account
    // field_len = commas[FIELD_APN] - field_start;
    field_len = len - field_start;    //no trailing comma    
    if (!VerifyAPNString((payload + field_start), field_len))
        return 0;

    // all data is valid, save it
    field_start = sizeof("IP:") - 1;
    field_len = commas[FIELD_IP] - field_start;    
    strncpy(mem_conf.ip, (payload + field_start), field_len);
    
    field_start = commas[FIELD_IP] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;
    strncpy(mem_conf.ip_proto, (payload + field_start), field_len);

    field_start = commas[FIELD_PROTO] + sizeof("PORT:");    // take the comma in account
    field_len = commas[FIELD_PORT] - field_start;    
    strncpy(mem_conf.ip_port, (payload + field_start), field_len);

    field_start = commas[FIELD_PORT] + sizeof("APN:");    // take the comma in account
    field_len = len - field_start;    //no trailing comma    
    strncpy(mem_conf.apn, (payload + field_start), field_len);

    return 1;
}


//devuelve los numeros en un string, en la posicion number
//devuele la cantidad de cifras leidas
unsigned char VerifyIsANumber (char * pn, unsigned int * number)
{
    unsigned char i;
    char new_number [6] = {0};

    //no mas de 6 caracteres
    for (i = 0; i < 6; i++)
    {
        if ((*(pn + i) < 48) || (*(pn + i) > 57))
            break;

        new_number[i] = *(pn + i);
    }

    if (i > 0)
        *number = atoi(new_number);

    return i;
}


unsigned short GetValue (unsigned char * pn, char delimiter)
{
	unsigned char i;
	unsigned char colon = 0;
	unsigned short new_val = 0xffff;

	//me fijo la posiciones de la , o ;
	for (i = 0; i < 6; i++)
	{
		if (*(pn + i) == delimiter)
		{
			colon = i;
			i = 6;
		}
	}

	if ((colon == 0) || (colon >= 5))
		return 0;

	switch (colon)
	{
		case 1:
			new_val = *pn - '0';
			break;

		case 2:
			new_val = (*pn - '0') * 10 + (*(pn + 1) - '0');
			break;

		case 3:
			new_val = (*pn - '0') * 100 + (*(pn + 1) - '0') * 10 + (*(pn + 2) - '0');
			break;

		case 4:
			new_val = (*pn - '0') * 1000 + (*(pn + 1) - '0') * 100 + (*(pn + 2) - '0') * 10 + (*(pn + 2) - '0');
			break;

	}
	return new_val;
}

//--- end of file ---//

