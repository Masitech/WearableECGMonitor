
#include <stdio.h>
#include "mbed.h"
#include "defines.h"
#include "MaxAFE.h"
#include "EcgUart.h"


Ticker flipper;

DigitalOut fclockc(ECG_FCLOCK);

volatile bool status = false;

void flip() {
    if(status){
         NRF_GPIO->OUTSET |= 1<<12;
         status = false;
        }
        else{
        NRF_GPIO->OUTCLR |= 1<<12;
         status = true;
            }

}
int main() {
flipper.attach_us(&flip, 30);
	MaxAFE_t::EcgAfeInit();
	EcgUart_t::EcgUartInit();
	while(1){}
}

