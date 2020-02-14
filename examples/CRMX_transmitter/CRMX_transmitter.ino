/*  
 *   Wireless DMX transmit example
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

int DEBUG = 2;
byte mode = 1;

#include <CRMX_TimoTwo.h>

// when using ZERO boards MKR
#include <sam.h>

// CRMX TimoTwo variable needed to make the CRMX to work
int IRQPin = 6;
int SSPin  = 7;

bool UART_EN = 1; // External DMX output on 6-pin header
bool RADIO_ENABLE = 1; 

bool RADIO_TX_RX_MODE = true; // 0 = receiver, 1 = transmitter 
bool detected = false;

byte _dataBuffer[11]; // buffer readout for multiple commands

// LEDS
int LED   = LED_BUILTIN; // DMX Receive LED 
// LEDs connected on the MKR board
int redPin   = A0;
int greenPin = A1;
int bluePin  = A2;

// DMX parameters
int16_t START_ADDRESS_WINDOW = 1;    
int16_t WINDOW_SIZE = 64;

uint32_t DMX_REFRESH_PERIOD        = 35000; // 32 bits DMX length in uS
uint16_t TimeBetweenChannels  = 4; // time between channels in uS
uint16_t AmountOfChannels    = 512; // amount of DMX channels to generate

CRMX_TimoTwo timotwo;

bool IRQ_pending = false; // global parameter

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
    Serial.println("--------------------------------------");
    Serial.println("LumenRadio CRMX timotwo shield STARTED");
    
        if (RADIO_TX_RX_MODE == true){
             Serial.println("      Wireless Transmitter Mode");
        }else{
             Serial.println("       Wireless Receiver Mode");
        }
    Serial.println("--------------------------------------");
    }


   // config CRMX TimoTwo shield
   timotwo.begin(IRQPin, SSPin);
   timotwo.setCONFIG(UART_EN, RADIO_TX_RX_MODE, RADIO_ENABLE); // set Radio
   timotwo.setDMX_CONTROL(1); // set RXD/TXD active
   //timotwo.setDMX_WINDOW(START_ADDRESS_WINDOW, WINDOW_SIZE); 
   
   if (DEBUG >=1){Serial.println("CRMX timotwo started");}
   
   BlinkLed(5);

   // Read Hardware & Software version 
   detectTimoTwo(); // Detect CRMX board and show version numbers if debug = 1
   readDMXsettings();
 
   //timotwo.setDMX_SPEC(DMX_REFRESH_PERIOD, TimeBetweenChannels, AmountOfChannels);
   

   delay(1000);

}

void loop() {


  switch (mode){

      case 1:
                 // Change first 3 DMX channels

                      timotwo.writeDMX(1, 255);  // set DMX channel 1 @ 255
                      timotwo.writeDMX(2, 0);    // set DMX channel 2 @ 0
                      timotwo.writeDMX(3, 0);    // set DMX channel 3 @ 0
                      timotwo.transmitDMX();     // Wirlessly tranmit DMX universe
                       setColor(LOW, HIGH, HIGH);  
                    
                      delay(500);
                    
                      timotwo.writeDMX(1, 0);    // set DMX channel 1 @ 255
                      timotwo.writeDMX(2, 255);  // set DMX channel 2 @ 0
                      timotwo.writeDMX(3, 0);    // set DMX channel 3 @ 0
                      timotwo.transmitDMX();     // Wirlessly tranmit DMX universe
                       setColor(HIGH, LOW, HIGH);  
                    
                      delay(500);
                    
                      timotwo.writeDMX(1, 0);    // set DMX channel 1 @ 255
                      timotwo.writeDMX(2, 0);    // set DMX channel 2 @ 0
                      timotwo.writeDMX(3, 0);  // set DMX channel 3 @ 0
                      timotwo.transmitDMX();     // Wirlessly tranmit DMX universe
                       setColor(HIGH, HIGH, HIGH);  
                      
                      delay(500);


      break;

      case 2:
                 int length = 128;
                  for (int i=1;i<length;i++){
                    timotwo.writeDMX(i, i); 
                    
                  }
                
                  timotwo.transmitDMX();     // Wirlessly tranmit DMX universe
                  delay(10000);


      break;

    
  }
  

 

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



void readDMXsettings(){

  
  byte value[8]; // create array
  memset(value, 0, sizeof(value)); // load "0"'s

   for (int i=0;i<8;i++){
      value[i] = timotwo.getDMX_SPEC(i);

        
   }

    
    if (DEBUG >= 1){
      
      SerialUSB.println("DMX settings:");
      SerialUSB.println("-------------");
        SerialUSB.print("Amount of DMX channels: ");

        int Amount = (value[0] << 8) + value [1];
            SerialUSB.println(Amount);
      
            SerialUSB.print("Time between channels: ");
       int TimeBetween = (value[2] << 8) + value [3];
            SerialUSB.println(TimeBetween);

            SerialUSB.print("Universe length: ");


        unsigned long UniverseLength = ((value[4]<<24) | (value[5]<<16) | (value[6]<<8) | value[7]);

            SerialUSB.println(UniverseLength);
      
      
      
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
