
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Servo.h>
#include <EEPROM.h>
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

// Create a new servo object:
Servo myservo;
#define servoPin 9

#define PlusButtonPin           3                       // Plus button pin
#define MinusButtonPin          4                       // Minus button pin
#define ShutterButtonPin        5                       // shutter button pin
#define MenuButtonPin           6                       // menu button pin

boolean PlusButtonState;                // "+" button state
boolean MinusButtonState;               // "-" button state
boolean ShutterButtonState;            // Metering button state
boolean MenuButtonState;                // menu button state

boolean mainscreen = true;            // indicates which screen displayed
boolean selftimermenu = false;
boolean adjustmenu = false;

// EEPROM for memory recording
#define ShutterSpeedAddr        1
#define ShutterOpenAddr         2
#define ShutterCloseAddr        3
#define ShutterReliefAddr       4
#define ButtonDelayAddr         5
#define ServoDelayAddr          6

// default values if running for the first time
#define defaultShutterOpen      68        // shutter open angle
#define defaultShutterClose     0         // shutter close angle
#define defaultShutterRelief    4         // angle that servo backs off after opening/closing
#define defaultButtonDelay      100       // button repeat delay
#define defaultShutterSpeedIndex 10       // default shutter spped
#define defaultServoDelay       10        // compensates for servo speed
#define MaxShutterIndex         45        // matches number if items in spvalues

// load values from EEPROM
uint8_t ShutterSpeedIndex =   EEPROM.read(ShutterSpeedAddr);
uint8_t ShutterOpen =   EEPROM.read(ShutterOpenAddr);
uint8_t ShutterClose =   EEPROM.read(ShutterCloseAddr);
uint8_t ShutterRelief =   EEPROM.read(ShutterReliefAddr);
uint8_t buttondelay =   EEPROM.read(ButtonDelayAddr);
uint8_t ServoDelay =   EEPROM.read(ServoDelayAddr);

int ShutterState = 0;  //closed
int selftimer = 0;      // self time in seconds
int adjustmenuitem = 1;  // selected adjust menu item

// Shutter speed values in seconds. .001 = TIME setting
float spvalues[] = {.001, .125, .150, .200, .250, .300, .400, .500, .600, .800, 1.000, 1.250, 1.500, 2.000, 2.500, 3.000, 4.000, 5.000, 6.000, 7.000, 8.000, 9, 10, 11, 12, 13, 14, 15.00, 20.00, 25.00, 30.00, 35.00, 40.00, 45.00, 60.00, 90.00, 120.0, 150.0, 180.0, 240.0, 300.0, 360.0, 420.0, 480.0, 540.0, 600.0};

// Store settings
void SaveSettings() {
  EEPROM.write(ShutterSpeedAddr, ShutterSpeedIndex);
  EEPROM.write(ShutterOpenAddr, ShutterOpen);
  EEPROM.write(ShutterCloseAddr, ShutterClose);
  EEPROM.write(ShutterReliefAddr, ShutterRelief);
  EEPROM.write(ButtonDelayAddr, buttondelay);
  EEPROM.write(ServoDelayAddr, ServoDelay);
}

// read button state
void readButtons() {
  PlusButtonState = digitalRead(PlusButtonPin);
  MinusButtonState = digitalRead(MinusButtonPin);
  ShutterButtonState = digitalRead(ShutterButtonPin);
  MenuButtonState = digitalRead(MenuButtonPin);
}

