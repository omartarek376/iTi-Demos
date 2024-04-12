#include "HAL/Switch.h"

const Switchcfg_t Switches[_Switch_num]=
{
    [UP_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_7,
        .connection= Switch_conn_pushdown,
    },
    [DOWN_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_8,
        .connection= Switch_conn_pushdown,
    },
    [LEFT_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_11,
        .connection= Switch_conn_pushdown,
    },
    [RIGHT_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_12,
        .connection= Switch_conn_pushdown,
    },
    [OK_Switch]=
    {
        .port=GPIO_PORT_A,
        .pin=GPIO_PIN_13,
        .connection= Switch_conn_pushdown,
    },
    [MODE_Switch]=
    {
        .port=GPIO_PORT_C,
        .pin=GPIO_PIN_13,
        .connection= Switch_conn_pushdown,
    },
    [EDIT_Switch]=
    {
        .port=GPIO_PORT_C,
        .pin=GPIO_PIN_14,
        .connection= Switch_conn_pushdown,
    }
};