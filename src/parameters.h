//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//- Data Struct to Backup ------------------------------------------------------
// Beware this struct must be 4 bytes align and be multiple of 8
#define PHONE_NUMBER_MAX_LEN    19
#define SITE_MAX_LEN    150    // sms msg len - 15 from rep sitio
typedef struct parameters {

    char num_reportar [PHONE_NUMBER_MAX_LEN + 1];    //20
    char imei [24];    //44
    char num_propio [PHONE_NUMBER_MAX_LEN + 1];    //64
    char sitio_propio [SITE_MAX_LEN + 1];    //215

    // conf backup flags
    unsigned char bkp_envios_ok;    //216
    unsigned char bkp_timer_reportar;    //217
    unsigned char bkp_prender_ring;    //218
    unsigned char bkp_sense_bat;    //219
    unsigned char memory_saved_flag;    //220

    // conf running flags
    unsigned int internal_comms_flag;    //224 4 bytes if its aligned to 4

    // gprs settings
    char ip1 [16];    //240
    char ip_port1 [6];    //246
    char ip2 [16];    //262
    char ip_port2 [6];    //268
    char ip_proto [4];    //272

    char apn [68];    //340

    // monitor account
    char client_number [7];    //347
    unsigned char bkp_socket_use;    //348
    unsigned short keepalive;    //350 2 bytes if its aligned to 4

    // // // complete alignment with dummys
    unsigned char dummy1 [50];    //400    (multiply of eight)

} parameters_typedef;



// Configuration saved info
#define envios_ok_conf    (mem_conf.bkp_envios_ok)
#define timer_rep_conf    (mem_conf.bkp_timer_reportar)
#define prender_ring_conf    (mem_conf.bkp_prender_ring)
#define battery_check_conf    (mem_conf.bkp_sense_bat)
#define socket_use_enable_conf    (mem_conf.bkp_socket_use)

#define num_tel_imei    (mem_conf.imei)
#define num_tel_prop    (mem_conf.num_propio)
#define sitio_prop    (mem_conf.sitio_propio)
#define num_tel_rep    (mem_conf.num_reportar)


//- End of Data Struct to Backup -----------------------------------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