// menu routine
void menu() {
  
// Menu button is pressed, cycle through menus:
  if(MenuButtonState == 0) {
    if(mainscreen) {
      showtimermenu();
    } else if(selftimermenu) {
      showadjustmenu();
    } else {
      refresh();
      delay(buttondelay);
    } 
  }

// self timer menu active:    
  if(selftimermenu) {
    if(PlusButtonState == 0) {
      selftimer++;                        // increase self timer by 2 seconds
      selftimer++;      
    } else if (MinusButtonState == 0) {
      if (selftimer > 0) {
        selftimer--;                      // decrease self timer by 2 seconds
        selftimer--;
      }
    }
    if(PlusButtonState == 0 || MinusButtonState == 0) {
      showtimermenu();                    // show changes in self timer
    }
  }

// on main screen and a plus or minus button is pressed:
  if (mainscreen) {
    if(PlusButtonState == 0) {
     if(ShutterSpeedIndex >= MaxShutterIndex) {
       ShutterSpeedIndex = 0;
      } else {
       ShutterSpeedIndex++;               // Increase shutter speed time
     }
    refresh();                            // redisplay main screen
    SaveSettings();
   }
    if(MinusButtonState == 0) {
     if(ShutterSpeedIndex == 0) {
      ShutterSpeedIndex = MaxShutterIndex;
    } else {
     ShutterSpeedIndex--;               // decrease shutter speed time
    }
    refresh();
    SaveSettings();
   }  
  }

// Array of conditional which changes adjust menu items
  if(adjustmenu && PlusButtonState == 0 && adjustmenuitem == 1) {
      ShutterOpen++;
      clearadjustitem(1);
      oled.print(ShutterOpen);
  } 
  if(adjustmenu && PlusButtonState == 0 && adjustmenuitem == 2) {
      ShutterClose++;
      clearadjustitem(2);
      oled.print(ShutterClose);
  } 
  if(adjustmenu && PlusButtonState == 0 && adjustmenuitem == 3) {
      ShutterRelief++;
      clearadjustitem(3);
      oled.print(ShutterRelief);
  } 
  if(adjustmenu && PlusButtonState == 0 && adjustmenuitem == 4) {
      buttondelay = buttondelay + 10;
      clearadjustitem(4);
      oled.print(buttondelay);
  } 
  if(adjustmenu && PlusButtonState == 0 && adjustmenuitem == 5) {
      ServoDelay++;
      clearadjustitem(5);
      oled.print(ServoDelay);
  }   
  if(adjustmenu && MinusButtonState == 0 && adjustmenuitem == 1) {
      ShutterOpen--;
      clearadjustitem(1);
      oled.print(ShutterOpen);
  } 
  if(adjustmenu && MinusButtonState == 0 && adjustmenuitem == 2) {
      ShutterClose--;
      clearadjustitem(2);
      oled.print(ShutterClose);
  } 
  if(adjustmenu && MinusButtonState == 0 && adjustmenuitem == 3) {
      ShutterRelief--;
      clearadjustitem(3);
      oled.print(ShutterRelief);
  } 
  if(adjustmenu && MinusButtonState == 0 && adjustmenuitem == 4) {
      buttondelay = buttondelay - 10;
      clearadjustitem(4);
      oled.print(buttondelay);
  }
  if(adjustmenu && MinusButtonState == 0 && adjustmenuitem == 5) {
      ServoDelay--;
      clearadjustitem(5);
      oled.print(ServoDelay);
  }   

}

// clear and get ready to display adjust value
void clearadjustitem(int adjust) {
  oled.setCursor(94, adjust + 2);
  oled.print(F("    "));
  oled.setCursor(94, adjust + 2);
  SaveSettings();
  delay(200);
}

// display self timer menu
void showtimermenu() {
  mainscreen = false;
  selftimermenu = true;
  adjustmenu = false;
  oled.clear();
  printdivider(1);
  oled.setCursor(10, 0);
  oled.print(F("Self Timer:"));
  oled.set2X();
  oled.setCursor(25, 3);
  if(selftimer == 0) {            // self timer not active
    oled.print(F("OFF"));
  } else {
  oled.print(selftimer);
  oled.print(F(" sec"));
  }
  delay(buttondelay);             // button repeat rate
}

// display shutter adjust menu
void showadjustmenu() {
  mainscreen = false;
  selftimermenu = false;
  adjustmenu = true;
  oled.clear();
  printdivider(1);
  oled.setCursor(10, 0);
  oled.print(F("Shutter Adjust:"));
  oled.set1X();
  oled.setCursor(22, 3);
  oled.print(F("Open Angle: "));        // display the 5 items
  oled.print(ShutterOpen);
  oled.setCursor(16, 4);
  oled.print(F("Close Angle: "));
  oled.print(ShutterClose);
  oled.setCursor(10, 5);
  oled.print(F("Relief Angle: "));
  oled.print(ShutterRelief);
  oled.setCursor(10, 6);
  oled.print(F("Button Delay: "));
  oled.print(buttondelay);
  oled.setCursor(16, 7);
  oled.print(F("Servo Delay: "));
  oled.print(ServoDelay);
  oled.setCursor(0, adjustmenuitem + 2);    // display the asterisk beside the item selected
  oled.print(F("*"));
  delay(buttondelay);
}

