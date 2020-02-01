/*  CCUdecoder library for Arduino for CCU TX
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



#include "CCU_decoder.h"
#include "VidorGraphics.h"
#include "Vidor_GFX.h"

 #include <Arduino.h>

CCU_decoder::CCU_decoder() {}

Vidor_GFX  vdgfx;


 void CCU_decoder::begin(){


   HardwareSerial & SDI2HDMI = Serial1;
   SDI2HDMI.begin(115200); // data from SDI 2 HDMI modified board
   delay(800);
  //Uart SDIbrd (&sercom3, 0, 1, SERCOM_RX_PAD_1, UART_TX_PAD_0); // Create the new UART instance assigning it to pin 0 and 1
   Vidor_GFX  vdgfx;

   Wire.begin(); // join i2c bus (address optional for master)

   // scanning I2C
      for (uint8_t dev=1; dev<128; ++dev) {
              Wire.beginTransmission (dev);
              if (Wire.endTransmission() == 0) {
                  if (DEBUG >= 1){
                    Serial.print (F("- I2C device found at 0x"));
                  }
                  }
                  Serial.println (dev, HEX);
                  if (dev == 0x10){
                    if (DEBUG >= 1){Serial.println ("- DisplayPCB detected");}
                    I2C_OK = true;
                  }

                  if (dev == INaddr){
                    if (DEBUG >= 1){Serial.println ("- Radio I2C detected");}
                    I2C_Radio_OK = true;
                  }
              }
          
          if (I2C_OK == false){
            if (DEBUG >= 1){Serial.println ( "- DisplayPCB not detected");}
          }


          // Initialize the FPGA
                  if (!FPGA.begin()) {
                    if (DEBUG >= 1){Serial.println("Initialization failed!");
                    while (1) {}
                    }
                  }
                
                  delay(4000);  
      
     

  
}


void CCU_decoder::HDMIenable (bool en){

  bool value = en;
  if (value == true){

    //VIDOR_HDMI.HDMI_start();
    HDMI_start();
    
  }else{

    //VIDOR_HDMI.HDMI_end();
    HDMI_end();
    
  }
}





 void CCU_decoder::camEnable(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h)

{
   // enable camera 1-8 for processing
    _readEnable[1] = a;
    _readEnable[2] = b;
    _readEnable[3] = c;
    _readEnable[4] = d;
    _readEnable[5] = e;
    _readEnable[6] = f;
    _readEnable[7] = g;
    _readEnable[8] = h;
    
}



void CCU_decoder::checkCCU(){


        if (SDI2HDMI.available() > 0) {
                
                     CCUreceived();
                     //delay(5);
                     //currentMillis = millis();
                     //int temp = currentMillis - previousMillisSDI;
                     // Serial.println(temp);          
                   } // end SDIbrd data available
        
          
}

void CCU_decoder::CCUreceived(){

    // read CCU DATA check if Byte is Startbyte
    
              delay(1);
              _message = SDI2HDMI.read();
     
              // Check if startbyte has been detected
              if (_message == 0xF0){  // Startbyte
                      digitalWrite(tallyLED, LOW); 
                
                      // Check if following Byte is second CheckByte
                        delay(1);
                        _messageType = SDI2HDMI.read();
                        
                     // CCU command detected
                     if (_messageType > 240){  // messageType detected
                      
                      
                        switch (_messageType) {

                          
                                  // Check if data is SDI_OK

                                  case SDIdata:

                                              delay(1);
                                              _dataByte[0]  = SDI2HDMI.read();
                                              delay(1);
                                              _dataByte[1]  = SDI2HDMI.read();
                                             
                                              
                                              if ((_dataByte[0] != -1) && (_dataByte[1] != -1) && (_dataByte[1] = StopByte)){

                                                       _RXdata_OK(255); // send back it's received
                                                       
                                                       if (SDI_OK != _dataByte[0]){
                                                            SDI_OK = _dataByte[0];
                                                            
                                                            // Transmit data to MicroView
                                                                //setSDI_OK_data = SDI_OK;
                                                                setSDI_OK(SDI_OK);
                                                                
                                                          if (DEBUG >= 1){
                                                                 Serial.print("SDI_OK: ");
                                                                  Serial.println(SDI_OK);
                                                          }
                                                                
                                                       } else {
                                                          _RXdata_OK(0); // resend data please 0 = resend, 255 = OK
                                                         }
                                              }
                                  break;
                                  
                                  
                                  // Check if data is TALLY
                                  case TallyData: // Tally Data

                                            delay(1);
                                              _cam          = SDI2HDMI.read();
                                            delay(1);
                                              _dataByte[0]  = SDI2HDMI.read();
                                            delay(1);
                                              _dataByte[1]  = SDI2HDMI.read();

                                            if ((_cam != -1) && (_dataByte[1] != -1) && (_dataByte[1] = StopByte)){

                                                     if (_tally[_cam] !=  _dataByte[0]){
                                                          _HDMIflag = 1;
                                                          _tally[_cam] =  _dataByte[0];

                                                          if (I2C_OK == true && _cam == CAMnumber){

                                                            
                                                           setTallyDisplay(_tally[CAMnumber]);  // send tally to Display
                                                        }
                                                      
                                                     }
                                                   
                                                     _RXdata_OK(255); 
                                                 
                                                      
                                                  
                                              } else {
                                                  _RXdata_OK(0); // resend data please 0 = resend, 255 = OK
                                              }
                                                
                                                
                                               

                                                    
                                               
                                                  if (DEBUG >= 1){

                                                            if (_cam == CAMnumber){
        
                                                                  switch (_tally[_cam]){
                  
                                                                    case 0: 
                                                                            HDMItallyWhite();
                  
                                                                    break;
                  
                                                                    case 1: 
                                                                            HDMItallyRed();
                  
                                                                    break;
                  
                                                                    case 2: 
                                                                            HDMItallyGreen();
                  
                                                                    break;
                  
                                                                     case 3: 
                                                                            HDMItallyRed();
                  
                                                                    break;
                  
                                                                  }
                                                            }
        
                                                        if (DEBUG >= 1){
                                                                  Serial.println();
                                                                  Serial.print("TallY: ");
                                                                  Serial.print(_cam);
                                                                  Serial.print(" = ");
                                                                  Serial.print(_tally[_cam]);
                                                                  HDMIdebugTally();
                                                        }
                                                  
                                                }

                                                  if (DEBUG >=2){
                                                    Serial.print("   Data: ");
                                                    Serial.print(_message);
                                                    Serial.print(" - ");
                                                    Serial.print(_messageType);
                                                    Serial.print(" - ");
                                                    Serial.print(_cam);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[0]);
                                                    Serial.print(" - ");
                                                    Serial.println(_dataByte[1]);
                                                  }
        
                                  break;

                                  // Check CCU DATA with 2 Bytes data
                                  case CCUData2: 

                                            //delay(1);
                                              _groupID      = SDI2HDMI.read();
                                            //delay(1);
                                              _parameter    = SDI2HDMI.read();
                                            //delay(1);
                                              _cam          = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[0]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[1]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[2]  = SDI2HDMI.read();

                                              if (DEBUG == 2){
                                                    Serial.print("   Data: ");
                                                    Serial.print(_message);
                                                    Serial.print(" - ");
                                                    Serial.print(_messageType);
                                                    Serial.print(" - ");
                                                    Serial.print(_groupID);
                                                    Serial.print(" - ");
                                                    Serial.print(_parameter);
                                                    Serial.print(" - ");
                                                    Serial.print(_cam);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[0]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[1]);
                                                    Serial.print(" - ");
                                                    Serial.println(_dataByte[2]);
                                                  }

                                            if ((_dataByte[2] == 0xF7) && 
                                                (_groupID >= 0  &&  _groupID <= 12 ) &&
                                                (_parameter >= 0  &&  _parameter <= 15 ) &&
                                                (_cam >= 1  &&  _cam <= 8 ) &&
                                                (_dataByte[0] >= 0  &&  _dataByte[0] <= 255 ) &&
                                                (_dataByte[1] >= 0  &&  _dataByte[1] <= 255 ) &&
                                                (_dataByte[2] >= 0  &&  _dataByte[2] <= 255 ) 
                                                ){

                                                      _processCCU();
                                           
                                                 } else {
                                                  _RXdata_OK(0); // resend data please 0 = resend, 255 = OK
                                                 }


                                  break;
                                  
                                  // Check CCU DATA with 4 Bytes data
                                  case CCUData4: 
                                            //delay(1);
                                              _groupID      = SDI2HDMI.read();
                                            //delay(1);
                                              _parameter    = SDI2HDMI.read();
                                            //delay(1);
                                              _cam          = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[0]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[1]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[2]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[3]  = SDI2HDMI.read();
                                            //delay(1);
                                              _dataByte[4]  = SDI2HDMI.read();


                                              if (DEBUG == 2){
                                                    Serial.print("   Data: ");
                                                    Serial.print(_message);
                                                    Serial.print(" - ");
                                                    Serial.print(_messageType);
                                                    Serial.print(" - ");
                                                    Serial.print(_groupID);
                                                    Serial.print(" - ");
                                                    Serial.print(_parameter);
                                                    Serial.print(" - ");
                                                    Serial.print(_cam);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[0]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[1]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[2]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[3]);
                                                    Serial.print(" - ");
                                                    Serial.println(_dataByte[4]);
                                                  }


                                              if ((_dataByte[4] == 0xF7) && 
                                                (_groupID >= 0  &&  _groupID <= 12 ) &&
                                                (_parameter >= 0  &&  _parameter <= 15 ) &&
                                                (_cam >= 1  &&  _cam <= 8 ) &&
                                                (_dataByte[0] >= 0  &&  _dataByte[0] <= 255 ) &&
                                                (_dataByte[1] >= 0  &&  _dataByte[1] <= 255 ) &&
                                                (_dataByte[2] >= 0  &&  _dataByte[2] <= 255 ) &&
                                                (_dataByte[3] >= 0  &&  _dataByte[3] <= 255 ) 
                                                
                                                ){

                                                      _processCCU();

                                                } else {
                                                  _RXdata_OK(0); // resend data please 0 = resend, 255 = OK
                                                }

                                            
                                           
                                             
                                                  
                                            


                                  break;

                                  
                                  // Check CCU DATA with 8 Bytes data
                                  case CCUData8: 

                                            //delay(1);
                                              _groupID      = SDI2HDMI.read();
                                            //delay(1);
                                              _parameter    = SDI2HDMI.read();
                                            //delay(1);
                                              _cam          = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[0]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[1]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[2]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[3]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[4]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[5]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[6]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[7]  = SDI2HDMI.read();
                                            delayMicroseconds(50);
                                              _dataByte[8]  = SDI2HDMI.read();
                                           

                                              
                                              if (DEBUG ==2){
                                                    Serial.print("   Data: ");
                                                    Serial.print(_message);
                                                    Serial.print(" - ");
                                                    Serial.print(_messageType);
                                                    Serial.print(" - ");
                                                    Serial.print(_groupID);
                                                    Serial.print(" - ");
                                                    Serial.print(_parameter);
                                                    Serial.print(" - ");
                                                    Serial.print(_cam);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[0]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[1]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[2]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[3]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[4]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[5]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[6]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[7]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[8]);
                                                    Serial.println("     ");
                                              }
                                                   
                                                  
                                            if ((_dataByte[8] == 0xF7) && 
                                                (_groupID >= 0  &&  _groupID <= 12 ) &&
                                                (_parameter >= 0  &&  _parameter <= 15 ) &&
                                                (_cam >= 1  &&  _cam <= 8 ) &&
                                                (_dataByte[0] >= 0  &&  _dataByte[0] <= 255 ) &&
                                                (_dataByte[1] >= 0  &&  _dataByte[1] <= 255 ) &&
                                                (_dataByte[2] >= 0  &&  _dataByte[2] <= 255 ) &&
                                                (_dataByte[3] >= 0  &&  _dataByte[3] <= 255 ) &&
                                                (_dataByte[4] >= 0  &&  _dataByte[4] <= 255 ) &&
                                                (_dataByte[5] >= 0  &&  _dataByte[5] <= 255 ) &&
                                                (_dataByte[6] >= 0  &&  _dataByte[6] <= 255 ) &&
                                                (_dataByte[7] >= 0  &&  _dataByte[7] <= 255 ) 
                                                
                                                ){

                                                      _processCCU();

                                                      /*
                                                       if (DEBUG ==2){
                                                    Serial.print("   Data: ");
                                                    Serial.print(_message);
                                                    Serial.print(" - ");
                                                    Serial.print(_messageType);
                                                    Serial.print(" - ");
                                                    Serial.print(_groupID);
                                                    Serial.print(" - ");
                                                    Serial.print(_parameter);
                                                    Serial.print(" - ");
                                                    Serial.print(_cam);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[0]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[1]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[2]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[3]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[4]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[5]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[6]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[7]);
                                                    Serial.print(" - ");
                                                    Serial.print(_dataByte[8]);
                                                    Serial.println("     ");
                                                    */
                                                   

                                                } else {
                                                  if (DEBUG >= 1){Serial.println(" Wong CCU Data received ");}
                                                  
                                                  _RXdata_OK(0); // resend data please 0 = resend, 255 = OK
                                                 
                                                }


                                  break;

                                  
                          


                        }
                     } // end messageType
                      
              } // If 0xF0 start byte


  
} // end CCUreceived



