#ifndef DEFINES_H
#define DEFINES_H 1

#include "mbed.h"
#define BREADBOARD 0

#if BREADBOARD

#define LED 					p7
#define RX_PIN  			p15
#define TX_PIN  			p14
#define ECG_INT_PIN 	p16
#define ECG_MOSI 			p10
#define ECG_MISO 			p11
#define ECG_SCLK			p9
#define ECG_CS				p8

#define ECG_FCLOCK		p12  //NC

#define BATT_PIN			p2   //NC

#else

#define LED 					p27
#define RX_PIN  			p7
#define TX_PIN  			p6
#define ECG_INT_PIN 	p16
#define ECG_MOSI 			p10
#define ECG_MISO 			p11
#define ECG_SCLK			p9
#define ECG_CS				p8
#define ECG_FCLOCK		p12
#define BATT_PIN			p2


#endif

#define BATT_MIN 		2500 //2.9v
#define BATT_MAX		3600 //4.2v  1% = 9.2count 
#define BATT_CEL		50   //around 5% wiggle room 
#define BAUD_RATE		115200
#define ECGUART_UPDATE_RATE_MS 10 
//DEBUG _ Print DEBUG Info to UART 
#define BATT_DEBUG 0
#define ECG_DEBUG 0
#define BLE_DEBUG 0 
#define UART_DEBUG 0 
 
#endif