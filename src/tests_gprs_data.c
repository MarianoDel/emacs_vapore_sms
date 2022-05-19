//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "parameters.h"
#include "gprs_data.h"
#include "funcs_gsm.h"

//helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------

parameters_typedef mem_conf;

char cb_buff [5][200];
int cb_cnt = 0;


// For Test Module Private Functions -------------------------------------------
extern unsigned char VerifyAPNString (char * apn, unsigned char len);
extern unsigned char VerifyDNSString (char * dns, unsigned char len);
extern unsigned char VerifyIPString (char * ip, unsigned char len);
extern unsigned char VerifyIPProtocol (char * ip_proto, unsigned char len);
extern unsigned char VerifyPort (char * ip, unsigned char len);
extern unsigned char StringIsANumber (char * pn, unsigned short * number);    


// Module Mocked Functions -----------------------------------------------------
// void ChangeLedActivate (unsigned char number);
void MyCb (char * buff);
void MyCbClean (void);
void FuncsGSMSendGPRS_Answer (unsigned char answer);


// Module Functions for testing ------------------------------------------------
void Test_Verify_APN (void);
void Test_Verify_IP_Protocol (void);
void Test_Verify_IP_String (void);
void Test_Verify_IP_Port (void);

void Test_GPRS_Config (void);
void Test_GPRS_Send (void);


// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{

    // Test_Verify_APN ();

    // Test_Verify_IP_String ();

    // Test_Verify_IP_Protocol ();

    // Test_Verify_IP_Port ();
    
    Test_GPRS_Config ();

    // Test_GPRS_Send ();
}


void Test_GPRS_Send (void)
{
    gprs_pckt_t packet;
    char conf_test [165] = { 0 };
    char buff_to_send [] = {"activacion"};

    packet.alarm_input = 1;
    packet.panel_input = 0;
    packet.remote_number = 505;
    packet.buff = buff_to_send;

    printf("test start send gprs\n");

    strcpy(mem_conf.sitio_propio, "Mi sitio");

    // conf okeys
    strcpy(conf_test,"IP:192.168.1.1,PROTO:UDP,PORT:10000,APN:gprs.movistar.com");
    printf("Test gprs conf len %d %s :", strlen(conf_test), conf_test);
    if (GPRS_Config(conf_test, 0))
        PrintOK();
    else
        PrintERR();
    

    FuncsGSMSendGPRS_Answer (2);
    unsigned char answer = 0;
    do {
        answer = VerifyAndSendGPRS(&packet);
        
        printf("answer getted: %d\n", answer);
        
    } while ((answer != GPRS_NOT_PROPER_DATA) &&
             (answer != GPRS_SENT) &&
             (answer != GPRS_NOT_SEND));

    printf("test ended\n");
        
}

