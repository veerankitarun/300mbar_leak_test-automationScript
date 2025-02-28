#include <Arduino.h>
#include "Valve.h"
#include "SensataPressureSensor.h"
#include "config.h"
#include "LiquidCrystal.h"

// Initialize the LCD (change the pins according to your wiring)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // (rs, enable, d4, d5, d6, d7)

// Function Declaration
void readSerial();
void leaktest();

void setup() {
    Serial.begin(9600);

    lcd.begin(20, 4); // Initialize LCD for 16x2 display
    lcd.clear();

    pinMode(PUSH_IN, INPUT_PULLUP);  // Set the push button pin as input with pull-up resistor
    pinMode(buzzer, OUTPUT);
    pinMode(REDLED, OUTPUT);
    pinMode(GREENLED, OUTPUT);

    #if MAIN_DEBUG_EN
        Serial.println("Firmware loaded");
    #endif // MAIN_DEBUG_EN

    #if VALVE_EN
        valve_init();
    #endif // VALVE_EN

    #if SPR_EN
        sensata_init();
    #endif // SPR_EN

    #if SERIAL_READ_EN
        readSerial(); // Only called if SERIAL_READ_EN is set to 1
    #endif
} // END OF setup

void loop() {
    leaktest(); 
    readSerial();
}

/**
 * @brief Reads data from the serial
 */
void readSerial() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');

        #if MAIN_DEBUG_EN
            Serial.print("Received Line: ");
            Serial.println(cmd);
        #endif

        // Connection Check
        if (cmd == "AT\r") {
            Serial.println("OK");
        }
        // Valve Control
        else if (cmd.substring(0, 10) == VALVE_CMD) {
            //int valvepin = cmd.charAt(10) - '0';
            //int action = cmd.charAt(12) - '0';

            #if VALVE_DEBUG_EN
                Serial.print("valvepin: "); Serial.println(valvepin);
                Serial.print("action: "); Serial.println(action);
                Serial.print("Length: "); Serial.println(cmd.length());
            #endif

            #if VALVE_EN
                if (cmd.length() == 14 && cmd.charAt(11) == ',') {
                    valve_ctrl(valvepin, action);                
                } else if (cmd.length() == 12) {
                    valve_ctrl(valvepin);
                } else {
                    Serial.println("ERROR");
                }
            #else
                Serial.println("+VALVE_EN: 0");
                Serial.println("OK");
            #endif
        }
        // read Sensata Pressure Sensor data and print that its reading after pressing button then 2sec delay then read Sensata Pressure Sensor
        else if (cmd.substring(0, 8) == SPR_GET_PRESS) {
            #if SPR_EN
                if (cmd.substring(8) == "1,0\r") {
                    #if SPR_DEBUG_EN
                        Serial.println("Single measurement reading...");
                    #endif
                    //sensata_measure_string(false);
                } else if (cmd.substring(8) == "1,1\r") {
                    #if SPR_DEBUG_EN
                        Serial.println("Continuous measurement reading...");
                    #endif
                    while (true) {
                        //sensata_measure_string(true);
                        if (Serial.available()) {
                            cmd = Serial.readStringUntil('\n');
                            if (cmd.substring(8) == "0\r") {
                                Serial.println("OK");
                                break;
                            } else {
                                Serial.println("ERROR");
                            }
                        }
                        delay(1000); // Consider replacing with a non-blocking approach
                    }
                } else {
                    Serial.println("ERROR");
                }
            #else
                Serial.println("+SPR_EN: 0");
                Serial.println("OK");
            #endif
        } else {
            Serial.println("ERROR");
        }
    }
} // END OF readSerial





// void leaktest() {
//     static bool lastButtonState = HIGH;  // Last known state of the button
//     bool currentButtonState = digitalRead(PUSH_IN);  // Read the current state of the button

//     // Detect a button press (LOW due to INPUT_PULLUP)
//     if (currentButtonState == LOW && lastButtonState == HIGH) {
//         //Serial.println("Button pressed, reading Sensata Pressure Sensor...");

//         // Get pressure and temperature data as a string (without printing raw data)
//         String sensorData = sensata_measure_string(false);

//         // Check if we received valid data
//         if (sensorData != "ERROR") {
//             // Extract pressure and temperature values from the comma-separated data
//             int commaIndex = sensorData.indexOf(',');
//             if (commaIndex != -1) {
//                 // Extract the pressure and temperature strings
//                 String pressureValueStr = sensorData.substring(0, commaIndex);  // Pressure before the comma
//                 String temperatureValueStr = sensorData.substring(commaIndex + 1);  // Temperature after the comma

//                 // Convert the pressure and temperature values to floats
//                 float pressureValue = pressureValueStr.toFloat();
//                 float temperatureValue = temperatureValueStr.toFloat();

//                 // Print the pressure and temperature values (rounded if necessary)
//                 Serial.print("Pressure value: ");
//                 Serial.println(pressureValue, 4);  // Print pressure with 4 decimal places
//                 // Serial.print("Temperature value: ");
//                 // Serial.println(temperatureValue, 4);  // Print temperature with 4 decimal places

