#include "MCAL/MGPIO/MGPIO_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "CFG/HSWITCH/HSWITCH_config.h"

/*
 * Array: SWITCHES
 * ---------------
 * Description:
 * This array contains the configuration settings for each switch used in the system.
 * It is a constant array of Switch_CFG_t structures.
 * Each element of the array corresponds to a specific switch enumerated in Switches_t.
 * The configuration includes the GPIO port, pin, connection type, and optionally the initial status of each switch.
 * The array size is determined by the total number of switches (_Switch_Num).
 * Unused elements are left uninitialized.
 */
const Switch_CFG_t SWITCHES [_Switch_Num]=
{
// [Switch_1]=
// {
//     .Switch_Port=GPIO_PORTB,
//     .Switch_Pin=GPIO_PIN0,
//     .Switch_Connection=SWITCH_CONN_PULLUP,
//     //.Switch_InitStatus=SWITCH_STATUS_RELEASED,
// }
};