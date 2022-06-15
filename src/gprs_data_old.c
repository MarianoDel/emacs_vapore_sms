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
unsigned char VerifyDomainString (char * domain, unsigned char len);
unsigned char VerifyClientNumber (char * client, unsigned char len);
unsigned char VerifyKeepString (char * keep, unsigned char len);


// Module Functions -----------------------------------------------------------
unsigned char VerifyAndSendGPRS (char * message)
{
    unsigned char answer = GPRS_WORKING;
    
    answer = FuncsGSMSendGPRS(message, 1);

    if (answer == resp_gsm_ok)
    {
        Usart2Send("gprs connection ok\n");
        return GPRS_SENT;
    }
        
    if (answer > resp_gsm_ok)
    {
        Usart2Send("gprs connection fail!!!\n");
        return GPRS_NOT_SEND;
    }

    return GPRS_WORKING;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifySocketData (unsigned char which_ip)
{
    if (which_ip == IP1)
    {
        if (!VerifyIPString (mem_conf.ip1, strlen(mem_conf.ip1)))
            return 0;

        if (!VerifyPort (mem_conf.ip_port1, strlen(mem_conf.ip_port1)))
            return 0;
    }
    else
    {
        if (!VerifyIPString (mem_conf.ip2, strlen(mem_conf.ip2)))
            return 0;

        if (!VerifyPort (mem_conf.ip_port2, strlen(mem_conf.ip_port2)))
            return 0;
    }

    if (!VerifyIPProtocol (mem_conf.ip_proto, strlen(mem_conf.ip_proto)))
        return 0;

    if (!VerifyAPNString (mem_conf.apn, strlen(mem_conf.apn)))
        return 0;

    if (!VerifyClientNumber (mem_conf.client_number, strlen(mem_conf.client_number)))
        return 0;
    
    return 1;
}


unsigned char VerifyClientNumber (char * client, unsigned char len)
{
    if ((len < 4) || (len > 6))
        return 0;

    return 1;
}


unsigned char VerifyKeepString (char * keep, unsigned char len)
{
    if ((len < 2) || (len > 3))
        return 0;

    unsigned int num = 0;
    unsigned char digits = VerifyIsANumber (keep, &num);

    if (digits != len)
        return 0;

    if ((num < 10) || (num > 600))
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
#define CONFIG_FIELDS    8
#define FIELD_IP1    0
#define FIELD_PORT1    1
#define FIELD_IP2    2
#define FIELD_PORT2    3
#define FIELD_PROTO    4
#define FIELD_APN    5
#define FIELD_CLI    6
#define FIELD_KEEP    7

//answer 1 -> ok; 0 -> some error
unsigned char VerifyGPRSConfig (char * payload)
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

    // check for valid ip1
    field_start = sizeof("IP1:") - 1;
    field_len = commas[FIELD_IP1] - field_start;
    if (!VerifyIPString(payload + field_start, field_len))
        return 0;

    Usart2Debug("ip1 ok\n");

    // check for valid port1
    field_start = commas[FIELD_PORT1 - 1] + sizeof("PORT1:");    // take the comma in account
    field_len = commas[FIELD_PORT1] - field_start;
    if (!VerifyPort(payload + field_start, field_len))
        return 0;

    Usart2Debug("port1 ok\n");

    // check for valid ip2 or nothing
    field_start = commas[FIELD_IP2 - 1] + sizeof("IP2:");    // take the comma in account
    field_len = commas[FIELD_IP2] - field_start;
    if (!VerifyIPString(payload + field_start, field_len))
        return 0;

    Usart2Debug("ip2 ok\n");

    // check for valid port2 or nothing
    field_start = commas[FIELD_PORT2 - 1] + sizeof("PORT2:");    // take the comma in account
    field_len = commas[FIELD_PORT2] - field_start;
    if (!VerifyPort(payload + field_start, field_len))
        return 0;

    Usart2Debug("port2 ok\n");
    
    // check for valid ip protocol
    field_start = commas[FIELD_PROTO - 1] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;
    if (!VerifyIPProtocol(payload + field_start, field_len))
        return 0;

    Usart2Debug("proto ok\n");
        
    // check for valid apn
    field_start = commas[FIELD_APN - 1] + sizeof("APN:");    // take the comma in account
    field_len = commas[FIELD_APN] - field_start;
    if (!VerifyAPNString((payload + field_start), field_len))
        return 0;

    Usart2Debug("apn ok\n");

    // check for valid client
    field_start = commas[FIELD_CLI - 1] + sizeof("CLI:");    // take the comma in account
    field_len = commas[FIELD_CLI] - field_start;
    if (!VerifyClientNumber((payload + field_start), field_len))
        return 0;

    Usart2Debug("client ok\n");
    
    // check for valid keepalive
    field_start = commas[FIELD_KEEP - 1] + sizeof("KEEP:");    // take the comma in account
    // field_len = commas[FIELD_KEEP] - field_start;
    field_len = len - field_start;    //no trailing comma    
    if (!VerifyKeepString((payload + field_start), field_len))
        return 0;

    Usart2Debug("keepalive ok\n");
    
    // all data is valid, save it
    char * pcut;

    field_start = sizeof("IP1:") - 1;
    field_len = commas[FIELD_IP1] - field_start;    
    strncpy(mem_conf.ip1, (payload + field_start), field_len);
    pcut = mem_conf.ip1;
    *(pcut + field_len) = '\0';
    
    field_start = commas[FIELD_PORT1 - 1] + sizeof("PORT1:");    // take the comma in account
    field_len = commas[FIELD_PORT1] - field_start;
    strncpy(mem_conf.ip_port1, (payload + field_start), field_len);
    pcut = mem_conf.ip_port1;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_IP2 - 1] + sizeof("IP2:");    // take the comma in account
    field_len = commas[FIELD_IP2] - field_start;
    strncpy(mem_conf.ip2, (payload + field_start), field_len);
    pcut = mem_conf.ip2;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_PORT2 - 1] + sizeof("PORT2:");    // take the comma in account
    field_len = commas[FIELD_PORT2] - field_start;
    strncpy(mem_conf.ip_port2, (payload + field_start), field_len);
    pcut = mem_conf.ip_port2;
    *(pcut + field_len) = '\0';
    
    field_start = commas[FIELD_PROTO - 1] + sizeof("PROTO:");    // take the comma in account
    field_len = commas[FIELD_PROTO] - field_start;    
    strncpy(mem_conf.ip_proto, (payload + field_start), field_len);
    pcut = mem_conf.ip_proto;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_APN - 1] + sizeof("APN:");    // take the comma in account
    field_len = commas[FIELD_APN] - field_start;    
    strncpy(mem_conf.apn, (payload + field_start), field_len);
    pcut = mem_conf.apn;
    *(pcut + field_len) = '\0';

    field_start = commas[FIELD_CLI - 1] + sizeof("CLI:");    // take the comma in account
    field_len = commas[FIELD_CLI] - field_start;    
    strncpy(mem_conf.client_number, (payload + field_start), field_len);
    pcut = mem_conf.client_number;
    *(pcut + field_len) = '\0';    

    field_start = commas[FIELD_KEEP] + sizeof("KEEP:");    // take the comma in account
    field_len = len - field_start;    //no trailing comma
    mem_conf.keepalive = atoi(payload + field_start);

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


////////////////////////
// SMS Data Functions //
////////////////////////
//answer 1 -> ok; 0 -> some error
unsigned char VerifySMSData (void)
{
    if (!VerifyNumberString (mem_conf.num_reportar))
        return 0;
    
    return 1;
}


//answer 1 -> ok; 0 -> some error
unsigned char VerifyNumberString (char * number)
{
    unsigned char len = 0;
    len = strlen(number);

    if ((len > PHONE_NUMBER_MAX_LEN) || (len < 3))
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

