//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "funcs_gsm.h"
#include "parameters.h"
#include "comm.h"
#include "sms_data.h"
#include "contact_id.h"

//helper modules
#include "tests_ok.h"

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */

#include <sys/ioctl.h>    //for line control
#include <pthread.h>

#include <string.h>


// Module Functions for testing ------------------------------------------------
int OpenSerialPort (int * file_descp);

void * SerialInput (void * arg);
void * KeyboardInput (void * arg);
void * MillisTimeout (void * arg);

void Activation_Input_SMS (void);
void SetDefaults (void);
int Activation_Input_GPRS (void);

// Module Auxialiary Functions (Mocked) ----------------------------------------
void Led_On (void);
void Led_Off (void);
unsigned char Led_Status (void);

void PwrKey_On (void);
void PwrKey_Off (void);

void Status_Set (unsigned char new_val);

void Wait_ms (unsigned short);
void Usart2Send (char * s);


// Externals for Modules -------------------------------------------------------
// - Externals Funcs GSM
unsigned char register_status = 0;
unsigned char rssi_level = 0;

parameters_typedef mem_conf;
char SIM900APNSIM1[64];
char SIM900USUARIOSIM1[20];
char SIM900CLAVESIM1[20];

char SIM900APNSIM2[64];
char SIM900USUARIOSIM2[20];
char SIM900CLAVESIM2[20];
char SIM900IPREMOTE[20];
char SIM900PORTREMOTE[20];

extern t_GsmState gsm_state;

// Globals ---------------------------------------------------------------------
int main_quit = 0;

// -- envios de usart1 al puerto serie
int usart1_data_len = 0;
int usart1_data_send = 0;
char usart1_data_buff_rx [256] = { '\0' };
char usart1_data_buff_tx [256] = { '\0' };
int usart1_data_receiv = 0;

int dtr_line = -1;
int activation_sms = 0;
int activation_gprs = 0;

// -- miliseconds delay
int millis = 0;

// sms buffer
char my_own_buff [SITE_MAX_LEN + 20] = { 0 };

// Module Functions ------------------------------------------------------------
void main(void)
{
    int fd;/*File Descriptor*/
    pthread_t p1, p2, p3;

    if (OpenSerialPort (&fd))
    {
        printf("\nOpening serial port: ");
        PrintOK();


        int rc;
        rc = pthread_create(&p1, NULL, SerialInput, (void *)&fd);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            _exit(-1);
        }

        int rc2;
        rc2 = pthread_create(&p2, NULL, MillisTimeout, (void *)&rc2);
        if (rc2){
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            _exit(-1);
        }

        int rc3;
        rc3 = pthread_create(&p3, NULL, KeyboardInput, (void *)&rc3);
        if (rc3){
            printf("ERROR; return code from pthread_create() is %d\n", rc3);
            _exit(-1);
        }
        
    }
    else
    {
        printf("\nOpening serial port: ");
        PrintERR();
        return;
    }

    int DTR_flag;
    DTR_flag = TIOCM_DTR;

    SetDefaults ();
    
    while (!main_quit)
    {
        if (usart1_data_send)
        {
            usart1_data_send = 0;
            write (fd, &usart1_data_buff_tx, usart1_data_len);
        }

        if (dtr_line == 1)
        {
            dtr_line = -1;
            ioctl(fd, TIOCMBIC, &DTR_flag);
            printf("setting DTR\n");
        }

        if (dtr_line == 0)
        {
            dtr_line = -1;
            ioctl(fd, TIOCMBIS, &DTR_flag);
            printf("clearing DTR\n");
        }

        if (activation_sms)
        {
            activation_sms = 0;
            printf("Activation Input for SMS!\n");
            Activation_Input_SMS();
        }

        if (activation_gprs)
        {
            if (activation_gprs != 2)
            {
                printf("Activation Input for GPRS!\n");
                activation_gprs++;
            }
            
            int answer = Activation_Input_GPRS ();

            if (answer)
            {
                printf("end of GPRS routine\n");
                activation_gprs = 0;
            }
        }
        
        // for the function
        FuncsGSM();

        // give back control to OS
        usleep(1000);    //sleep 1ms

    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    pthread_join(p3, NULL);    


    // close(fd);
    // pongo status en 1
    // Status_Set (1);
    // for (int i = 0; i < 20000; i++)
    // {
        // // for the function
        // FuncsGSM();

        // // for the timeouts
        // GSMTimeoutCounters ();
        
    // }
}


