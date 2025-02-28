/**
  ******************************************************************************
  * @file    Valve.h
  * @author  Eugene Mwangi
  * @date    10/01/2024
  * @brief   Manages communication with DRV8833 Motor Driver
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2024 PayGo Energy, Inc.</center></h2>
  ******************************************************************************
  */
#ifndef _VALVE_h
#define _VALVE_h

#include <Arduino.h>
#include "../../include/config.h"

void valve_init();
void valve_ctrl(int valvepin);
void valve_ctrl(int valvepin, int action);

#endif //VALVE_h