# Servo-Shutter
A Servo operated 3D Printed Shutter for Arduino Nano

This project was designed for a 3D printed servo operated leaf shutter and Arduino Nano. Being servo operated means the top shutter speed is limited to the speed of the servo motor. Since most 9g servos operate around 100ms, the shutter will be limited to around 1/8 second, which works for my purposes in exposing paper and lith negatives. 

Features: 

- small footprint: will work with Nano Mega 168 boards
- 4 button operation
- uses I2C 128x64 SSD1306 display
- shutter speeds from 1/8 to 10 minutes
- cancelable self timer up to 255 seconds
- built in servo adjustments for Shutter Open, Close, and relief angles, as well as servo delay
- adjustable button repeat rate delay
- X or M flash sync (electronic flash for now)
- Operates on 6 volts (4AA) for the servo, and 3 volts for the arduino

Requirements:

- 3D printed servo operated shutter from https://www.thingiverse.com/thing:6033871
- Arduino Nano, 16K or 32K version
- 4 momentary switches
- I2C SSD1306 128x64 display
- 9g servo 
- TLP 127 Opto isolator with 330 Ohm resistor (Optional for flash sync)

Configuration:

Nano:
- D3: Minus Switch
- D4: Plus Switch
- D5: Shutter Switch
- D6: Menu Switch
- D9: Servo data
- D10: 330 Ohm resistor + TLP127 Opto Isolator
- A4: SDA on I2C SSD1306
- A5: SDL on I2C SSD1306
- 5V: 5V on I2C SSD1306 and power, can be as low as 2.5 volts
- GND: GND on I2C SSD1306, GND on servo

Servo:
- Data: D9 on Nano
- VCC: 6-8 volts separate power supply
- GND: GND on power supply and Nano

TLP127 Opto Isolator:
- Pin 1 (Anode): 330 Ohm resistor, then to D10
- Pin 3 (Cathode): GND
- Pin 4 (Emiter): GND (outside barrel) on flash PC connection
- Pin 6 (Collector): VCC (inside barrel) on flash PC connection

This project uses SSD1306ASCII.h library for the display since graphics aren't needed and the libraries are smaller. The code is easily configurable to different pin layouts and servo angle defaults and is documented for modification. The TLP127 was selected since it can handle 300v which some older electronic flashes use for flash sync. Although this has been tested with electronic flash, tests using flashbulbs failed to fire the flash. 

To Operate:

After powering up, the shutter will open for focusing/composing. Press the shutter button to close the shutter to ready it to pull the dark slide for exposure.

Select the shutter speed by pressing the plus or minus buttons. TIME is selected by navigating one less than 1/8. 

Press the shutter button to open the shutter for the specified time. If TIME is selected, press the shutter again to close. If self timer is specified, the timer will count down to 0 then open the shutter. If the shutter button is pressed during the countdown, the self timer will be cancelled and the shutter will not open. 

Press the menu button to select between the self timer, shutter adjust, and main screen.

While in the self timer menu, the plus and minus buttons will modify the self timer value. Pressing the Shutter button will open the shutter for focusing/composing and/or close it to get it ready to shoot.

While in the shutter adjust menu, pressing the shutter button will select between the options. Then, pressing the plus and minus buttons will modify the value of the option next to the asterisk:
1. Shutter Close is the angle of the servo with the shutter closed
2. Shutter Open is the angle of the servo with the shutter open
3. Shutter Relief is the angle the servo backs off after opening or closing the shutter. This is to prevent the servo from vibrating if the shutter is stiff
4. Button Delay is the value in milliseconds of how fast the buttons repeat if held down. 
5. Servo Delay is the value in milliseconds it takes to fully open the shutter. This value is subtracted from the shutter speed and used to calculate the flash sync. It cannot be higher than the fastest shutter speed (default 125ms, or 1/8 second).  
6. Flash Sync X is 0, meaning the flash will go off when the shutter is completely open. Flash Sync M is 20, which will be subtracted from the servo delay to fire the flash 20ms before the shutter is completely open. 

These fine adjustments enable you to tweak the servo settings without having to modify the code and upload it from a computer since it's difficult to perfectly install the servo at the exact angle, and the relief angle can help reduce strain on the servo if the shutter is stiff. 

All settings are saved to eeprom when changed, except for the self timer by design, since self timers usually reset on normal cameras when powered off. 
