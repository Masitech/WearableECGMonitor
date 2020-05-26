
#include "MaxAFE.h"
#include "mbed.h"
#include "BLE_Interface.h"
#include "main.h"

//Global Object
Thread 				EcgAFE_Thread;
 MaxAFE_t 		EcgAFE;
 SPI 					spiBus(ECG_MOSI, ECG_MISO, ECG_SCLK);     // SPIS_PSELMOSI = p10, SPIS_PSELMISO = p11,SPIS_PSELSCK  = p9,    
 uint32_t MaxAFE_t::packetCounter = 0;
//local objects 
InterruptIn ecgFIFO_int(ECG_INT_PIN);          // @todo fix pcb 
volatile bool ecgFIFOIntFlag;
//DigitalOut LEDX(LED);
extern "C" void MaxAFE_t::ISR_AFE_Int(void){
				ecgFIFOIntFlag = 1;
		};
		
void MaxAFE_t::EcgAfeInit(){
	  ecgFIFO_int.mode(PullUp);
    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    spiBus.format(8,0);//format 
    spiBus.frequency(1000000); //set freq
		ConfigAFE(EcgAFE); // setup max30003
		ecgFIFO_int.fall(&ISR_AFE_Int);    // ecg FIFO almost full interrupt
		EcgAFE_Thread.start(MaxAFE_t::EcgAFEThread);
    EcgAFE.writeRegister( MAX30003::SYNCH,0);
}


