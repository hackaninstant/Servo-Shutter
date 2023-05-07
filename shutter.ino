
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
#define MenuButtonPin           6

boolean PlusButtonState;                // "+" button state
boolean MinusButtonState;               // "-" button state
boolean ShutterButtonState;            // Metering button state
boolean MenuButtonState;

boolean mainscreen = true;
boolean selftimermenu = false;
boolean adjustmenu = false;

// EEPROM for memory recording
#define ShutterSpeedAddr        1
#define ShutterOpenAddr         2
#define ShutterCloseAddr        3
#define ShutterReliefAddr       4
#define ButtonDelayAddr         5

#define defaultShutterOpen      68
#define defaultShutterClose     0
#define defaultShutterRelief    4
#define defaultButtonDelay      100
#define defaultShutterSpeedIndex 10
#define MaxShutterIndex 45

uint8_t ShutterSpeedIndex =   EEPROM.read(ShutterSpeedAddr);
uint8_t ShutterOpen =   EEPROM.read(ShutterOpenAddr);
uint8_t ShutterClose =   EEPROM.read(ShutterCloseAddr);
uint8_t ShutterRelief =   EEPROM.read(ShutterReliefAddr);
uint8_t buttondelay =   EEPROM.read(ButtonDelayAddr);

// int ShutterOpen = 68;
// int ShutterClose = 0;
// int ShutterRelief = 4; // angle to back off to relieve stress on servo
// int buttondelay = 100;  // 
int ShutterState = 0;  //closed
int selftimer = 0;      // self time in seconds
int adjustmenuitem = 1;  // 

float spvalues[] = {.001, .125, .150, .200, .250, .300, .400, .500, .600, .800, 1.000, 1.250, 1.500, 2.000, 2.500, 3.000, 4.000, 5.000, 6.000, 7.000, 8.000, 9, 10, 11, 12, 13, 14, 15.00, 20.00, 25.00, 30.00, 35.00, 40.00, 45.00, 60.00, 90.00, 120.0, 150.0, 180.0, 240.0, 300.0, 360.0, 420.0, 480.0, 540.0, 600.0};

void SaveSettings() {
  EEPROM.write(ShutterSpeedAddr, ShutterSpeedIndex);
  EEPROM.write(ShutterOpenAddr, ShutterOpen);
  EEPROM.write(ShutterCloseAddr, ShutterClose);
  EEPROM.write(ShutterReliefAddr, ShutterRelief);
  EEPROM.write(ButtonDelayAddr, buttondelay);
}

void readButtons() {
  PlusButtonState = digitalRead(PlusButtonPin);
  MinusButtonState = digitalRead(MinusButtonPin);
  ShutterButtonState = digitalRead(ShutterButtonPin);
  MenuButtonState = digitalRead(MenuButtonPin);
}

void menu() {
  
// Menu button is pressed:
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
      selftimer++;
      selftimer++;      
    } else if (MinusButtonState == 0) {
      if (selftimer > 0) {
        selftimer--;
        selftimer--;
      }
    }
    if(PlusButtonState == 0 || MinusButtonState == 0) {
      showtimermenu();
    }
  }

// on main screen and a plus or minus button is pressed:
  if (mainscreen) {
    if(PlusButtonState == 0) {
     if(ShutterSpeedIndex >= MaxShutterIndex) {
       ShutterSpeedIndex = 0;
      } else {
       ShutterSpeedIndex++;
     }
    refresh();
    SaveSettings();
   }
    if(MinusButtonState == 0) {
     if(ShutterSpeedIndex == 0) {
      ShutterSpeedIndex = MaxShutterIndex;
    } else {
     ShutterSpeedIndex--;
    }
    refresh();
    SaveSettings();
   }  
  }

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
}

void clearadjustitem(int adjust) {
  oled.setCursor(90, adjust + 2);
  oled.print(F("    "));
  oled.setCursor(90, adjust + 2);
  SaveSettings();
  delay(200);
}

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
  if(selftimer == 0) {
    oled.print(F("OFF"));
  } else {
  oled.print(selftimer);
  oled.print(F(" sec"));
  }
  delay(buttondelay);
}

void showadjustmenu() {
  mainscreen = false;
  selftimermenu = false;
  adjustmenu = true;
  oled.clear();
  printdivider(1);
  oled.setCursor(10, 0);
  oled.print(F("Shutter Adjust:"));
  oled.set1X();
  oled.setCursor(20, 3);
  oled.print(F("Open Angle: "));
  oled.print(ShutterOpen);
  oled.setCursor(15, 4);
  oled.print(F("Close Angle: "));
  oled.print(ShutterClose);
  oled.setCursor(10, 5);
  oled.print(F("Relief Angle: "));
  oled.print(ShutterRelief);
  oled.setCursor(10, 6);
  oled.print(F("Button Delay: "));
  oled.print(buttondelay);
  oled.setCursor(0, adjustmenuitem + 2);
  oled.print(F("*"));
  delay(buttondelay);
}

