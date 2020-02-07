
#ifndef HEADER_GLOBALS
 #define HEADER_GLOBALS

 // Radio parameters
 
 extern int DEBUG;
 extern bool RADIO_TX_RX_MODE;
 //extern bool RADIO_ENABLE;

 // DMX parameters
 extern int16_t START_ADDRESS_WINDOW;    // Depends on CAM selection
 extern int16_t WINDOW_SIZE; // CCU CAMERA DATA SIZE 

 // IRQ Mask parameters

 //extern bool RX_DMX_IRQ_ENbit;
 //extern bool LOST_DMX_IRQ_ENbit;
 //extern bool DMX_CHANGED_IRQ_ENbit;
 //extern bool RF_LINK_IRQ_ENbit;
 //extern bool ASC_IRQ_ENbit;
 //extern bool IDENTIFY_IRQ_ENbit;
  
 
 // Internal parameters
 extern int _IRQPin;
 extern int _SSPin;

 extern byte _dataBuffer[11];
 extern byte _DMX[513];
 
#endif
