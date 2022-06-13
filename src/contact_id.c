//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CONTACT_ID.C ###############################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "contact_id.h"

#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
// keep alive example 18 CB83 01 602 01 000
void ContactIDString (contact_id_e report, char * client_num, char * remote_number, char * message)
{
    strcpy(message, "18 ");
    strcat(message, client_num);
    
    if (report == panic_alarm)
        strcat(message, " 01 120");
    
    if (report == ac_loss_opening)
        strcat(message, " 01 301");
    
    if (report == ac_loss_close)
        strcat(message, " 03 301");
    
    if (report == low_system_battery_opening)
        strcat(message, " 01 302");
    
    if (report == low_system_battery_close)
        strcat(message, " 03 302");
    
    if (report == keep_alive)
        strcat(message, " 01 602");

    strcat(message, " 01 ");
    strcat(message, remote_number);
}


//--- end of file ---//

