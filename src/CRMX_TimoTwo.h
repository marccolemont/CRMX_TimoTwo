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

#ifndef CRMX_TimoTwo_h
#define CRMX_TimoTwo_h

#include <Arduino.h>
#include <SPI.h>
#include "Globals.h"

#define TimoTwo_Settings (SPISettings(500000, MSBFIRST, SPI_MODE0))

// SPI commands
#define READ_DMX  0x81
#define READ_ASC  0x82
#define WRITE_DMX 0x91
#define NOP 0xFF

#define WRITE_REG 0x40 // + Register data
#define READ_REG  0x00 // + Register data

// Register MAP
#define CONFIG        0x00             // int8
      /*
       #define UART_ENbit            0 // Enable UART output of DMX frames (required for RDM) 0 = Disabled, 1 = Enabled (1)
       #define RADIO_TX_RX_MODEbit   1 // 0 = Receiver, 1 = Transmitter
       #define RADIO_ENABLEbit       7 // Enable wireless operation 0 = Disabled, 1 = Enabled (1)
       */
                
#define STATUS        0x01             // int8
      /*
       #define LINKEDbit             0 // 0 = Not linked, 1 = Linked to TX (or pairing) Write 1 to unlink
       #define RF_LINKbit            1 // 0 = No radio link, 1 = Active radio link On transmitter, write 1 to start linking
       #define IDENTIFYbit           2 // 0 = No identify, 1 = RDM identify active On transmitter, write 1 to start identifying all linked RX, write 0 to stop.(0)
       #define DMXbit                3 // 0 = No DMX available, 1 = DMX available (0)
       #define UPDATE_MODEbit        7 // 0 = chip operational, 1 = In driver update mode (0)
       */
        
#define IRQ_MASK      0x02             // int8
      /*
       #define RX_DMX_IRQ_ENbit      0 // Enable DMX frame reception interrupt (0)
       #define LOST_DMX_IRQ_ENbit    1 // Enable loss of DMX interrupt (0)
       #define DMX_CHANGED_IRQ_ENbit 2 // Enable DMX changed interrupt (0)
       #define RF_LINK_IRQ_ENbit     3 // Enable radio link status change interrupt (0)
       #define ASC_IRQ_ENbit         4 // Enable alternative start code interrupt (0)
       #define IDENTIFY_IRQ_ENbit    5 // Enable identify device interrupt (0)
       */
        
#define IRQ_FLAGS     0x03             // int8
      /*
       #define RX_DMX_IRQbit         0 // Complete DMX frame received interrupt (0)
       #define LOST_DMX_IRQbit       1 // Loss of DMX interrupt (0)
       #define DMX_CHANGED_IRQbit    2 // DMX changed in DMX window interrupt (0)
       #define RF_LINK_IRQbit        3 // Radio link status change interrupt (0)
       #define ASC_IRQbit            4 // Alternative start code frame received interrupt (0)
       #define IDENTIFY_IRQbit       5 // Identify device state change interrupt (0)
       #define SPI_DEVICE_BUSYbit    7 // SPI slave device is busy and cannot comply with command. Command sequence MUST be restarted. (0)
       */

#define DMX_WINDOW    0x04             // int32
      /*
       #define START_ADDRESSbit      0 // Start address of DMX window (0)
       #define WINDOW_SIZEbit        16 // Length of DMX window (512)
       */

#define ASC_FRAME     0x05             // int24
      /*
       #define START_CODEbit         0 // Start code of received ASC frame (0)
       #define ASC_FRAME_LENGTHbit   8 // Length of received ASC frame (0-512) (0)
       */

#define LINK_QUALITY  0x06             // int8          
                                       // Packet delivery rate (display as %) 0 = 0%, 255 = 100%
#define DMX_SPEC      0x08             // int64
      /*
       #define REFRESH_PERIODbit     0 // DMX frame length in µs (25000)
       #define INTERSLOT_TIMEbit     32 // Interslot spacing in µs (0)
       #define N_CHANNELSbit         48 // Number of slots/channels to generates (512)
       */

