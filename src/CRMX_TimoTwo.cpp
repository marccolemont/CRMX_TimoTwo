/*  CRMX library for Arduino
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

    Based on the data sheet provided by LumenRadio for the TimoTwo.
*/

 #include "CRMX_TimoTwo.h"

 #include <Arduino.h>
 #include <SPI.h>
 

CRMX_TimoTwo::CRMX_TimoTwo() {}

void CRMX_TimoTwo::begin()

{
  pinMode(_SSPin, OUTPUT);
  digitalWrite(_SSPin, HIGH); 
  pinMode(_IRQPin, INPUT);

  // Clear arrays
  memset(_DMX, 0, sizeof(_DMX)); 
  memset(_dataBuffer, 0, sizeof(_dataBuffer)); 
  SPI.begin();
}


void CRMX_TimoTwo::setCONFIG(){

  
  _dataBuffer[0] = 0b00000000;

  bitWrite(_dataBuffer[0], 0, UART_EN);
  bitWrite(_dataBuffer[0], 1, RADIO_TX_RX_MODE);
  bitWrite(_dataBuffer[0], 7, RADIO_ENABLE);

  writeRegister(CONFIG, 1); // tranmit to CRMX module
 
}


void CRMX_TimoTwo::setSTATUS(byte statusData){

  writeRegister(STATUS, statusData); // tranmit to CRMX module
 
}


void CRMX_TimoTwo::setDMX_CONTROL(byte CRMXdata){

  _dataBuffer[0] = CRMXdata;
  writeRegister(DMX_CONTROL, 1); // tranmit command to CRMX module, amount of bytes
  
}



void CRMX_TimoTwo::setRF_POWER(byte CRMXdata){

  _dataBuffer[0] = CRMXdata;
  writeRegister(RF_POWER, 1); // tranmit command to CRMX module, amount of bytes
  
}


void CRMX_TimoTwo::setIRQ_MASK()
{

     _dataBuffer[0] = 0b00000000;

    bitWrite(_dataBuffer[0], 0, RX_DMX_IRQ_ENbit);
    bitWrite(_dataBuffer[0], 1, LOST_DMX_IRQ_ENbit);
    bitWrite(_dataBuffer[0], 2, DMX_CHANGED_IRQ_ENbit);
    bitWrite(_dataBuffer[0], 3, RF_LINK_IRQ_ENbit);
    bitWrite(_dataBuffer[0], 4, ASC_IRQ_ENbit);
    bitWrite(_dataBuffer[0], 5, IDENTIFY_IRQ_ENbit);
    //bitWrite(_dataBuffer[0], 6, LOST_DMX_IRQ_EN);
    //bitWrite(_dataBuffer[0], 7, LOST_DMX_IRQ_EN);
  
    writeRegister(IRQ_MASK, 1); // tranmit to CRMX module, one byta of data

  
}


void CRMX_TimoTwo::setDMX_WINDOW(int16_t address, int16_t windowsize){

      _dataBuffer[0] = highByte(address);
      _dataBuffer[1] = lowByte (address);
      _dataBuffer[2] = highByte(windowsize);
      _dataBuffer[3] = lowByte (windowsize);
      
    
      writeRegister(DMX_WINDOW, 4); // tranmit to CRMX module, one byta of data
    
}




uint8_t CRMX_TimoTwo::getCONFIG(){

  readRegister(CONFIG, 1); // get data from CRMX module

  if (_CRMXbusy == true){
       readRegister(CONFIG, 1); // try again
  }

  return (_dataBuffer[0]);
  
          // Read received variables
        /*  
           UART_EN          = bitRead(dataBuffer[0], 0);
           RADIO_TX_RX_MODE = bitRead(dataBuffer[0], 1);
           RADIO_ENABLE     = bitRead(dataBuffer[0], 7);
          
        */
   
}





