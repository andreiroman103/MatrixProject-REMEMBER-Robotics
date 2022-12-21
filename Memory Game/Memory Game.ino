#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include "menu.h"
#include "joystickInputs.h"

byte lcdContrast = 130;


void setup() {
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.clearDisplay(0);// clear screen
  pinMode(RS, OUTPUT);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(joystickSWPin, INPUT_PULLUP);
  lcdBrightnessLevel = EEPROM.read(lcdBrightnessAdress);
  matrixBrightnessLevel = EEPROM.read(matrixBrightnessAdress);
  difficultyLevel = EEPROM.read(difficultyAdress);  
  analogWrite(lcdContrastPin, lcdContrast);
  analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel]);
  lc.setIntensity(0,matrixBrightnessValues[matrixBrightnessLevel]);
  randomSeed(analogRead(unconnectedPin));
  initialAnimation();
  menu();
}

void loop() { 
  joystickInput();
  if (systemState == 3) {
    updateGame();
  }  
}
