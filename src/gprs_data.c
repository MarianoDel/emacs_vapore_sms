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
#include <stdlib.h>


// Module Private Types Constants and Macros -----------------------------------
// #define VerifyDomainString((X),(Y))    VerifyAPNString((X),(Y))


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
unsigned char VerifyAPNString (char * apn, unsigned char len);
unsigned char VerifyIPString (char * ip, unsigned char len);
unsigned char VerifyIPProtocol (char * ip_proto, unsigned char len);
unsigned char VerifyPort (char * ip, unsigned char len);
unsigned char VerifyIsANumber (char * pn, unsigned int * number);
unsigned char VerifySocketData (void);
unsigned char VerifyDomainString (char * domain, unsigned char len);



// Module Functions -----------------------------------------------------------
unsigned char VerifyAndSendGPRS (char * message)
{
    unsigned char answer = GPRS_WORKING;
    
    answer = FuncsGSMSendGPRS(message);

    if (answer == resp_gsm_ok)
    {
        Usart2Send("connection ok\n");
        return GPRS_SENT;
    }
        
    if (answer > resp_gsm_ok)
    {
        Usart2Send("connection fail!!!\n");
        return GPRS_NOT_SEND;
    }

    return GPRS_WORKING;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifySocketData (void)
{
    if ((!VerifyIPString (mem_conf.ip, strlen(mem_conf.ip))) &&
        (!VerifyDomainString (mem_conf.domain, strlen(mem_conf.domain))))
        return 0;

    if (!VerifyIPProtocol (mem_conf.ip_proto, strlen(mem_conf.ip_proto)))
        return 0;

    if (!VerifyPort (mem_conf.ip_port, strlen(mem_conf.ip_port)))
        return 0;

    if (!VerifyAPNString (mem_conf.apn, strlen(mem_conf.apn)))
        return 0;

    return 1;
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
unsigned char VerifyDomainString (char * domain, unsigned char len)
{
    return VerifyAPNString (domain, len);
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
    short points_pos [4] = { 0 };
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


// get string with a number in pn
// answers the number in number
// return the digits quantity readed
unsigned char VerifyIsANumber (char * pn, unsigned int * number)
{
    unsigned char i;
    char new_number [8] = {0};

    // no more than six chars
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


// callback with sms payloads
#define CONFIG_FIELDS    4
#define FIELD_IP    0
#define FIELD_PROTO    1
#define FIELD_PORT    2
#define FIELD_APN    3
//answer 1 -> ok; 0 -> some error
unsigned char GPRS_Config (char * payload, unsigned char ipdomain)
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

    if (ipdomain)
    {
        // check for valid ip_domain
        field_start = sizeof("IPDN:") - 1;
        field_len = commas[FIELD_IP] - field_start;
        if (!VerifyAPNString (payload + field_start, field_len))
            return 0;
    }
    else
    {
        // check for valid ip
        field_start = sizeof("IP:") - 1;
        field_len = commas[FIELD_IP] - field_start;
        if (!VerifyIPString(payload + field_start, field_len))
            return 0;
    }

    Usart2Debug("ip ok\n");

    // check for valid ip protocol
    field_start = commas[FIELD_IP] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;
    if (!VerifyIPProtocol(payload + field_start, field_len))
        return 0;

    Usart2Debug("proto ok\n");
    
    // check for valid port
    field_start = commas[FIELD_PROTO] + sizeof("PORT:");    // take the comma in account
    field_len = commas[FIELD_PORT] - field_start;
    if (!VerifyPort(payload + field_start, field_len))
        return 0;

    Usart2Debug("port ok\n");
    
    // check for valid apn
    field_start = commas[FIELD_PORT] + sizeof("APN:");    // take the comma in account
    // field_len = commas[FIELD_APN] - field_start;
    field_len = len - field_start;    //no trailing comma    
    if (!VerifyAPNString((payload + field_start), field_len))
        return 0;

    Usart2Debug("apn ok\n");
    
    // all data is valid, save it
    char * pcut;

    if (ipdomain)
    {
        field_start = sizeof("IPDN:") - 1;
        field_len = commas[FIELD_IP] - field_start;    
        strncpy(mem_conf.domain, (payload + field_start), field_len);
        pcut = mem_conf.domain;
        *(pcut + field_len) = '\0';
    }
    else
    {
        field_start = sizeof("IP:") - 1;
        field_len = commas[FIELD_IP] - field_start;    
        strncpy(mem_conf.ip, (payload + field_start), field_len);
        pcut = mem_conf.ip;
        *(pcut + field_len) = '\0';
    }
    
    field_start = commas[FIELD_IP] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;
    strncpy(mem_conf.ip_proto, (payload + field_start), field_len);
    pcut = mem_conf.ip_proto;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_PROTO] + sizeof("PORT:");    // take the comma in account
    field_len = commas[FIELD_PORT] - field_start;    
    strncpy(mem_conf.ip_port, (payload + field_start), field_len);
    pcut = mem_conf.ip_port;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_PORT] + sizeof("APN:");    // take the comma in account
    field_len = len - field_start;    //no trailing comma    
    strncpy(mem_conf.apn, (payload + field_start), field_len);
    pcut = mem_conf.apn;
    *(pcut + field_len) = '\0';

    return 1;
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

