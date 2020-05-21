//----------------------------------------------------
// #### PROYECTO KIRNO REDONDA GSM - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPS_VKTEK.H ##################################
//----------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef GPS_VKTEL_H_
#define GPS_VKTEL_H_

// Includes for Configurations -------------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"

//--- Definicion de pines de hardware, los que no se conecten en la placa utilizar 1 (o lo que corresponda)
//--- se relacionanan con los nombres de hard.h

//--- GPS_PIN - salida al modulo ---//
#define GPS_PIN			EN_GPS
#define GPS_PIN_DISA	EN_GPS_OFF
#define GPS_PIN_ENA		EN_GPS_ON

//--- GPS_PPS - entrada desde el modulo ---//
#define GPS_PPS			PPS




typedef enum  {

	GPS_INIT = 0,
	GPS_INIT2,
	GPS_INIT3,
	GPS_INIT4,
	GPS_INIT5,
	GPS_INIT6,
	GPS_INIT7,
	GPS_INIT8,
	GPS_INIT9,
	GPS_INIT10,
	GPS_INIT11,
	GPS_INIT12,
	GPS_SAVE

} GPSState;

//--- Timeouts del GPS y del GSM ---//
#define TT_GPS_MINI		5


#define SIZEOF_GPSBUFF	128

// ------- String de configuracion del GPS -------
#define One_Output_Ten_Secs_String_Cmd	{ 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x10, 0x27, 0x01, 0x00, 0x01, 0x00, 0x4D, 0xDD, 0xB5, 0x62, 0x06, 0x08, 0x00, 0x00, 0x0E, 0x30 }
#define One_Output_One_Secs_String_Cmd	{ 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xE8, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x39 }

#define Disable_GPDTM_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x44, 0x54, 0x4d, 0x2a, 0x33, 0x42, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x0a, 0x00, 0x04, 0x23 }
#define Disable_GPGBS_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x42, 0x53, 0x2a, 0x33, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x09, 0x00, 0x03, 0x21 }
#define Disable_GPGGA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x47, 0x41, 0x2a, 0x32, 0x37, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x00, 0x00, 0xfa, 0x0f }
#define Disable_GPGLL_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x4c, 0x4c, 0x2a, 0x32, 0x31, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x01, 0x00, 0xfb, 0x11 }
#define Disable_GPGRS_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x52, 0x53, 0x2a, 0x32, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x06, 0x00, 0x00, 0x1b }
#define Disable_GPGSA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x41, 0x2a, 0x33, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x02, 0x00, 0xfc, 0x13 }
#define Disable_GPGST_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x54, 0x2a, 0x32, 0x36, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x07, 0x00, 0x01, 0x1d }
#define Disable_GPGSV_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x56, 0x2a, 0x32, 0x34, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x03, 0x00, 0xfd, 0x15 }
#define Disable_GPRMC_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x52, 0x4d, 0x43, 0x2a, 0x33, 0x41, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x04, 0x00, 0xfe, 0x17 }
#define Disable_GPVTG_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x56, 0x54, 0x47, 0x2a, 0x32, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x05, 0x00, 0xff, 0x19 }
#define Disable_GPZDA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x5a, 0x44, 0x41, 0x2a, 0x33, 0x39, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x08, 0x00, 0x02, 0x1f }

#define Enable_GPDTM_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x44, 0x54, 0x4d, 0x2a, 0x33, 0x42, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x0a, 0x01, 0x05, 0x24 }
#define Enable_GPGBS_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x42, 0x53, 0x2a, 0x33, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x09, 0x01, 0x04, 0x22 }
#define Enable_GPGGA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x47, 0x41, 0x2a, 0x32, 0x37, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x00, 0x01, 0xfb, 0x10 }
#define Enable_GPGLL_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x4c, 0x4c, 0x2a, 0x32, 0x31, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x01, 0x01, 0xfc, 0x12 }
#define Enable_GPGRS_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x52, 0x53, 0x2a, 0x32, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x06, 0x01, 0x01, 0x1c }
#define Enable_GPGSA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x41, 0x2a, 0x33, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x02, 0x01, 0xfd, 0x14 }
#define Enable_GPGST_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x54, 0x2a, 0x32, 0x36, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x07, 0x01, 0x02, 0x1e }
#define Enable_GPGSV_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x56, 0x2a, 0x32, 0x34, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x03, 0x01, 0xfe, 0x16 }
#define Enable_GPRMC_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x52, 0x4d, 0x43, 0x2a, 0x33, 0x41, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x04, 0x01, 0xff, 0x18 }
#define Enable_GPVTG_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x56, 0x54, 0x47, 0x2a, 0x32, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x05, 0x01, 0x00, 0x1a }
#define Enable_GPZDA_String_Cmd { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x5a, 0x44, 0x41, 0x2a, 0x33, 0x39, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x08, 0x01, 0x03, 0x20 }



#define Set_Factory_String_Cmd { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x07, 0x1F, 0x9E }
#define Save_Changes_String_Cmd { 0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x31, 0xBF}

#define GPSStartResetSM GPSConfigResetSM
#define GPSResetFactoryResetSM GPSConfigResetSM


// Module exported Functions ---------------------------------------------------
unsigned char GPSStart(void);
unsigned char GPSConfig(void);
void GPSConfigResetSM (void);
void GPSTimeoutCounters (void);
void GPSProcess (void);
unsigned char GPSResetFactory(void);

#endif /* GPS_VKTEL_H_ */

//--- end of file ---//

