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

 //#include <Arduino.h>
 //#include <SPI.h>
 

CRMX_TimoTwo::CRMX_TimoTwo() {}

CRMX_TimoTwo * CRMX_TimoTwo::instance0_ = NULL;

void CRMX_TimoTwo::begin(uint8_t interruptPin, uint8_t SPIpin)

{
    _SSPin  = SPIpin;
    _IRQPin = interruptPin;
    
  pinMode(_SSPin, OUTPUT);
  digitalWrite(_SSPin, HIGH); 
  pinMode(_IRQPin, INPUT);
    
  // activate the IRQ
  attachInterrupt(digitalPinToInterrupt(_IRQPin), isr0, FALLING);
    instance0_ = this;
    
    
  // Clear arrays
  memset(_DMX, 0, sizeof(_DMX)); 
  memset(_dataBuffer, 0, sizeof(_dataBuffer)); 
  SPI.begin();
    if (DEBUG >=1){
        SerialUSB.println("SPI started");
        SerialUSB.print("SPI Pin: ");
        SerialUSB.println(_SSPin);
        SerialUSB.print("IRQ Pin: ");
        SerialUSB.println(_IRQPin);
    }
    
}

void CRMX_TimoTwo::isr0 (){
    if (CRMX_TimoTwo::instance0_ != NULL){
        CRMX_TimoTwo::instance0_->IRQ_handler();
        
    }
}  // end

void CRMX_TimoTwo::IRQ_handler(){
         
    IRQ_pending = true;
          
}



void CRMX_TimoTwo::setCONFIG(bool a, bool b, bool c){

  
  _dataBuffer[0] = 0b00000000;

  bitWrite(_dataBuffer[0], 0, a); // UART_ENbit
  bitWrite(_dataBuffer[0], 1, b); // RADIO_TX_RX_MODEbit
  bitWrite(_dataBuffer[0], 7, c); // RADIO_ENABLEbit

  
  
    writeRegister(CONFIG, 1); // tranmit to CRMX module
        
        if (_CRMXbusy == true){
             writeRegister(CONFIG, 1); // try again
        }
 if (DEBUG >=2 && _CRMXbusy == false){
     SerialUSB.print("setCONFIG data: ");
     SerialUSB.println(_dataBuffer[0]);
 }
}


bool CRMX_TimoTwo::IRQ_detected(){
    
    noInterrupts();
    bool pending = IRQ_pending;
    IRQ_pending = false;
    interrupts();
    return pending;
    
}
                  
                  
void CRMX_TimoTwo::setSTATUS(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h){
    
    bitWrite(_dataBuffer[0], 0, a);
    bitWrite(_dataBuffer[0], 1, b);
    bitWrite(_dataBuffer[0], 2, c);
    bitWrite(_dataBuffer[0], 3, d);
    bitWrite(_dataBuffer[0], 4, e);
    bitWrite(_dataBuffer[0], 5, f);
    bitWrite(_dataBuffer[0], 6, g);
    bitWrite(_dataBuffer[0], 7, h);
    
  
  writeRegister(STATUS, 1); // tranmit to CRMX module
    
        if (_CRMXbusy == true){
             writeRegister(STATUS, 1); // try again
        }
    
    if (DEBUG >=2 && _CRMXbusy == false){
        SerialUSB.print("setSTATUS data: ");
        SerialUSB.println(_dataBuffer[0]);
    }
}


void CRMX_TimoTwo::setDMX_CONTROL(byte CRMXdata){

  _dataBuffer[0] = CRMXdata;
    
  
 writeRegister(DMX_CONTROL, 1); // tranmit command to CRMX module, amount of bytes
      
      if (_CRMXbusy == true){
           writeRegister(DMX_CONTROL, 1); // try again
      }
  if (DEBUG >=2 && _CRMXbusy == false){
      SerialUSB.print("setDMX_CONTROL data: ");
      SerialUSB.println(_dataBuffer[0]);
  }
}


