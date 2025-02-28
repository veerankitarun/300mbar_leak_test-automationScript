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

#include "SensataPressureSensor.h"

static bool check_status(uint8_t status);
static float get_pressure(uint16_t val);
static float get_temperature(uint8_t, uint8_t);

/**
 * @brief Initializes I2C communication with the sensata pressure sensor
 *
 */

void sensata_init()
{
    Wire.begin();
}

/**
 * @brief Checks the status of the data.
 *
 * @retval returns true if status is ok else false
 */
static bool check_status(uint8_t status)
{
    if (status == 0x00)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Calculates the pressure reading in mbar
 *
 * @retval pressure value
 */
static float get_pressure(uint16_t val)
{
    // Convert the pressure value into pounds per square inch
    float pressure_psig = (val / 16383.0) * PRESSURE_RANGE_PSIG;
    // float pressure_psig = (PRESSURE_RANGE_PSIG/13107) * (val - 1638);
    // Convert the pressure value into mbar
    float pressure_mbar = pressure_psig * 0.0689475728 * 1000;

    return pressure_mbar;
}

/**
 * @brief Calculates the temperature reading in degrees Celsius (°C)
 *
 * @retval temperature value
 */
static float get_temperature(uint8_t val1, uint8_t val2)
{
    // Convert the fetched temperature data into degrees Celsius (°C)
    float temperature = ((val1 * 64 - val2) / pow(2, 14)) * 165 - 40;

    return temperature;
}

/**
 * @brief Gets the pressure and temperature readings from the sensata pressure sensor
 *
 */
// 

String sensata_measure_string(bool cont) {
    Wire.beginTransmission(I2C_SLAVE_ADDRESS);
    Wire.write(0x00); // Send a request to write data
    Wire.endTransmission();

    delay(3);

    // The current sensor in use operates in UPDATE mode (refer to the user manual)
    Wire.requestFrom(I2C_SLAVE_ADDRESS, DATA_LEN);
    
    if (Wire.available() < DATA_LEN) {
        // Return empty string if not enough data is available
        return "ERROR";
    } else {
        uint8_t rx_buffer[DATA_LEN];

        // Read the data
        for (int i = 0; i < DATA_LEN; i++) {
            rx_buffer[i] = Wire.read();
        }

        // Check if the status is ok
        uint8_t status = rx_buffer[0] >> 6;
        if (!check_status(status)) {
            // Return error if status is not valid
            return "ERROR";
        } else {
            // Get the pressure reading
            uint16_t pressureRaw = rx_buffer[1] | ((rx_buffer[0] & ~0xC0) << 8);
            float pressure_mbar = get_pressure(pressureRaw);

            // Validate the pressure range (for example, between 0 and 1000 mbar)
            if (pressure_mbar < 0 || pressure_mbar > 1000) {
                // Return error if the pressure is out of range
                return "ERROR";
            }

            // Get the temperature reading
            uint8_t temperatureRaw1 = rx_buffer[2];
            uint8_t temperatureRaw2 = rx_buffer[3] >> 2;
            float temperature_degCel = get_temperature(temperatureRaw1, temperatureRaw2);

            // Return the formatted result as a string without printing
            String result = String(pressure_mbar, 4) + "," + String(temperature_degCel, 4);
            return result;
        }
    }
}