//                 // Example action based on pressure
//                 if (pressureValue > 22.19) {
//                     // Add valve control code here if needed
//                 }
//             } else {
//                 Serial.println("ERROR: Sensor data format incorrect");
//             }
//         } else {
//             Serial.println("ERROR: Failed to read sensor data");
//         }
//     }

//     // Update the last button state
//     lastButtonState = currentButtonState;
// }


void leaktest() {
    static bool lastButtonState = HIGH;  // Last known state of the button
    bool currentButtonState = digitalRead(PUSH_IN);  // Read the current state of the button

    if (currentButtonState == LOW && lastButtonState == HIGH) {
        String initialPressureData = sensata_measure_string(false);

        if (initialPressureData != "ERROR") {
            int commaIndex = initialPressureData.indexOf(',');
            if (commaIndex != -1 ) {
                String initialPressureValueStr = initialPressureData.substring(0, commaIndex);
                float initialPressureValue = initialPressureValueStr.toFloat();
                // thrshd_pr is set to 150mbar in config; it is defined 
                if (initialPressureValue < thrshd_pr){
                   Serial.println("Warning: pressure is not set to 150mbar");
                   lcd.clear();
                   lcd.print("Warning: pressure is not set to 150mbar");
                } else {

                Serial.print("Initial Pressure: ");
                Serial.println(initialPressureValue, 2);
                digitalWrite(GREENLED, LOW);
                digitalWrite(REDLED, LOW);
                lcd.clear();
                delay(1000);
                lcd.print("Initial Pressure:");
                lcd.setCursor(0, 1);
                lcd.print(initialPressureValue, 2);
                lcd.print("mbar");

                digitalWrite(buzzer, HIGH);  
                delay(1000);  
                digitalWrite(buzzer, LOW); 

                Serial.println("Wait for 40 Seconds");
                lcd.clear();
                lcd.setCursor(0,1);
                lcd.print("Waiting 40 Sec...   ");
                
                // for (int i=0; i<16; i++){
                    
                //     lcd.print("|");
                //     delay(2000);
                // }
                delay(WAIT_TIME);
             


                String finalPressureData = sensata_measure_string(false);
                if (finalPressureData != "ERROR") {
                    int commaIndex2 = finalPressureData.indexOf(',');
                    if (commaIndex2 != -1) {
                        String finalPressureValueStr = finalPressureData.substring(0, commaIndex2);
                        float finalPressureValue = finalPressureValueStr.toFloat();
                        
                        Serial.print("Final Pressure: ");
                        Serial.println(finalPressureValue, 2);

                        lcd.clear();
                        lcd.print("Final Pressure:");
                        lcd.setCursor(0, 1);
                        lcd.print(finalPressureValue, 2);
                        lcd.print("mbar");

                        digitalWrite(buzzer, HIGH);  
                        delay(2000);  
                        digitalWrite(buzzer, LOW);  
                        
                        float difference = initialPressureValue - finalPressureValue;

                        if (difference >= -pr_diff_thresh && difference <= pr_diff_thresh) { 

                            Serial.println("Leak test Passed");
                            lcd.clear();

                            lcd.setCursor(0,0);
                            lcd.print("Leak test Passed ");

                            lcd.setCursor(0,1);
                            lcd.print("Pr Diff: ");
                            lcd.print(difference);
                            lcd.print("mbar");

                            // Display pressures in next two rows
                            lcd.setCursor(0, 2);
                            lcd.print("Init: ");
                            lcd.print(initialPressureValue, 2);
                            lcd.print(" mbar");

                            lcd.setCursor(0, 3);
                            lcd.print("Final: ");
                            lcd.print(finalPressureValue, 2);
                            lcd.print(" mbar");
                            
                            digitalWrite(REDLED, LOW);
                            digitalWrite(GREENLED, HIGH);
                            delay(4000);
                            //digitalWrite(GREENLED, LOW);
                        } else {
                            Serial.println("Leak test Failed");
                            lcd.clear();
                            lcd.setCursor(0,0);
                            lcd.print("Leak test Failed ");
                            lcd.setCursor(0,1);
                            lcd.print("Pr Diff: ");
                            lcd.print(difference);
                            lcd.print("mbar");
                            
                            // Display pressures in next two rows
                            lcd.setCursor(0, 2);
                            lcd.print("Init: ");
                            lcd.print(initialPressureValue, 2);
                            lcd.print(" mbar");

                            lcd.setCursor(0, 3);
                            lcd.print("Final: ");
                            lcd.print(finalPressureValue, 2);
                            lcd.print(" mbar");

                            digitalWrite(GREENLED, LOW);
                            digitalWrite(REDLED, HIGH);
                            delay(4000);
                            //digitalWrite(REDLED, LOW);
                        }
                    } else {
                        Serial.println("ERROR: Final pressure data format incorrect");
                        lcd.clear();
                        lcd.print("Error: Format");
                    }
                } else {
                    Serial.println("ERROR: Failed to read final sensor data");
                    lcd.clear();
                    lcd.print("Error: Read Fail");
                }
            }
             }else {
                Serial.println("ERROR: Initial pressure data format incorrect");
                lcd.clear();
                lcd.print("Error: Format");
            }
        } else {
            Serial.println("ERROR: Failed to read initial sensor data");
            lcd.clear();
            lcd.print("Error: Read Fail");
        }
    }

    lastButtonState = currentButtonState;
}