void CRMX_TimoTwo::setDMX_SPEC(uint32_t REFRESH, uint16_t INTERSLOT, uint16_t AmountCHANNELS){
    
    _dataBuffer[0] = AmountCHANNELS >> 8;
    _dataBuffer[1] = AmountCHANNELS;
    
    _dataBuffer[2] = INTERSLOT >> 8;
    _dataBuffer[3] = INTERSLOT ;
    
    _dataBuffer[4] = REFRESH >> 24;
    _dataBuffer[5] = REFRESH >> 16;
    _dataBuffer[6] = REFRESH >> 8;
    _dataBuffer[7] = REFRESH;
    
    N_CHANNELS = AmountCHANNELS;
    

    if (DEBUG >=3){
        
        int i;
        SerialUSB.print("REFRESH: ");
        for (i=4;i<8;i++){
            SerialUSB.print(_dataBuffer[i], BIN);
            
        }
        SerialUSB.println("");
        
        SerialUSB.print("  Interslot: ");
        for (i=2;i<4;i++){
            SerialUSB.print(_dataBuffer[i], BIN);
            
        }
        SerialUSB.println("");
        SerialUSB.print("  AmountOfChannels: ");
        for (i=0;i<2;i++){
            SerialUSB.print(_dataBuffer[i], BIN);
            
        }
        SerialUSB.println("");
    }
    

    writeRegister(DMX_SPEC, 1); // tranmit to CRMX module, 8x one byta of data

        if (_CRMXbusy == true){
             writeRegister(DMX_SPEC, 8); // try again
        }
    
 
}




void CRMX_TimoTwo::setRF_POWER(byte CRMXdata){

  _dataBuffer[0] = CRMXdata;
    
  
  writeRegister(RF_POWER, 1); // tranmit command to CRMX module, amount of bytes
      
    if (_CRMXbusy == true){
           writeRegister(RF_POWER, 1); // try again
      }
    
    if (DEBUG >=2 && _CRMXbusy == false){
        SerialUSB.print("setRF-POWER data: ");
        SerialUSB.println(_dataBuffer[0]);
    }
}


void CRMX_TimoTwo::setIRQ_MASK(bool a, bool b, bool c, bool d, bool e, bool f)
{

     _dataBuffer[0] = 0b00000000;
    bitWrite(_dataBuffer[0], 0, a); // RX_DMX_IRQ_ENbit
    bitWrite(_dataBuffer[0], 1, b); // LOST_DMX_IRQ_ENbit
    bitWrite(_dataBuffer[0], 2, c); // DMX_CHANGED_IRQ_ENbit
    bitWrite(_dataBuffer[0], 3, d); // RF_LINK_IRQ_ENbit
    bitWrite(_dataBuffer[0], 4, e); // ASC_IRQ_ENbit
    bitWrite(_dataBuffer[0], 5, f); // IDENTIFY_IRQ_ENbit
//    bitWrite(_dataBuffer[0], 6,  LOST_DMX_IRQ_EN);
//    bitWrite(_dataBuffer[0], 7, LOST_DMX_IRQ_EN)
    
  
   writeRegister(IRQ_MASK, 1); // tranmit to CRMX module, one byta of data
  
      if (_CRMXbusy == true){
           writeRegister(IRQ_MASK, 1); // try again
      }
  if (DEBUG >=2 && _CRMXbusy == false){
      SerialUSB.print("setMASK data: ");
      SerialUSB.println(_dataBuffer[0]);
  }
}


void CRMX_TimoTwo::setDMX_WINDOW(int16_t address, int16_t windowsize){

      _dataBuffer[0] = windowsize >> 8;;
      _dataBuffer[1] = windowsize;
      _dataBuffer[2] = address >> 8;
      _dataBuffer[3] = address;
    
    WINDOW_SIZE = windowsize;
    START_ADDRESS_WINDOW = address;
      
    
     writeRegister(DMX_WINDOW, 4); // tranmit to CRMX module, one byta of data
    
        if (_CRMXbusy == true){
             writeRegister(DMX_WINDOW, 4); // try again
        }
    if (DEBUG >=2 && _CRMXbusy == false){
        SerialUSB.print("setDMX_WINDOW address: ");
        SerialUSB.print(address);
        //SerialUSB.print(_dataBuffer[0]);
        //SerialUSB.print(_dataBuffer[1]);
        SerialUSB.print(" Size: ");
        SerialUSB.print(windowsize);
        //SerialUSB.print(_dataBuffer[2]);
        //SerialUSB.println(_dataBuffer[3]);
    }
}


