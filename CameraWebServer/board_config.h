#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.

// ===================
// Select camera model
// ===================
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "camera_pins.h"

// Motor control configuration (PCA9685 + 2x MX1508 for 4 motors)
#define MOTOR_I2C_SDA_PIN 14
#define MOTOR_I2C_SCL_PIN 15
#define MOTOR_PWM_FREQ_HZ 1000
#define MOTOR_WATCHDOG_DEFAULT_MS 300
#define MOTOR_WATCHDOG_MAX_MS 2000

#endif  // BOARD_CONFIG_H
