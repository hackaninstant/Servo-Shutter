# Servo-Shutter
A Servo operated 3D Printed Shutter

This project was designed for a 3D printed servo operated leaf shutter and Arduino Nano. Being servo operated means the top shutter speed is limited to the speed of the servo motor. Since most 9g servos operate around 100ms, the shutter will be limited to around 1/8 second, which works for my purposes in exposing paper and lith negatives. 

Features: 

- small footprint: will work with Mega 168 boards
- 4 button operation
- uses I2C 128x64 SSD1306 display
- shutter speeds from 1/8 to 10 minutes
- cancelable self timer
- built in servo angle adjustments for Shutter Open, Close, and relief
- adjustable button delay
- Operates on 6 volts (4AA)

Requirements:

- 3D printed servo operated shutter from https://www.thingiverse.com/thing:4404243
- Arduino Nano
- 4 momentary switches
- I2C SSD1306 display
- 9g servo

Configuration:

Nano:
- D3: Minus Switch
- D4: Plus Switch
- D5: Shutter Switch
- D6: Menu Switch
- D9: Servo data
- A4: SDA on I2C SSD1306
- A5: SDL on I2C SSD1306
- 5V: 5V on I2C SSD1306
- GND: GND on I2C SSD1306, GND on servo

Servo:
- Data: D9 on Nano
- VCC: 6 volts separate power supply
- GND: GND on power supply and Nano

This project uses SSD1306ASCII.h since graphics aren't needed and the libraries are smaller. The code is easily configurable to different pin layouts and servo angle defaults.

To Operate:

After powering up, the shutter will open for focusing/composing. Press the shutter button to close the shutter to ready for exposure.

Select the shutter speed by pressing the plus or minus buttons. TIME is selected by navigating one less than 1/8. 

Press the shutter button to open the shutter for the specified time. If TIME is selected, press the shutter again to close. If self timer is specified, the timer will count down to 0 then open the shutter. If the shutter button is pressed during the countdown, the self timer will be cancelled and the shutter will not open. 

Press the menu button to select between the self timer, shutter adjust, and main screen.

While in the self timer menu, the plus and minus buttons will modify the self timer value. Pressing the Shutter button will open the shutter for focusing/composing.

While in the shutter adjust menu, pressing the shutter button will select between the options. Then, pressing the plus and minus buttons will modify the value of the option next to the asterisk:
1. Shutter Close is the angle of the servo with the shutter closed
2. Shutter Open is the angle of the servo with the shutter open
3. Shutter Relief is the angle the servo backs off after opening or closing the shutter. This is to prevent the servo from vibrating if the shutter is stiff
4. Button Delay is the value in milliseconds of how fast the buttons repeat if held down. 

These fine adjustments enable you to tweak the servo settings since it's difficult to perfectly install the servo at the exact angle. 

All settings are saved to eeprom when changed, except for the self timer. 
