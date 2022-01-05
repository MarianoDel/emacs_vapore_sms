//---------------------------------------------------------
// #### PROYECTO COMUNICADOR VAPORE SMS - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### BATTERY.H #########################################
//---------------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _BATTERY_H_
#define _BATTERY_H_


// Module Configurations -------------------------------------------------------


// Exported Types Constants and Macros -----------------------------------------


// Exported Functions ----------------------------------------------------------
void Battery_Check_Init (void);
void Battery_Check (void);
unsigned short Battery_Get_Voltage (void);
void Battery_Voltage (unsigned char * v_int, unsigned char * v_dec);

#endif    /* _BATTERY_H_ */

//--- end of file ---//
