// OmniDice v1 by Rae O'Neil/Gamma Rae Design
// March 2025
// Uses accelerometer and hall effect sensors
// to provide both a tactile experience and 
// proper seed randomization to simulate the
// dice rolls of any face dice
// Coded to work on an Arduino Nano

/*
    Copyright 2025 Rae O'Neil

    Redistribution and use in source and binary forms, with or without modification, 
    are permitted provided that the following conditions are met:

    1) Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    2) Redistributions in binary form must reproduce the above copyright notice, this list 
    of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS 
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY 
    AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    USE OF THIS SOFTWARE, 3D MODELS, ASSETS OR OF THE OMNIDICE NAME AS ATTACHED TO THIS SOFTWARE OR PROJECT 
    IS FORBIDDEN TO ANYONE WHO IS DESCRIBED BY ANY OF THE BELOW UNTIL FURTHER REVISIONS BY RAE O'NEIL SPECIFY 
    OTHERWISE.

    1) Members of the US Republican Party/GOP
    2) Members of the Conservative Party of Canada(CPC) or People's Party of Canada(PPC)
    3) Members of the government of Israel while Palestine's statehood and territory are not 
       recognized and respected.
    4) Members of any social group or political group predecated on the subjugation, 
       harrassment, erasure or any form of violence against peopl based on race, national 
       or ethnic origin, colour, religion, age, sex, sexual orientation, marital status, 
       family status, disability, and a conviction for which a pardon has been granted or 
       a record suspended.
    5) Anyone who's net worth is above 1 billion US Dollars exchange rate adjusted to March 2025
    6) JK Rowling or Elon Musk specifically
    7) Anyone for whom the International Criminal Court(ICC) has a warrant or conviction

*/

#include <Adafruit_MPU6050.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "Fonts/FreeSerifBoldItalic12pt7b.h"
#include "Fonts/Font5x7Fixed.h"
#include "Fonts/Org_01.h"
#include "Fonts/FreeSerifBoldItalic12pt7b.h"
#include "Fonts/FreeSansBold9pt7b.h"
#include "Fonts/Picopixel.h"

const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Button pins
#define MODE_BTN 2 // select view on screen
#define DM_BTN 3 // decrement dice number by 1
#define DP_BTN 4 //  increment dice number by 1
#define FM_BTN 5 // decrement die faces by 1 
#define FP_BTN 6 // increment die faces by 1
#define FTM_BTN 7 // decrement die faces by 10
#define FTP_BTN 8 // increment die faces by 10
#define SHIFT_BTN 9 // if held, buttons become shortcuts
#define BTN_DELAY 200 // how long buttons need to cool down for
#define SHAKE_DELAY 400 // makes sure you need to actually shake it to set it off
#define HALLEFFECT1 A0 // Analog pin the first hall effect sensor is on
#define HALLEFFECT2 A1 // Analog pin the second hall effect sensor is on

// Uncomment for debugging
//#define Sprintln(a) (Serial.println(a))
//#define Sprint(a) (Serial.print(a))

// Comment for debugging
#define Sprintln(a)   
#define Sprint(a)   

unsigned long startmillis;
unsigned long buttonTracker[] = {0,0,0,0,0,0,0};
unsigned long currentMillis = 0;
byte shakeLoops = 0;
unsigned int total = 0;



byte shaking = 0;
unsigned long seed = 0.0;
unsigned long prevseed = 0.0;

const byte ha1= HALLEFFECT1; // Analog pin connected to the Analog Hall Effect Sensor module
const byte ha2= HALLEFFECT2;
const byte dplus = 2;
const byte dminus = 2;
byte faces = 6;
byte numdice = 1;
// 0 = detailed view
// 1 = only totals
// 3 = Magic 8Ball
byte displaymode = 1;

