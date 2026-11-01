#pragma once
#include "HardwareConfig.h"
#include "knxprod.h"

#ifdef BOARD_OAM_DIGITAL_LED_CONTROLLER
#define DEVICE_ID "OFM-DigitalLEDController"
#define HARDWARE_NAME "OFM Digital LED Controller"
#define DEVICE_NAME "OFM KNX TP Digital LED Controller"
#define PROG_LED_PIN 2
#define PROG_LED_PIN_ACTIVE_ON HIGH
#define PROG_BUTTON_PIN 7
#define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
#define KNX_UART_NUM 0
#define KNX_UART_TX_PIN 0
#define KNX_UART_RX_PIN 1
#define SAVE_INTERRUPT_PIN 6
#define DIGITAL_LED_SERIAL Serial2
#define DIGITAL_LED_UART_TX_PIN 4
#define DIGITAL_LED_UART_RX_PIN 5
#define DIGITAL_LED_UART_BAUD_RATE 115200
#else
#error Unsupported board
#endif