void CRMX_TimoTwo::setBATTERY_LEVEL(byte level){

      _dataBuffer[0] = level;
   
    
     writeRegister(BATTERY_LEVEL, 1); // tranmit to CRMX module, one byta of data
    
        if (_CRMXbusy == true){
             writeRegister(BATTERY_LEVEL, 1); // try again
        }
    
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
        
            if (_CRMXbusy == true){
                 readRegister(VERSION, 8); // try again
            }

      if (versionData <= 8){
        int value = _dataBuffer[versionData];     
             return (value);
      } 
  
}


uint8_t CRMX_TimoTwo::getDMX_SPEC(int8_t dmxData){

      readRegister(DMX_SPEC, 8); // get data from CRMX module
        
            if (_CRMXbusy == true){
                 readRegister(DMX_SPEC, 8); // try again
            }

      if (dmxData <= 8){
        int value = _dataBuffer[dmxData];
             return (value);
      }
  
}



uint8_t CRMX_TimoTwo::getDMX_WINDOW(byte data){

      readRegister(DMX_WINDOW, 4); // get data from CRMX module
        
            if (_CRMXbusy == true){
                 readRegister(DMX_WINDOW, 4); // try again
            }

      if (data <= 4){
        int value = _dataBuffer[data];
             return (value);
      }
  
}


uint8_t CRMX_TimoTwo::getLINK_QUALITY(){

      readRegister(LINK_QUALITY, 1); // get data from CRMX module
    
            if (_CRMXbusy == true){
                 readRegister(LINK_QUALITY, 1); // try again
            }
      return (_dataBuffer[0]);
  
}



uint8_t CRMX_TimoTwo::getRF_POWER(){

      readRegister(RF_POWER, 1); // get data from CRMX module
    
            if (_CRMXbusy == true){
                 readRegister(RF_POWER, 1); // try again
            }
      return (_dataBuffer[0]);

  
}

uint8_t CRMX_TimoTwo::getIRQ_MASK(){

      
      readRegister(IRQ_MASK, 1); // get data from CRMX module
            if (_CRMXbusy == true){
                 readRegister(IRQ_MASK, 1); // try again
            }
      return (_dataBuffer[0]);

  
}

uint8_t CRMX_TimoTwo::getIRQ_FLAGS(){

      
      readRegister(IRQ_FLAGS, 1); // get data from CRMX module
            if (_CRMXbusy == true){
                 readRegister(IRQ_FLAGS, 1); // try again
            }
      return (_dataBuffer[0]);
  
}


uint8_t CRMX_TimoTwo::getIRQ(){

   noInterrupts();

      SPI.beginTransaction(TimoTwo_Settings);


          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                  _dataBuffer[0] = SPI.transfer(NOP); // Send command byte and read this first byte IRQ_flags
      
          digitalWrite(_SSPin, HIGH);
         

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
            delayMicroseconds(550);  // default delay
                return (0xFF);
          } else {
                return (_dataBuffer[0]);
          }

          
          delayMicroseconds(100);  // default delay
          
       SPI.endTransaction();

  interrupts();
  
}




uint8_t CRMX_TimoTwo::readDMXch(byte index)
{
   if(index >= 0 && index <= WINDOW_SIZE)
   {
      //return DMX[index];
      return (_DMX[index]);
      
   }
   else 
   {
     // return -9999;
   }
}





void CRMX_TimoTwo::writeDMX(int16_t index, byte value)
{
    _DMX[index] = value;
    
    if (DEBUG >=3 && _CRMXbusy == false){
        SerialUSB.print("write DMX data: ");
        SerialUSB.println(_DMX[index]);
    }
    
}

                               
                               