// Module Auxialiary Functions -------------------------------------------------
int OpenSerialPort (int * file_descp)
{
    int fd;    //local fd
    
    printf("\n +----------------------------------+");
    printf("\n |        Serial Port Write         |");
    printf("\n +----------------------------------+");

    /*------------------------------- Opening the Serial Port -------------------------------*/

    /* Change /dev/ttyUSB0 to the one corresponding to your system */

    // ttyUSB0 is the FT232 based USB2SERIAL Converter
    fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY);
    // fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);
    
    /* O_RDWR Read/Write access to serial port           */
    /* O_NOCTTY - No terminal will control the process   */
    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
    /* -the status of DCD line,Open() returns immediatly */                                        
									
    if(fd == -1)						/* Error Checking */
    {
        printf("\n  Error! in Opening ttyUSB0  ");
        return 0;
    }
    else
        printf("\n  ttyUSB0 Opened Successfully ");

	
    /*---------- Setting the Attributes of the serial port using termios structure --------- */
		
    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */
	
    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
		
		
    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag = 0;
    SerialPortSettings.c_lflag = 0;    

    /* Setting Time outs */
    // SerialPortSettings.c_cc[VMIN] = 10; /* Read at least 10 characters */
    // SerialPortSettings.c_cc[VTIME] = 0; /* Wait indefinetly   */
    SerialPortSettings.c_cc[VMIN] = 1;
    // SerialPortSettings.c_cc[VTIME] = 1;    //100ms entre lecturas
    SerialPortSettings.c_cc[VTIME] = 5;    //500ms entre lecturas
    
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    {
        printf("\n  ERROR ! in Setting attributes");
        return 0;
    }
    else
        printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
			
    /*------------------------------- Write data to serial port -----------------------------*/

    char write_buffer[] = "ANSWER";	/* Buffer containing characters to write into port	     */	
    int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */ 

    bytes_written = write(fd,write_buffer,sizeof(write_buffer));/* use write() to send data to port                                            */
    /* "fd"                   - file descriptor pointing to the opened serial port */
    /*	"write_buffer"         - address of the buffer containing data	            */
    /* "sizeof(write_buffer)" - No of bytes to write                               */	
    printf("\n  %s written to ttyUSB0",write_buffer);
    printf("\n  %d Bytes written to ttyUSB0", bytes_written);
    printf("\n +----------------------------------+\n\n");

    /*------------------------------- Read data from serial port -----------------------------*/

    tcflush(fd, TCIFLUSH);   /* Discards old data in the rx buffer            */

    // char read_buffer[32];   /* Buffer to store the data received              */
    // int  bytes_read = 0;    /* Number of bytes read by the read() system call */
    // int i = 0;

    // bytes_read = read(fd,&read_buffer,32); /* Read the data                   */
			
    // printf("\n\n  Bytes Rxed -%d", bytes_read); /* Print the number of bytes read */
    // printf("\n\n  ");

    // for(i=0;i<bytes_read;i++)	 /*printing only the received characters*/
    //     printf("%c",read_buffer[i]);
	
    // printf("\n +----------------------------------+\n\n\n");

    *file_descp = fd;
    return 1;
}


