
#ifndef  CCUTXglobals
 #define CCUTXglobals

 // don't give a value here to a variable

  
   extern byte DEBUG;
   extern int versionMajor;
   extern int versionMinor;
  
   extern byte _DMX[512];
   extern byte _tally[8];
  
  
   extern int CAMnumber;
   //extern byte _HDMIflag;
  
   extern bool CAMnumberChangedFlag;

  extern uint8_t ip[4];        // Will hold the Arduino IP address
  extern uint8_t atem_ip[4];  // Will hold the ATEM IP address
  extern uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)
  
  extern uint8_t subnet[4];
  extern uint8_t router[4];
  extern byte requestReturn[8];
  extern int changeDetected;
  extern int serverPort; //TouchOSC (incoming port)
  extern int destPort;    //TouchOSC (outgoing port)
  extern int OSCdelay;



 #endif
