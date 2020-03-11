#ifndef DEFINES_H
#define DEFINES_H 1

#include "mbed.h"


#define RX_PIN  			p7
#define TX_PIN  			p6
#define ECG_INT_PIN 	p16
#define ECG_MOSI 			p10
#define ECG_MISO 			p11
#define ECG_SCLK			p9
#define ECG_CS				p8
#define ECG_FCLOCK		p12

#define BAUD_RATE		115200
#define ECGUART_UPDATE_RATE_MS 10 
#endif