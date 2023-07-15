# Servo-Shutter
A Servo operated 3D Printed Shutter for Arduino Nano

This project was designed for a 3D printed servo operated leaf shutter and Arduino Nano. Being servo operated means the top shutter speed is limited to the speed of the servo motor. Since most 9g servos operate around 100ms, the shutter will be limited to around 1/10 second fully open. Faster speeds can be added, but the shutter won't open completely, which should work fine for smaller apertures.

The schematic can be used for this and other arduino projects that use 3 pin sensors. The zip file contains gerber files for PCB production. 

Features: 

- small footprint: will work with Nano Mega 168 boards
- 4 button operation
- optional rotary encoder capability
- uses I2C 128x64 SSD1306 display
- shutter speeds from 1/13 to 10 minutes (coded down to 1/20 but blades may not open fully)
- cancelable self timer up to 255 seconds
- built in servo adjustments for Shutter Open, Close, and relief angles, as well as servo delay
- adjustable button repeat rate delay
- X or M flash sync (electronic flash for now)
- Operates on 6 volts (4AA) for the servo, and 7.4 volts for the arduino. The arduino will run with as little as 3 volts.

Requirements:

- 3D printed servo operated shutter from https://www.thingiverse.com/thing:6033871 (or available here)
- Arduino Nano, 16K or 32K version
- 4 momentary switches
- I2C SSD1306 128x64 display
- 9g servo 
- TLP 127 Opto isolator with 330 Ohm resistor (Optional for flash sync)
- rotary encoder (optional)

Configuration:

Nano:
- D3: Minus Switch
- D4: Plus Switch
- D5: Shutter Switch
- D6: Menu Switch
- D9: Servo data
- D10: (optional) 330 Ohm resistor then to TLP127 Opto Isolator
- A4: SDA on I2C SSD1306
- A5: SCL on I2C SSD1306
- 3.3V: VCC on I2C SSD1306
- VIN: 7.5v (5 AA or 2 LiPo batteries)
- GND: GND on I2C SSD1306, GND on servo, pin 3 on TLP127
- D2: (optional) rotary encoder switch
- D7: (optional) rotary encoder CLK
- D8: (optional) rotary encoder DT

SSD1306 OLED display:
- SDA: A4 on Nano
- SCL: A5 on Nano
- GND: GND on Nano
- VCC: 3.3v on Nano

Servo:
- Data: D9 on Nano
- VCC: 6-7 volts on separate battery from Nano
- GND: GND on Nano

TLP127 Opto Isolator (optional):
- Pin 1 (Anode): 330 Ohm resistor, then to D10
- Pin 3 (Cathode): GND
- Pin 4 (Emiter): GND (outside barrel) on flash PC connection
- Pin 6 (Collector): VCC (inside barrel) on flash PC connection

Rotary Encoder (optional):
- GND: GND on Nano
- VCC: 5v on Nano
- CLK: Pin D7 on Nano
- DT: Pin D8 on Nano
- SW: Pin 02 on Nano

Separate power supplies for servo/arduino is the easiest way to deal with servo noise and the arduino. If you are successful in using a capacitor to reduce erratic arduino behaviour, please share that info here. (I tried a 330mF from servo VCC to gnd and it didn't help)

This project uses the SSD1306ASCII.h library for the display since graphics aren't needed and the libraries are smaller. The code is easily configurable to different pin layouts and servo angle defaults and is documented for modification. The TLP127 was selected since it can handle 300v which some older electronic flashes use for flash sync. The TLP127 lets enough current through for an electronic flash, but would need an additional transistor to let enough current through for a flashbulb. 

An optional rotary encoder can be included which allows faster navigation. 

To Operate:

After powering up, the shutter will open for focusing/composing. Press the shutter button to close the shutter to ready it to pull the dark slide for exposure.

Select the shutter speed by pressing the plus or minus buttons (or turning the rotary encoder). TIME is selected by navigating one less than 1/8. 

Press the shutter button to open the shutter for the specified time. If TIME is selected, press the shutter again to close. If self timer is specified, the timer will count down to 0 then open the shutter. If the shutter button is pressed during the countdown, the self timer will be cancelled and the shutter will not open. 

Press the menu button (or the switch on the rotary encoder) to select between the self timer, shutter adjust, and main screen.

While in the self timer menu, the plus and minus buttons (or turning the rotary encoder) will modify the self timer value. Pressing the Shutter button will open the shutter for focusing/composing and/or close it to get it ready to shoot.

While in the shutter adjust menu, pressing the shutter button will select between the options. Then, pressing the plus and minus buttons (or turning the rotary encoder) will modify the value of the option next to the asterisk:
1. Shutter Close is the angle of the servo with the shutter closed
2. Shutter Open is the angle of the servo with the shutter open
3. Shutter Relief is the angle the servo backs off after opening or closing the shutter. This is to prevent the servo from vibrating if the shutter is stiff
4. Button Delay is the value in milliseconds of how fast the buttons repeat if held down. This does not affect the rotary encoder. 
5. Servo Delay is the value in milliseconds it takes to fully open the shutter. This value is subtracted from the shutter speed and used to calculate the flash sync, unless this value is higher than the shutter speed in milliseconds.   
6. Flash Sync X is 0, meaning the flash will go off when the shutter is completely open. Flash Sync M is 20, which will be subtracted from the servo delay to fire the flash 20ms before the shutter is completely open. 

These fine adjustments enable you to tweak the servo settings without having to modify the code and upload it from a computer since it's difficult to perfectly install the servo at the exact angle, and the relief angle can help reduce strain on the servo if the shutter is stiff. 

All settings are saved to eeprom when changed, except for the self timer by design, since self timers usually reset on normal cameras when powered off. 
