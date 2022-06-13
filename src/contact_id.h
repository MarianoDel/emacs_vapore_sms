//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CONTACT_ID.H ###############################
//--------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef _CONTACT_ID_H_
#define _CONTACT_ID_H_


// Exported Types Constants and Macros -----------------------------------------
typedef enum {
    panic_alarm,
    ac_loss_opening,
    ac_loss_close,    
    low_system_battery_opening,
    low_system_battery_close,
    keep_alive
    
} contact_id_e;
    


// Module Exported Functions ---------------------------------------------------
void ContactIDString (contact_id_e report, char * client_num, char * remote_number, char * message);


#endif    /* _CONTACT_ID_H_ */

//--- end of file ---//