void setup() {
  millis();
  for(byte i =0; i < 7; i++){
    buttonTracker[i] = millis();
  }
  pinMode(ha1, INPUT); // Set the Hall Sensor pin as INPUT
  pinMode(ha2, INPUT); // Set the Hall Sensor pin as INPUT
  pinMode(MODE_BTN,INPUT_PULLUP); 
  pinMode(DP_BTN,INPUT_PULLUP); 
  pinMode(DM_BTN,INPUT_PULLUP); 
  pinMode(FP_BTN,INPUT_PULLUP); 
  pinMode(FM_BTN,INPUT_PULLUP); 
  pinMode(FTP_BTN,INPUT_PULLUP); 
  pinMode(FTM_BTN,INPUT_PULLUP); 
  pinMode(SHIFT_BTN,INPUT_PULLUP); 
  currentMillis = millis();
  Serial.begin(9600); // Initialize serial communication for debugging (optional)

  Wire.begin();                      // Initialize comunication
  Wire.beginTransmission(MPU);       // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);                  // Talk to the register 6B
  Wire.write(0x00);                  // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);        //end the transmission
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) 
  {
    Sprintln(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();

  // Give things time to init
  delay(1000);
  Sprintln("Booted!");

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setFont(&Org_01);
  display.setCursor(70, 7);
  display.println("Ready 2 Roll");
  display.display();
}

// This slows down button input so your values don't zoom around.
bool slowBtn(byte btn) {
  if(digitalRead(btn) == LOW) {
    if((unsigned long)(currentMillis - buttonTracker[(btn-4)]) >= BTN_DELAY) {
      buttonTracker[(btn-4)] = currentMillis;
      return true;
    }
    else {
      return false;
    }
  }
  return false;

}

void loop() {
  int ha1v = analogRead(ha1); // Read the analog value from the Analog Hall Effect Sensor
  int ha2v = analogRead(ha2);

// unsign that value if signed
// hall effects return negative 
// depending on polarity, so this
// makes sure the values are always positive
  if(ha1v < 0) {
    ha1v = ha1v * -1;
  }
  if(ha2v < 0) {
    ha2v = ha2v * -1;
  }
  Sprint("HA 1: ");
  Sprint(ha1v);
  Sprint(", HA 2: ");
  Sprint(ha2v);
  int buttonInt;
  readButtonStates();
  // get the time at the start of this loop()
  currentMillis = millis();

  // === Read acceleromter data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  //For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
  // Calculating Roll and Pitch from the accelerometer data
   // === Read gyroscope data === //
  //previousTime = currentTime;        // Previous time is stored before the actual time read
  //currentTime = millis();            // Current time actual time read
  //elapsedTime = (currentTime - previousTime) / 1000; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 4 registers total, each axis value is stored in 2 registers
  Sprint(", AccX: ");
  Sprint(AccX);
  Sprint(", AccY: ");
  Sprint(AccY);
  Sprint(", AccZ: ");
  Sprint(AccZ);
 

 // we're gonna unsign these too
  if(AccX < 0) {
    AccX *= -1;
  }
  if(AccY < 0) {
    AccY *= -1;
  }
  if(AccZ < 0) {
    AccZ *= -1;
  }

  // if there's sufficient X or Y acceleration, initiate a reroll
  // we don't want it going off on pickup, so Z ignored
  if(AccX >= 1.5 || AccY >= 1.5) {
    shakeLoops++;
    if(shakeLoops < 10) {
      Sprintln("Unsure if real shake.");
    }
    else {
      Sprintln("Real shake detected.");
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(0, 10);
    display.println("~SHAKE~~~~~~");
    display.setCursor(0, 21);
    display.println("~~~RATTLE~~~");
    display.setCursor(0, 31);
    display.println("~~~~~ROLL~~~");
    display.display();
    // gives a few loops before ending shake mode
    shaking = 10;
    // craft a seed value for randomseed
    seed = (seed+AccX+AccY+AccZ+(ha1v*0.1)+(ha2v*0.1))*(AccX);
    }
  }
  else {
    if(shaking > 0) {
      shaking -= 1;
    }
    else {
      shaking = 0;
      if(seed > 0) {

      randomSeed(seed);
      shakeLoops = 0;
      displaySelectedMode(true);
      
      }
      seed = 0.0;
      
    }
    if(seed > 0) {
      // used to keep track of seed if debugging
      prevseed = seed;
    }
    Sprint(", Seed: ");
    Sprint(prevseed);
    Sprintln("");
  }
  delay(10);
}

// screen shown when picking die number and faces
void diceSelectorDisplay() {

      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setFont(&FreeSerifBoldItalic12pt7b);
      display.setCursor(0, 20);
      display.setTextWrap(0);
      display.setCursor(27, 20);
      char ddisplay[13] = " ";
      sprintf(ddisplay,"%d x d%d", numdice, faces);
      display.println(ddisplay);
      display.display();

}


void displaySelectedMode(bool reroll) {

      switch (displaymode) {
        case 0:
          updateDiceValuesDetailed(numdice,faces, reroll);
          break;
        case 1:
          updateDiceValuesTotal(numdice,faces, reroll);
          break;
        default: 
          updateDiceValuesDetailed(numdice,faces, reroll);
          break;
      }

}

// menu for selecting detail or totals view
// some code here for a custom mode that would
// leverage an SD card but the poor nano ran
// out of memory. 
void displayMode() {
  switch(displaymode) {
    case 0:
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(0, 23);
        display.setTextWrap(0);
        display.setCursor(22, 23);
        display.println("~DETAIL~");
        display.setCursor(0, 5);
        display.setFont(&Org_01);
        display.println("MODE:");
        display.setFont(&Picopixel);
        display.setCursor(30, 5);
        display.println("DETAIL  TOTAL");
        display.drawRect(28, -1, 25, 10, 1);
        display.display();
        delay(1000);
        updateDiceValuesDetailed(numdice,faces, false);
        break;
    case 1:
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(0, 23);
        display.setTextWrap(0);
        display.setCursor(24, 23);
        display.println("~TOTAL~");
        display.setCursor(0, 5);
        display.setFont(&Org_01);
        display.println("MODE:");
        display.setFont(&Picopixel);
        display.setCursor(30, 5);
        display.println("DETAIL  TOTAL");
        display.drawRect(53, -1, 25, 10, 1);
        display.display();
        delay(1000);
        updateDiceValuesTotal(numdice,faces, false);
        break;    
  /*  case 2:
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(0, 23);
        display.setTextWrap(0);
        display.setCursor(16, 23);
        display.println("~SPECIAL~");
        display.setCursor(0, 5);
        display.setFont(&Org_01);
        display.println("MODE:");
        display.setFont(&Picopixel);
        display.setCursor(30, 5);
        display.println("DETAIL  TOTAL  SPECIAL");
        display.drawRect(78, -1, 29, 10, 1);
        display.display();
        delay(1000);
        updateDiceValuesDetailed(numdice,faces, false);
        break;   */
      default: 
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(0, 23);
        display.setTextWrap(0);
        display.setCursor(22, 23);
        display.println("~DETAIL~");
        display.setCursor(0, 5);
        display.setFont(&Org_01);
        display.println("MODE:");
        display.setFont(&Picopixel);
        display.setCursor(30, 5);
        display.println("DETAIL  TOTAL");
        display.drawRect(28, -1, 25, 10, 1);
        display.display();
        delay(1000);
        updateDiceValuesDetailed(numdice,faces, false);
        break;
  }

}



// each loop check what button is being pressed
// act accordingly 
void readButtonStates() {

  if(digitalRead(SHIFT_BTN) == HIGH) {
    if(slowBtn(MODE_BTN)) {
      if(displaymode >= 1) {
        displaymode = 0;
      }
      else {
        displaymode += 1;
      }
      displayMode();
    } 
    if(slowBtn(DP_BTN)) {
      if(numdice < 99) {
        numdice++;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    } 
    if(slowBtn(DM_BTN)) {
      if(numdice > 1) {
        numdice--;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    } 
    if(slowBtn(FP_BTN)) {
      if(faces < 254) {
        faces++;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    } 
    if(slowBtn(FM_BTN)) {
      if(faces > 2) {
        faces--;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    } 
    if(slowBtn(FTP_BTN)) {
      if(faces < 244) {
        faces = faces+10;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    } 
    if(slowBtn(FTM_BTN)) {
      if(faces > 11) {
        faces  = faces-10;
        diceSelectorDisplay();
      }
      else {
        diceSelectorDisplay();
      }
    }     
  }
  else {
    if(slowBtn(MODE_BTN)) {
      faces = 2;
      diceSelectorDisplay();
    } 
    if(slowBtn(DM_BTN)) {
      faces = 4;
      diceSelectorDisplay();
    } 
    if(slowBtn(DP_BTN)) {
      faces = 6;
      diceSelectorDisplay();
    } 
    if(slowBtn(FM_BTN)) {
      faces = 8;
      diceSelectorDisplay();
    } 
    if(slowBtn(FP_BTN)) {
      faces = 10;
      diceSelectorDisplay();
    } 
    if(slowBtn(FTM_BTN)) {
      faces = 12;
      diceSelectorDisplay();
    } 
    if(slowBtn(FTP_BTN)) {
      faces = 20;
      diceSelectorDisplay();
    }     
  }
}




void updateDiceDisplaySmall(unsigned int dice,unsigned int faces) {
    char dicestring[21] = "a x db";
    display.clearDisplay();
    display.drawLine(0, 9, 55, 9, 1);
    display.setTextSize(1);
    display.setFont(NULL);
    display.setCursor(0, 0);
    char diceprint[11] = " ";
    sprintf(diceprint,"%d x d%d",dice,faces);
    display.drawLine(60, 0, 55, 9, 1);
    display.println(diceprint);
    display.display();

}

void updateDiceDisplayBig(unsigned int dice,unsigned int faces) {

    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setFont(&Org_01);
    display.setCursor(5, 4);
    char diceprint[11] = " ";
    sprintf(diceprint,"%d x d%d",dice,faces);
    display.println(diceprint);
    display.drawLine(0, 6, 60, 6, 1);
    display.drawLine(65, 0, 60, 6, 1);
    display.display();

}

char dicerolls[200] = " ";

void rollDice(unsigned int dice, unsigned int faces) {
  Sprintln("Rerolling");
    total = 0;
    bool trunc = false;
    sprintf(dicerolls," ");
    unsigned int val = random(1,(faces+1));
    for (int i=0; i<(dice); i++) {
      delay(100);
      val = random(1,(faces+1));
      total = total + val;
      if(CountString(dicerolls) < 35){
        char valchar[10];
        sprintf(valchar,"%d.",val);
        
        strcat(dicerolls,valchar);
      }
      else {
        trunc = true;
      }
    }
    if(trunc) {
      strcat(dicerolls,"[...]");
    }
}


void updateDiceValuesTotal(unsigned int dice,unsigned int faces, bool reroll) {
   display.clearDisplay();
   updateDiceDisplayBig(dice, faces);
   char diceoutput[39];
   if(reroll) {
    rollDice(dice, faces);
   }

   display.setTextSize(1);
   display.setCursor(5, 30);
   display.setFont(&FreeSerifBoldItalic12pt7b);
   display.setTextWrap(0);
   display.setCursor(34, 30);
   display.println(total);
   display.display();

   
}

void updateDiceValuesDetailed(unsigned int dice,unsigned int faces, bool reroll) {
   display.clearDisplay();
   updateDiceDisplaySmall(dice, faces);
   if(reroll) {
    rollDice(dice, faces);
   }
   display.setCursor(65, 3);
   display.println(dicerolls);
   display.setCursor(0, 10);
   display.setTextWrap(1);
   display.drawLine(65, 20, 60, 32, 1);
   display.drawLine(65, 20, 128, 20, 1);
   display.setFont(&Font5x7Fixed);
   display.setTextSize(1);
   display.print("= ");
   display.print(total);
   display.display();
  
}
 // used to check if the detail view needs truncation
byte CountString( const char *c_Str )
  {
    byte b_Return = 0;
    while( c_Str[ b_Return ] != '\0' ) ++b_Return;
    return b_Return;
  }


char * diceprintout(int dice, int faces) {
   char * diceoutput = (char *) malloc(300);
   //char diceoutput[300];
   char dicechar[3];
   char facechar[6];
   itoa(dice,dicechar,10);
   itoa(faces,facechar,10);
   strcpy(diceoutput,dicechar);
   strcat(diceoutput,"d");
   strcat(diceoutput,facechar);
   strcat(diceoutput," |");
   char dicerolls[200] = " ";
   for (int i=0; i<(dice); i++) {
    unsigned int val = random(1,(faces+1));
    char valchar[6];
    sprintf(valchar,"< %d > ",val);
    strcat(dicerolls,valchar);
   }
   strcat(diceoutput,dicerolls);
   return diceoutput;
}



