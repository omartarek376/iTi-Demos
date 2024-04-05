#include "HAL/Switch.h"

const Switchcfg_t Switches[_Switch_num]=
{
    [Led_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_7,
        .connection= Switch_conn_pushdown,
    }
};