void CRMX_TimoTwo::transmitDMX()
{  // Tranmit DMX universe
    
    
    int error = -1;
    byte counter = 0;
    bool firsttime = true;
    
    if (DEBUG >=3){SerialUSB.println("Transmitting DMX");}

    //if (RADIO_TX_RX_MODE == true){ // if radio is set as transmitter
    
    // try upto 3x if error occurs
    while (error < 0 && counter < 3){
        
        // set error variable to start commands
        if (firsttime == true){
            firsttime = false;
            error = 0;
        }
            
            // write upto 4 blocks of 128 channels (depending on window)
            for (int i=0;i<4;i++){
                    
                    // calulate start address for each block
                    channel_start = (i * 128) ;
                    channel_end   = channel_start + 128;
                
                    if (DEBUG >=1){
                        SerialUSB.print("Write Channels: ");
                        SerialUSB.print(channel_start);
                        SerialUSB.print(" : ");
                        SerialUSB.println(channel_end);
                        SerialUSB.print("Window Size ");
                        SerialUSB.println(WINDOW_SIZE);
                    }
                                  
                    // read another window of 128 channels if needed, don't if error occured
                    if (channel_start < N_CHANNELS && error >= 0)
                        {
                            
                            // calculate the amount of channels to transmit
                            int channels;
                                if ((N_CHANNELS - channel_start) >= 128){
                                    
                                    channels = 128;
                                    
                                }else{
                                
                                    channels = (N_CHANNELS - channel_start)-1;
                                }
                                    
                                    // read DMX values into _DMX[i] based on channel_start value
                                      writeRegister(WRITE_DMX, channels);
                                      if (_CRMXbusy == true){
                                          error = -1;
                                      }
                        } // end transmit
                  
              } // end 4 blocks of 128 bytes
        
        // if error occured during getting 4 upto blocks of 128 channels
        if (error < 0){
                counter++;
            readRegister(NOP, 1); // refresh command before trying again/
        }
        
    } // end While
    
    // we tried 3x times to read DMX
    if (counter >= 3){
        //return;
        _CRMXbusy = true;
    
    }
    

   //return 0; // all OK
  
}
                               




void CRMX_TimoTwo::getDMX(){  // Tranmit DMX universe
    
    int error = -1;
    byte counter = 0;
    bool firsttime = true;

    //if (RADIO_TX_RX_MODE == false){ // if radio is set as receiver
    
    // try upto 3x if error occurs
    while (error < 0 && counter < 3){
        
        // set error variable to start commands
        if (firsttime == true){
            firsttime = false;
            error = 0;
        }
            
            // write upto 4 blocks of 128 channels (depending on window)
            for (int i=0;i<4;i++){
                    
                    // calulate start address for each block
                    channel_start = (i * 128);
                    channel_end   = channel_start + 128;
                                  
                    // read another window of 128 channels if needed, don't if error occured
                    if (channel_start < WINDOW_SIZE && error >= 0)
                        {
                              // calculate the amount of channels to transmit
                              int channels;
                                  if ((WINDOW_SIZE - channel_start) >= 128){
                                      
                                      channels = 128;
                                      
                                  }else{
                                  
                                      channels = (WINDOW_SIZE - channel_start)-1;
                                  }
                              if (DEBUG >=3){
                                  SerialUSB.print("Read Channels: ");
                                  SerialUSB.print(channel_start);
                                  SerialUSB.print(" : ");
                                  SerialUSB.println(channel_start + channels);
                                  SerialUSB.print("Window Size ");
                                  SerialUSB.println(WINDOW_SIZE);
                              }
                                
                                // read DMX values into _DMX[i] based on channel_start value
                                  readRegister(READ_DMX, channels);
                            if (_CRMXbusy == true){
                                error = -1;
                            }

                        } // end transmit
                  
              } // end 4 blocks of 128 bytes
        
        // if error occured during getting 4 upto blocks of 128 channels
        if (error < 0){
                counter++;
            delay(10);
            readRegister(IRQ_MASK, 1); // refresh command before trying again/
        }
        
    } // end While
    
    // we tried 3x times to read DMX
    if (counter >= 3){
        //return ();
        _CRMXbusy = true;
    
    }
    
    readRegister(IRQ_MASK, 1); // refresh command before reading DMX again
    
}

