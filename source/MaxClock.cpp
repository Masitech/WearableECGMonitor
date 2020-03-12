
#include "mbed.h"
#include "defines.h"
#include "MaxClock.h"
/** TIMTER2 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
 */
extern "C" {
void TIMER3_IRQHandler(void)
{
    if (NRF_TIMER3->EVENTS_COMPARE[0] == 1){   
				//body of interrupt 
				nrf_gpio_pin_toggle(ECG_FCLOCK);    //31250hz (max fclock rerecoomend 32768hz) close enough. 
        NRF_TIMER3->EVENTS_COMPARE[0] = 0;
    }
}
}
void MaxAFE_Clock_Init(void)
{		//set the gpio pin as an output
		nrf_gpio_pin_dir_set(ECG_FCLOCK,NRF_GPIO_PIN_DIR_OUTPUT);
   // 32-bit timer
    NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
 
   // 1us timer period
    NRF_TIMER3->PRESCALER = 8 << TIMER_PRESCALER_PRESCALER_Pos; //31250 *2
 
  // 1000 us compare value, generates EVENTS_COMPARE[0]
    NRF_TIMER3->CC[0] = 1;
 
    // Enable IRQ on EVENTS_COMPARE[0]
    NRF_TIMER3->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
 
    // Clear the timer when COMPARE0 event is triggered
    NRF_TIMER3->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
		NVIC_SetVector(TIMER3_IRQn, (uint32_t)TIMER3_IRQHandler);
    NVIC_EnableIRQ(TIMER3_IRQn);
    NRF_TIMER3->TASKS_START = 1;                                                            // Start TIMER
}
		