void Test_GPRS_Config (void)
{
    char conf_test [165] = { 0 };
    char ip [20] = { 0 };
    char proto [20] = { 0 };
    char port [20] = { 0 };
    char apn [67] = { 0 };
    char domain [67] = { 0 };    
    
    // conf okeys
    strcpy(conf_test,"IP:192.168.1.1,PROTO:UDP,PORT:10000,APN:gprs.movistar.comOK");
    printf("Test gprs conf len %d %s :", strlen(conf_test), conf_test);
    unsigned char len = strlen(conf_test);
    *(conf_test + len - 2) = '\0';
    printf("Test gprs conf len %d %s :", strlen(conf_test), conf_test);    
    if (GPRS_Config(conf_test, 0))
        PrintOK();
    else
        PrintERR();

    strcpy(conf_test,"IP:2.168.1.1,PROTO:TCP,PORT:2,APN:gprs.movistar.com");
    printf("Test gprs conf len %d %s :", strlen(conf_test), conf_test);    
    if (GPRS_Config(conf_test, 0))
        PrintOK();
    else
        PrintERR();

    // conf errors
    strcpy(conf_test,"IP:2.168.1.1,PROTO:TCP,PORT:1,APN:gprs.movistar.com");
    printf("Test gprs error conf len %d %s :", strlen(conf_test), conf_test);
    if (!GPRS_Config(conf_test, 0))
        PrintOK();
    else
        PrintERR();

    // verify config flash
    strcpy(ip, "8.8.8.8");
    strcpy(proto, "UDP");
    strcpy(port, "8888");
    strcpy(apn, "gprs.movistar.com");
    sprintf(conf_test, "IP:%s,PROTO:%s,PORT:%s,APN:%s",
            ip,
            proto,
            port,
            apn);
    
    printf("Test gprs memory save len %d %s :", strlen(conf_test), conf_test);
    printf("\nmem ip %s", mem_conf.ip);
    printf("\nmem ip_proto %s", mem_conf.ip_proto);
    printf("\nmem ip_port %s", mem_conf.ip_port);
    printf("\nmem apn %s", mem_conf.apn);    
    if (GPRS_Config(conf_test, 0) &&
        (strncmp(mem_conf.ip, ip, strlen(ip)) == 0) &&
        (strncmp(mem_conf.ip_proto, proto, strlen(proto)) == 0) &&
        (strncmp(mem_conf.ip_port, port, strlen(port)) == 0) &&
        (strncmp(mem_conf.apn, apn, strlen(apn)) == 0))
        PrintOK();
    else
        PrintERR();

    printf("\nmem ip %s", mem_conf.ip);
    printf("\nmem ip_proto %s", mem_conf.ip_proto);
    printf("\nmem ip_port %s", mem_conf.ip_port);
    printf("\nmem apn %s", mem_conf.apn);    


    
    strcpy(domain, "vhfpilotage.duckdns.com");
    strcpy(proto, "TCP");
    strcpy(port, "8080");
    strcpy(apn, "gprs.movistar.com");
    sprintf(conf_test, "IPDN:%s,PROTO:%s,PORT:%s,APN:%s",
            domain,
            proto,
            port,
            apn);

    printf("Test gprs domain memory save len %d %s :", strlen(conf_test), conf_test);
    printf("\nmem domain %s", mem_conf.domain);
    printf("\nmem ip_proto %s", mem_conf.ip_proto);
    printf("\nmem ip_port %s", mem_conf.ip_port);
    printf("\nmem apn %s", mem_conf.apn);    
    if (GPRS_Config(conf_test, 1) &&
        (strncmp(mem_conf.domain, domain, strlen(domain)) == 0) &&
        (strncmp(mem_conf.ip_proto, proto, strlen(proto)) == 0) &&
        (strncmp(mem_conf.ip_port, port, strlen(port)) == 0) &&
        (strncmp(mem_conf.apn, apn, strlen(apn)) == 0))
        PrintOK();
    else
        PrintERR();
    
    printf("\nmem domain %s", mem_conf.domain);
    printf("\nmem ip_proto %s", mem_conf.ip_proto);
    printf("\nmem ip_port %s", mem_conf.ip_port);
    printf("\nmem apn %s", mem_conf.apn);    
    
    printf("\n");
}


