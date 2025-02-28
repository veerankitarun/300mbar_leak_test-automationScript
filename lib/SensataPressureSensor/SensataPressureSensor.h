/**
 ******************************************************************************
 * @file    SensataPressureSensor.h
 * @author  Tarun Kumar
 * @date    14/06/2024
 * @brief   Provides communication with 8MPP2 SERIES pressure sensor
 ******************************************************************************
 * @attention
 *
 * User manual: https://www.mouser.com/pdfDocs/8MPP2_User_Manual.pdf
 * Datasheet: https://eu.mouser.com/datasheet/2/657/sensata_8mpp2_series_digital_low_pressure_sensors_-2005804.pdf
 *
 * <h2><center>&copy; COPYRIGHT(c) 2024 PayGo Energy, Inc.</center></h2>
 ******************************************************************************
 */

#ifndef SENSATA_PRESSURE_SENSOR_H
#define SENSATA_PRESSURE_SENSOR_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Wire.h"

#define I2C_SLAVE_ADDRESS 0x38
#define DATA_LEN 4
#define PRESSURE_RANGE_PSIG 5

// struct SensataPressTemp {
//     float pressure;
//     float temperature;
// };

void sensata_init();
// Function declaration for measuring and returning sensor data as a string
String sensata_measure_string(bool cont);

#endif // SENSATA_PRESSURE_SENSOR_H