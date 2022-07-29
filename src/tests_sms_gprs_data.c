//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "sms_gprs_data.h"
#include "parameters.h"
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
extern unsigned char VerifyKeepNumber (unsigned short keep);
extern unsigned char VerifyKeepString (char * keep, unsigned char len);
extern unsigned char VerifyDomainString (char * keep, unsigned char len);


// Module Mocked Functions -----------------------------------------------------
// void ChangeLedActivate (unsigned char number);
void MyCb (char * buff);
void MyCbClean (void);
void FuncsGSMSendGPRS_Answer (unsigned char answer);


// Module Functions for testing ------------------------------------------------
void Test_Verify_APN (void);
void Test_Verify_Domain (void);
void Test_Verify_IP_Protocol (void);
void Test_Verify_IP_String (void);
void Test_Verify_IP_Port (void);
void Test_Verify_Keep_String (void);

void Test_GPRS_Config (void);
void Test_GPRS_Send (void);
void Test_Verify_Socket_Data (void);

///////////////////////////
// Sms Testing Functions //
///////////////////////////
void Test_Verify_Site (void);
void Test_Verify_Number (void);
void Test_Verify_And_Send_SMS (void);


// Module Functions ------------------------------------------------------------
int main(int argc, char *argv[])
{
    ///////////////////////////
    // Sms Testing Functions //
    ///////////////////////////
    Test_Verify_Site ();

    Test_Verify_Number ();

    ////////////////////////////
    // Gprs Testing Functions //
    ////////////////////////////
    Test_Verify_APN ();

    Test_Verify_Domain ();

    Test_Verify_IP_String ();

    Test_Verify_IP_Protocol ();

    Test_Verify_IP_Port ();

    Test_Verify_Keep_String ();

    Test_Verify_Socket_Data ();
    
    // Test_Verify_And_Send_SMS ();

    Test_GPRS_Config ();

    // Test_GPRS_Send ();
}