void CCU_decoder::_processCCU(){

 // digitalWrite(tallyLED, HIGH); 
                    if ((_readEnable[_cam] == 1) && (_cam >= 1 && _cam <=8)){ // process only the selected camera
                   //if (cam >= 1 && cam <=8) { // Process camera 1 -8

                       // Serial.println("Cam ");
                       // Serial.print(cam);
                       // Serial.print(": ");

                        _datachanged = 1;
                        
                       switch (_groupID) {
                        
                           case 0:    // Lens
                              

                                      switch (_parameter) {
                                          case 0:    // Lens
                                          //Serial.println("Focus");
                                                
                                                _focus[_cam][0] = _dataByte[0];
                                                _focus[_cam][1] = _dataByte[1];
                                                _RXdata_OK(255); 
                                                
                              
                                            break;

                                          case 1:    // AutoFocus
                                          //Serial.println("Auto Focus");
                                                
                                               _autoFocus[_cam]++;  // add a value each time the autofocus is requested
                                               if (_autoFocus[_cam] >= 255)
                                                    {
                                                      _autoFocus[_cam] = 1;
                                                    }
                                               _RXdata_OK(255); 
                              
                                           break;
                                            
                                          case 2:    // Lens
                                          //Serial.println("Iris");


                                                if (_iris[_cam][0] != _dataByte[0]  ||  _iris[_cam][1] != _dataByte[1]){
                                                 _HDMIflag = 1;
                                                }
                                                
                                                _iris[_cam][0] = _dataByte[0];
                                                _iris[_cam][1] = _dataByte[1];
                                                bitSet(_FlagByte[_cam],0);  // Set bit 0 high
                                                HDMI_setIris();
                                                _RXdata_OK(255); 
                                                /*
                                                    Serial.print("Iris:");
                                                    Serial.print(iris[_cam][0]);
                                                    Serial.print(" ");
                                                    Serial.println(iris[_cam][1]);
                                                */
                                               
                              
                                            break;
                                          case 9:    // Lens
                                          //Serial.println("Zoom");
                                                
                                                _zoom[_cam][0] = _dataByte[0];
                                                _zoom[_cam][1] = _dataByte[1];
                                                _RXdata_OK(255); 
                                                
                              
                                            break;
                                          default:
                                              // if nothing else matches, do the default
                                              // default is optional
                                          break;
                                      }
                              
                              break;
                            
                            case 1:    // video
                              //Serial.print("  Video");

                                switch (_parameter) {
                                          case 0:    // Video mode
                                          //Serial.println("VideoMode");
                                               
                                                _videoMode[_cam][0] = _dataByte[0];
                                                _RXdata_OK(255); 
                              
                                            break;
                                          case 1:    // sensorGain
                                          //Serial.println("SensorGain");
                                                
                                                _sensorGain[_cam][0] = _dataByte[0];
                                                bitSet(_FlagByte[_cam],1);  // Set bit high
                                                _RXdata_OK(255); 
                              
                                            break;
                                          case 2:    // WB
                                          //Serial.println("WhiteBalance");
                                                
                                                _WB[_cam][0] = _dataByte[0];
                                                _WB[_cam][1] = _dataByte[1];
                                                _WB[_cam][2] = _dataByte[2];
                                                _WB[_cam][3] = _dataByte[3];
                                                bitSet(_FlagByte[_cam],1);  // Set bit high
                                                _RXdata_OK(255); 
                                                
                              
                                            break;
                                          case 5:    // Shutter
                                          //Serial.println("Shutter");

                                                if (_shutter[_cam][0] != _dataByte[0]  ||  _shutter[_cam][1] != _dataByte[1]){
                                                 _HDMIflag = 1;
                                                }

                                                
                                                _shutter[_cam][0] = _dataByte[0];
                                                _shutter[_cam][1] = _dataByte[1];
                                                bitSet(_FlagByte[_cam],1);  // Set bit high
                                                HDMI_setShutter();
                                                _RXdata_OK(255); 
                                                
                                               
                                                
                              
                                            break;
                                          case 8:    // Detail
                                          //Serial.println(" Sharpening");
                                                
                                                _detail[_cam][0] = _dataByte[0];
                                                _detail[_cam][1] = _dataByte[1];
                                                _RXdata_OK(255); 
                              
                                            break;
                                          default:
                                              // if nothing else matches, do the default
                                              // default is optional
                                          break;
                                         }
                              break;
                            case 2:    // audio
                             
                              break;
                            case 3:    // Output
                              
                              break;
                            case 4:    // Display
                                      switch (_parameter) {
                                        
                                          case 4:    // Colorbar
                                              //Serial.println("ColorBar");

                                                _colorBar[_cam][0] = _dataByte[0];
                                                _RXdata_OK(255); 
                                          break;

                                          default:
                                              // if nothing else matches, do the default
                                              // default is optional
                                          break;
                                      }
                              break;
                                      
                            case 5:    // Tally
                              //Serial.print("  Tally");
                              break;
                            case 6:    // Reference

                                       switch (_parameter) {
                                                    case 0:    // Ref Source 0= int, 1 = PRG, 2 = Ext
                                                    //Serial.print("Reference");
                                                         
                                                         _reference[_cam][0] = _dataByte[0];
                                                         _RXdata_OK(255); 
                                        
                                                      break;
                                                    
                                                   
                                                    default:
                                                        // if nothing else matches, do the default
                                                        // default is optional
                                                    break;
                                                   }
                             
                              break;
                            case 7:    // Configuration
                              //Serial.print("  Configuration");
                              break;
                              
                            case 8:    // Color correction
                              
                                        //Serial2.write(groupID);
                                        switch (_parameter) {
                                                  case 0:    // Lift Adjust
                                                  //Serial.println("Black");
                                                  int temp;

                                                        if (_black[_cam][6] != _dataByte[6]  ||  _black[_cam][7] != _dataByte[7]){
                                                          _HDMIflag = 1;
                                                        }

                                                          if (((_dataByte[1]*255) + _dataByte[0]) >= 256 || ((_dataByte[1]*255) + _dataByte[0]) <= 32000) {
                                                        //black[_cam][0] = _dataByte[0];
                                                        //black[_cam][1] = _dataByte[1];
                                                        }

                                                         if (((_dataByte[3]*255) + _dataByte[2]) >= 256 || ((_dataByte[3]*255) + _dataByte[2]) <= 32000){
                                                        //black[_cam][2] = _dataByte[2];
                                                        //black[_cam][3] = _dataByte[3];
                                                         }

                                                          if (((_dataByte[5]*255) + _dataByte[4]) >= 256 || ((_dataByte[5]*255) + _dataByte[4]) <= 32000){
                                                        //black[_cam][4] = _dataByte[4];
                                                        //black[_cam][5] = _dataByte[5];
                                                          }

                                                         if (((_dataByte[7]*255) + _dataByte[6]) >= 256 || ((_dataByte[7]*255) + _dataByte[6]) <= 32000){
                                                        _black[_cam][6] = _dataByte[6];
                                                        _black[_cam][7] = _dataByte[7];
                                                         }

                                                         /*
                                                        _black[_cam][0] = _dataByte[0];
                                                        _black[_cam][1] = _dataByte[1];
                                                        _black[_cam][2] = _dataByte[2];
                                                        _black[_cam][3] = _dataByte[3];
                                                        _black[_cam][4] = _dataByte[4];
                                                        _black[_cam][5] = _dataByte[5];
                                                        _black[_cam][6] = _dataByte[6];
                                                        _black[_cam][7] = _dataByte[7];
                                                        */
                                                        bitSet(_FlagByte[_cam],2);  // Set bit high
                                                        HDMI_setBlack();
                                                        _RXdata_OK(255); 
                                                  if (DEBUG >= 1){
                                                        Serial.print("Black: ");
                                                        Serial.print(_dataByte[0]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[1]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[2]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[3]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[4]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[5]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[6]);
                                                        Serial.print(" ");
                                                        Serial.print(_dataByte[7]);
                                                        Serial.println(" ");
                                                  }
        
                                                    break;
                                                  case 1:    // Gamma Adjust
                                                  //Serial.println("Gamma");
                                                        
                                                        
                                                           if (((_dataByte[1]*255) + _dataByte[0]) >= 256 || ((_dataByte[1]*255) + _dataByte[0]) <= 32000) {
                                                        _Gamma[_cam][0] = _dataByte[0];
                                                        _Gamma[_cam][1] = _dataByte[1];
                                                        }

                                                         if (((_dataByte[3]*255) + _dataByte[2]) >= 256 || ((_dataByte[3]*255) + _dataByte[2]) <= 32000){
                                                        _Gamma[_cam][2] = _dataByte[2];
                                                        _Gamma[_cam][3] = _dataByte[3];
                                                         }

                                                          if (((_dataByte[5]*255) + _dataByte[4]) >= 256 || ((_dataByte[5]*255) + _dataByte[4]) <= 32000){
                                                        _Gamma[_cam][4] = _dataByte[4];
                                                        _Gamma[_cam][5] = _dataByte[5];
                                                          }

                                                         if (((_dataByte[7]*255) + _dataByte[6]) >= 256 || ((_dataByte[7]*255) + _dataByte[6]) <= 32000){
                                                        _Gamma[_cam][6] = _dataByte[6];
                                                        _Gamma[_cam][7] = _dataByte[7];
                                                         }
                                                        
                                                        /*
                                                        _Gamma[_cam][0] = _dataByte[0];
                                                        _Gamma[_cam][1] = _dataByte[1];
                                                        _Gamma[_cam][2] = _dataByte[2];
                                                        _Gamma[_cam][3] = _dataByte[3];
                                                        _Gamma[_cam][4] = _dataByte[4];
                                                        _Gamma[_cam][5] = _dataByte[5];
                                                        _Gamma[_cam][6] = _dataByte[6];
                                                        _Gamma[_cam][7] = _dataByte[7];
                                                        */
                                                        bitSet(_FlagByte[_cam],3);  // Set bit high
                                                        _RXdata_OK(255); 
        
                                                        
                                                    break;
                                                  case 2:    // Gain
                                                  //Serial.println("Gain");

                                                        if ((_Gain[_cam][0] != _dataByte[0] ) || 
                                                            (_Gain[_cam][1] != _dataByte[1] ) || 
                                                            (_Gain[_cam][2] != _dataByte[2] ) || 
                                                            (_Gain[_cam][3] != _dataByte[3] ) || 
                                                            (_Gain[_cam][4] != _dataByte[4] ) || 
                                                            (_Gain[_cam][5] != _dataByte[5] ) ||
                                                            (_Gain[_cam][6] != _dataByte[6] ) ||  
                                                            (_Gain[_cam][7] != _dataByte[7] )){
                                                              _HDMIflag = 1;
                                                        }

                                                        if (((_dataByte[1]*255) + _dataByte[0]) >= 256){
                                                        _Gain[_cam][0] = _dataByte[0];
                                                        _Gain[_cam][1] = _dataByte[1];
                                                        }

                                                         if (((_dataByte[3]*255) + _dataByte[2]) >= 256){
                                                        _Gain[_cam][2] = _dataByte[2];
                                                        _Gain[_cam][3] = _dataByte[3];
                                                         }

                                                          if (((_dataByte[5]*255) + _dataByte[4]) >= 256){
                                                        _Gain[_cam][4] = _dataByte[4];
                                                        _Gain[_cam][5] = _dataByte[5];
                                                          }

                                                         if (((_dataByte[7]*255) + _dataByte[6]) >= 256){
                                                        _Gain[_cam][6] = _dataByte[6];
                                                        _Gain[_cam][7] = _dataByte[7];
                                                         }
                                                         
                                                        bitSet(_FlagByte[_cam],4);  // Set bit high
                                                        HDMI_setGain();
                                                        _RXdata_OK(255); 
                                                        
                                                        
        
                                                        
                                      
                                                    break;
                                                  case 3:    // Offset
                                                 // Serial.println("Offset");
                                   
                                                    break;
                                                  case 4:    // Contrast
                                                  //Serial.println(" Contrast");
                                                        
                                                        _contrastCam[_cam][0] = _dataByte[0];
                                                        _contrastCam[_cam][1] = _dataByte[1];
                                                        _contrastCam[_cam][2] = _dataByte[2];
                                                        _contrastCam[_cam][3] = _dataByte[3];
                                                        bitSet(_FlagByte[_cam],5);  // Set bit high
                                                        _RXdata_OK(255); 
                                      
                                                    break;
                                                  case 5:    // Luma
                                                  //Serial.println(" Luma");
                                                        
                                                        _luma[_cam][0] = _dataByte[0];
                                                        _luma[_cam][1] = _dataByte[1];
                                                        bitSet(_FlagByte[_cam],7);  // Set bit high
                                                        _RXdata_OK(255); 
                                      
                                                    break;
                                                  case 6:    // Color Adjust
                                                  //Serial.println(" Color Adjust Sat");
        
                                                        //Hue
                                                        _hue[_cam][0] = _dataByte[0];
                                                        _hue[_cam][1] = _dataByte[1];
                                                        
                                                        // Saturation
                                                        _saturation[_cam][0] = _dataByte[2];
                                                        _saturation[_cam][1] = _dataByte[3];
                                                        bitSet(_FlagByte[_cam],6);  // Set bit high
                                                        _RXdata_OK(255); 
                                      
                                                    break;
                                                  case 7:    // Luma
                                                  //Serial.print(" Reset Defaults");
                                                   
                                      
                                                    break;
                                                  
                                                  
                                                  } // end switch parameters 8
                              break; // 8

                               case 11:    // Color correction

                                            switch (_parameter) {
                                                    case 0:    // Pan /Tilt Velocity
                                                    //Serial.print("Pan/Tilt");
                                                         
                                                          _panTilt[_cam][0] = _dataByte[0];
                                                          _panTilt[_cam][1] = _dataByte[1];
                                                          _panTilt[_cam][2] = _dataByte[2];
                                                          _panTilt[_cam][3] = _dataByte[3];
                                                          _RXdata_OK(255); 
                                                          
                                        
                                                      break;
                                                    case 1:    // Memory preset
                                                    //Serial.print("MemoryPreset");
                                                          
                                                          _ptPreset[_cam][0] = _dataByte[0];  //Load first data Byte: 0= reset, 1 = store, 2 = recall
                                                          _ptPreset[_cam][1] = _dataByte[1]; //Load first data Byte: 0-5 presets
                                                          _RXdata_OK(255); 
                                                          
                                        
                                                      break;
                                                   
                                                    default:
                                                        // if nothing else matches, do the default
                                                        // default is optional
                                                    break;
                                                   }
                             
                               break; // 11

                               default:
                                                         _datachanged = 0;
                                                       if (DEBUG >= 1){ Serial.println(" Wrong Data!");}
                                                        // if nothing else matches, do the default
                                                        // default is optional
                                                        _RXdata_OK(0); 
                               break;
      
                           } // End check of groupID, Parameters and Types
                         } // End Processonly selected camera



                  if (DEBUG >= 3){

                        Serial.print("FlagByte Cam: ");
                        Serial.print(_cam);
                        
                        Serial.print(" : ");
                        for (int b = 7; b >= 0; b--)
                          {
                            Serial.print(bitRead(_FlagByte[_cam], b));
                          }
                         
                        Serial.println(" ");

                        
                     }
                     digitalWrite(tallyLED, HIGH); 
  
}