void printdivider(int row) {                // prints a divider
  oled.set1X();
  oled.setCursor(0, row);
  int count = 1;
  while (count < 22) {
    oled.print(F("-"));
   count++;
  }
}

// Display main screen
void refresh() {
  mainscreen = true;
  selftimermenu = false;
  adjustmenu = false;
  float T = spvalues[ShutterSpeedIndex];      // get shutter speed from array
  uint8_t Tdisplay = 0;                       // Flag for shutter speed display style (fractional, seconds, minutes)
  double  Tfr = 0;                            // value for shutter speed fraction
  float   Tmin = 0;                           // shutter speed in minutes, if over 60 seconds
  if (T >= 60) {
    Tdisplay = 0;                             // Exposure is in minutes
    Tmin = T / 60;

  } else if (T < 60 && T >= 0.6) {            // speed in seconds
    Tdisplay = 2;                             // Exposure in in seconds

  } else if (T < 0.6 && T > .01) {            // speed in fractions
    Tdisplay = 1;                             // Display is in fractional form
    Tfr = round(1 / T);
  } else if ( T = .001) {
    Tdisplay = 3;                             // Exposure is TIME
  }
  // Start main display
  oled.clear();
  oled.set1X();
  oled.setCursor(10, 0);
  oled.print(F("Shutter Control:"));
  printdivider(1);
  oled.set1X();
  oled.setCursor(40, 2);
  oled.print(F("|"));
  oled.setCursor(40, 3);
  oled.print(F("|"));
  oled.setCursor(0, 2);
  oled.print(F("Shutter"));
  oled.setCursor(0, 3);
  oled.print(F("Speed"));
  oled.set2X();
  oled.set2X();
  oled.setCursor(50, 2);
  if (Tdisplay == 0) {                // display shutter speed
    oled.print(Tmin, 1);              // in minutes
    oled.print(F("m"));
  } 
  if (Tdisplay == 1) {                // or in fractions
   if (T > 0) {
     oled.print(F("1/"));
     oled.print(Tfr, 0);
   }
  }
  if (Tdisplay == 2) {                // or in seconds
    oled.print(T, 1);
    oled.print(F("s"));
  }
  if (Tdisplay == 3) {                // or TIME
    oled.print(("TIME"));
  }
  showshutterstate(0);                // shutter is closed and ready

  if(selftimer > 0) {                 // display self timer value if any
  oled.setCursor(40, 4);
  oled.print(F("|"));
  oled.setCursor(40, 5);
  oled.print(F("|"));    
    oled.setCursor(0, 4);
    oled.print(F("Self"));
    oled.setCursor(0, 5);
    oled.print(F("Timer"));
    oled.set2X();
    oled.setCursor(50, 4);
    oled.print(selftimer);
    oled.print(F(" sec"));
  }
  delay(buttondelay);
}

// display shutter state on bottom line
void showshutterstate(int expose) {
  printdivider(6);
  oled.setCursor(0, 7);
  oled.print(F("State: "));
  if(ShutterState == 0) {                 // 0 is shutter closed and ready to shoot
    oled.print(F("CLOSED--READY "));
  } else {
    oled.print(F("OPEN--"));              // if the shutter is open...
    if(expose == 1) {
      oled.print(F("EXPOSING"));          // 1 = shutter open for exposure
     } else {
        oled.print(F("FOCUS   "));        // any other number is shutter is open for focusing
      }
    }
  if(expose == 2) {                       // 2 is shutter is closed, but self time counting down
    oled.setCursor(0, 7);
    oled.print(F("State: COUNTING DOWN "));
  }
  if(expose == 3) {                       // 3 is shutter is open for focusing
    oled.setCursor(0, 7);
    oled.print(F("State: FOCUS         "));
  }
}

boolean useselftimer(int stimer) {        // self timer routine, sends back an abort flag if shutter is pressed
 boolean abort = false;
 if(stimer > 0) {
  showshutterstate(2);                    // display timer is counting down status
  oled.set2X();
  while(stimer > 0) {                     // stimer in seconds
    int countdown = 100;                  // for counting down 1 second
    oled.setCursor(0, 4);
    oled.print(F("          "));          // clear self timer value row
    oled.setCursor(0, 4);
    while(countdown > 0) {  
      oled.print(stimer);                 // display self timer value
      countdown = countdown - 10;         // reduce counter by 1/10
      delay(100);                         // wait 1/10 second
      readButtons();                      // read button state 
      if(ShutterButtonState == 0) {       // if the shutter button is pressed
        abort = true;                     // flag abort
        countdown = 0;                    // stop countdown
        stimer=0;                         // stop self timer
      }
    }
    stimer--;                             // reduce timer by 1 second
  }
  oled.setCursor(0, 4);
  oled.print(F("0000000000"));            // last count
 }
  return(abort);                          // return the abort flag
}