void Test_Verify_IP_Protocol (void)
{
    char proto_test [80] = { 0 };

    strcpy(proto_test, "TCP");
    printf("Test verify ip protocol %s: ", proto_test);
    if (VerifyIPProtocol(proto_test, strlen(proto_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(proto_test, "UDP");
    printf("Test verify ip protocol %s: ", proto_test);
    if (VerifyIPProtocol(proto_test, strlen(proto_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(proto_test, "X25");
    printf("Test verify error on ip protocol %s: ", proto_test);
    if (!VerifyIPProtocol(proto_test, strlen(proto_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(proto_test, "TCP ");
    printf("Test verify error on ip protocol %s: ", proto_test);
    if (!VerifyIPProtocol(proto_test, strlen(proto_test)))
        PrintOK();
    else
        PrintERR();
    
    printf("\n");
}


void Test_Verify_APN (void)
{
    char apn_test [80] = { 0 };
    
    strcpy(apn_test, "gprs.personal.com");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "igprs.claro.com.ar");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "wap.gprs.unifon.com.ar");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "datos.personal.com");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();
    
    strcpy(apn_test, "gprs.personal.88com");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();

    strcpy(apn_test, "gprs-personal.88com");
    printf("Test verify apn: %s: ", apn_test);
    if (VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();
    
    strcpy(apn_test, "gprs.personal. com");
    printf("Test verify error on apn: %s: ", apn_test);
    if (!VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();    
    
    printf("\n");
    
}


void Test_Verify_IP_String (void)
{
    char ip1 [] = {"192.168.1.1"};
    char ip2 [] = {"2.168.1.1"};
    char ip3 [] = {"255.255.255.255"};
    char ip4 [] = {"2.168.1.300"};
    char ip5 [] = {"...1"};
    char ip6 [] = {"0.0.0.."};
    
    printf("Test verify ip %s: ", ip1);
    if (VerifyIPString(ip1, sizeof(ip1) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip %s: ", ip2);
    if (VerifyIPString(ip2, sizeof(ip2) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip %s: ", ip3);
    if (VerifyIPString(ip3, sizeof(ip3) - 1))
        PrintOK();
    else
        PrintERR();
    
    printf("Test verify error on ip %s: ", ip4);
    if (!VerifyIPString(ip4, sizeof(ip4) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify error on ip %s: ", ip5);
    if (!VerifyIPString(ip5, sizeof(ip5) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify error on ip %s: ", ip6);
    if (!VerifyIPString(ip6, sizeof(ip6) - 1))
        PrintOK();
    else
        PrintERR();
    
    
    printf("\n");
    
}


void Test_Verify_IP_Port (void)
{
    // ok ports
    char ip1 [] = {"11"};
    char ip2 [] = {"111"};
    char ip3 [] = {"1111"};
    char ip4 [] = {"11111"};
    char ip5 [] = {"65000"};
    char ip55 [] = {"2"};
    // ports with errors
    char ip6 [] = {"75000"};
    char ip7 [] = {"1"};    
    char ip8 [] = {"0"};
    
    printf("Test verify ip port %s: ", ip1);
    if (VerifyPort(ip1, sizeof(ip1) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip port %s: ", ip2);
    if (VerifyPort(ip2, sizeof(ip2) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip port %s: ", ip3);
    if (VerifyPort(ip3, sizeof(ip3) - 1))
        PrintOK();
    else
        PrintERR();
    
    printf("Test verify ip port %s: ", ip4);
    if (VerifyPort(ip4, sizeof(ip4) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip port %s: ", ip5);
    if (VerifyPort(ip5, sizeof(ip5) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify ip port %s: ", ip55);
    if (VerifyPort(ip55, sizeof(ip55) - 1))
        PrintOK();
    else
        PrintERR();
    
    printf("Test verify error on ip port %s: ", ip6);
    if (!VerifyPort(ip6, sizeof(ip6) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify error on ip port %s: ", ip7);
    if (!VerifyPort(ip7, sizeof(ip7) - 1))
        PrintOK();
    else
        PrintERR();

    printf("Test verify error on ip port %s: ", ip8);
    if (!VerifyPort(ip8, sizeof(ip8) - 1))
        PrintOK();
    else
        PrintERR();
    
    
    printf("\n");
    
}


// Module Mocked Functions -----------------------------------------------------
void ChangeLedActivate (unsigned char number)
{
    if (number > 1)
        printf("led change to %d pulses\n", number);
    else
        printf("led change to %d pulse\n", number);
    
}


void Activation_12V_On (void)
{
    printf("12V ON!\n");
}


void Activation_12V_Off (void)
{
    printf("12V OFF!\n");
}


unsigned char funcs_gsm_answer = resp_gsm_continue;
unsigned char FuncsGSMSendGPRS (gprs_pckt_t * packet)
{
    printf("  GPRS sent alarm: %d panel: %d remote: %d\n       this message: %s\n",
           packet->alarm_input,
           packet->panel_input,
           packet->remote_number,
           packet->buff);
    
    return funcs_gsm_answer;
}


void FuncsGSMSendGPRS_Answer (unsigned char answer)
{
    funcs_gsm_answer = answer;
}


// unsigned char funcs_gsm_answer = resp_gsm_continue;
unsigned char FuncsGSMSendSMS (char * msg, char * number)
{
    printf("  SMS  send to number: %s\n       this message: %s\n", number, msg);
    return funcs_gsm_answer;
}


// void FuncsGSMSendSMS_Answer (unsigned char answer)
// {
//     funcs_gsm_answer = answer;
// }


void MyCb (char * buff)
{
    printf("callback called %d\n", cb_cnt + 1);
    strcpy(&cb_buff[cb_cnt][0], buff);
    
    if (cb_cnt < 5)
        cb_cnt++;
    else
        cb_cnt = 0;
}


void MyCbClean (void)
{
    cb_cnt = 0;
    // memcpy(&cb_buff[0][0], '\0', 200);
    // memcpy(&cb_buff[1][0], '\0', 200);
    // memcpy(&cb_buff[2][0], '\0', 200);
    // memcpy(&cb_buff[3][0], '\0', 200);
    // memcpy(&cb_buff[4][0], '\0', 200);    
}

//--- end of file ---//