void MaxAFE_t::EcgAFEThread(void){
	EcgUart.printf("ECG Thread Started \n");
	static uint32_t ecgFIFO, readECGSamples, idx, ETAG[32], status,RtoR;
  static int16_t ecgSample[32];
	MaxAFE_t::packetCounter = 0;
	float BPM;
	int16_t simpleECG = 0;
	//LEDX=1;
    while(1) {
        /* Read back ECG samples from the FIFO */
        if( ecgFIFOIntFlag ) {
            ecgFIFOIntFlag = 0;
            EcgUart.printf("Interrupt received....\r\n");
            status = EcgAFE.readRegister( MAX30003::STATUS );      // Read the STATUS register
            //EcgUart.printf("Status : 0x%x\r\n" "Current BPM is %3.2f\r\n\r\n", status, BPM);
            // Check if R-to-R interrupt asserted
            if( ( status & RTOR_STATUS ) == RTOR_STATUS ){           
            
                EcgUart.printf("R-to-R Interrupt \r\n");
                
                // Read RtoR register
                RtoR = EcgAFE.readRegister( MAX30003::RTOR ) >>  RTOR_REG_OFFSET;   
                
                // Convert to BPM
                BPM = 1.0f / ( RtoR * RTOR_LSB_RES / 60.0f );   
                
                // Print RtoR              
                EcgUart.printf("RtoR : %d\r\n\r\n", RtoR);   
            
								AFEBLE.updateHR((uint16_t)BPM);
            }  
             
            // Check if EINT interrupt asserted
            if ( ( status & EINT_STATUS_MASK ) == EINT_STATUS_MASK ) {     
            
                EcgUart.printf("FIFO Interrupt \r\n");
                readECGSamples = 0;                        // Reset sample counter
                
                do {
                    ecgFIFO = EcgAFE.readRegister( MAX30003::ECG_FIFO );       // Read FIFO
                    ecgSample[readECGSamples] = ecgFIFO >> 8;                  // Isolate voltage data
                    ETAG[readECGSamples] = ( ecgFIFO >> 3 ) & ETAG_BITS_MASK;  // Isolate ETAG
                    readECGSamples++;                                          // Increment sample counter
                
                // Check that sample is not last sample in FIFO                                              
                } while ( ETAG[readECGSamples-1] == FIFO_VALID_SAMPLE_MASK || 
                          ETAG[readECGSamples-1] == FIFO_FAST_SAMPLE_MASK ); 
            
                EcgUart.printf("%d samples read from FIFO \r\n", readECGSamples);
                
                // Check if FIFO has overflowed
                if( ETAG[readECGSamples - 1] == FIFO_OVF_MASK ){                  
                    EcgAFE.writeRegister( MAX30003::FIFO_RST , 0); // Reset FIFO
                    //LEDX=0;
                }
                
                // Print results 
                for( idx = 0; idx < readECGSamples; idx++ ) {
                  EcgUart.printf("Sample : %6d, \tETAG : 0x%x\r\n", ecgSample[idx], ETAG[idx]);
									AFEBLE.updateECG_S( ecgSample[idx]);
									//AFEBLE.updateHR((uint16_t)simpleECG);
                }
                EcgUart.printf("\r\n\r\n\r\n"); 
                                         
            }
        }
    }
}
void MaxAFE_t::ConfigAFE(MAX30003& ecgAFE){
    // Reset ECG to clear registers
    ecgAFE.writeRegister( MAX30003::SW_RST , 0);
    
    // General config register setting
    MAX30003::GeneralConfiguration_u CNFG_GEN_r;
    CNFG_GEN_r.bits.en_ecg = 1;     // Enable ECG channel
    CNFG_GEN_r.bits.rbiasn = 1;     // Enable resistive bias on negative input
    CNFG_GEN_r.bits.rbiasp = 1;     // Enable resistive bias on positive input
    CNFG_GEN_r.bits.en_rbias = 1;   // Enable resistive bias
    CNFG_GEN_r.bits.imag = 2;       // Current magnitude = 10nA
    CNFG_GEN_r.bits.en_dcloff = 1;  // Enable DC lead-off detection   
    ecgAFE.writeRegister( MAX30003::CNFG_GEN , CNFG_GEN_r.all);
        
    
    // ECG Config register setting
    MAX30003::ECGConfiguration_u CNFG_ECG_r;
    CNFG_ECG_r.bits.dlpf = 1;       // Digital LPF cutoff = 40Hz
    CNFG_ECG_r.bits.dhpf = 1;       // Digital HPF cutoff = 0.5Hz
    CNFG_ECG_r.bits.gain = 3;       // ECG gain = 160V/V
    CNFG_ECG_r.bits.rate = 2;       // Sample rate = 128 sps
    ecgAFE.writeRegister( MAX30003::CNFG_ECG , CNFG_ECG_r.all);
      
    
    //R-to-R configuration
    MAX30003::RtoR1Configuration_u CNFG_RTOR_r;
    CNFG_RTOR_r.bits.wndw = 0b0011;         // WNDW = 96ms
    CNFG_RTOR_r.bits.rgain = 0b1111;        // Auto-scale gain
    CNFG_RTOR_r.bits.pavg = 0b11;           // 16-average
    CNFG_RTOR_r.bits.ptsf = 0b0011;         // PTSF = 4/16
    CNFG_RTOR_r.bits.en_rtor = 1;           // Enable R-to-R detection
    ecgAFE.writeRegister( MAX30003::CNFG_RTOR1 , CNFG_RTOR_r.all);
       
        
    //Manage interrupts register setting
    MAX30003::ManageInterrupts_u MNG_INT_r;
    MNG_INT_r.bits.efit = 0b00011;          // Assert EINT w/ 4 unread samples
    MNG_INT_r.bits.clr_rrint = 0b01;        // Clear R-to-R on RTOR reg. read back
    ecgAFE.writeRegister( MAX30003::MNGR_INT , MNG_INT_r.all);
    
    
    //Enable interrupts register setting
    MAX30003::EnableInterrupts_u EN_INT_r;
    EN_INT_r.bits.en_eint = 1;              // Enable EINT interrupt
    EN_INT_r.bits.en_rrint = 1;             // Enable R-to-R interrupt
    EN_INT_r.bits.intb_type = 3;            // Open-drain NMOS with internal pullup
    ecgAFE.writeRegister( MAX30003::EN_INT , EN_INT_r.all);
       
       
    //Dyanmic modes config
    MAX30003::ManageDynamicModes_u MNG_DYN_r;
    MNG_DYN_r.bits.fast = 0;                // Fast recovery mode disabled
    ecgAFE.writeRegister( MAX30003::MNGR_DYN , MNG_DYN_r.all);
    
    // MUX Config
    MAX30003::MuxConfiguration_u CNFG_MUX_r;
    CNFG_MUX_r.bits.openn = 0;          // Connect ECGN to AFE channel
    CNFG_MUX_r.bits.openp = 0;          // Connect ECGP to AFE channel
    ecgAFE.writeRegister( MAX30003::CNFG_EMUX , CNFG_MUX_r.all);   
    // General config register setting
//    MAX30003::GeneralConfiguration_u CNFG_GEN_r;
//    CNFG_GEN_r.bits.en_ecg = 1;     // Enable ECG channel
//    CNFG_GEN_r.bits.rbiasn = 0;     // Enable resistive bias on negative input
//    CNFG_GEN_r.bits.rbiasp = 0;     // Enable resistive bias on positive input
//    CNFG_GEN_r.bits.en_rbias = 0;   // Enable resistive bias
//    CNFG_GEN_r.bits.imag = 4;       // Current magnitude = 50nA
//    CNFG_GEN_r.bits.en_dcloff = 1;  // Enable DC lead-off detection  
//		CNFG_GEN_r.bits.fmstr = 1; 			//32khz
//    ecgAFE.writeRegister( MAX30003::CNFG_GEN , CNFG_GEN_r.all);
       
   
    // ECG Config register setting
//    MAX30003::ECGConfiguration_u CNFG_ECG_r;
//    CNFG_ECG_r.bits.dlpf = 1;       // Digital LPF cutoff = 40Hz
//    CNFG_ECG_r.bits.dhpf = 1;       // Digital HPF cutoff = 0.5Hz
//    CNFG_ECG_r.bits.gain = 3;       // ECG gain = 160V/V
//    CNFG_ECG_r.bits.rate = 2;       // Sample rate = 125 sps
//    ecgAFE.writeRegister( MAX30003::CNFG_ECG , CNFG_ECG_r.all);
     
   
    //R-to-R configuration
//    MAX30003::RtoR1Configuration_u CNFG_RTOR_r;
//    CNFG_RTOR_r.bits.en_rtor = 1;           // Enable R-to-R detection
//    ecgAFE.writeRegister( MAX30003::CNFG_RTOR1 , CNFG_RTOR_r.all);
       
       
//    //Manage interrupts register setting
//    MAX30003::ManageInterrupts_u MNG_INT_r;
//    MNG_INT_r.bits.efit = 0b00011;          // Assert EINT w/ 4 unread samples
//    MNG_INT_r.bits.clr_rrint = 0b01;        // Clear R-to-R on RTOR reg. read back
//    ecgAFE.writeRegister( MAX30003::MNGR_INT , MNG_INT_r.all);


//    //Enable interrupts register setting
//    MAX30003::EnableInterrupts_u EN_INT_r;
//    EN_INT_r.all = 0;
//    EN_INT_r.bits.en_eint = 1;              // Enable EINT interrupt
//    EN_INT_r.bits.en_rrint = 0;             // Disable R-to-R interrupt
//    EN_INT_r.bits.intb_type = 3;            // Open-drain NMOS with internal pullup
//    ecgAFE.writeRegister( MAX30003::EN_INT , EN_INT_r.all);
       
       
    //Dyanmic modes config
//    MAX30003::ManageDynamicModes_u MNG_DYN_r;
//    MNG_DYN_r.bits.fast = 0;                // Fast recovery mode auto
//    ecgAFE.writeRegister( MAX30003::MNGR_DYN , MNG_DYN_r.all);
   
    // MUX Config
//    MAX30003::MuxConfiguration_u CNFG_MUX_r;
//    CNFG_MUX_r.bits.openn = 0;          // Connect ECGN to AFE channel
//    CNFG_MUX_r.bits.openp = 0;          // Connect ECGP to AFE channel
//    ecgAFE.writeRegister( MAX30003::CNFG_EMUX , CNFG_MUX_r.all);


//procentral arduino settings
// ecgAFE.writeRegister(MAX30003::CNFG_GEN,0x081007);
// ecgAFE.writeRegister(MAX30003::CNFG_CAL,0x720000);
// ecgAFE.writeRegister(MAX30003::CNFG_EMUX,0x0B0000);
// ecgAFE.writeRegister(MAX30003::CNFG_ECG,0x805000);
// ecgAFE.writeRegister(MAX30003::CNFG_RTOR1,0x3fc600);      

//mbed forum from maxim engineer 
// ecgAFE.writeRegister(MAX30003::CNFG_GEN,0x080000);
// ecgAFE.writeRegister(MAX30003::CNFG_CAL,0x000000);
// ecgAFE.writeRegister(MAX30003::CNFG_EMUX,0x000000);
// ecgAFE.writeRegister(MAX30003::CNFG_ECG,0x805000);
// ecgAFE.writeRegister(MAX30003::CNFG_RTOR1,0x3F6300);  
// ecgAFE.writeRegister(MAX30003::CNFG_RTOR1,0x202400);  
 ecgAFE.writeRegister( MAX30003::SYNCH,0);
}