void CCU_decoder::_RXdata_OK(byte OK_status){ // Data for SDI2HDMI CCU board

   const byte TXdata[] = {

        0xF0,     // start byte
        0xF6,     // DATA OK 0 = error, FF = OK
        OK_status,  // data
        0xF7,     // Stop Byte
        
       };

       SDI2HDMI.write(TXdata, sizeof(TXdata));
       Serial.flush();
    
  
}

void CCU_decoder::_RXsettings(byte command, byte first){  // Data for SDI2HDMI CCU board

const byte TXdata[] = {

        0xF0,     // start byte
        0xF5,     // DATA OK
        command,  // data
        first,  // data
        0xF7,     // Stop Byte
        
       };

       SDI2HDMI.write(TXdata, sizeof(TXdata));
       Serial.flush();

   
}



void CCU_decoder::ProcessTX() { // Convert the CCU data into a DMX table


                byte value;
              
                
                for (int i = 1; i <= 8; i++) { 


                  if (_FlagByte[i] == 0){
                          _cameraDefaults(i);
                          //Serial.print("Defaults Loaded");
                        }
                  
                           if (_readEnable[i] == 1 ){
                               value = 
                               _DMX[_CCUaddress[i]] = _FlagByte[i];              // tramsmit flagBits
                               
                               _DMX[_CCUaddress[i]+2] = _panTilt[i][0];           // Highbyte Iris
                               _DMX[_CCUaddress[i]+3] = _panTilt[i][1];             // Low byte Iris
                               _DMX[_CCUaddress[i]+4] = _panTilt[i][2];           // Highbyte Iris
                               _DMX[_CCUaddress[i]+5] = _panTilt[i][3];             // Low byte Iris
        
                               _DMX[_CCUaddress[i]+6] = _ptPreset[i][0];           // Highbyte Iris
                               _DMX[_CCUaddress[i]+7] = _ptPreset[i][1];             // Low byte Iris
        
                               _DMX[_CCUaddress[i]+8] = _reference[i][0];                 // Tally data 
                               
                               _DMX[_CCUaddress[i]+10] = _tally[i];                 // Tally data 
                          
                               _DMX[_CCUaddress[i]+11] = _iris[i][0];           // Highbyte Iris
                               _DMX[_CCUaddress[i]+12] = _iris[i][1];             // Low byte Iris
        
        
                               _DMX[_CCUaddress[i]+13] = _focus[i][0];          // Highbyte Focus
                               _DMX[_CCUaddress[i]+14] = _focus[i][1];            // Low byte Focus
                               
                               _DMX[_CCUaddress[i]+15] = _zoom[i][0];           // Highbyte Zoom
                               _DMX[_CCUaddress[i]+16] = _zoom[i][1];             // Low byte Zoom
                          
                               _DMX[_CCUaddress[i]+17] = _WB[i][0];             // Highbyte WhiteBalance
                               _DMX[_CCUaddress[i]+18] = _WB[i][1];             // Low byte WhiteBalance
                               
                               _DMX[_CCUaddress[i]+19] = _shutter[i][0];        // Highbyte Shutter
                               _DMX[_CCUaddress[i]+20] = _shutter[i][1];          // Low byte Shutter
                          
                               _DMX[_CCUaddress[i]+21] = _sensorGain[i][0];     // Highbyte sensorGain
                              
                               _DMX[_CCUaddress[i]+22] = _videoMode[i][0];         // Low byte sensorGain
                          
                               _DMX[_CCUaddress[i]+23] = _Gain[i][0];          // Highbyte GainY
                               _DMX[_CCUaddress[i]+24] = _Gain[i][1];            // Low byte GainY
                               _DMX[_CCUaddress[i]+25] = _Gain[i][2];          // Highbyte GainR
                               _DMX[_CCUaddress[i]+26] = _Gain[i][3];            // Low byte GainR                  
                               _DMX[_CCUaddress[i]+27] = _Gain[i][4];          // Highbyte GainG
                               _DMX[_CCUaddress[i]+28] = _Gain[i][5];            // Low byte GainG                 
                               _DMX[_CCUaddress[i]+29] = _Gain[i][6];          // Highbyte GainY
                               _DMX[_CCUaddress[i]+30] = _Gain[i][7];            // Low byte GainY
                               
                          
                               _DMX[_CCUaddress[i]+31] = _Gamma[i][0];          // Highbyte gammaY
                               _DMX[_CCUaddress[i]+32] = _Gamma[i][1];            // Low byte gammaY                 
                               _DMX[_CCUaddress[i]+33] = _Gamma[i][2];          // Highbyte gammaR
                               _DMX[_CCUaddress[i]+34] = _Gamma[i][3];            // Low byte gammaR                 
                               _DMX[_CCUaddress[i]+35] = _Gamma[i][4];          // Highbyte gammaG
                               _DMX[_CCUaddress[i]+36] = _Gamma[i][5];            // Low byte gammaG                 
                               _DMX[_CCUaddress[i]+37] = _Gamma[i][6];          // Highbyte gammaY
                               _DMX[_CCUaddress[i]+38] = _Gamma[i][7];            // Low byte gammaY
                               
                          
                               _DMX[_CCUaddress[i]+39] = _black[i][0];          // Highbyte blackY
                               _DMX[_CCUaddress[i]+40] = _black[i][1];            // Low byte blackY                 
                               _DMX[_CCUaddress[i]+41] = _black[i][2];          // Highbyte blackR
                               _DMX[_CCUaddress[i]+42] = _black[i][3];            // Low byte blackR                  
                               _DMX[_CCUaddress[i]+43] = _black[i][4];          // Highbyte blackG
                               _DMX[_CCUaddress[i]+44] = _black[i][5];            // Low byte blackG                  
                               _DMX[_CCUaddress[i]+45] = _black[i][6];          // Highbyte blackY
                               _DMX[_CCUaddress[i]+46] = _black[i][7];            // Low byte blackY
                          
                          
                               _DMX[_CCUaddress[i]+47] = _contrastCam[i][0];        // Highbyte contrast
                               _DMX[_CCUaddress[i]+48] = _contrastCam[i][1];
                               _DMX[_CCUaddress[i]+49] = _contrastCam[i][2];          // Highbyte VideoMode
                               _DMX[_CCUaddress[i]+50] = _contrastCam[i][3];         // Low byte contrast
                          
                               _DMX[_CCUaddress[i]+53] = _hue[i][0];             // Highbyte hue
                               _DMX[_CCUaddress[i]+54] = _hue[i][1];               // Low byte hue
                               _DMX[_CCUaddress[i]+51] = _saturation[i][0];      // Highbyte saturation
                               _DMX[_CCUaddress[i]+52] = _saturation[i][1];        // Low byte saturation
                                                           
                               _DMX[_CCUaddress[i]+55] = _detail[i][0];          // Highbyte Detail
                               _DMX[_CCUaddress[i]+56] = _detail[i][1];            // Low byte Detail
        
                               _DMX[_CCUaddress[i]+57] = _luma[i][0];          // Highbyte Detail
                               _DMX[_CCUaddress[i]+58] = _luma[i][1];            // Low byte Detail
                          
                               _DMX[_CCUaddress[i]+59] = _colorBar[i][0];               // ColorBar
                          
                               _DMX[_CCUaddress[i]+60] = _autoFocus[i];               // ColorBar
        
                                if ( _datachanged == 1 && DEBUG == 3){
                                
                                        Serial.print("Iris:");
                                        Serial.print(_CCUaddress[i]+11);
                                        Serial.print(" ");
                                        Serial.print(_iris[i][0]);
                                        Serial.print(" ");
                                        Serial.println(_iris[i][1]);
        
                                        Serial.print("WB:");
                                        Serial.print(_CCUaddress[i]+17);
                                        Serial.print(" ");
                                        Serial.print(_WB[i][0]);
                                        Serial.print(" ");
                                        Serial.println(_WB[i][1]);
        
                                        Serial.print(_CCUaddress[i]+19);
                                        Serial.print(" ");
                                        Serial.print(_shutter[i][0]);
                                        Serial.print(" ");
                                        Serial.println(_shutter[i][1]);
                                        
                                        Serial.print("sensorGain:");
                                        Serial.print(_CCUaddress[i]+21);
                                        Serial.print(" ");
                                        Serial.println(_sensorGain[i][0]);
        
                                        
                                         Serial.println(" ");
                                          Serial.println(" ");
                                    
                                    if (i == 8 ){
                                    _datachanged = 0;    
                                      }
                                        
                                    //datachanged = 0;                    
                               }

                       if ( _datachanged == 1 && DEBUG == 5 && i == 2){
                        
                                Serial.print("black:");
                                Serial.print(_CCUaddress[i]+39);
                                Serial.print(" ");
                                Serial.print(_black[i][0]);
                                Serial.print(" ");
                                Serial.print(_black[i][1]);
                                Serial.print(" ");
                                Serial.print(_black[i][2]);
                                Serial.print(" ");
                                Serial.print(_black[i][3]);
                                Serial.print(" ");
                                Serial.print(_black[i][4]);
                                Serial.print(" ");
                                Serial.print(_black[i][5]);
                                Serial.print(" ");
                                Serial.print(_black[i][6]);
                                Serial.print(" ");
                                Serial.println(_black[i][7]);
                                

                                
                                 Serial.println(" ");
                                  Serial.println(" ");
                            
                            if (i == 2 ){
                            _datachanged = 0;    
                              }
                                
                            //datachanged = 0;                    
                       }

                       if ( _datachanged == 1 && DEBUG == 4 && i == 2){
                        
                                Serial.print("Contrast:");
                                Serial.print(_CCUaddress[i]+39);
                                Serial.print(" ");
                                Serial.print(_contrastCam[i][0]);
                                Serial.print(" ");
                                Serial.print(_contrastCam[i][1]);
                                Serial.print(" ");
                                Serial.print(_contrastCam[i][2]);
                                Serial.print(" ");
                                Serial.println(_contrastCam[i][3]);
                                
                                

                                
                                 Serial.println(" ");
                                  Serial.println(" ");
                            
                            if (i == 2 ){
                            _datachanged = 0;    
                              }
                                
                            //_datachanged = 0;                    
                       }
                   }
                  
                  
                }
               
                
                
} // end Process

