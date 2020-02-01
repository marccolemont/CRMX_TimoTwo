/*  CRMX library for Arduino
    Copyright (C) 2020 MC-productions 
    Marc Colemont (marc.colemont@mc-productions.be)

    This librarty works together with the MKR CRMX_TimoTwo shield
    and MKR VIDOR Board
    
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

#ifndef CCU_encoder_h
#define CCU_encoder_h

#include <Arduino.h>

#include "CCUTXglobals.h"
#include "VidorGraphics.h"
#include "Vidor_GFX.h"

// when using ZERO boards MKR
#include <sam.h>
#include <FlashAsEEPROM.h>  // EEprom to flash library



#define SDI2HDMI Serial1

#define StartByte     0xF0  // Start command
#define TallyData     0xF1  // Start command
#define CCUData2      0xF2  // Start command
#define CCUData4      0xF3  // Start command
#define CCUData8      0xF4  // Start command
#define SDIdata       0xF6  // SDI_OK lock data



// serial commands  like MIDI
#define CAMERA             101    //  Data (1) Byte    // Selected camera for TALLY display
#define CAM_enable         102  //  Data (8) Bytes   // Camera 0-7 enable
#define TALLYI2C           130 //  Data (1) Byte    // Tally status of selected camera
#define IP_client          131 //  Data (4) Bytes   // IP ADRESS client
#define IP_subnet          132 //  Data (4) Bytes   // IP subnet 255.255.255.0
#define IP_ATEM            133 //  Data (4) Bytes   // Send all values to Camera
#define IP_router          134 //  Data (4) Bytes   // Send all values to Camera
#define MACaddr            135 //  Data (4) Bytes   // Send all values to Camera
#define ATEMstat           140 //  Data (4) Bytes   // Send all values to Camera
//"#define MACaddr            141 //  Data (12) Bytes  // MAC address Client
#define CMTXstat           142 //  Data (4) Bytes   // Send all values to Camera
#define ReqUpdate          150 // Request if there is an update, return 0 for no updates
#define SetUpdate          151 // Request if there is an update, return 0 for no updates
#define Available          152 // Request if there is an response
#define TALLY               30 //  Data (1) Byte // Send all values to Camera
#define DEBUGMODE           40  //  Data (1) Byte 
#define AccessPointMODE     41
#define BATTERY             42
#define TXradio             43
#define SDI                 44
#define LINK                55 // Link receiver & transmitter

#define StopByte           0xF7
#define I2C_end            0xF7

#define TX                 129


// I2C DisplayPCB
#include <Wire.h>
#define DisplayAddr   0x10  // I2C adress Display board 
#define I2C_start     0xF0  // Start command
#define StartByte     0xF0  // Start command
#define TallyData     0xF1  // Start command
#define CCUData2      0xF2  // Start command
#define CCUData4      0xF3  // Start command
#define CCUData8      0xF4  // Start command
#define SDIdata       0xF6  // SDI_OK lock data

// 9555  Address range is 0x20-0x27 (same as 8574)
#define INaddr  0x20  // 0x38 8574A addr 000
#define OUTaddr 0x21  // 0x39 8574A addr 001
#define NXP_INPUT      (0)  // For NXP9555
#define NXP_OUTPUT     (2)  // See data sheet
#define NXP_INVERT     (4)  // for details...
#define NXP_CONFIG     (6)





class CCU_decoder
{

  public:

        CCU_decoder();
        void begin();
        void camEnable(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h);  // enable & processCam 1-8
        void HDMIenable(bool en);
        void checkCCU();
        
        void ProcessTX(); // prepare for transmission

        // Variables
        void setSDI_OK(int c);
        void setTXradio(int c);
        void setBatteryStatus(int c);
        void statusLinkRX_TX(int c);
        void CamNumberDisplay(int c);
        void setTallyDisplay(int c);
        void requestDebug(int c);
        void setCAMdataToDisplay();
        void requestUpdatesFromDisplay();
        void setIPtoDisplay();
      
        int temp;
        byte radioData[2];
        byte linkFlag = 0;
      
        int newCCUdata = 0;
        int TXbufferEmpty =1;
      
        int RadioLinkEnablePin = 7;

        int refresh = 1;

        

  private:


  
     int tallyLED = LED_BUILTIN;

      // SDI BOARD
      bool SDI_OK = false;
      bool I2C_OK = false;
      bool I2C_Radio_OK = false;
  
      // 0 = changebit, 1-8 = Camera, 9 = percentage CAMnumber
      int  _iris[9][2];
      int  _focus[9][2];
      int  _autoFocus[2];
      int  _zoom[9][2];
      int  _shutter[9][2];
      int  _sensorGain[9][2];
      int  _WB[9][2];
      int  _Gain[9][8];
      int  _Gamma[9][8];
      int  _black[9][8];
      //int  WBalanceY[9][2], WBalanceR[9][2], WBalanceG[9][2], WBalanceB[9][2];
      int  _contrastCam[9][2];
      int  _saturation[9][2];
      int  _hue[9][2];
      int  _luma[9][2];
      int  _colorBar[9][2];
      int  _detail[9][2];
      int  _videoMode[9][2];
      int  _autofocus[9][2];
      int  _panTilt[9][4];
      int  _ptPreset[9][2];
      int  _reference[9][2];
      int  _FlagByte[9];
      
      
      /*
        FlagByte Bit set in first Byte of CCUaddress:
         0: Iris
         1: SensorGain/Shutter/WB
         2: Black RGBY
         3: Gamma RGBY
         4: Gain RGBY
         5: Contrast
         6: Hue/Sat
         7: Luma
      
       */
      
      int _dimmer[9];

      bool _HDMI_en = false;
      bool DEBUG = false;
      int _dataLength; // to read raw camera data
      bool dataAvailable;
      int _previousOffset;
      
      int _checkWrongByte = 0;
      
      int _groupID;
      int _cam;
      int _message;
      int _messageType;
      int _parameter;
      int _type;
      
      
      int _dataByte[24];

      void _RXdata_OK(byte OK_status);
      void _RXsettings(byte command, byte first);
      void _processCCU();
      void _cameraDefaults(int d);
      void CCUreceived();  // check if CCU data has been received

      int _offset;
      int _forceRefresh = 1;
      int _AmountOfNewBytes= 0;
      int _datachanged =0;
      
      bool _readEnable[9] {0,1,1,1,1,1,1,1,1};
      int  _CCUaddress[9] {0,1,64,128,192,256,320,384,448}; // Count addresses + 1 for real DMX address

      
      // BlackMagic CCU values
      //int _tally[9] {0,0,0,0,0,0,0,0,0};

      // VIDOR graphics
      void HDMI_start();
      void HDMI_end();
      void HDMImenu();
      void HDMIupdateCAMnumber();
      void HDMItallyRed();
      void HDMItallyWhite();
      void HDMItallyGreen();
      void HDMIdebugTally();
      void HDMI_setIris();
      void HDMI_setShutter();
      void HDMI_setBlack();
      void HDMI_setGain();
      void HDMIdrawDebugScreen();

      

      // HDMI screen variables
      int _startHeight       =    98;
      int _blankingHeight    =    20;
      int _blankingWidth     =    50;
      int _textOffset        =    40;
      int _heightOffset      =    35;
      int _row1              =    20;
      int _row2              =    100;
      int _HDMIflag          =    1;
      


};

#endif
