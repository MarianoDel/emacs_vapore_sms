//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ESP8266.C ####################################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "ESP8266.h"
#include "uart.h"
#include "stm32f0xx.h"
#include "hard.h"

#include <string.h>
#include <stdio.h>


// Externals -------------------------------------------------------------------
extern unsigned short esp_timeout;
extern unsigned char esp_mini_timeout;
extern unsigned char esp_answer;
extern unsigned char esp_unsolicited_pckt;

extern volatile unsigned char bufftcp[];

extern volatile unsigned char tx2buff[];
extern volatile unsigned char rx2buff[];

// Globals ---------------------------------------------------------------------
void (* pCallBack) (char *);

unsigned char esp_command_state = 0;
unsigned char esp_timeout_cnt = 0;
unsigned char esp_mode = UNKNOW_MODE;
volatile unsigned char * prx;
volatile unsigned char at_start = 0;
volatile unsigned char at_finish = 0;
volatile unsigned char pckt_start = 0;
volatile unsigned char pckt_finish = 0;
enum EspConfigState esp_config_state = CONF_INIT;


// Constants -------------------------------------------------------------------


// Module Functions ------------------------------------------------------------
//OJO OJO OJO no esta terminada y no se si se necesita
unsigned char ESP_EnableNewConn (unsigned char p)
{
    unsigned char resp = resp_continue;

    if (p == CMD_RESET)
    {
        esp_config_state = ENA_INIT;
        return resp;
    }

    switch (esp_config_state)
    {
    case ENA_INIT:
        esp_config_state++;
        SendCommandWaitAnswerResetSM();
        break;

    case ENA_ASK_AT:
        resp = SendCommandWaitAnswer((const char *) "at+reconn=1\r\n");
        break;

    default:
        esp_config_state = ENA_INIT;
        break;
    }
    return resp;
}


//Resetea la maquina de estados de SendConfig
void ESP_SendConfigResetSM (void)
{
    esp_config_state = CONF_INIT;
}