void setup() {
pinMode(PlusButtonPin, INPUT_PULLUP);         // setup buttons to pins
pinMode(MinusButtonPin, INPUT_PULLUP);
pinMode(ShutterButtonPin, INPUT_PULLUP);
pinMode(MenuButtonPin, INPUT_PULLUP);

Wire.begin();
myservo.attach(servoPin);
oled.begin(&Adafruit128x64, I2C_ADDRESS);
oled.setFont(Adafruit5x7);
myservo.write(ShutterOpen);                   // open shutter for viewing, focusing
delay(200);
myservo.write(ShutterOpen - ShutterRelief);  // back off to relieve stress on servo
ShutterState = 1;                             // shutter open

// load defaults if first time running
if (ShutterSpeedIndex > MaxShutterIndex) {
  ShutterSpeedIndex = defaultShutterSpeedIndex;
}
if (ShutterOpen > 200) {
  ShutterOpen = defaultShutterOpen;
}
if (ShutterClose > 90) {
  ShutterClose = defaultShutterClose;
}if (ShutterRelief > 20) {
  ShutterRelief = defaultShutterRelief;
}if (buttondelay > 250) {
  buttondelay = defaultButtonDelay;
}
if (ServoDelay > 250) {
  ServoDelay = defaultServoDelay;
}
refresh();

}
void loop() {

readButtons();                          // get button state
menu();                                 // do stuff with button press

// shutter button pressed during self timer menu
if(ShutterButtonState == 0 && selftimermenu) { 
    myservo.write(ShutterOpen);         // open the shutter for focus, viewing
    delay(200);
    readButtons();
    myservo.write(ShutterOpen - ShutterRelief);
    ShutterState = 1;
    showshutterstate(3);    // open shutter for focus
}

// shutter button pressed during adjust menu
if(ShutterButtonState == 0 && adjustmenu) {
  delay(300);                                 // wait enough time to release shutter button
  readButtons();                              // clear the buttons
  oled.setCursor(0, adjustmenuitem + 2);
  oled.print(F(" "));                         // clear adjust option item
  adjustmenuitem++;                           // go to next adjust menu item
  if(adjustmenuitem > 5) {                    // cycle back to first item 
    adjustmenuitem = 1;
  }
  oled.setCursor(0, adjustmenuitem + 2);
  oled.print(F("*"));                         // Item selection indicator
}

// shutter button pressed
if(ShutterButtonState == 0) {
  boolean abort = false;
  if(ShutterState == 1) {           // If the shutter is open, close it
    myservo.write(ShutterClose);
    delay(200);
    myservo.write(ShutterClose + ShutterRelief);
    ShutterState = 0;
    showshutterstate(ShutterState);
  } else {                   // If the shutter is closed
   abort = useselftimer(selftimer);  // go to self timer routine
   if (!abort) {                      // if self timer hasn't been aborted
   float shutterdelay = spvalues[ShutterSpeedIndex];
   shutterdelay = shutterdelay * 1000;  // time in milliseconds
   ShutterState = 1;
   showshutterstate(ShutterState);
   myservo.write(ShutterOpen);
    if(shutterdelay > 1000 ) {        // shutter relief is time is over a second
     delay(200);
     myservo.write(ShutterOpen - ShutterRelief);
    }
   if(shutterdelay > 1) {             // 1 = Timer mode
    delay(shutterdelay - ServoDelay);  // OPEN Shutter for exposure            
   } else {
    delay(500);       // wait until shutter button is released
    readButtons();   // clear button state
    while(ShutterButtonState) {   // as long as shutter button isn't pressed, keep open
      readButtons();              // wait here until shutter button pressed. 
    }
   }
   myservo.write(ShutterClose);
   delay(200);
   myservo.write(ShutterClose + ShutterRelief);
   ShutterState=0;
   showshutterstate(ShutterState);
  } 
  refresh();
  delay(1000);        // wait a second so the shutter can't open twice
 }
}
}