uint8_t CRMX_TimoTwo::readRegister(byte command, byte length){

    
    currentMillisTime  = millis();
    previousMillisCRMX = currentMillisTime;
    

      SPI.beginTransaction(TimoTwo_Settings);

      
          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                        switch (command){
                                
                                case READ_DMX:
                                        IRQ_flagData = SPI.transfer(READ_DMX); // Send command byte and read this first byte IRQ_flags
                                            if (DEBUG >=3){
                                                SerialUSB.print("READ_DMX: 0x");
                                                SerialUSB.println(READ_DMX, HEX);
                                            }
                                    break;
                                    
                                default:
                                        IRQ_flagData = SPI.transfer(command+READ_REG); // Send command byte and read this first byte IRQ_flags
                                
                                            if (DEBUG >=3){
                                                SerialUSB.print("Read command: 0x");
                                                SerialUSB.println(command+READ_REG, HEX);
                                            }
                                    break;
                              
                        }
    
                  
          digitalWrite(_SSPin, HIGH);
          

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
              if (DEBUG >=1){
                  SerialUSB.print("ERROR 1 IRQ_flag data: ");
                  SerialUSB.println(IRQ_flagData);
              }
            return -1 ;
          }
    
            

          // check interrupt to go low within a certain time
          delayMicroseconds(100);  // default delay
    
    //delayMicroseconds(800);  // default delay
    
          

//                while ((digitalRead(_IRQPin)) == HIGH &&
//                       (currentMillisTime - previousMillisCRMX <= intervalCRMX)){
//                    currentMillisTime  = millis();
    
                 while(!IRQ_detected()) {
                     
                     // Check if Timeout occured
                     if (currentMillisTime - previousMillisCRMX >= intervalCRMX){
                       _CRMXbusy = true;
                         if (DEBUG >=1){
                             SerialUSB.println("ERROR Timeout ");
                         }
                       return -1 ;
                     }
                     
                 }
                        
                
    
                        
     
    // if the command is other then NOP
    if (command != NOP){
              // Receive data
             digitalWrite(_SSPin, LOW);
              
              delayMicroseconds(5);  // extra delay befor start

                  // start Byte
                  IRQ_flagData = SPI.transfer(NOP);
                  delayMicroseconds(5);  // extra delay befor start
        
                    // Check if CRMX is busy
                             if (bitRead (IRQ_flagData, 7) == HIGH){
                               _CRMXbusy = true;
                                 if (DEBUG >=1){
                                     SerialUSB.print("ERROR 2 IRQ_flag data: ");
                                     SerialUSB.println(IRQ_flagData);
                                 }
                               return -1 ;
                             }
                
                            switch (command){
                                    
                                    case READ_DMX:
                                            if (DEBUG >=3){
                                                SerialUSB.print("DMX channel start: ");
                                                SerialUSB.println(channel_start);
                                            }
                                            // Data bytes to receive and store in the dataBuffer
                                            for (int16_t i=channel_start; i<length+channel_start; i++){
                                              _DMX[i] = SPI.transfer(NOP); // Send command byte and read this first byte IRQ_flags
                                                if (DEBUG >=2){
                                                    SerialUSB.print("DMX ");
                                                    SerialUSB.print(i);
                                                    SerialUSB.print(": ");
                                                    SerialUSB.println(_DMX[i]);
                                                    
                                                }
                                                delayMicroseconds(5);  // extra delay befor start
                                            }
                                            
                                        
                                        break;
                                        
                                    default:
                                            // Data bytes to receive and store in the dataBuffer
                                            for (int i=0;i<length;i++){
                                              _dataBuffer[i] = SPI.transfer(NOP); // Send command byte and read this first byte IRQ_flags
                                            }
                                        break;
                                    
                                    
                            }
          
              digitalWrite(_SSPin, HIGH);
            
    }
          
          delayMicroseconds(500);  // default delay


       SPI.endTransaction();
       return 0; // all OK


  
}


