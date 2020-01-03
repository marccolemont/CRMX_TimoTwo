# CRMX_TimoTwo Library
CRMX TimoTwo Library for CRMX TimoTwo MKR Shield

![](images/MKR-CRMXTimoTwo.png)
Library works on the CRMX TimoTwo shield
- MKR shield board compatible
- Based on RadioLumen CRMX TimoTwo transceiver with external antenna
- 4 LED's on lightguide for status & RF receiving quality
- 1 RGB LED on lightguide with Universe color
- Button to Link two devices / clear linking
- 6 pin header with RX/TX and control signals (3V3 compatible) 
  for extention board with 5-Pin XLR galvanic shielded DMX-512
  - Pin 1: DMX_TXD
  - Pin 2: GND
  - Pin 3: DMX_RXD
  - Pin 4: RS485_DE
  - Pin 5: RS485_RE
  - Pin 6: 3V3
- SPI library to control:
  - Receiver / Transmitter mode
  - Settings for DMX-512 timings
  - Data tranmission / receiving of the DMX universe
  - IRQ settings and flag control for RF reception, new DMX values etc...
  - DMX window
  - Hardware version / firmware version UID readout
  
Copyright (c) 2020 MC-Productions.be