void printdivider(int row) {
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
  float T = spvalues[ShutterSpeedIndex];
  uint8_t Tdisplay = 0; // Flag for shutter speed display style (fractional, seconds, minutes)
  double  Tfr = 0;
  float   Tmin = 0;
  if (T >= 60) {
    Tdisplay = 0;  // Exposure is in minutes
    Tmin = T / 60;

  } else if (T < 60 && T >= 0.6) {
    Tdisplay = 2;  // Exposure in in seconds

  } else if (T < 0.6 && T > .01) {
    Tdisplay = 1;  // Exposure is in fractional form
    Tfr = round(1 / T);
  } else if ( T = .001) {
    Tdisplay = 3;  // Exposure is T
  }
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
  if (Tdisplay == 0) {
    oled.print(Tmin, 1);
    oled.print(F("m"));
  } 
  if (Tdisplay == 1) {
   if (T > 0) {
     oled.print(F("1/"));
     oled.print(Tfr, 0);
   }
  }
  if (Tdisplay == 2) {
    oled.print(T, 1);
    oled.print(F("s"));
  }
  if (Tdisplay == 3) {
    oled.print(("TIME"));
  }
  showshutterstate(0);

  if(selftimer > 0) {
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
void showshutterstate(int expose) {
  printdivider(6);
  oled.setCursor(0, 7);
  oled.print(F("State: "));
  if(ShutterState == 0) {
    oled.print(F("CLOSED--READY "));
  } else {
    oled.print(F("OPEN--"));
    if(expose == 1) {
      oled.print(F("EXPOSING"));
     } else {
        oled.print(F("FOCUS   "));
      }
    }
  if(expose == 2) {
    oled.setCursor(0, 7);
    oled.print(F("State: COUNTING DOWN "));
  }
  if(expose == 3) {
    oled.setCursor(0, 7);
    oled.print(F("State: FOCUS         "));
  }
}

boolean useselftimer(int stimer) {
 boolean abort = false;
 if(stimer > 0) {
  showshutterstate(2);  // timer activated
  oled.set2X();
  while(stimer > 0) {
    int countdown = 100;
    oled.setCursor(0, 4);
    oled.print(F("          "));
    oled.setCursor(0, 4);
    while(countdown > 0) {
      oled.print(stimer);
      countdown = countdown - 10;
      delay(100);
      readButtons();
      if(ShutterButtonState == 0) {
        abort = true;
        countdown = 0;
        stimer=0;
      }
    }
    // delay(1000);
    stimer--;
  }
  oled.setCursor(0, 4);
  oled.print(F("0000000000"));
 }
  return(abort);
}

void setup() {
pinMode(PlusButtonPin, INPUT_PULLUP);
pinMode(MinusButtonPin, INPUT_PULLUP);
pinMode(ShutterButtonPin, INPUT_PULLUP);
pinMode(MenuButtonPin, INPUT_PULLUP);

Wire.begin();
myservo.attach(servoPin);
oled.begin(&Adafruit128x64, I2C_ADDRESS);
oled.setFont(Adafruit5x7);
myservo.write(ShutterOpen);
delay(200);
myservo.write(ShutterOpen - ShutterRelief);  // back off to relieve stress on servo
ShutterState = 1;  // shutter open

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
refresh();

}
void loop() {

readButtons();
menu();

if(ShutterButtonState == 0 && selftimermenu) {  // if the shutter button is pressed on self timer menu
    myservo.write(ShutterOpen);
    delay(200);
    readButtons();
    myservo.write(ShutterOpen - ShutterRelief);
    ShutterState = 1;
    showshutterstate(3);    // open shutter for focus
}

if(ShutterButtonState == 0 && adjustmenu) {
  delay(300);
  readButtons(); // clear the buttons
  oled.setCursor(0, adjustmenuitem + 2);
  oled.print(F(" "));
  adjustmenuitem++;
  if(adjustmenuitem > 4) {
    adjustmenuitem = 1;
  }
  oled.setCursor(0, adjustmenuitem + 2);
  oled.print(F("*"));
}

if(ShutterButtonState == 0) {
  boolean abort = false;
  if(ShutterState == 1) {           // If the shutter is open, close it
    myservo.write(ShutterClose);
    delay(200);
    myservo.write(ShutterClose + ShutterRelief);
    ShutterState = 0;
    showshutterstate(ShutterState);
  } else {                   // If the shutter is closed
   abort = useselftimer(selftimer);
   if (!abort) {
   float shutterdelay = spvalues[ShutterSpeedIndex];
   shutterdelay = shutterdelay * 1000;
   ShutterState = 1;
   showshutterstate(ShutterState);
   myservo.write(ShutterOpen);
    if(shutterdelay > 1000 ) {
     delay(200);
     myservo.write(ShutterOpen - ShutterRelief);
    }
   if(shutterdelay > 1) {
    delay(shutterdelay);
   } else {
    delay(500);
    readButtons();   // give a chance to release the shutter button
    while(ShutterButtonState) {
      readButtons();
    }
   }
   myservo.write(ShutterClose);
   delay(200);
   myservo.write(ShutterClose + ShutterRelief);
   ShutterState=0;
   showshutterstate(ShutterState);
  } 
  refresh();
  delay(1000);
 }
}
}