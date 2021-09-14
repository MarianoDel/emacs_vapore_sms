//--------------------------------------------------
// #### VAPORE SMS COMMS PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMM_FROM_PANEL.H ##########################
//--------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_from_panel.h"
#include "usart.h"

#include <string.h>
#include <stdlib.h>
// #include <stdio.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile char comm_from_panel_local_buffer [100] = { 0 };


// Module Private Functions ----------------------------------------------------
unsigned char StringIsANumber (char * pn, unsigned short * number);


// Module Functions ------------------------------------------------------------
unsigned char Panel_Check_Alarm (unsigned short * number)
{
    unsigned char answer = 0;
    
    if (Usart2HaveActivationBuffer())
    {
        Usart2HaveActivationBufferReset();
        // Usart2SendUnsigned("algo nuevo\n", sizeof("algo nuevo\n") - 1);

        // buffer is already copied from int
        char * pStr = (char *) comm_from_panel_local_buffer;
        if (!strncmp(pStr, "Activo: ", sizeof("Activo: ") -1))
        {
            // Usart2SendUnsigned("en activo\n", sizeof("en activo\n") - 1);
            pStr += sizeof("Activo: XXX ") - 1;
            if (!strncmp(pStr, "B1", sizeof("B1") -1))
            {
                unsigned short my_number;
                pStr = ((char *) comm_from_panel_local_buffer + sizeof("Activo: ") - 1);

                if (StringIsANumber (pStr, &my_number) == 3)
                {
                    if ((my_number >= 0) && (my_number < 1000))
                    {
                        *number = my_number;
                        answer = 1;
                    }
                }
            }
        }
    }

    return answer;
}


//devuelve los numeros de un string, en la posicion number
//devuele la cantidad de cifras leidas
unsigned char StringIsANumber (char * pn, unsigned short * number)
{
    unsigned char i;
    char new_number [6] = {0};

    //no mas de 6 caracteres
    for (i = 0; i < 6; i++)
    {
        if ((*(pn + i) < '0') || (*(pn + i) > '9'))
            break;

        new_number[i] = *(pn + i);
    }

    if (i > 0)
        *number = atoi(new_number);

    return i;
}



//--- end of file ---//