uint8_t CRMX_TimoTwo::getSTATUS(){

  readRegister(STATUS, 1); // get data from CRMX module

  if (_CRMXbusy == true){
       readRegister(CONFIG, 1); // try again
  }

  return (_dataBuffer[0]);
  
          // Read received variables
        /*  
           UART_EN          = bitRead(dataBuffer[0], 0);
           RADIO_TX_RX_MODE = bitRead(dataBuffer[0], 1);
           RADIO_ENABLE     = bitRead(dataBuffer[0], 7);
          
        */
   
}


uint8_t CRMX_TimoTwo::getVersionNumber(int8_t versionData){

      readRegister(VERSION, 8); // get data from CRMX module

      if (versionData <= 8){
        int value = _dataBuffer[versionData];     
             return (value);
      } 
  
}


uint8_t CRMX_TimoTwo::getLINK_QUALITY(){

      readRegister(LINK_QUALITY, 1); // get data from CRMX module

  
}



uint8_t CRMX_TimoTwo::getRF_POWER(){

      readRegister(RF_POWER, 1); // get data from CRMX module


  
}

uint8_t CRMX_TimoTwo::getIRQ_MASK(){

      
      readRegister(IRQ_MASK, 1); // get data from CRMX module

  
}

uint8_t CRMX_TimoTwo::getIRQ_FLAGS(){

      
      readRegister(IRQ_FLAGS, 1); // get data from CRMX module
      return (_dataBuffer[0]);
  
}


uint8_t CRMX_TimoTwo::getIRQ(){

   noInterrupts();

      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  IRQ_flagData = SPI.transfer(NOP); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
         

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
                return (0xFF);
          } else {
                return (IRQ_flagData);

          
          delayMicroseconds(100);  // default delay
          
       SPI.endTransaction();

  interrupts();
  
}


void CRMX_TimoTwo::readRegister(byte command, byte length){

   noInterrupts();
    
    int _counter = 0;

      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  IRQ_flagData = SPI.transfer(command+READ_REG); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
         

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
            return;
          }

          
          delayMicroseconds(100);  // default delay
          

                while ((digitalRead(_IRQPin)) == HIGH && _counter <= 5){
                  delayMicroseconds(300);
                  _counter++;
                  
                  
                } // add more delay if interrupt is still high
              
          // Receive data
          digitalWrite(_SSPin, LOW);
          
          delayMicroseconds(5);  // extra delay befor start

              // start Byte
              IRQ_flagData = SPI.transfer(NOP);
              delayMicroseconds(5);  // extra delay befor start

              // Data bytes to receive and store in the dataBuffer
               for (int i=0;i<length;i++){
                  _dataBuffer[i] = SPI.transfer(NOP); // Send command byte and read this first byte IRQ_flags
                  //SerialUSB.println(_dataBuffer[i]);
               }
              

      
          digitalWrite(_SSPin, HIGH);
          

          delayMicroseconds(550);  // default delay


       SPI.endTransaction();

  interrupts();
  
}


void CRMX_TimoTwo::writeRegister(byte command, byte length){

   noInterrupts();
    
    int _counter = 0;

      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  IRQ_flagData = SPI.transfer(command+WRITE_REG); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
          

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
            return;
          }

          
          delayMicroseconds(100);  // default delay
          

                while ((digitalRead(_IRQPin)) == HIGH && _counter <= 5){
                  delayMicroseconds(300);
                  _counter++;
                  
                  
                } // add more delay if interrupt is still high
              
          // Receive data
         digitalWrite(_SSPin, LOW);
          
          delayMicroseconds(5);  // extra delay befor start

              // start Byte
              IRQ_flagData = SPI.transfer(NOP);
              delayMicroseconds(5);  // extra delay befor start

              // Data bytes to receive and store in the dataBuffer
               for (int i=0;i<length;i++){
                 SPI.transfer(_dataBuffer[i]); // Send command byte and read this first byte IRQ_flags
               }
              

      
          digitalWrite(_SSPin, HIGH);
          
          delayMicroseconds(550);  // default delay


       SPI.endTransaction();

interrupts();
  
}


uint8_t CRMX_TimoTwo::readDMX(byte index)
{
   if(index >= 0 && index < WINDOW_SIZE)
   {
      //return DMX[index];
      return (_DMX[index]);
      
   }
   else 
   {
     // return -9999;
   }
}