void Test_Verify_Socket_Data (void)
{
    
    printf("verify socket data for ip1\n");
    strcpy(mem_conf.ip1, "ho.la");
    printf("  error on ip1: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.ip1, "255.255.255.255");
    strcpy(mem_conf.ip_port1, "67000");
    printf("  error on ip_port1: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.ip_port1, "10000");
    strcpy(mem_conf.ip_proto, " ");    
    printf("  error on ip_proto: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.ip_proto, "UDP");
    strcpy(mem_conf.apn, "  ");    
    printf("  error on apn: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.apn, "gprs.perso.com");
    strcpy(mem_conf.client_number, "  ");
    printf("  error on client_number: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.client_number, "CB53");
    mem_conf.keepalive = 1000;
    printf("  error on keepalive: ");
    if (!VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    mem_conf.keepalive = 120;
    printf("  all data ok for ip1: ");
    if (VerifySocketData(IP1))
        PrintOK();
    else
        PrintERR();

    
    printf("\nverify socket data for ip2\n");
    strcpy(mem_conf.ip2, "ho.la");
    printf("  error on ip2: ");
    if (!VerifySocketData(IP2))
        PrintOK();
    else
        PrintERR();

    strcpy(mem_conf.ip2, "255.255.255.255");
    strcpy(mem_conf.ip_port2, " ");    
    printf("  error on ip_port2: ");
    if (!VerifySocketData(IP2))
        PrintOK();
    else
        PrintERR();
    
    strcpy(mem_conf.ip_port2, "255");
    printf("  all data ok for ip2: ");
    if (VerifySocketData(IP2))
        PrintOK();
    else
        PrintERR();
    
    
}
// void Test_Verify_And_Send_SMS (void)
// {
//     char serial_buff [200];
//     char buff [200];
//     sms_pckt_t sms_info;
//     unsigned char answer = 0;

//     // load defaults
//     mem_conf.
//     mem_conf..panel_input = 0;
//     mem_conf..remote_number = 0;
//     mem_conf..buff = buff;

//     strcpy(mem_conf.num_reportar, " ");
    
//     // test for alarm input
//     Usart2Callback(&MyCb);
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms alarm input: ");
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no phone number", sizeof("no phone number") - 1)) &&        
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();


//     // test for panel input
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input: ");
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no phone number", sizeof("no phone number") - 1)) &&        
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();


//     // test for alarm input with phone
//     sms_info.alarm_input = 1;
//     sms_info.panel_input = 0;
//     strcpy(mem_conf.num_reportar, "+541122");
//     MyCbClean();
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s: ", mem_conf.num_reportar);
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (!(strncmp(&cb_buff[1][0], "no site saved", sizeof("no site saved") - 1)) &&
//          (answer == SMS_NOT_PROPER_DATA)))
//         PrintOK();
//     else
//         PrintERR();

//     // test for alarm input with phone and site
//     sms_info.alarm_input = 1;
//     sms_info.panel_input = 0;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_ok);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms alarm input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "External 12V: ", sizeof("External 12V: ") - 1)) &&
//         (answer == SMS_SENT))
//         PrintOK();
//     else
//         PrintERR();

//     // test for panel input with phone and site
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_ok);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&    
//         (answer == SMS_SENT))
//         PrintOK();
//     else
//         PrintERR();


//     // test for panel input with phone and site not send
//     sms_info.alarm_input = 0;
//     sms_info.panel_input = 1;
//     strcpy(mem_conf.num_reportar, "+541122");
//     strcpy(mem_conf.sitio_propio, "quirno 457");
//     MyCbClean();
//     FuncsGSMSendSMS_Answer(resp_gsm_error);
//     answer = VerifyAndSendSMS (&sms_info);

//     Usart2ReadBuffer(serial_buff, sizeof(serial_buff));

//     printf("Test send sms panel input phone %s site %s: ",
//            mem_conf.num_reportar,
//            mem_conf.sitio_propio);
//     if (!(strncmp(&cb_buff[0][0], "Keypad ACT: ", sizeof("Keypad ACT: ") - 1)) &&    
//         (answer == SMS_NOT_SEND))
//         PrintOK();
//     else
//         PrintERR();
    
    
// }


void Test_Verify_Number (void)
{
    char number_test [40] = { 0 };
    strcpy(number_test, "11567");
    printf("Test verify number %d: %s: ", strlen(number_test), number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();
    
    strcpy(number_test, "012345678901234567890");
    printf("Test verify long number %d: %s: ", strlen(number_test), number_test);
    if (!VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+110123456789");
    printf("Test verify number plus signed %d: %s: ", strlen(number_test), number_test);
    if (VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+11012 3456789");
    printf("Test verify broken number: %s: ", number_test);
    if (!VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();

    strcpy(number_test, "+11012a");
    printf("Test verify mixed number: %s: ", number_test);
    if (!VerifyNumberString(number_test))
        PrintOK();
    else
        PrintERR();
    
}


void Test_Verify_Site (void)
{
    char my_new_site2 [] = {"ALARMA VECINAL QUIRNO 457 CIUDAD AUTONOMA DE BUENOS AIRES ENTRE CALLE AV DIRECTORIO Y TANDIL CODIGO POSTAL 1406"};
    
    printf("\n");
    char site_test [80] = { 0 };
    strcpy(site_test, "La esquina de los pibes");
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    strcpy(site_test, "La esquina..~ 345 de los pibes");
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    strcpy(site_test, my_new_site2);
    printf("Test verify site: %s: ", site_test);
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    printf("after verifing: %s\n", site_test);

    char too_long_site [200];
    char too_long_s [] = {"00 ALARMA 0123456789"};

    sprintf(too_long_site, "%s %s %s %s %s %s %s %s",
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s,
           too_long_s);

    printf("Verify site too long: \n%s\n", too_long_site);

    printf("Test site too long: ");    
    if (!VerifySiteString(too_long_site))
        PrintOK();
    else
        PrintERR();

    char too_short_site [] = {"ho"};

    printf("Verify site too short: \n%s\n", too_short_site);

    printf("Test site too short: ");    
    if (!VerifySiteString(too_short_site))
        PrintOK();
    else
        PrintERR();
    
    printf("Verify accent letters A E I O U: ");
    strcpy(site_test, "аимсз");
    if (VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();

    printf("Verify char numbers above 127: ");
    sprintf(site_test, "%s", " some test");
    site_test[0] = 128;
    if (!VerifySiteString(site_test))
        PrintOK();
    else
        PrintERR();
    
    printf("\n");
    
}


void Test_Verify_Keep_String (void)
{
    char keep_str [20]; 
    unsigned char len = 0;
    
    strcpy(keep_str, "09");    
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();

    strcpy(keep_str, "1440");    
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();

    strcpy(keep_str, "999");
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();

    strcpy(keep_str, "256");
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();
    
    strcpy(keep_str, "1000");
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();

    strcpy(keep_str, "1aa");
    printf("Verify keep string error %s: ", keep_str);
    len = strlen(keep_str);
    if (!VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();
    
    strcpy(keep_str, "100");
    printf("Verify keep string ok %s: ", keep_str);
    len = strlen(keep_str);
    if (VerifyKeepString(keep_str, len))
        PrintOK();
    else
        PrintERR();
    
    printf("\n");
    
}


// void Test_GPRS_Send (void)
// {
//     gprs_pckt_t packet;
//     char conf_test [165] = { 0 };
//     char buff_to_send [] = {"activacion"};

//     packet.alarm_input = 1;
//     packet.panel_input = 0;
//     packet.remote_number = 505;
//     packet.buff = buff_to_send;

//     printf("test start send gprs\n");

//     strcpy(mem_conf.sitio_propio, "Mi sitio");

//     // conf okeys
//     strcpy(conf_test,"IP:192.168.1.1,PROTO:UDP,PORT:10000,APN:gprs.movistar.com");
//     printf("Test gprs conf len %d %s :", strlen(conf_test), conf_test);
//     if (GPRS_Config(conf_test, 0))
//         PrintOK();
//     else
//         PrintERR();
    

//     FuncsGSMSendGPRS_Answer (2);
//     unsigned char answer = 0;
//     do {
//         answer = VerifyAndSendGPRS(&packet);
        
//         printf("answer getted: %d\n", answer);
        
//     } while ((answer != GPRS_NOT_PROPER_DATA) &&
//              (answer != GPRS_SENT) &&
//              (answer != GPRS_NOT_SEND));

//     printf("test ended\n");
        
// }

void Test_GPRS_Config (void)
{
    char conf_test [165] = { 0 };
    
    // conf ip1 with trailing OK
    strcpy(conf_test,"IP1:192.168.1.1,PORT1:10000,PROTO:UDP,APN:gprs.movistar.com,CLI:554433,KEEP:60OK");
    printf("Test gprs conf ip1 len %d %s :", strlen(conf_test), conf_test);
    unsigned char len = strlen(conf_test);
    *(conf_test + len - 2) = '\0';
    printf("Test gprs conf ip1 len %d %s :", strlen(conf_test), conf_test);    
    if (VerifyGPRSConfig_IP1(conf_test))
        PrintOK();
    else
        PrintERR();
    

    // conf ip1 with errors
    strcpy(conf_test,"IP:2.168.1.1,PROTO:TCP,PORT:2,APN:gprs.movistar.com");
    printf("Test gprs conf ip1 len %d %s :", strlen(conf_test), conf_test);    
    if (!VerifyGPRSConfig_IP1(conf_test))
        PrintOK();
    else
        PrintERR();

    // strcpy(conf_test,"IP:2.168.1.1,PROTO:TCP,PORT:1,APN:gprs.movistar.com");
    // printf("Test gprs error conf len %d %s :", strlen(conf_test), conf_test);
    // if (!GPRS_Config(conf_test, 0))
    //     PrintOK();
    // else
    //     PrintERR();

    // // verify config flash
    // strcpy(ip, "8.8.8.8");
    // strcpy(proto, "UDP");
    // strcpy(port, "8888");
    // strcpy(apn, "gprs.movistar.com");
    // sprintf(conf_test, "IP:%s,PROTO:%s,PORT:%s,APN:%s",
    //         ip,
    //         proto,
    //         port,
    //         apn);
    
    // printf("Test gprs memory save len %d %s :", strlen(conf_test), conf_test);
    // printf("\nmem ip %s", mem_conf.ip);
    // printf("\nmem ip_proto %s", mem_conf.ip_proto);
    // printf("\nmem ip_port %s", mem_conf.ip_port);
    // printf("\nmem apn %s", mem_conf.apn);    
    // if (GPRS_Config(conf_test, 0) &&
    //     (strncmp(mem_conf.ip, ip, strlen(ip)) == 0) &&
    //     (strncmp(mem_conf.ip_proto, proto, strlen(proto)) == 0) &&
    //     (strncmp(mem_conf.ip_port, port, strlen(port)) == 0) &&
    //     (strncmp(mem_conf.apn, apn, strlen(apn)) == 0))
    //     PrintOK();
    // else
    //     PrintERR();

    // printf("\nmem ip %s", mem_conf.ip);
    // printf("\nmem ip_proto %s", mem_conf.ip_proto);
    // printf("\nmem ip_port %s", mem_conf.ip_port);
    // printf("\nmem apn %s", mem_conf.apn);    


    
    // strcpy(domain, "vhfpilotage.duckdns.com");
    // strcpy(proto, "TCP");
    // strcpy(port, "8080");
    // strcpy(apn, "gprs.movistar.com");
    // sprintf(conf_test, "IPDN:%s,PROTO:%s,PORT:%s,APN:%s",
    //         domain,
    //         proto,
    //         port,
    //         apn);

    // printf("Test gprs domain memory save len %d %s :", strlen(conf_test), conf_test);
    // printf("\nmem domain %s", mem_conf.domain);
    // printf("\nmem ip_proto %s", mem_conf.ip_proto);
    // printf("\nmem ip_port %s", mem_conf.ip_port);
    // printf("\nmem apn %s", mem_conf.apn);    
    // if (GPRS_Config(conf_test, 1) &&
    //     (strncmp(mem_conf.domain, domain, strlen(domain)) == 0) &&
    //     (strncmp(mem_conf.ip_proto, proto, strlen(proto)) == 0) &&
    //     (strncmp(mem_conf.ip_port, port, strlen(port)) == 0) &&
    //     (strncmp(mem_conf.apn, apn, strlen(apn)) == 0))
    //     PrintOK();
    // else
    //     PrintERR();
    
    // printf("\nmem domain %s", mem_conf.domain);
    // printf("\nmem ip_proto %s", mem_conf.ip_proto);
    // printf("\nmem ip_port %s", mem_conf.ip_port);
    // printf("\nmem apn %s", mem_conf.apn);    

    // conf ip2 with trailing OK
    strcpy(conf_test,"IP2:192.168.1.1,PORT2:10000OK");
    printf("Test gprs conf ip2 len %d %s :", strlen(conf_test), conf_test);
    len = strlen(conf_test);
    *(conf_test + len - 2) = '\0';
    printf("Test gprs conf ip2 len %d %s :", strlen(conf_test), conf_test);    
    if (VerifyGPRSConfig_IP2(conf_test))
        PrintOK();
    else
        PrintERR();
    
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

    memset(apn_test, 'm', 68);
    printf("Test verify error too long on apn: %s: ", apn_test);
    if (!VerifyAPNString(apn_test, strlen(apn_test)))
        PrintOK();
    else
        PrintERR();    
    
    printf("\n");
    
}


void Test_Verify_Domain (void)
{
    char domain [80] = { 0 };
    
    strcpy(domain, "gprs.personal.com");
    printf("Test verify domain ok: %s: ", domain);
    if (VerifyDomainString(domain, strlen(domain)))
        PrintOK();
    else
        PrintERR();

    strcpy(domain, "gprs.personal. com");
    printf("Test verify domain error: %s: ", domain);
    if (!VerifyDomainString(domain, strlen(domain)))
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

    char new_ip [22];
    strcpy(new_ip, "01234567890123456789");
    printf("Test verify error on ip %s: ", new_ip);
    if (!VerifyIPString(new_ip, strlen(new_ip)))
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

    char new_port[22];
    strcpy(new_port, "aaaa");
    printf("Test verify error on ip port %s: ", new_port);
    if (!VerifyPort(new_port, strlen(new_port)))
        PrintOK();
    else
        PrintERR();

    strcpy(new_port, "112233");
    printf("Test verify error on ip port %s: ", new_port);
    if (!VerifyPort(new_port, strlen(new_port)))
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


void Usart2Debug (char * s)
{
    int len = strlen(s);
    PrintCyan("debug port -> ");
    printf("%d bytes: %s\n", len, s);
}


unsigned char funcs_gsm_answer = resp_gsm_continue;
unsigned char FuncsGSMSendGPRS (char * packet, unsigned char which_ip)
{
    // printf("  GPRS sent alarm: %d panel: %d remote: %d\n       this message: %s\n",
    //        packet->alarm_input,
    //        packet->panel_input,
    //        packet->remote_number,
    //        packet->buff);
    
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