void CCU_decoder::_cameraDefaults(int d){

// create Camera defaults on TX
      
         
         _Gain[d][0] = 0 ; // R
         _Gain[d][2] = 0 ; // G
         _Gain[d][4] = 0 ; // B
         _Gain[d][6] = 0 ; // Y
         _Gain[d][1] = 8 ; // R
         _Gain[d][3] = 8 ; // G
         _Gain[d][5] = 8 ; // B
         _Gain[d][7] = 8 ; // Y
         

         _shutter[d][0] = 64;  // 1/25th
         _shutter[d][1] = 156;

         _iris[d][0] = 128;  // Aperture 3,5
         _iris[d][1] = 25;

         _hue[d][0]  = 0;
         _hue[d][1]  = 8;

         _saturation[d][0]  = 0;
         _saturation[d][1]  = 4;

         _luma[d][0]  = 0;
         _luma[d][1]  = 8;

        // contrastCam[d][0]  = 0;
        // contrastCam[d][1]  = 4;
        // contrastCam[d][2]  = 0;
        // contrastCam[d][3]  = 8;

         //sensorGain
      
  
}



// ------------------ VIDOR GRAPHICS --------------------------------------------------------------------------------------
// ------------------ VIDOR GRAPHICS --------------------------------------------------------------------------------------