uint8_t CRMX_TimoTwo::writeRegister(byte command, byte length){
    
    currentMillisTime  = millis();
    previousMillisCRMX = currentMillisTime;
    int _counter = 0;

      SPI.beginTransaction(TimoTwo_Settings);

      
          _CRMXbusy = false;
          // command byte
          digitalWrite(_SSPin, LOW);
          
           delayMicroseconds(5);  // extra delay befor start
 
                        switch (command){
                                
                                case WRITE_DMX:
                                        IRQ_flagData = SPI.transfer(WRITE_DMX); // Send command byte and read this first byte IRQ_flags
                                            if (DEBUG >=3){
                                                SerialUSB.print("WRITE_DMX: 0x");
                                                SerialUSB.println(WRITE_DMX, HEX);
                                            }
                                    break;
                                    
                                default:
                                    
                                    byte transmit = command + WRITE_REG;
                                        IRQ_flagData = SPI.transfer(transmit); // Send command byte and read this first byte IRQ_flags
                                            if (DEBUG >=3){
                                                SerialUSB.print("Write command: 0x");
                                                SerialUSB.println(transmit, HEX);
                                            }
                                
                                break;
                              
                        }
    
                  
          digitalWrite(_SSPin, HIGH);
          

          
          // Check if CRMX is busy
          if (bitRead (IRQ_flagData, 7) == HIGH){
            _CRMXbusy = true;
              if (DEBUG >=1){
                  SerialUSB.print("ERROR 1 IRQ_flag data: ");
                  SerialUSB.println(IRQ_flagData);
              }
            return (_CRMXbusy) ;
          }

          // check interrupt to go low within a certain time
          //delayMicroseconds(100);  // default delay
    
                                while(!IRQ_detected()) {
                                    
                                    // Check if Timeout occured
                                    if (currentMillisTime - previousMillisCRMX >= intervalCRMX){
                                      _CRMXbusy = true;
                                        if (DEBUG >=1){
                                            SerialUSB.println("ERROR Timeout ");
                                        }
                                      return -1 ;
                                    }
                                    
                                }
     
         
            
          // Receive data
         digitalWrite(_SSPin, LOW);
          
          delayMicroseconds(5);  // extra delay befor start

              // start Byte
              IRQ_flagData = SPI.transfer(NOP);
              delayMicroseconds(5);  // extra delay befor start
    
                // Check if CRMX is busy
                         if (bitRead (IRQ_flagData, 7) == HIGH){
                           _CRMXbusy = true;
                           return (_CRMXbusy) ;
                             if (DEBUG >=1){
                                 SerialUSB.print("ERROR 2 IRQ_flag data: ");
                                 SerialUSB.println(IRQ_flagData);
                             }
                         }
            
                        switch (command){
                                
                                case WRITE_DMX:
                                        // Data bytes to receive and store in the dataBuffer
                                        for (int16_t i=channel_start;i<length+channel_start;i++){
                                          SPI.transfer(_DMX[i+1]); // Send command byte and read this first byte IRQ_flags
                                            if (DEBUG >=3){
                                                SerialUSB.print("DMX ");
                                                SerialUSB.print(i);
                                                SerialUSB.print(": ");
                                                SerialUSB.println(_DMX[i]);
                                                
                                            }
                                        }
                                    
                                    break;
                                    
                                default:
                                        // Data bytes to receive and store in the dataBuffer
                                        for (int i=0;i<length;i++){
                                          SPI.transfer(_dataBuffer[i]); // Send command byte and read this first byte IRQ_flags
                                        }
                                    break;
                                
                                
                        }
      
          digitalWrite(_SSPin, HIGH);
          
          delayMicroseconds(500);  // default delay


       SPI.endTransaction();
    return 0; // all OK

//interrupts();
  
}

