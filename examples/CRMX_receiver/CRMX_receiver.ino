/*  
 *   Wireless DMX receiver example
 *   
 *   CRMX library for Arduino
    Copyright (C) 2020 MC-productions 
    Marc Colemont (marc.colemont@mc-productions.be)

    This librarty works together with the MKR CRMX_TimoTwo shield
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    This code is licensed under the GNU LGPL and is open for ditrbution
    and copying in accordance with the license.
    This header must be included in any derived code or copies of the code.

    Based on the data sheet provided by LumenRadio for the CRMX TimoTwo.

    To Pair two devices, press first on the receiver(s) the Link button.
    Then on the transmitter Press the Link button. 
    After a few seconds the boards will be linked together.
    On the transmitters and receivers the RGB LED will go on in magenta.
    On the receivers the reception quality will be shown on the orange leds.
*/

int DEBUG = 1;

#include <CRMX_TimoTwo.h>
#include <CCU_decoder.h>




// when using ZERO boards MKR
#include <sam.h>

// CRMX TimoTwo variable needed to make the CRMX to work
// CONFIG
int _IRQPin = 6;
int _SSPin  = 7;

// DMX Fixture window
int16_t START_ADDRESS_WINDOW = 0;    
int16_t WINDOW_SIZE = 10;

bool UART_EN = 1; // External DMX output on 6-pin header
bool RADIO_ENABLE = 1; 

bool RADIO_TX_RX_MODE = false; // 0 = receiver, 1 = transmitter 
bool detected = false;

#define UART_MASK          (1 << 0)
#define RADIO_TX_RX_MASK   (1 << 1)
#define RADIO_ENABLE_MASK  (1 << 7)

// IRQ MAsk parameters extern bool RX_DMX_IRQ_ENbit;
bool RX_DMX_IRQ_ENbit       = 0;
bool LOST_DMX_IRQ_ENbit     = 0;
bool DMX_CHANGED_IRQ_ENbit  = 1;
bool RF_LINK_IRQ_ENbit      = 0;
bool ASC_IRQ_ENbit          = 0; 
bool IDENTIFY_IRQ_ENbit     = 0;

// IRQ
#define RX_DMX_IRQ_MASK       (1 << 0)
#define LOST_DMX_IRQ_MASK     (1 << 1)
#define DMX_changed_IRQ_MASK  (1 << 2)
#define RF_LINK_IRQ_MASK      (1 << 3)
#define ASC_IRQ_MASK          (1 << 4)
#define IDENTIFY_IRQ          (1 << 5)

// STATUS
#define LINKED_MASK           (1 << 0)
#define RF_LINK_MASK          (1 << 1)
#define IDENTIFY_MASK         (1 << 2)
#define DMX_OK_MASK           (1 << 3)

#define UPDATE_MODE_MASK      (1 << 7)



byte _dataBuffer[11]; // buffer readout for multiple commands
byte DMX[513];
//extern byte _DMX[513];  // extern DMX register LumenRadio (updated after getDMX command)

// LEDS
int LED   = LED_BUILTIN; // DMX Receive LED 
// LEDs connected on the MKR board
int redPin   = A0;
int greenPin = A1;
int bluePin  = A2;


CRMX_TimoTwo timotwo;


void setup() {


   // Setup LEDS
   pinMode(LED, OUTPUT);
   pinMode(redPin, OUTPUT);
   pinMode(greenPin, OUTPUT);
   pinMode(bluePin, OUTPUT);

   // Setup debug
   if (DEBUG >=1){
    SerialUSB.begin(115200);
    delay(5000);
    SerialUSB.println("--------------------------------------");
    SerialUSB.println("LumenRadio CRMX timotwo shield STARTED");
    
        if (RADIO_TX_RX_MODE == true){
             SerialUSB.println("      Wireless Transmitter Mode");
        }else{
             SerialUSB.println("       Wireless Receiver Mode");
        }
    SerialUSB.println("--------------------------------------");
    }

   // config CRMX TimoTwo shield
   timotwo.begin(_IRQPin, _SSPin); // start SPI CRMX TimoTwo & interrupt
   timotwo.setCONFIG(UART_EN, RADIO_TX_RX_MODE, RADIO_ENABLE); // set Radio
   timotwo.setDMX_CONTROL(0); // set RXD/TXD active
   timotwo.setIRQ_MASK(RX_DMX_IRQ_ENbit, LOST_DMX_IRQ_ENbit, DMX_CHANGED_IRQ_ENbit, RF_LINK_IRQ_ENbit, ASC_IRQ_ENbit, IDENTIFY_IRQ_ENbit);
   timotwo.setDMX_WINDOW(START_ADDRESS_WINDOW, WINDOW_SIZE); // create window in DMX receiver which returns an interrupt when set
   timotwo.setBATTERY_LEVEL(255);

   if (DEBUG >=1){SerialUSB.println("CRMX timotwo started");}
   
   BlinkLed(5);

   // Read Hardware & Software version 
   detectTimoTwo(); // Detect CRMX board and show version numbers if debug =1
   checkDMXconfig();
   
    
   delay(1000);

}