#define DMX_CONTROL   0x09             // int8
       /*
       #define ENABLEbit             0 // 0 = internal generation disabled 1 = internal generation enabled (0)
       */

#define VERSION       0x10             // int64
      /*
       #define DRIVER_VERSIONbit     0 // Software version
       #define HW_VERSIONbit         32 // Hardware revision
       */
       
#define RF_POWER      0x11            // RF Output power in transmitter mode (3) 0-7

#define BLOCKED_CH    0x12             // int88            
                                      // Blocked channel flags 0-87 (0)
#define BINDING_UID   0x20             // int48            
                                       // RDM UID of the host device 0-47 (0)
#define BLE_STATUS    0x30             // int8
        /*
        #define BLE_ENABLEDbit       0 // 0 = BLE disabled 1 = BLE enabled (1)
        #define BLE_PINbit           1 // 0 = BLE PIN disabled 1 = BLE PIN enabled 
        */

#define BLE_PIN       0x31             // int48             
                                       // PIN used for BLE connections 0-47
#define BATTERY_LEVEL 0x32            // int8 Battery level, in percent, reported by the BLE Battery Service
                                      // 0 = 0%, 100 = 100%, 255=No battery available, 101-254 reserved. (255)

#define TIMO_UNIVERSE_COLOR_REG 0x33    // 3 bytes

#define TIMO_OEM_INFO_REG 0x34          // 6 bytes





class CRMX_TimoTwo
{

    // IRQ_stuff
    static void isr0 ();
    static CRMX_TimoTwo * instance0_;
    volatile bool IRQ_pending;
    
  public:

      CRMX_TimoTwo();
    
      void IRQ_handler();

      void begin(uint8_t interruptPin, uint8_t SPIpin);
      
      bool IRQ_detected();  //Check is interrupt is available
    
      // variables
      int16_t N_CHANNELS = 512;  // AmountOfDMX channels TX
      int CRMX_detected;
      byte IRQ_maskData;
      byte IRQ_flagData;
      bool UART_EN = 1;
      uint8_t _IRQPin;
      uint8_t _SSPin;
    
      // Wireless data
      void writeDMX(int16_t index, byte value);
      void transmitDMX(); // transmit DMX universe
      uint8_t readDMXch(byte channel);
      void getDMX();

      // functions
      void setCONFIG(bool a, bool b, bool c);
      uint8_t getCONFIG();
    
      void setSTATUS(bool a, bool b, bool c, bool d, bool e, bool f, bool g, bool h);
      uint8_t getSTATUS();
      
      void setDMX_CONTROL(byte CRMXdata);
      void setDMX_WINDOW(int16_t address, int16_t windowsize);
      uint8_t getDMX_WINDOW(byte data);
      
      void setRF_POWER(byte CRMXdata);
      uint8_t getRF_POWER();
    
      void setIRQ_MASK(bool a, bool b, bool c, bool d, bool e, bool f);
      uint8_t getIRQ_MASK();
    
      uint8_t getIRQ_FLAGS();
    
      uint8_t getVersionNumber(int8_t versionData);
      uint8_t getLINK_QUALITY();
    
      uint8_t getIRQ();
      
      void setBATTERY_LEVEL(byte level);
      void setDMX_SPEC(uint32_t REFRESH, uint16_t INTERSLOT, uint16_t AmountCHANNELS);
      
      uint8_t getDMX_SPEC(int8_t dmxData);
      
        
  
  private:

        bool _CRMXbusy = false;
        uint8_t  writeRegister(byte command, byte amount);
        uint8_t  readRegister(byte command, byte amount);
    
        unsigned long currentMillisTime; // timer
        unsigned long previousMillisCRMX = 0;
        const long intervalCRMX   = 3;
        int channel_start;
        int channel_end;
        // DMX parameters
        int16_t START_ADDRESS_WINDOW;    // Depends on CAM selection
        int16_t WINDOW_SIZE; // CCU CAMERA DATA SIZE
    
        // Internal parameters
        
        byte _DMX[513];
    
        
      
};



#endif