void CCU_decoder::HDMI_start(){

    //Vidor_GFX  vdgfx;
    bool _HDMI_en = true;
    delay(500);

    // Initialize the FPGA
   if (!FPGA.begin()) {
    //Serial.println("Initialization failed!");
    while (1) {}
   }


}


void CCU_decoder::HDMI_end(){

    
    bool _HDMI_en = false;
    
}



 
void CCU_decoder::HDMImenu(){

           if (_HDMI_en == true){
                  
                  vdgfx.text.setAlpha(180);
                  vdgfx.text.setSize(1);
                  vdgfx.text.setColor(vdgfx.White());
                 // Fill the screen with a white background
                  vdgfx.fillRect(0,0,640,480,vdgfx.Black());   
                  delay(1000);   
                  //    Frame & basic textx
                  //            x,y.x-size, y-size
                  vdgfx.fillRect(0,4,640,40,vdgfx.Red());         // Banner Up
                  vdgfx.fillRect(0,440,640,40,vdgfx.Red());         // Banner down
                  vdgfx.drawLine(0,44,640,44,vdgfx.White(),255);  // line menu 
                  vdgfx.drawLine(0,74,640,74,vdgfx.White(),255);  // line 2 menu
                  
                  vdgfx.fillRect(0,0,640,4,vdgfx.White());        // White up
                  vdgfx.drawLine(0,440,640,440,vdgfx.White(),255);  // line 2 menu
                  vdgfx.fillRect(0,476,640,4,vdgfx.White());      // White down
                  vdgfx.fillRect(0,0,4,480,vdgfx.White());        // White left
                  vdgfx.fillRect(636,0,4,480,vdgfx.White());    // White Right
                  

                  vdgfx.text.setCursor(125,30);
                  vdgfx.text.setAlpha(255);
                  vdgfx.text.setSize(1);
                  vdgfx.text.setColor(vdgfx.White());
                  vdgfx.print("LiveCut.tv WirelessCCU TX   ");
                  //vdgfx.text.setCursor(400,30);
                  vdgfx.print("V:");
                   vdgfx.print(versionMajor);
                   vdgfx.print(".");
                   vdgfx.println(versionMinor);

                  vdgfx.text.setCursor(16,30);
                   vdgfx.print("CAM:");
                   vdgfx.println(CAMnumber);

                   vdgfx.text.setCursor(45,463);
                   vdgfx.println("Copyright(c)2016-2019 MC-Productions.be");



                  if (DEBUG == 0){

                        vdgfx.drawLine(0,353,640,353,vdgfx.White(),255);  // line 2 menu
                        vdgfx.text.setCursor(160,66);
                        vdgfx.text.setAlpha(180);
                        vdgfx.text.setSize(1);
                        vdgfx.text.setColor(vdgfx.White());
                        vdgfx.println("   NO DEBUGGING ACTIVE ");

                        vdgfx.text.setCursor(_row1 + 150, _startHeight+(_heightOffset*(4-1)));             
                        vdgfx.print("Use Display to turn ON ");
                    
                  }

                  if (DEBUG >= 1){

                        vdgfx.drawLine(0,353,640,353,vdgfx.White(),255);  // line 2 menu
                        vdgfx.text.setCursor(160,66);
                        vdgfx.text.setAlpha(180);
                        vdgfx.text.setSize(1);
                        vdgfx.text.setColor(vdgfx.White());
                        vdgfx.println("DEBUGGING MENU CAM 1-8");

                        vdgfx.text.setAlpha(180);
                        for (int i = 1; i <= 8; i++) { 

                          
                          vdgfx.text.setCursor(_row1, _startHeight+(_heightOffset*(i-1)));  
                          vdgfx.print(i);
                          vdgfx.println(":");

                           // Iris
                          vdgfx.text.setCursor(_row1 + 50, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("AP: ");

                           // Black
                          vdgfx.text.setCursor(_row1 + 150, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("BL: ");
                           
                           // SH
                          vdgfx.text.setCursor(_row1 + 240, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("SH: ");

                           // Red
                          vdgfx.text.setCursor(_row1 + 350, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("R: ");
                          
                           // Green
                          vdgfx.text.setCursor(_row1 + 435, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("G: ");

                           // Blue
                          vdgfx.text.setCursor(_row1 + 520, _startHeight+(_heightOffset*(i-1)));             
                          vdgfx.print("B: ");

                                  
                        }
                  }
           }
}




void CCU_decoder::HDMIupdateCAMnumber(){

         if (_HDMI_en == true){

                   vdgfx.fillRect(10,4,150,40,vdgfx.Red());         // Banner Up
                   
                   vdgfx.text.setColor(vdgfx.White());
                   vdgfx.text.setAlpha(180);
                   vdgfx.text.setSize(1);
                   vdgfx.text.setCursor(16,30);
                   vdgfx.print("CAM:");
                   vdgfx.println(CAMnumber);
         }
  
}



void CCU_decoder::HDMItallyRed(){

         if (_HDMI_en == true){    
            
            if (DEBUG >= 1){

                  vdgfx.fillRect(0,0,640,4,vdgfx.Red());        // White up
                  vdgfx.fillRect(0,476,640,4,vdgfx.Red());      // White down
                  vdgfx.fillRect(0,0,4,480,vdgfx.Red());        // White Left
                  vdgfx.fillRect(636,0,4,480,vdgfx.Red());    // White Right
            }
         }
}


void CCU_decoder::HDMItallyWhite(){
         
          
          
        if (_HDMI_en == true){   
          
          if (DEBUG >= 1){

                  vdgfx.fillRect(0,0,640,4,vdgfx.White());        // White up
                  vdgfx.fillRect(0,476,640,4,vdgfx.White());      // White down
                  vdgfx.fillRect(0,0,4,480,vdgfx.White());        // White Left
                  vdgfx.fillRect(636,0,4,480,vdgfx.White());    // White Right
          }
        }
}


void CCU_decoder::HDMItallyGreen(){

        if (_HDMI_en == true){   
          
          if (DEBUG >= 1){

                  vdgfx.fillRect(0,0,640,4,vdgfx.Green());        // White up
                  vdgfx.fillRect(0,476,640,4,vdgfx.Green());      // White down
                  vdgfx.fillRect(0,0,4,480,vdgfx.Green());        // White Left
                  vdgfx.fillRect(636,0,4,480,vdgfx.Green());    // White Right
          }
        }
}



void CCU_decoder::HDMIdebugTally(){


       if (_HDMI_en == true){

                        int tempOffset = 28;
                        int boxHeigth = 20;
                        int boxWidth  = 15;
          
                   if (DEBUG >= 1 && _HDMIflag == 1){
                          
                          
                          for (int i = 1; i <= 8; i++) { 
          
                            //vdgfx.text.setCursor(row1+tempOffset, _startHeight+(_heightOffset*(i-1)));     
          
                                    switch (_tally[i]){
          
                                                            case 0: 
                                                                    vdgfx.fillRect(_row1+tempOffset, _startHeight-16+(_heightOffset*(i-1)), boxWidth, boxHeigth, vdgfx.Black()); 
          
                                                            break;
          
                                                            case 1: 
                                                                    vdgfx.fillRect(_row1+tempOffset, _startHeight-16+(_heightOffset*(i-1)), boxWidth, boxHeigth, vdgfx.Red()); 
          
                                                            break;
          
                                                            case 2: 
                                                                    vdgfx.fillRect(_row1+tempOffset, _startHeight-16+(_heightOffset*(i-1)), boxWidth, boxHeigth, vdgfx.Green()); 
          
                                                            break;
          
                                                             case 3: 
                                                                    vdgfx.fillRect(_row1+tempOffset, _startHeight-16+(_heightOffset*(i-1)), boxWidth, boxHeigth, vdgfx.Red()); 
          
                                                            break;
          
          
                                                            
                                                          }
                                                          
                          }
                }
       }
  
}

void CCU_decoder::HDMI_setIris(){


         if (_HDMI_en == true){

                  if (DEBUG >= 1 && _HDMIflag == 1){

                      vdgfx.text.setAlpha(255);
                      vdgfx.text.setSize(1);
                      vdgfx.text.setColor(vdgfx.Yellow());
                  
                        int colomn = 1;
                        int result;
                        uint16_t temp = (_iris[_cam][1]*255) + _iris[_cam][0];
                        int tempOffset = 42;
                        
                        
                            // blank field on screen
                            vdgfx.fillRect((_row1+50+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1), _blankingWidth, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+50+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            
                            result = map(temp,18480,3200,0,100);              
                            vdgfx.print(result);
                            
                      _HDMIflag = 0;
                          
                        
                  }
                  
         }

  
}


void CCU_decoder::HDMI_setShutter(){

           if (_HDMI_en == true){

                  if (DEBUG >= 1 && _HDMIflag == 1){

                      vdgfx.text.setAlpha(255);
                      vdgfx.text.setSize(1);
                      vdgfx.text.setColor(vdgfx.White());
                  
                        int colomn = 1;
                        int result;
                        uint16_t temp = (_shutter[_cam][1]*255) + _shutter[_cam][0];
                        int tempOffset = 42;
                        
                        
                            // blank field on screen
                            vdgfx.fillRect((_row1+240+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1),_blankingWidth+10, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+240+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            
                            switch (temp) {

                                    case 41505:
                                          result = 24;
                                    break;

                                    case 39844:
                                          result = 25;
                                    break;

                                    case 33203:
                                          result = 30;
                                    break;

                                    case 19922:
                                          result = 50;
                                    break;

                                    case 16602:
                                          result = 60;
                                    break;

                                    case 13281:
                                          result = 75;
                                    break;

                                    case 11068:
                                          result = 90;
                                    break;

                                    case 9961:
                                          result = 100;
                                    break;

                                    case 8301:
                                          result = 120;
                                    break;

                                    case 6641:
                                          result = 150;
                                    break;

                                    case 5535:
                                          result = 180;
                                    break;

                                    case 3985:
                                          result = 250;
                                    break;

                                    case 2768:
                                          result = 360;
                                    break;

                                    case 1993:
                                          result = 500;
                                    break;

                                    case 1374:
                                          result = 725;
                                    break;

                                    case 997:
                                          result = 1000;
                                    break;

                                    case 688:
                                          result = 1450;
                                    break;

                                    case 499:
                                          result = 2000;
                                    break;

                                    

                                      
                            }
                            vdgfx.print(result);
                            
                      /*
                      // debug raw value
                            vdgfx.fillRect((_row1+50+tempOffset), (_startHeight+(_heightOffset*(9)))-(_blankingHeight-1), _blankingWidth+80, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+50+tempOffset, _startHeight+(_heightOffset*(9)));     
                            vdgfx.print(temp);
                      */
                            
                      _HDMIflag = 0;
                          
                        
                  }
                  
           }

  
}


void CCU_decoder::HDMI_setBlack(){


       if (_HDMI_en == true){

          if (DEBUG >= 1 && _HDMIflag == 1){

                      vdgfx.text.setAlpha(255);
                      vdgfx.text.setSize(1);
                      vdgfx.text.setColor(vdgfx.White());
                  
                        int colomn = 1;
                        int result;
                        uint16_t temp = (_black[_cam][7]*255) + _black[_cam][6];
                        
                        int tempOffset = 42;
                        
                        
                            // blank field on screen
                            vdgfx.fillRect((_row1+150+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1), _blankingWidth, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+150+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            
                            
                            if (temp >=0 && temp <= 32000){
                                result = map(temp,0,4092, 0,100); 
                            } else {
                                result = map(temp,65535,61200, 0,-100); 
                            }
                            //result = map(temp,-4092,4092,-100,100);              
                            vdgfx.print(result);
                            
                      _HDMIflag = 0;
                          
                      /*   
                      // debug raw value
                            vdgfx.fillRect((_row1+50+tempOffset), (_startHeight+(_heightOffset*(9)))-(_blankingHeight-1), _blankingWidth+80, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+50+tempOffset, _startHeight+(_heightOffset*(9)));     
                            vdgfx.print(temp);
                      */
                  }

       }

  
}



void CCU_decoder::HDMI_setGain(){


       if (_HDMI_en == true){

          if (DEBUG >= 1 && _HDMIflag == 1){

                      vdgfx.text.setAlpha(255);
                      vdgfx.text.setSize(1);
                      
                  
                        int colomn = 1;
                        int result;
                        uint16_t temp = (_Gain[_cam][1]*255) + _Gain[_cam][0];
                        
                        int tempOffset = 20;
                        
                        
                            // RED
                            vdgfx.fillRect((_row1+360+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1), _blankingWidth+5, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+360+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            

                                result = map(temp,0,32640, 0, 160); 
                            
                            vdgfx.text.setColor(vdgfx.Red());           
                            vdgfx.print(result/10);
                            vdgfx.print(".");
                            int result2 = (result/10)*10;
                            result = result-result2;
                            vdgfx.print(result);
                            
                            
                            // GREEN
                            temp = (_Gain[_cam][3]*255) + _Gain[_cam][2];
                            tempOffset = 23;
    
                            // blank field on screen
                            vdgfx.fillRect((_row1+440+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1), _blankingWidth+5, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+440+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            

                                result = map(temp,0,32640, 0, 160); 
                            
                            vdgfx.text.setColor(vdgfx.Green());           
                            vdgfx.print(result/10);
                            vdgfx.print(".");
                              result2 = (result/10)*10;
                            result = result-result2;
                            vdgfx.print(result);


                            
                            
                            // BLUE
                            temp = (_Gain[_cam][5]*255) + _Gain[_cam][4];
                              tempOffset = 30;
    
                            // blank field on screen
                            vdgfx.fillRect((_row1+520+tempOffset), (_startHeight+(_heightOffset*(_cam-1)))-(_blankingHeight-1), _blankingWidth+5, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+520+tempOffset, _startHeight+(_heightOffset*(_cam-1)));        
                            

                                result = map(temp,0,32640, 0, 160); 
                            
                            vdgfx.text.setColor(vdgfx.Blue());           
                            vdgfx.print(result/10);
                            vdgfx.print(".");
                             result2 = (result/10)*10;
                            result = result-result2;
                            vdgfx.print(result);
                            
                      _HDMIflag = 0;
                          
                         /*
                      // debug raw value
                            vdgfx.fillRect((_row1+50+tempOffset), (_startHeight+(_heightOffset*(9)))-(_blankingHeight-1), _blankingWidth+80, _blankingHeight, vdgfx.Black()); 
                            vdgfx.text.setCursor(_row1+50+tempOffset, _startHeight+(_heightOffset*(9)));     
                            vdgfx.print(temp);
                      */
                  }

       }

  
}


void CCU_decoder::HDMIdrawDebugScreen(){

       if (_HDMI_en == true){

          // textx
                  vdgfx.text.setAlpha(255);
                  vdgfx.text.setSize(1);
                  vdgfx.text.setColor(vdgfx.White());

          // Iris
                  vdgfx.text.setCursor(_row2, _startHeight*1);                
                  vdgfx.print("Iris: ");

       }

}




// ---------------------------- MicroViewDisplay ------------------------------------


void CCU_decoder::setSDI_OK(int c){

       Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(SDI));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      

  
}


void CCU_decoder::setTXradio(int c){

       Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(TXradio));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      

  
}



void CCU_decoder::setBatteryStatus(int c){

       Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(BATTERY));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      

  
}


void CCU_decoder::statusLinkRX_TX(int c){

        Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                delay(1);
                Wire.write(byte(LINK));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      
        
        if (c == 0){
          
          delay(20);
          Wire.requestFrom(DisplayAddr, 1);
              if(1 <= Wire.available())    // if two bytes were received
                     
                temp = Wire.read();
                //Serial.print(" Link data:");
                          // Serial.println(temp);
                if (linkFlag != temp){
                  linkFlag = temp; // Reads the data from the register  
                   if (DEBUG >=1){
                           
                           Serial.print(" Link data:");
                           Serial.println(linkFlag);
                           
                      }
                
                   if (linkFlag == 255){

                    // Activate Link procedure on receiver
                    digitalWrite(RadioLinkEnablePin, LOW);
                    delay(350);
                    digitalWrite(RadioLinkEnablePin, HIGH);

                    Wire.beginTransmission(DisplayAddr); // transmit to Display to blink Yellow
                      Wire.write(byte(I2C_start));
                      Wire.write(byte(LINK));
                      Wire.write(byte(128));
                    Wire.endTransmission();      // stop transmitting and read byte from Display    
                      //HDMIupdateCAMnumber();
                   
                    delay(850);

                    Wire.beginTransmission(DisplayAddr); // transmit to Display to blink Yellow
                      Wire.write(byte(I2C_start));
                      Wire.write(byte(LINK));
                      Wire.write(byte(1));
                    Wire.endTransmission();      // stop transmitting and read byte from Display    

                    
                      if (DEBUG >=1){
                           
                           Serial.print(" Link request received:");
                           
                      }

                      linkFlag = 128;
                   }
                  
                }

                
                
              }
        
   
  
}



void CCU_decoder::CamNumberDisplay(int c){

        Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(CAMERA));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      
        
        if (c == 0){
          
          delay(70);
          Wire.requestFrom(DisplayAddr, 1);
              if(1 <= Wire.available())    // if two bytes were received
                     
                temp = Wire.read();
                if (CAMnumber != temp){
                  CAMnumber = temp; // Reads the data from the register  
                  //HDMIupdateCAMnumber();
                  CAMnumberChangedFlag = true;
                  
                      if (DEBUG >=1){
                           
                           Serial.print(" CAMnumber received:");
                           Serial.println(CAMnumber);
                      }
                  
                }
                
                
              }
        
   
  
}



void CCU_decoder::setTallyDisplay(int c){

       Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(TALLY));
                Wire.write(byte(_tally[CAMnumber]));
        Wire.endTransmission();      // stop transmitting and read byte from Display      

  
}


void CCU_decoder::requestDebug(int c){

        Wire.beginTransmission(DisplayAddr); // transmit to Display
                Wire.write(byte(I2C_start));
                Wire.write(byte(DEBUGMODE));
                Wire.write(byte(c));
        Wire.endTransmission();      // stop transmitting and read byte from Display      
        
        if (c == 0){
          
          delay(70);
          Wire.requestFrom(DisplayAddr, 1);
              if(1 <= Wire.available())    // if two bytes were received
                     
                temp = Wire.read();
                if (DEBUG != temp){
                  DEBUG = temp; // Reads the data from the register  
                  HDMImenu();
                  
                      
                  
                }
        }
                
                
            
  
}



void CCU_decoder::setCAMdataToDisplay(){

  /*
   *  #define CAMERA             1  //  Data (1) Byte    // Selected camera for TALLY display
      #define CAM_enable         2  //  Data (8) Bytes   // Camera 0-7 enable
   */
if (I2C_OK == true){
      // SET_router address
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(CAMERA));
                Wire.write(byte(CAMERA));
                    
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         /*
      delay(20);
      
        // SET_CAM enables
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(CAM_enable));
                    Wire.write(byte(readEnable[0]));
                    Wire.write(byte(readEnable[1]));
                    Wire.write(byte(readEnable[2]));
                    Wire.write(byte(readEnable[3]));
                    Wire.write(byte(readEnable[4]));
                    Wire.write(byte(readEnable[5]));
                    Wire.write(byte(readEnable[6]));
                    Wire.write(byte(readEnable[7]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         */
  }
  
}


void CCU_decoder::requestUpdatesFromDisplay(){

/*
 * 
#define CAMERA             1  //  Data (1) Byte    // Selected camera for TALLY display
#define CAM_enable         2  //  Data (8) Bytes   // Camera 0-7 enable
#define TALLY              30 //  Data (1) Byte    // Tally status of selected camera
#define IP_client      31 //  Data (4) Bytes   // IP ADRESS client
#define IP_subnet      32 //  Data (4) Bytes   // IP subnet 255.255.255.0
#define IP_ATEM        33 //  Data (4) Bytes   // Send all values to Camera
#define IP_router      34 //  Data (4) Bytes   // Send all values to Camera
#define MACaddr        35 //  Data (4) Bytes   // Send all values to Camera
#define ATEMstat           40 //  Data (4) Bytes   // Send all values to Camera
#define MACaddr            41 //  Data (12) Bytes  // MAC address Client
#define CMTXstat           42 //  Data (4) Bytes   // Send all values to Camera
#define ReqUpdate          50 // Request if there is an update, return 0 for no updates
#define SetUpdate          51 // Request if there is an update, return 0 for no updates
#define Available          52 // Request if there is an response
 */
   changeDetected = 0; // Reset change detector
   int i=0;
   int amount = 0;

   if (I2C_OK == true){
   
         // Step 1: Request if there are changes
          Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(ReqUpdate));
                Wire.write(byte(0)); // request status
              Wire.write(byte(I2C_end));
          Wire.endTransmission();      // stop transmitting
      
        // step 2: wait for readings to happen
        delay(70);                   // datasheet suggests at least 65 milliseconds
      
        // step 3: request reading from sensor
        Wire.requestFrom(DisplayAddr, 8);    // request 8 bytes from slave device #112
      
        // step 4: receive reading from sensor
          if (8 <= Wire.available()) { // if 8 bytes were received
            
            for (int i=0; i <= 7; i++){
                 requestReturn[i] = Wire.read();  // read all menu changes
                if (requestReturn[i] != 0){
                  changeDetected = 1;
                }
            }
            
          }
      
      
          // step 5 Process changes if there was a change detected
          if (changeDetected == 1)
          {
      
              // CAMnumber has changed
              if (requestReturn[0] != 0)
              {
                  CAMnumber = requestReturn[0];  // Load CAMnumber with received value
                  EEPROM.write(30, CAMnumber);   // write data to EEprom
                  requestReturn[0] = 0;  // reset value
                  refresh = 1;
              }
      
      
             // CAM_enable has changed
              if (requestReturn[1] != 0)
              {
                  amount = 8;
                  refresh = 1;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(CAM_enable));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request 8 bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            _readEnable[i] = Wire.read();  // read all read enables
                          }
                          // write readCAM
                          EEPROM.write(31, _readEnable[0]);
                          EEPROM.write(32, _readEnable[1]);
                          EEPROM.write(33, _readEnable[2]);
                          EEPROM.write(34, _readEnable[3]);
                          EEPROM.write(35, _readEnable[4]);
                          EEPROM.write(36, _readEnable[5]);
                          EEPROM.write(37, _readEnable[6]);
                          EEPROM.write(38, _readEnable[7]);
                      }
                  requestReturn[1] = 0;  // reset value
               }
      
      
      
               // IP client has changed
              if (requestReturn[2] != 0)
              {
                  amount = 4;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(IP_client));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request 4 bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            ip[i] = Wire.read();  // read all read enables
                          }
                          // write EEprom 
                          EEPROM.write(0, ip[0]);
                          EEPROM.write(1, ip[1]);
                          EEPROM.write(2, ip[2]);
                          EEPROM.write(3, ip[3]);
                      }
                  requestReturn[2] = 0;  // reset value
               }
      
      
      
              // IP ATEM has changed
              if (requestReturn[3] != 0)
              {
                  amount = 4;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(IP_ATEM));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request amount bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            atem_ip[i] = Wire.read();  // read all read enables
                          }
                          // write EEprom 
                          EEPROM.write(6, atem_ip[0]);
                          EEPROM.write(7, atem_ip[1]);
                          EEPROM.write(8, atem_ip[2]);
                          EEPROM.write(9, atem_ip[3]);
                      }
                  requestReturn[3] = 0;  // reset value
               }
      
      
      
               // IP_subnet has changed
              if (requestReturn[4] != 0)
              {
                  amount = 4;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(IP_subnet));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request amount bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            subnet[i] = Wire.read();  // read all read enables
                          }
                          // write EEprom 
                          EEPROM.write(16, subnet[0]);
                          EEPROM.write(17, subnet[1]);
                          EEPROM.write(18, subnet[2]);
                          EEPROM.write(19, subnet[3]);
                      }
                  requestReturn[4] = 0;  // reset value
               }
      
      
      
                // IP_router has changed
              if (requestReturn[5] != 0)
              {
                  amount = 4;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(IP_router));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request amount bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            router[i] = Wire.read();  // read all read enables
                          }
                          // write EEprom 
                          EEPROM.write(20, router[0]);
                          EEPROM.write(21, router[1]);
                          EEPROM.write(22, router[2]);
                          EEPROM.write(23, router[3]);
                      }
                  requestReturn[5] = 0;  // reset value
               }
      
      
      
                 // MAC address has changed
              if (requestReturn[6] != 0)
              {
                  amount = 6;
                 Wire.beginTransmission(DisplayAddr); // transmit to Display
                      Wire.write(byte(I2C_start));
                        Wire.write(byte(ReqUpdate));
                        Wire.write(byte(MACaddr));
                      Wire.write(byte(I2C_end));
                 Wire.endTransmission();      // stop transmitting
      
                  // step 2: wait for readings to happen
                  delay(70);                   // datasheet suggests at least 65 milliseconds
                
                  // step 3: request reading from sensor
                  Wire.requestFrom(DisplayAddr, amount);    // request amount bytes from slave device 
                
                  // step 4: receive reading from sensor
                    if (amount <= Wire.available()) { // if two bytes were received
                      
                      for (int i=0; i <= amount-1; i++)
                          {
                            mac[i] = Wire.read();  // read all read enables
                          }
                          // write EEprom 
                          EEPROM.write(10, mac[0]);
                          EEPROM.write(11, mac[1]);
                          EEPROM.write(12, mac[2]);
                          EEPROM.write(13, mac[3]);
                          EEPROM.write(14, mac[4]);
                          EEPROM.write(15, mac[5]);
                      }
                  requestReturn[6] = 0;  // reset value
               }




              // DEBUG ON/OFF
                if (requestReturn[8] != DEBUG)
              {
                  DEBUG =  requestReturn[8];  // Load DEBUG VALUE
                  //EEPROM.write(30, CAMnumber);   // write data to EEprom
                  requestReturn[8] = 0;  // reset value
                  refresh = 1;
              }
          }
   }


            
          
 } // end if changes detected