//Configura como cliente WiFi
unsigned char ESP_SendConfigClient (void)
{
    unsigned char resp = resp_continue;

    switch (esp_config_state)
    {
    case CONF_INIT:
        esp_config_state++;
        ESPToATMode(CMD_RESET);
        break;

    case CONF_ASK_AT:
        resp = ESPToATMode(CMD_PROC);

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_0;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_0:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_0B;
        break;

    case CONF_AT_CONFIG_0B:
        resp = SendCommandWaitAnswer((const char *) "AT+CWMODE_CUR=1\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_1;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_1:
        SendCommandWithAnswer((const char *) "AT+CWJAP_CUR=\"TP-LINK_962DFC\",\"telefonica123\"\r\n");
        esp_config_state = CONF_AT_CONFIG_1B;
        esp_timeout = 20000;	//20 secs timeout
        break;

    case CONF_AT_CONFIG_1B:
        if ((esp_answer == resp_timeout) || (!esp_timeout))
        {
            resp = resp_timeout;
        }

        if (esp_answer == resp_ready)
        {
            esp_answer = resp_no_answer;		//blanqueo esp_answer
            esp_config_state = CONF_AT_CONFIG_2;
        }
        break;

    case CONF_AT_CONFIG_2:
        ESPPreParser((unsigned char *)rx2buff);
        //sigo esperando la respuesta
        esp_config_state = CONF_AT_CONFIG_1B;

        if (strncmp((char *)rx2buff, (const char *) "OK", (sizeof((const char *) "OK") - 1)) == 0)
        {
            //tengo link e IP
            //resp = resp_ok;
            resp = resp_continue;
            esp_config_state = CONF_AT_CONFIG_3;	//avanzo
        }

        if (strncmp((char *)rx2buff, (const char *) "FAIL", (sizeof((const char *) "FAIL") - 1)) == 0)
        {
            //tengo algun error
            resp = resp_nok;
        }




        //esta funcion contesta cosas raras y atemporales como
        //WIFI DISCONNECT
        //WIFI CONNECTED
        //WIFI GOT IP
        //
        //OK

        //en caso de error en passwd
        //WIFI DISCONNECT
        //+CWJAP:2
        //
        //FAIL

        //error de SSID
        //WIFI DISCONNECT
        //+CWJAP:3
        //
        //FAIL
        break;

    case CONF_AT_CONFIG_3:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_3B;
        break;

    case CONF_AT_CONFIG_3B:
        resp = SendCommandWaitAnswer((const char *) "AT+CWDHCP_CUR=1,1\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_4;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_4:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_4B;
        break;

    case CONF_AT_CONFIG_4B:
        resp = SendCommandWaitAnswer((const char *) "AT+CIPMUX=1\r\n");
        //utilizo esta respuesta como salida de la funcion

        break;


    default:
        esp_config_state = CONF_INIT;
        break;
    }

    return resp;
}


unsigned char ESP_SendConfigAP (void)
{
    unsigned char resp = resp_continue;

    switch (esp_config_state)
    {
    case CONF_INIT:
        esp_config_state++;
        ESPToATMode(CMD_RESET);
        break;

    case CONF_ASK_AT:
        resp = ESPToATMode(CMD_PROC);

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_0;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_0:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_0B;
        break;

    case CONF_AT_CONFIG_0B:
        resp = SendCommandWaitAnswer((const char *) "AT+CWMODE_CUR=2\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_1;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_1:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_1B;
        break;

    case CONF_AT_CONFIG_1B:
        resp = SendCommandWaitAnswer((const char *) "AT+CWSAP_CUR=\"KIRNO_WIFI\",\"12345678\",5,3\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_2;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_2:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_2B;
        break;

    case CONF_AT_CONFIG_2B:
        resp = SendCommandWaitAnswer((const char *) "AT+CWDHCP_CUR=0,1\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_3;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_3:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_3B;
        break;

    case CONF_AT_CONFIG_3B:
        resp = SendCommandWaitAnswer((const char *) "AT+CIPAP_CUR=\"192.168.1.254\"\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_4;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_4:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_4B;
        break;

    case CONF_AT_CONFIG_4B:
        resp = SendCommandWaitAnswer((const char *) "AT+CIPMUX=1\r\n");

        if (resp == resp_ok)
        {
            esp_config_state = CONF_AT_CONFIG_5;
            resp = resp_continue;
        }
        break;

    case CONF_AT_CONFIG_5:
        SendCommandWaitAnswerResetSM();
        esp_config_state = CONF_AT_CONFIG_5B;
        break;

    case CONF_AT_CONFIG_5B:
        resp = SendCommandWaitAnswer((const char *) "AT+CIPSERVER=1,10002\r\n");
        //utilizo esta respuesta como salida de la funcion

        break;

    default:
        esp_config_state = CONF_INIT;
        break;
    }

    return resp;
}


void ESP_OpenSocketResetSM (void)
{
    esp_config_state = OPEN_SOCKET_INIT;
}


//Configura como cliente WiFi
unsigned char ESP_OpenSocket (void)
{
    unsigned char resp = resp_continue;

    switch (esp_config_state)
    {
    case OPEN_SOCKET_INIT:
        esp_config_state++;
        break;

    case OPEN_SOCKET_RST:
        SendCommandWaitAnswerResetSM();
        esp_config_state = OPEN_SOCKET_ASK_SOCKET;
        break;

    case OPEN_SOCKET_ASK_SOCKET:
        //protocol server IP & port

        //resp = SendCommandWaitAnswer((const char *) "AT+CIPSTART=0,\"TCP\",\"192.168.0.102\",1883\r\n");
        //resp = SendCommandWaitAnswer((const char *) "AT+CIPSTART=\"TCP\",\"192.168.0.102\",1883\r\n");
        //resp = SendCommandWaitAnswer((const char *) "AT+CIPSTART=\"TCP\",\"192.168.0.100\",1883\r\n");
        resp = SendCommandWaitAnswer((const char *) "AT+CIPSTART=0,\"TCP\",\"192.168.0.100\",1883\r\n");

        if ((resp == resp_ok) || (resp == resp_ready))
        {
            //ajusto buffer
            //unsigned char len = sizeof ((const char *) "AT+CIPSTART=\"TCP\",\"192.168.0.102\",1883");	//me da 4 que loco??
            //unsigned char len = 38;
            unsigned char len = 40;
            strcpy ((char *) rx2buff, (char *) (rx2buff + len));

            //reviso respuestas
//				if (strncmp((char *) rx2buff, (char *) (const char *) "CONNECTOK", sizeof ((const char *) "CONNECTOK") - 1) == 0)
            if (strncmp((char *) rx2buff, (char *) (const char *) "0,CONNECTOK", sizeof ((const char *) "0,CONNECTOK") - 1) == 0)
                resp = resp_ok;
            else if (strncmp((char *)rx2buff, (char *) (const char *) "ALREADY CONNECTED", sizeof ((const char *) "ALREADY CONNECTED") - 1) == 0)
                resp = resp_ok;
            else
            {
                resp = resp_continue;
                esp_timeout = TT_AT_3SEG;
                esp_config_state++;
            }
        }

        if ((resp == resp_nok) || (resp == resp_timeout))
        {
            resp = resp_nok;
            esp_config_state = OPEN_SOCKET_INIT;
        }

//			SendCommandWithAnswer((const char *) "AT+CIPSTART=\"TCP\",\"192.168.0.102\",1883\r\n");
        break;

    case OPEN_SOCKET_WAIT_OK:
        //me quedo esperando el ok de envio o timeout
        if (esp_answer == resp_ready)
        {
            //reviso si se conecto en el buffer
            ESPPreParser((unsigned char *)rx2buff);

            //reviso respuestas
//				if (strncmp((char *) (const char *) "CONNECTOK", (char *)rx2buff, (sizeof ((const char *) "CONNECTOK")) - 1) == 0)
            if (strncmp((char *) (const char *) "0,CONNECTOK", (char *)rx2buff, (sizeof ((const char *) "0,CONNECTOK")) - 1) == 0)
                resp = resp_ok;
            else if (strncmp((char *) (const char *) "ALREADY CONNECTED", (char *)rx2buff, (sizeof ((const char *) "ALREADY CONNECTED")) - 1) == 0)
                resp = resp_ok;
            else
                resp = resp_nok;
        }

        if (!esp_timeout)
        {
            //tengo timeout, no pude abrir socket
            //resp = resp_timeout;
            resp = resp_nok;
        }

        break;

    default:
        esp_config_state = OPEN_SOCKET_INIT;
        break;
    }

    return resp;
}


void ESP_SendDataResetSM (void)
{
    esp_config_state = SEND_DATA_INIT;
}


//resetar primero la maquina de estados con ESP_SendDataResetSM()
//contesta resp_continue, resp_timeout, resp_nok o resp_ok
//el bufftcpsend de transmision es port,lenght,data
unsigned char ESP_SendData (unsigned char port, unsigned char * pbuf)
{
    unsigned char resp = resp_continue;
    char a [30];

    switch (esp_config_state)
    {
    case SEND_DATA_INIT:
        esp_config_state++;
        ESP_SetMode(AT_MODE);		//TODO: ver si todo no puede ser AT
        break;

    case SEND_DATA_RST:
        SendCommandWaitAnswerResetSM();
        esp_config_state++;
        break;

    case SEND_DATA_ASK_CHANNEL:
        sprintf (a, "AT+CIPSEND=%i,%i\r\n", *pbuf, *(pbuf+1));
        resp = SendCommandWaitAnswer(a);

        if (resp == resp_ok)
        {
            resp = resp_continue;
            SendCommandWithAnswer((char *)(pbuf + 2));		//blanquea esp_answer
            esp_timeout = TT_AT_3SEG;
            esp_config_state++;
        }

        //tengo timeout, termino transmision
        //resp = resp_timeout;
        break;

    case SEND_DATA_WAIT_SEND_OK:
        //me quedo esperando el ok de envio o timeout
        if (esp_answer == resp_ready)
        {
            //reviso si tengo SEND OK en el buffer
            ESPPreParser((unsigned char *)rx2buff);

            //si me recibe los bytes doy como el paquete enviado
            if (strncmp((char *) (const char *) "Recv ", (char *)rx2buff, (sizeof ((const char *) "Recv ")) - 1) == 0)
                resp = resp_ok;
            else if (strncmp((char *) (const char *) "SEND OK", (char *)rx2buff, (sizeof ((const char *) "SEND OK")) - 1) == 0)
                resp = resp_ok;
            else
                resp = resp_nok;
        }

        if (!esp_timeout)
        {
            //tengo timeout, termino transmision
            resp = resp_timeout;
        }
        break;

    default:
        esp_config_state = SEND_DATA_INIT;
        break;
    }

    return resp;
}


unsigned char ESP_GetIP (char * s_ip)
{
    unsigned char resp = resp_continue;
    unsigned char i;

    switch (esp_config_state)
    {
    case GET_IP_INIT:
        esp_config_state++;
        ESP_SetMode(AT_MODE);		//TODO: ver si todo no puede ser AT
        break;

    case GET_IP_RST:
        SendCommandWaitAnswerResetSM();
        esp_config_state++;
        esp_timeout = TT_AT_3SEG;
        break;

    case GET_IP_WAIT_ANS:
        resp = SendCommandWaitAnswer((const char *) "AT+CIFSR\r\n");
//			+CIFSR:STAIP,"192.168.0.101"
//			+CIFSR:STAMAC,"60:01:94:0c:0e:0c"
//
//			OK

        if (resp == resp_ok)
        {
            esp_config_state = GET_IP_CHECK;
            resp = resp_continue;
        }
        break;

    case GET_IP_CHECK:
        ESPPreParser((unsigned char *)rx2buff);
        //copio desde la primer comilla a la segunda con 15 posiciones como maximo
        for (i = 0; i < 15; i++)
        {
            if (*(rx2buff + 22 + i) != '"')
                *(s_ip + i) = *(rx2buff + 22 + i);
            else
            {
                *(s_ip + i) = '\0';
                i = 15;
            }
        }

        esp_config_state = GET_IP_INIT;
        resp = resp_ok;
        break;

    default:
        esp_config_state = GET_IP_INIT;
        break;
    }

    return resp;
}


//con CMD_RESET hace reset de la maquina, con CMD_PROC recorre la rutina
//contesta resp_continue, resp_timeout, resp_nok o resp_ok
unsigned char ESPToATMode (unsigned char p)
{
    unsigned char resp = resp_continue;

    if (p == CMD_RESET)
    {
        esp_command_state = COMM_INIT;
        return resp;
    }

    //hago polling hasta que este con modo AT
    switch (esp_command_state)
    {
    case COMM_INIT:
        esp_timeout = TT_ESP_AT_MODE;
        esp_timeout_cnt = 0;
        esp_command_state = COMM_TO_AT;
        esp_mode = GOING_AT_MODE;
        break;

    case COMM_TO_AT:
        if (!esp_timeout)
        {
            esp_command_state = COMM_AT_ANSWER;
            SendCommandWithAnswer((const char *) "AT+GMR\r\n");	//blanquea esp_answer
            esp_timeout = 3000;
        }
        break;

    case COMM_AT_ANSWER:
        if ((esp_answer == resp_timeout) || (!esp_timeout))
        {
            if (esp_timeout_cnt >= 3)
                resp = resp_timeout;
            else
            {
                esp_timeout_cnt++;
                esp_command_state = COMM_TO_AT;
            }
        }

        if (esp_answer == resp_ready)
        {
            //esp_command_state = COMM_WAIT_PARSER;
            ESPPreParser((unsigned char *)rx2buff);

            resp = ESPVerifyVersion((unsigned char *)rx2buff);

            if (resp == resp_ok)
                esp_mode = AT_MODE;
            else
                esp_mode = UNKNOW_MODE;
        }
        break;

    default:
        esp_command_state = COMM_INIT;
        break;
    }

    return resp;
}


void SendCommandWaitAnswerResetSM (void)
{
    esp_command_state = COMM_INIT;
}


//Envia un comando al ESP y espera revisar la respuesta
//Contesta resp_timeout, resp_continue, resp_nok, resp_ok
unsigned char SendCommandWaitAnswer (const char * comm)	//blanquea esp_answer
{
    unsigned char i, length = 0;
    unsigned char resp = resp_continue;
    char s_comm [80];

    switch (esp_command_state)
    {
    case COMM_INIT:
        esp_command_state = COMM_WAIT_ANSWER;
        SendCommandWithAnswer(comm);	//blanquea esp_answer
        esp_timeout = TT_AT_1SEG;
        break;

    case COMM_WAIT_ANSWER:
        if ((esp_answer == resp_timeout) || (!esp_timeout))
        {
            resp = resp_timeout;
        }

        if (esp_answer == resp_ready)
        {
            esp_answer = resp_no_answer;				//blanqueo esp_answer aunque aca no lo necesite
            esp_command_state = COMM_VERIFY_ANSWER;
        }
        break;

    case COMM_VERIFY_ANSWER:
        for (i = 0; i < sizeof(s_comm); i++)	//copio hasta el primer \r
        {
            if (comm[i] != '\r')
                s_comm[i] = comm[i];
            else
            {
                length = i;
                s_comm[i] = '\0';
                i = sizeof(s_comm);
            }
        }

        ESPPreParser((unsigned char *)rx2buff);

        resp = resp_nok;
        if (strncmp(s_comm, (char *)rx2buff, length) == 0)
        {
            resp = resp_ready;		//si no pega las respuestas comunes igual aviso que el comando era correcto
            if ((*(rx2buff + length) == 'O') && (*(rx2buff + length + 1) == 'K'))
                resp = resp_ok;

            if (strncmp((char *) (const char *) "no change OK", (char *)(rx2buff + length), (sizeof ((const char *) "no change OK")) - 1) == 0)
                resp = resp_ok;

            if (*(rx2buff + length) == '>')
                resp = resp_ok;

            if (*(rx2buff + length) == '+')		//es una respuesta con informacion adicional
                resp = resp_ok;					//que queda cargada en rx2buff
        }
        break;

    default:
        esp_command_state = COMM_INIT;
        break;
    }

    return resp;
}


//Manda un comando al ESP sin esperar la respuesta
//blanquea esp_answer
void SendCommandWithAnswer(const char * str)
{
    esp_answer = resp_no_answer;
    SerialSend((char *) str);
}


unsigned char ESP_AskMode(void)
{
    return esp_mode;	//TRANSPARENT_MODE, AT_MODE, UNKNOW_MODE
}


void ESP_SetMode(unsigned char m)
{
    esp_mode = m;	//TRANSPARENT_MODE, AT_MODE, UNKNOW_MODE
}


//me llaman desde el proceso principal para update
void ESP_ATProcess (void)
{
    //reviso timeouts para dar aviso del fin de mensajes at
    if ((at_start) && (at_finish) && (!esp_mini_timeout))
    {
        at_start = 0;
        at_finish = 0;
        *prx = '\0';				//ESTO es un peligro no se donde quedo apuntado
        esp_answer = resp_ready;	//aviso que tengo una respuesta para revisar
    }

    if ((pckt_start) && (pckt_finish) && (!esp_mini_timeout))
    {
        pckt_start = 0;
        pckt_finish = 0;
        *prx = '\0';				//ESTO es un peligro no se donde quedo apuntado
        esp_unsolicited_pckt = resp_ready;	//aviso que tengo una respuesta para revisar
    }
}


//TODO: ver que pasa si llega AT y nada mas como salgo por timeout EN ESTA O LA DE ARRIBA
//me llaman desde usart rx si estoy en modo AT
void ESP_ATModeRx (unsigned char d)
{
    //tengo que ver en que parte del AT estoy
    if ((!at_start) && (!pckt_start))
    {
        if ((d == 'A') || (d == 'R') || (d == 'S') || (d == 'W') || (d == 'O') || (d == 'F') || (d == 'C'))
        {																//AT o Recv o SEND OK
            prx = rx2buff;												//WIFI... o OK o FAIL
            *prx = d;													//ALREADY... o CONNECT
            prx++;
            at_start = 1;
        }
        else if (((d >= '0') && (d <= '4'))	|| (d == '+'))	//pueden ser nuevas conexiones o paquete TCP
        {
            prx = (unsigned char *) bufftcp;
            *prx = d;
            prx++;
            pckt_start = 1;
        }
    }
    else if (at_start)
    {
        if (d == '\n')		//no se cuantos finales de linea voy a tener en la misma respuesta
            at_finish = 1;

        *prx = d;
        if (prx < &rx2buff[SIZEOF_DATA])
            prx++;
        else
        {
            //recibi demasiados bytes juntos
            //salgo por error
            prx = rx2buff;
            esp_answer = resp_nok;
        }
    }
    else if (pckt_start)
    {
        if (d == '\n')		//no se cuantos finales de linea voy a tener en la misma respuesta
            pckt_finish = 1;

        *prx = d;
        if (prx < &bufftcp[SIZEOF_BUFFTCP])
            prx++;
        else
        {
            //recibi demasiados bytes juntos
            //salgo por error
            prx = (unsigned char *) bufftcp;
            esp_unsolicited_pckt = resp_nok;
        }
    }

    //mientras reciba bytes hago update del timer
    esp_mini_timeout = TT_ESP_AT_MINI;
}


//achica el buffer recibido a solo numeros letras y espacios
void ESPPreParser(unsigned char * d)
{
    unsigned char i;
    unsigned char * l;

    l = d;
    for (i = 0; i < SIZEOF_DATA256; i++)
    {
        if (*d != '\0')
        {
            if ((*d > 31) && (*d < 127))		//todos los codigos numeros y letras
            {
                *l = *d;
                l++;
            }
            d++;
        }
        else
        {
            *l = '\0';
            break;
        }
    }
}


//achica el buffer recibido a todos los codigos numeros letras y espacios
void ESPPreParser2 (unsigned char * orig, unsigned char * dest)
{
    unsigned char i;

    for (i = 0; i < SIZEOF_DATA256; i++)
    {
        if (*orig != '\0')
        {
            if ((*orig > 31) && (*orig < 127))		//todos los codigos numeros y letras
            {
                *dest = *orig;
                dest++;
            }
            orig++;
        }
        else
        {
            *dest = '\0';
            break;
        }
    }
}


unsigned char ESPVerifyVersion(unsigned char * d)
{
    char comp;

    //primero reviso el echo del at
    comp = strncmp ((char *) d, (const char *) "AT+GMR", (sizeof ((const char *) "AT+GMR")) - 1);
    if (comp == 0)
    {
        //ahora reviso solo algunos valores
        if ((*(d+6) == 'A') && (*(d+9) == 'v') && (*(d+16) == ':'))
            return resp_ok;
    }
    return resp_nok;
}


void CheckVersion (char * answer)
{
    unsigned char comp = 0;

    comp = strncmp (answer, (const char *) "VER 1.8", (sizeof ((const char *) "VER 1.8")) - 1);

    if (comp == 0)
        esp_answer = resp_ok;
    else
        esp_answer = resp_nok;
}


//--- end of file ---//