void loop() {


      // Read IRQ Flags
      if (timotwo.newEventIRQ()){
          
              
              /*
              RX_DMX_IRQbit         0 // Complete DMX frame received interrupt (0)
              LOST_DMX_IRQbit       1 // Loss of DMX interrupt (0)
              DMX_CHANGED_IRQbit    2 // DMX changed in DMX window interrupt (0)
              RF_LINK_IRQbit        3 // Radio link status change interrupt (0)
        
              */

          int flagData = timotwo.getIRQ_FLAGS();
              
                          if (DEBUG >=4){
                            SerialUSB.print("IRQ_Flags: ");
            
                              for (int i = 7; i >= 0; i--) {
                                    if (flagData & (1 << i)) {
                                      SerialUSB.print('1');
                                    } else {
                                      SerialUSB.print('0');
                                    }
                                    SerialUSB.println("");
                              }
                          }
               
              
                      // Check flags
                      if (flagData & DMX_changed_IRQ_MASK){
                      
                          checkDMX();
                          RGB_DMXled();
                      }
                
                      
      }
      

} // end loop



void RF_LINK_changed(){

  byte value = timotwo.getLINK_QUALITY();

  if (DEBUG >= 2){
      
      SerialUSB.print("RF Link quality: ");
      SerialUSB.println(value);
  }
}



void checkDMXconfig(){

      byte value[4]; // create array
        memset(value, 0, sizeof(value)); // load "0"'s
      
         for (int i=0;i<4;i++){
            value[i] = timotwo.getDMX_WINDOW(i);
      
              
         }
      
          
           if (DEBUG >= 1){
            
            SerialUSB.println("DMX WINDOW settings:");
            SerialUSB.println("-------------");
              SerialUSB.print("Start Address: ");
      
              int16_t start    = (value[2] << 8) + value [3];
                  SerialUSB.println(start);
            
                  SerialUSB.print("Window size: ");
              int16_t channels = (value[0] << 8) + value [1];
                  SerialUSB.println(channels);
            
            
            } 


}

void checkDMX(){

     timotwo.getDMX();     // Read the DMX window / frame into the DMX register
    
        int  length  = WINDOW_SIZE;
        bool changed = false;
        int  value;
        
              // read the first 3 DMX values
              for (int i=0;i<length;i++){
            
                value = timotwo.readDMXch(i);
            
                      // check is the DMX value has been changed from last time
                      if (value != DMX[i])
                      {
                        DMX[i] = value;
                        changed = true;
                      }
              }
      
    
      if (changed == true)
      {
        
        changed = false; 
      
          if (DEBUG >= 1){
              
              SerialUSB.print("DMX channels 1-3: ");
                SerialUSB.print(DMX[0]);
                SerialUSB.print(" : ");
                SerialUSB.print(DMX[1]);
                SerialUSB.print(" : ");
              SerialUSB.println(DMX[2]);
          }
      }
            
       

  
}

void RGB_DMXled(){

    bool red;
    bool green;
    bool blue;
    
    
    if (DMX[0]<= 128){
      red = true;
    } else {
      red = false;
    }

    if (DMX[1] <= 128){
      green = true;
    } else {
      green = false;
    }

    if (DMX[2] <= 128){
      blue = true;
    } else {
      blue = false;
    }

    digitalWrite(redPin  , red);
    digitalWrite(greenPin, green);
    digitalWrite(bluePin , blue); 
   
}

void detectTimoTwo(){

  detected = false;
  byte value[8]; // create array
  memset(value, 0, sizeof(value)); // load "0"'s

       for (int i=0;i<8;i++){
          value[i] = timotwo.getVersionNumber(i);
    
            if (value[i] > 0){
              detected = true;
            
          }
          delay(10);
       }
    
       if (detected == true)
       { 
        BlinkLed(3);
        setColor(HIGH, HIGH, LOW);    // BLUE
        if (DEBUG >= 1 && detected == true){
          
          SerialUSB.println("LumenRadio Detected");
          SerialUSB.println("-------------------");
            SerialUSB.print("Hardware version: ");
          SerialUSB.print(value[0]);
          SerialUSB.print(".");
          SerialUSB.print(value[1]);
          SerialUSB.print(".");
          SerialUSB.print(value[2]);
          SerialUSB.print(".");
          SerialUSB.println(value[3]);
    
            SerialUSB.print("Firmware version: ");
          SerialUSB.print(value[4]);
          SerialUSB.print(".");
          SerialUSB.print(value[5]);
          SerialUSB.print(".");
          SerialUSB.print(value[6]);
          SerialUSB.print(".");
          SerialUSB.println(value[7]);
    
          } 
       }      
}

void BlinkLed(byte num)         // Basic blink function
{
    for (byte i=0;i<num;i++)
    {
            setColor(LOW, LOW, LOW);    // white
            digitalWrite(LED, HIGH);
       
        delay(50);
        
            setColor(HIGH, HIGH, HIGH);    // black
            digitalWrite(LED, LOW);
        delay(50);
    }
}

void setColor(bool red, bool green, bool blue) {
  digitalWrite(redPin  , red);
  digitalWrite(greenPin, green);
  digitalWrite(bluePin , blue); 
}