void CRMX_TimoTwo::transmitDMX(){  // Tranmit DMX universe

    noInterrupts();
    int channel_start;
    int channel_end;

    if (RADIO_TX_RX_MODE == true){ // if radio is set as transmitter
    
          for (int i=0;i<4;i++){ // write 8 blocks of 64 channels
      
                channel_start = (i * 128) + 1;
                channel_end   = channel_start + 128;
                
                    _transmitDMX128ch(channel_start, channel_end); // Transmit block of 64 DMX channels
                    
                      // check if TimoTwo was not busy receiving the update 
                      if (_CRMXbusy == true){
                        
                           _transmitDMX128ch(channel_start, channel_end); // Transmit block of 64 DMX channels
                      }
            
          }
    }
    
    interrupts();
  
}




void CRMX_TimoTwo::getDMX(){  // Tranmit DMX universe

    noInterrupts();
    int channel_start;
    int channel_end;

    if (RADIO_TX_RX_MODE == false){ // if radio is set as receiver
    
          for (int i=0;i<4;i++){ // write 8 blocks of 64 channels
      
                channel_start = (i * 128) + 1;
                channel_end   = channel_start + 128;
                
              if (channel_start < WINDOW_SIZE){ // read another window of 128 channels if needed
                    _receiveDMX128ch(channel_start, channel_end); // Transmit block of 64 DMX channels
                    
                      // check if TimoTwo was not busy receiving the update
                      if (_CRMXbusy == true){
                        
                           _receiveDMX128ch(channel_start, channel_end); // Transmit block of 64 DMX channels
                      }
              }
            
          }
    }
    
  interrupts();
}






void CRMX_TimoTwo::_transmitDMX128ch(int channel_start, int channel_end)
{
  
  // tranmit block of 128 DMX channels
  int  _counter  = 0;


      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  IRQ_flagData = SPI.transfer(WRITE_DMX); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
          

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
            return;
          }

          
          delayMicroseconds(100);  // default delay
          

                while ((::digitalRead(_IRQPin)) == HIGH && _counter <= 5){
                  delayMicroseconds(300);
                  _counter++;
                  
                  
                } // add more delay if interrupt is still high
              
          // Receive data
         digitalWrite(_SSPin, LOW);
          
          delayMicroseconds(5);  // extra delay befor start

              // start Byte
              IRQ_flagData = SPI.transfer(NOP);
              delayMicroseconds(5);  // extra delay befor start

              // Data bytes to receive and store in the dataBuffer
               for (int i=channel_start;i<channel_end;i++){
                 SPI.transfer(_DMX[i]); // Send command byte and read this first byte IRQ_flags
               }
              

      
          digitalWrite(_SSPin, HIGH);
          
          delayMicroseconds(550);  // default delay


       SPI.endTransaction();



  
}

void CRMX_TimoTwo::_receiveDMX128ch(int channel_start, int channel_end)
{
  
  // receive block of 128 DMX channels
  int  _counter  = 0;


      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  IRQ_flagData = SPI.transfer(READ_DMX); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
          

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
            return;
          }

          
          delayMicroseconds(100);  // default delay
          

                while ((::digitalRead(_IRQPin)) == HIGH && _counter <= 5){
                  delayMicroseconds(300);
                  _counter++;
                  
                  
                } // add more delay if interrupt is still high
              
          // Receive data
         digitalWrite(_SSPin, LOW);
          
          delayMicroseconds(5);  // extra delay befor start

              // start Byte
              IRQ_flagData = SPI.transfer(NOP);
              delayMicroseconds(5);  // extra delay befor start

              // Data bytes to receive and store in the dataBuffer
               for (int i=channel_start;i<channel_end;i++){
                _DMX[i] = SPI.transfer(NOP); // Receive DMX byte
               }
              

      
          digitalWrite(_SSPin, HIGH);
          
          delayMicroseconds(550);  // default delay


       SPI.endTransaction();



  
}
