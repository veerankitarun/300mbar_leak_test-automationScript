#ifndef _CONFIG_h
#define _CONFIG_h
// DUT SOV MOTOR DRIVER CONTROL PINS
#define DUT_IN1_PIN         47 // (-)
#define DUT_IN2_PIN         45 // (+)

// INLET SOV MOTOR DRIVER CONTROL PINS
#define INLET_IN1_PIN         31 // (-)
#define INLET_IN2_PIN         29 // (+)

// OUTLET SOV MOTOR DRIVER CONTROL PINS
#define OUTLET_IN1_PIN         35 // (-)
#define OUTLET_IN2_PIN         33 // (+)

// defining the Push button pin 
#define PUSH_IN   10

#define buzzer 26
#define REDLED 28
#define GREENLED 12

// VALVE SERIAL COMMANDS
#define VALVE_CMD      "AT+TVALVE="

// DEBUG ENABLE
#define VALVE_DEBUG_EN       0
#define SPR_DEBUG_EN         0
#define MAIN_DEBUG_EN        0

// MODULE ENABLE
#define VALVE_EN        0
#define SPR_EN          1

// DELAY SETTINGS (in milliseconds)
#define VALVE_DELAY     1000

// SENSATA PRESSURE SENSOR SERIAL COMMANDS
#define SPR_GET_PRESS   "AT+TSPR="

// leak test delay time
#define WAIT_TIME 40000

// VALVE IDENTITY 
#define DUT_VALVE       1
#define INLET_VALVE     2
#define OUTLET_VALVE    3

// VALVE ACTIONS
#define ALL_VALVES_CLOSE    0
#define ALL_VALVES_OPEN     1
#define CLOSE_ACTION        0
#define OPEN_ACTION         1

#endif // _CONFIG_h

// ENABLE/DISABLE Serial Reading Feature
#define SERIAL_READ_EN   0 // Set to 1 to enable, 0 to disable

#define pr_diff_thresh 0.4

#define thrshd_pr 150

