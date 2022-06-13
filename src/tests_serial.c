//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
// #include "funcs_gsm_gateway.h"

//helper modules
#include "tests_ok.h"

#include <stdio.h>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */ 
#include <errno.h>   /* ERROR Number Definitions           */

#include <sys/ioctl.h>    //for line control
#include <pthread.h>


// Module Functions for testing ------------------------------------------------
int OpenSerialPort (int * file_descp);

void * SerialInput (void * arg);
void * KeyboardInput (void * arg);


// Globals ---------------------------------------------------------------------
int main_quit = 0;

int keep_alive = 1;
int data_ready = 0;
char buff_rx [256] = { '\0' };

int data_write = 0;
char send_char = '0';

int dtr_line = -1;

// Module Functions ------------------------------------------------------------
void main(void)
{
    int fd;/*File Descriptor*/
    pthread_t p1, p2;

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
        rc2 = pthread_create(&p2, NULL, KeyboardInput, (void *)&rc2);
        if (rc2){
            printf("ERROR; return code from pthread_create() is %d\n", rc2);
            _exit(-1);
        }
    }
    else
    {
        printf("\nOpening serial port: ");
        PrintERR();
        _exit(1);
    }

    int DTR_flag;
    DTR_flag = TIOCM_DTR;
    
    while (!main_quit)
    {
        if (data_ready)
        {
            data_ready = 0;
            printf("usart rx: %s\n", buff_rx);
        }

        if (data_write)
        {
            data_write = 0;
            write (fd, &send_char, 1);
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
    }

    keep_alive = 0;
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);


    close(fd);
}


int OpenSerialPort (int * file_descp)
{
    int fd;    //local fd
    
    printf("\n +----------------------------------+");
    printf("\n |        Serial Port Write         |");
    printf("\n +----------------------------------+");

    /*------------------------------- Opening the Serial Port -------------------------------*/

    /* Change /dev/ttyUSB0 to the one corresponding to your system */

    // fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
    fd = open("/dev/ttyUSB0",O_RDWR | O_NOCTTY | O_NDELAY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
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
    // SerialPortSettings.c_cc[VMIN] = 0;
    // SerialPortSettings.c_cc[VTIME] = 5;    //500ms 
    SerialPortSettings.c_cc[VMIN] = 1;
    SerialPortSettings.c_cc[VTIME] = 5;    //500ms 
    
    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
    {
        printf("\n  ERROR ! in Setting attributes");
        return 0;
    }
    else
        printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
			
    /*------------------------------- Write data to serial port -----------------------------*/

    char write_buffer[] = "A";	/* Buffer containing characters to write into port	     */	
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


void * SerialInput (void * arg)
{
    int fd = *(int *) arg;
    int bytes_read = 0;

    printf("thread fd: %d\n", fd);
    while (keep_alive)
    {
        bytes_read = read(fd, &buff_rx, sizeof(buff_rx));
        usleep(5000);
        
        if (bytes_read > 0)
        {
            PrintBoldWhite("rx <- ");
            printf("%s\n", buff_rx);    
        }
    }

    pthread_exit(NULL);
    
}


void * KeyboardInput (void * arg)
{
    int loop = 1;
    char key;

    while(loop)
    {
        printf("\ncharacter to send (q to quit): ");
        scanf(" %c", &key);
        if (key == 'q')
        {
            loop = 0;
            main_quit = 1;
        }
        else if (key == '1')
        {
            dtr_line = 1;
        }
        else if (key == '0')
        {
            dtr_line = 0;            
        }
        else
        {
            send_char = key;
            data_write = 1;
        }
    }
    
    pthread_exit(NULL);
}


//--- end of file ---//