int pckt_flag = 0;
int pckt_bytes = 0;
int buffer_rx_full = 0;
void * SerialInput (void * arg)
{
    int fd = *(int *) arg;
    int bytes_read = 0;
    char buff_rx [256] = { '\0' };


    printf("thread fd: %d\n", fd);
    while (!main_quit)
    {
        memset(buff_rx, '\0', sizeof(buff_rx));
        bytes_read = read(fd, buff_rx, sizeof(buff_rx));        

        if (bytes_read > 0)
        {
            pckt_flag = 1;
            if (sizeof(usart1_data_buff_rx) > pckt_bytes + bytes_read)
            {
                strcpy((char *) (usart1_data_buff_rx + pckt_bytes), buff_rx);
                // strncpy((char *) (usart1_data_buff_rx + pckt_bytes), buff_rx, bytes_read);
                // strncpy((char *) (usart1_data_buff_rx + pckt_bytes + bytes_read), '\0', 1);
                pckt_bytes += bytes_read;
            }
            else
            {
                PrintBoldWhite("rx buff full\n");
                tcflush(fd, TCIFLUSH);    // Discards old data in the rx buffer
                pckt_bytes = 0;
                pckt_flag = 0;            
            }
        }
        else if (pckt_flag)
        {
            char buff [20];
            sprintf(buff, "rx %d <- ", pckt_bytes);
            PrintBoldWhite(buff);
            printf("%s\n", usart1_data_buff_rx);
            pckt_bytes = 0;
            pckt_flag = 0;            
            usart1_data_receiv = 1;
        }

        usleep(2000);
    }

    pthread_exit(NULL);
    
}


void * KeyboardInput (void * arg)
{
    int loop = 1;
    char key;

    while(loop)
    {
        // printf("\ncharacter to send (q to quit): ");
        scanf(" %c", &key);
        if (key == 'q')
        {
            loop = 0;
            main_quit = 1;
            printf("ending simulation\n");
        }
        else if (key == '1')
        {
            dtr_line = 1;
        }
        else if (key == '0')
        {
            dtr_line = 0;            
        }
        else if (key == 's')
        {
            activation_sms = 1;
        }
        else if (key == 'g')
        {
            activation_gprs = 1;
        }
        else if (key == 'm')
        {
            printf("\n  Memory Params...\n");
            printf("  num_reportar: %s\n", mem_conf.num_reportar);
            printf("  imei: %s\n", mem_conf.imei);
            printf("  num_propio: %s\n", mem_conf.num_propio);
            printf("  sitio_propio: %s\n", mem_conf.sitio_propio);

            printf("  ip1: %s\n", mem_conf.ip1);
            printf("  ip_port1: %s\n", mem_conf.ip_port1);
            printf("  ip2: %s\n", mem_conf.ip2);
            printf("  ip_port2: %s\n", mem_conf.ip_port2);
            printf("  ip_proto: %s\n", mem_conf.ip_proto);
            printf("  apn: %s\n", mem_conf.apn);
            printf("  client_number: %s\n", mem_conf.client_number);
            printf("  keepalive: %d\n", mem_conf.keepalive);

            printf("\n  Backup Flags...\n");
            printf("  bkp_envios_ok: %d\n", mem_conf.bkp_envios_ok);
            printf("  bkp_timer_reportar: %d\n", mem_conf.bkp_timer_reportar);
            printf("  bkp_prender_ring: %d\n", mem_conf.bkp_prender_ring);
            printf("  bkp_sense_bat: %d\n", mem_conf.bkp_sense_bat);
            printf("  bkp_socket_use: %d\n", mem_conf.bkp_socket_use);
            
        }
        else
        {
            // send_char = key;
            // data_write = 1;
        }
    }
    
    pthread_exit(NULL);
}


void * MillisTimeout (void * arg)
{
    int millis = 0;
    
    while(!main_quit)
    {
        usleep(1000);    //sleep 1ms

        // for the timeouts
        GSMTimeoutCounters ();

        // for the funcs_gsm timeouts
        FuncsGSMTimeoutCounters ();
        
        if (millis < 5000)
            millis++;
        else
        {
            printf("gsm_state: %d ", gsm_state);
            printf("(q to quit) (1/0 DTR) (s act sms) (g act gprs) (m for mem_conf)\n");
            millis = 0;
        }
        
    }
    
    pthread_exit(NULL);
}