void CCU_decoder::setIPtoDisplay(){

/*
#define CAMERA             1  //  Data (1) Byte    // Selected camera for TALLY display
#define CAM_enable         2  //  Data (8) Bytes   // Camera 0-7 enable
#define TALLY              30 //  Data (1) Byte    // Tally status of selected camera
#define IP_client      31 //  Data (4) Bytes   // IP ADRESS client
#define IP_subnet      32 //  Data (4) Bytes   // IP subnet 255.255.255.0
#define IP_ATEM        33 //  Data (4) Bytes   // Send all values to Camera
#define IP_router      34 //  Data (4) Bytes   // Send all values to Camera
#define MACaddr        35 //  Data (4) Bytes   // Send all values to Camera
#define ATEMstat           40 //  Data (4) Bytes   // Send all values to Camera
#define MACaddr            41 //  Data (12) Bytes  // MAC address Client
#define CMTXstat           42 //  Data (4) Bytes   // Send all values to Camera
#define ReqUpdate          50 // Request if there is an update, return 0 for no updates
#define SetUpdate          51 // Request if there is an update, return 0 for no updates
#define Available          52 // Request if there is an response

 */

if (I2C_OK == true){
          // Set IP client address
          Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(IP_client));   
                    Wire.write(byte(ip[0]));
                    Wire.write(byte(ip[1]));
                    Wire.write(byte(ip[2]));
                    Wire.write(byte(ip[3]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
      
      delay(20);
      
             // Set IP ATEM address
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(IP_ATEM));
                    Wire.write(byte(atem_ip[0]));
                    Wire.write(byte(atem_ip[1]));
                    Wire.write(byte(atem_ip[2]));
                    Wire.write(byte(atem_ip[3]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         
      delay(20);
      
      // Set MAC address
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(MACaddr));
                    Wire.write(byte(mac[0]));
                    Wire.write(byte(mac[1]));
                    Wire.write(byte(mac[2]));
                    Wire.write(byte(mac[3]));
                    Wire.write(byte(mac[4]));
                    Wire.write(byte(mac[5]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         
      delay(20);
      
      
      // SET_IP_subnet address
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(IP_subnet));
                    Wire.write(byte(subnet[0]));
                    Wire.write(byte(subnet[1]));
                    Wire.write(byte(subnet[2]));
                    Wire.write(byte(subnet[3]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         
      delay(20);
      
      
      // SET_router address
         Wire.beginTransmission(DisplayAddr); // transmit to Display
              Wire.write(byte(I2C_start));
                Wire.write(byte(SetUpdate));
                Wire.write(byte(IP_router));
                    Wire.write(byte(router[0]));
                    Wire.write(byte(router[1]));
                    Wire.write(byte(router[2]));
                    Wire.write(byte(router[3]));
             Wire.write(byte(I2C_end));
         Wire.endTransmission();      // stop transmitting
         
      delay(20);
      
    }

}
          
