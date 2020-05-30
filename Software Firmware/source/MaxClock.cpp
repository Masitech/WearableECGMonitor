
#include "mbed.h"
#include "defines.h"
#include "MaxClock.h"
#include "nrf_gpiote.h"
#include "nrfx_pwm.h"

static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

///** TIMTER3 peripheral interrupt handler. This interrupt handler is called whenever there it a TIMER2 interrupt
// */
//extern "C" {
//void TIMER3_IRQHandler(void)
//{
//    if (NRF_TIMER3->EVENTS_COMPARE[0] == 1){   
//				//body of interrupt 
//				nrf_gpio_pin_toggle(ECG_FCLOCK);    //31250hz (max fclock rerecoomend 32768hz) close enough. 
//        NRF_TIMER3->EVENTS_COMPARE[0] = 0;
//    }
//}

//}
void MaxAFE_Clock_Init(void){
	nrfx_pwm_config_t const configPWM = {
	        .output_pins =
        {
            ECG_FCLOCK | NRFX_PWM_PIN_INVERTED, // channel 0
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 488,
        .load_mode    = NRF_PWM_LOAD_INDIVIDUAL,
        .step_mode    = NRF_PWM_STEP_AUTO
};
		nrfx_pwm_init(&m_pwm0, &configPWM, NULL);
    // This array cannot be allocated on stack (hence "static") and it must
    // be in RAM (hence no "const", though its content is not changed).
    static nrf_pwm_values_individual_t /*const*/ seq_values[] =
    {
        { 244 }
    };
		
		    nrf_pwm_sequence_t const seq =
    {
        .values.p_individual = seq_values,
        .length              = NRF_PWM_VALUES_LENGTH(seq_values),
        .repeats             = 0,
        .end_delay           = 0
    };

    (void)nrfx_pwm_simple_playback(&m_pwm0, &seq, 1, NRFX_PWM_FLAG_LOOP);
	

		//OLD CODE
		
//{		//set the gpio pin as an output
//		nrf_gpio_pin_dir_set(ECG_FCLOCK,NRF_GPIO_PIN_DIR_OUTPUT);
//   // 32-bit timer
//    NRF_TIMER3->BITMODE = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
//   // 16mhz timer
//    NRF_TIMER3->PRESCALER = 0 << TIMER_PRESCALER_PRESCALER_Pos; //31250 *2
//		// Compare value -> generates EVENTS_COMPARE[0]
//    NRF_TIMER3->CC[0] = 244;
//    // Enable IRQ on EVENTS_COMPARE[0]
//    NRF_TIMER3->INTENSET = TIMER_INTENSET_COMPARE0_Enabled << TIMER_INTENSET_COMPARE0_Pos;
//    // Clear the timer when COMPARE0 event is triggered
//    NRF_TIMER3->SHORTS = TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos;
//		NVIC_SetVector(TIMER3_IRQn, (uint32_t)TIMER3_IRQHandler);
//		NVIC_SetPriority(TIMER3_IRQn, 0);
//    NVIC_EnableIRQ(TIMER3_IRQn);
//    NRF_TIMER3->TASKS_START = 1;  // Start TIMER
	
	
//    nrf_gpiote_task_configure(0, 27, NRF_GPIOTE_POLARITY_TOGGLE, NRF_GPIOTE_INITIAL_VALUE_LOW);
//    
//    // Configure PPI channel 0 to toggle PWM_OUTPUT_PIN on every TIMER2 COMPARE[0] match.
//    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_TIMER2->EVENTS_COMPARE[0];
//    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_GPIOTE->TASKS_OUT[0];
//    
//    // Enable PPI channel 0.
//    NRF_PPI->CHEN = (PPI_CHEN_CH0_Enabled << PPI_CHEN_CH0_Pos);    
//    
//    // Init TIMER2 for 10kHz PWM
//    NRF_TIMER2->MODE = TIMER_MODE_MODE_Timer;           // Set the timer in Counter Mode
//    NRF_TIMER2->TASKS_CLEAR = 1;                        // Clear the tasks first to be usable for later
//    NRF_TIMER2->PRESCALER = 0;                          // 2^PRESCALER (values authorized 1 to 9) 4 : 16MHz / 16 = 10MHz
//    NRF_TIMER2->BITMODE = TIMER_BITMODE_BITMODE_32Bit;  //Set counter to 16 bit resolution
//    
//    // Init OC0 PWM
//    NRF_TIMER2->CC[0] = 244UL;
//    NRF_TIMER2->SHORTS = (TIMER_SHORTS_COMPARE0_CLEAR_Enabled << TIMER_SHORTS_COMPARE0_CLEAR_Pos);
//    
//    // Enable OC1 interrupt on TIMER2
//    NVIC_EnableIRQ(TIMER2_IRQn);
//    
//    // Start TIMER2
//    NRF_TIMER2->TASKS_START = true;



//OLD PWM 
//	uint32_t pwm_seq[4] = {244};
//	nrf_gpio_pin_dir_set(ECG_FCLOCK,NRF_GPIO_PIN_DIR_OUTPUT);
//  
//  NRF_PWM2->PSEL.OUT[0] = (ECG_FCLOCK << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

//   // NRF_PWM2->PSEL.OUT[1] = (14 << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected << PWM_PSEL_OUT_CONNECT_Pos);

//  NRF_PWM2->ENABLE = (PWM_ENABLE_ENABLE_Enabled << PWM_ENABLE_ENABLE_Pos);

//  NRF_PWM2->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos);

//  NRF_PWM2->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1 << PWM_PRESCALER_PRESCALER_Pos);

//  NRF_PWM2->COUNTERTOP = (488 << PWM_COUNTERTOP_COUNTERTOP_Pos); //1 msec

//  NRF_PWM2->LOOP = (PWM_LOOP_CNT_Disabled << PWM_LOOP_CNT_Pos);

//  NRF_PWM2->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos) | (PWM_DECODER_MODE_RefreshCount << PWM_DECODER_MODE_Pos);

//  NRF_PWM2->SEQ[0].PTR = ((uint32_t)(pwm_seq) << PWM_SEQ_PTR_PTR_Pos);

//  NRF_PWM2->SEQ[0].CNT = ((sizeof(pwm_seq) / sizeof(uint32_t)) << PWM_SEQ_CNT_CNT_Pos);

//  NRF_PWM2->SEQ[0].REFRESH = 0;

//  NRF_PWM2->SEQ[0].ENDDELAY = 0;

//  NRF_PWM2->TASKS_SEQSTART[0] = 1;
}
		