void Activation_Input_SMS (void)
{
    unsigned char sms_ready = 1;

    //check num_tel_rep before send sms
    if (sms_ready)
    {
        sms_ready = VerifyNumberString(num_tel_rep);

        if (!sms_ready)
        {
            Usart2Send("sin numero\n");
            return;
        }
    }

    //check sitio_prop before send sms
    if (sms_ready)
    {
        sms_ready = VerifySiteString(sitio_prop);

        if (!sms_ready)
        {
            Usart2Send("sin sitio\n");
            return;
        }
    }

    if (sms_ready)
    {
        strcpy(my_own_buff, "Activacion en: ");
        strcat(my_own_buff, sitio_prop);
        // printf("reportar: %s\n", buff);
    }

    unsigned char answer = SMS_NOT_SEND;
    
    // check data and send a GSM packet
    answer = VerifyAndSendSMS (my_own_buff);

    if (answer == SMS_NOT_SEND)
    {
        Usart2Send("sms bad network\n");                    
    }

    if (answer == SMS_SENT)
    {
        Usart2Send("sms packet load OK give some time...\n");
    }
    
}


int Activation_Input_GPRS (void)
{
    unsigned char answer = GPRS_WORKING;
    
    // check data to send a GPRS packet
    ContactIDString(panic_alarm, mem_conf.client_number, "989", my_own_buff);
    answer = VerifyAndSendGPRS(my_own_buff);
            
    if (answer == GPRS_NOT_SEND)
    {
        Usart2Send("gprs bad network go to sms\n");
        return 1;
    }

    if (answer == GPRS_SENT)
    {
        Usart2Send("gprs packet sent OK\n");
        return 1;
    }

    return 0;
}


void SetDefaults (void)
{
    // For SMS communications
    // sitio
    strcpy(mem_conf.sitio_propio, "Kirno 457");
    // num reporte
    strcpy(mem_conf.num_reportar, "1145376762");

    // For GPRS connections
    strcpy(mem_conf.apn, "gprs.personal.com");
    strcpy(mem_conf.ip_proto, "UDP");
    strcpy(mem_conf.ip1, "186.18.4.68");
    strcpy(mem_conf.ip_port1, "11000");

    // For monitoring system
    strcpy(mem_conf.client_number, "CB53");

}


// Module Mocked Functions -----------------------------------------------------
unsigned char led_status = 0;
void Led_On (void)
{
    printf("LED_ON\n");
    led_status = 1;
}


void Led_Off (void)
{
    printf("LED_OFF\n");
    led_status = 0;
}


unsigned char Led_Status (void)
{
    return led_status;
}

void Wait_ms (unsigned short a)
{
    
}


unsigned char status_status = 0;
unsigned char Status_Status (void)
{
    return status_status;
}


void Status_Set (unsigned char new_val)
{
    status_status = new_val;
}


unsigned char pwrkey_status = 0;
void PwrKey_On (void)
{
    printf("PWRKEY_ON\n");
    pwrkey_status = 1;
    // if (status_status)
    //     status_status = 0;
    // else
    status_status = 1;

}


void PwrKey_Off (void)
{
    printf("PWRKEY_OFF\n");
    pwrkey_status = 0;
    status_status = 0;
}


unsigned char PwrKey_Status (void)
{
    return pwrkey_status;
}


unsigned char netlight_status = 0;
unsigned char NetLight_Status (void)
{
    return netlight_status;
}


void Usart2Send (char * s)
{
    PrintCyan("debug port -> ");
    printf("%s\n", s);
}


void Usart2Debug (char * s)
{
    int len = strlen(s);
    PrintCyan("debug port -> ");
    printf("%d bytes: %s\n", len, s);
}


void Usart1Send (char * s)
{
    PrintBoldWhite("tx -> ");
    printf("%s\n", s);

    strcpy(usart1_data_buff_tx, s);
    usart1_data_len = strlen(s);
    usart1_data_send = 1;
}


unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(usart1_data_buff_rx);
    if (max_len > len)
        strcpy(bout, usart1_data_buff_rx);
    else
    {
        printf("error on Usart1ReadBuffer max_len\n");
        len = 0;
    }

    return len;
}


unsigned char Usart1HaveData (void)
{
    return (unsigned char) usart1_data_receiv;
}


void Usart1HaveDataReset (void)
{
    usart1_data_receiv = 0;
}


// Mock battery.c module -----------
void Battery_Voltage (unsigned char * v_int, unsigned char * v_dec)
{
    *v_int = 13;
    *v_dec = 8;
}


//--- end of file ---//


