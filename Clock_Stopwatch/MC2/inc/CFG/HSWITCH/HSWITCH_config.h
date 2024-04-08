#ifndef HAL_SWITCH_HAL_config_H_
#define HAL_SWITCH_HAL_config_H_

// Enumeration defining the switches used in the system
typedef enum
{
  UP_SWITCH,
  DOWN_SWITCH,
  RIGHT_SWITCH,
  LEFT_SWITCH,
  START_SWITCH,
  STOP_SWITCH,
  RESET_SWITCH,
  OK_SWITCH,
  EDIT_SWITCH,
  MODE_SWITCH,
 _Switch_Num         // Total number of switches (must be last)
} Switches_t;

#endif /* HAL_SWITCH_HAL_config_H_ */