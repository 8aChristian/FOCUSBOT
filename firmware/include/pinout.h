#pragma once
#include <Arduino.h>

// =============================================================================
// FOCUSBOT HARDWARE PINOUT ARCHITECTURE v12.0
// MCU: ESP32-S3-WROOM-1-N16R8 (16MB Flash, 8MB PSRAM Octal)
// =============================================================================

// --- 1. DRV8833 DUAL H-BRIDGE MOTOR CONTROLLER ---
#define PIN_MOTOR_L_IN1     15  // Left Motor Forward PWM
#define PIN_MOTOR_L_IN2     16  // Left Motor Reverse PWM
#define PIN_MOTOR_R_IN1     17  // Right Motor Forward PWM
#define PIN_MOTOR_R_IN2     18  // Right Motor Reverse PWM

// --- 2. SG90 PAN HEAD SERVO CONTROLLER ---
#define PIN_SERVO_PWM       21  // Head Pan Axis (0 to 180 deg)

// --- 3. I2C BUS (MPU-6050 6-DOF IMU) ---
#define PIN_I2C_SDA         8   // IMU SDA (2.2k pull-up)
#define PIN_I2C_SCL         9   // IMU SCL (2.2k pull-up)
#define MPU6050_I2C_ADDR    0x68

// --- 4. I2S DIGITAL AUDIO (MAX98357A DAC & INMP441 MEMS MIC) ---
#define PIN_I2S_BCLK        12  // Bit Clock (Shared Amp & Mic)
#define PIN_I2S_WS          11  // Word Select / LRC (Shared Amp & Mic)
#define PIN_I2S_SPK_DATA    10  // MAX98357A Audio Serial Data (DIN)
#define PIN_I2S_MIC_DATA    13  // INMP441 Audio Serial Data (DOUT)

// --- 5. ST7789 2.0" IPS DISPLAY (240x320, SPI) ---
#define PIN_LCD_MOSI        35  // Master Out Slave In
#define PIN_LCD_SCLK        36  // SPI Clock
#define PIN_LCD_DC          47  // Data / Command Select
#define PIN_LCD_CS          48  // Chip Select (Active Low)

// --- 6. OV2640 DVP AI VISION CAMERA ---
#define PIN_CAM_SIOC        4   // SCCB I2C Clock
#define PIN_CAM_SIOD        5   // SCCB I2C Data
#define PIN_CAM_XCLK        6   // External Clock (20MHz)
#define PIN_CAM_PCLK        7   // Pixel Clock
#define PIN_CAM_HREF        14  // Horizontal Reference
#define PIN_CAM_VSYNC       43  // Vertical Sync
#define PIN_CAM_D0          37  // Data Bit 0
#define PIN_CAM_D1          38  // Data Bit 1
#define PIN_CAM_D2          39  // Data Bit 2
#define PIN_CAM_D3          40  // Data Bit 3
#define PIN_CAM_D4          41  // Data Bit 4
#define PIN_CAM_D5          42  // Data Bit 5
#define PIN_CAM_D6          2   // Data Bit 6
#define PIN_CAM_D7          1   // Data Bit 7

// --- 7. POWER & SYSTEM TELEMETRY ---
#define PIN_BAT_ADC         44  // LiPo Voltage Sense (1:2 Voltage Divider)
#define PIN_BOOT_SW         0   // Native Boot/Flash Button
#define PIN_USB_DM          19  // USB Native D-
#define PIN_USB_DP          20  // USB Native D+

// Battery Constants (1S LiPo: 3.0V cut-off to 4.2V fully charged)
#define BAT_R1_OHMS         100000.0f
#define BAT_R2_OHMS         100000.0f
#define BAT_DIVIDER_RATIO   ((BAT_R1_OHMS + BAT_R2_OHMS) / BAT_R2_OHMS) // 2.0x
#define BAT_MIN_VOLTS       3.20f
#define BAT_MAX_VOLTS       4.20f
#define BAT_LOW_THRESHOLD   3.40f
