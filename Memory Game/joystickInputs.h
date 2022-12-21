#include "EEPROM.h"
#include "Stream.h"
#include "HardwareSerial.h"
#include "Arduino.h"
// Joystick
const int joystickXPin = A0;
const int joystickYPin = A1;
const int joystickSWPin = 2;

const int minThreshold = 200;
const int maxThreshold = 600;

int joystickXValue = 0;
int joystickYValue = 0;
int shortPressInterval = 50;
int longPressInterval = 2000;

byte joystickSWValue = LOW;
byte lastJoystickSWValue = LOW;
byte joystickSWState = LOW;
byte joystickReading = LOW;
byte lastJoystickReading = HIGH;
byte joystickState = LOW;
byte longPressed = LOW;

unsigned long lastDebounceTime = 0;
unsigned long lastSWDebounceTime = 0;

bool joystickMoved = false;
bool canChangeLevel = false; // added to solve a bug in the settings menu
bool buttonPressed = false;

int nrMistakes = 0;
int nrClicks = 0;


void joystickInput() {
  // read the joystick button value
  joystickSWValue = digitalRead(joystickSWPin);
  if (joystickSWValue != lastJoystickSWValue) {
    lastSWDebounceTime = millis();
  } 
  if(millis() - lastDebounceTime > longPressInterval && systemState == 3) {
    if(joystickSWValue != longPressed) {
      longPressed = joystickSWValue;
      if(longPressed == LOW) {
        nrMistakes = 0;
        for (int row = 0; row < matrixSize; row++) {
          for (int col = 0; col < matrixSize; col++) {
            if (playMatrix[row][col] != generatedMatrix[row][col]) {
              nrMistakes++;
            }
          }
        }
        if (nrMistakes < 1) {
          score += 20;
        }
        else if (nrMistakes < 3) {
          score += 5;
        }
        level++;
        nrClicks = 0;
        playMatrix[xPosition][yPosition] = 0;
        updateLCDInGame();
        playMatrixChanged = true;
        generatedMatrixChanged = true;
        updateGame();
        resetPlayMatrix();           
        resetGeneratedMatrix();
        playMatrixChanged = false;
        generatedMatrixChanged = false;
        play(); 
        if (level > 5) {
          lcdFinalGame();
          matrixFinalGameAnimation();
          delay(3000);
          systemState = 0;
          menu();
          level = 0;
          lives = 3;          
        }       
      }
    }
  }
  else if ((millis() - lastSWDebounceTime) > shortPressInterval) {
    if (joystickSWValue != joystickSWState) {
      joystickSWState = joystickSWValue;
      if (joystickSWState == LOW) {
        if (currentMenuPosition == 0 && systemState != 3) {
          updateLCDInGame();
          lc.clearDisplay(0);          
          play();
        }
        if (systemState == 3) {
          buttonPressed = true;
          nrClicks++;
          if (nrMaxClicks - nrClicks == 0) {
            lives--; 
            updateLCDInGame();             
          }
          if (lives == 0) {
            lcdGameOver();
            matrixGameOverAnimation();
            delay(3000);
            systemState = 0;
            menu();
            level = 0;
            lives = 3;
          }          
        }
        if (currentMenuPosition == 1) {
          instructions();           
        }
        else if (currentMenuPosition == 3) {
          canChangeLevel = true;
          settings();           
          systemState = 1;
        }
        else if (currentMenuPosition == 4) {
          about();
        }
        if (systemState == 1) {
          if (currentSettingsPosition == 0) {
            lcdBrightness();
          }
          else if (currentSettingsPosition == 1) {
            matrixBrightness();
          }            
          else if (currentSettingsPosition == 2) {
            difficulty();
          }
          else if (currentSettingsPosition == 3) {
            backToMenu();
          }
        }
      }
    }
  } 
  lastJoystickSWValue = joystickSWValue;

  // getting the direction of the movement from the joystick
  joystickXValue = analogRead(joystickXPin);
  joystickYValue = analogRead(joystickYPin);

  if ((joystickXValue > minThreshold && joystickXValue < maxThreshold) || (joystickYValue > minThreshold && joystickYValue < maxThreshold)) {
    joystickReading = LOW;   
  }
  if((joystickXValue < minThreshold || joystickXValue > maxThreshold) || (joystickYValue < minThreshold || joystickYValue > maxThreshold)) {
    joystickReading = HIGH;   
  }
  if(joystickReading != lastJoystickReading) {
    lastDebounceTime = millis();
  }
  if (millis() - lastDebounceTime > shortPressInterval) {
    if(joystickReading != joystickState) {
      joystickState = joystickReading;
      if(joystickReading == HIGH) {
        joystickMoved=false;
      }
    }
  }
  
  // up movement
  if (joystickXValue > maxThreshold && joystickMoved == false) {
    joystickMoved = true;
    if (systemState == 0) {
      if (currentMenuPosition > firstMenuPosition) {
        currentMenuPosition--;
        menu();
      }
    }
    else if (systemState == 1) {
      if (currentSettingsPosition > firstSettingsPosition) {
        Serial.println("up in settings");
        canChangeLevel = false;
        settingsMatrixAnimation();
        currentSettingsPosition--;
        settings();
      }
    }
    else if (systemState == 3) {
      if (xPosition > 0) {
        xPosition--;
      }
      else {
        xPosition = matrixSize - 1;
      }
    }      
  } 
  // down movement      
  if (joystickXValue < minThreshold && joystickMoved == false) {
    joystickMoved = true;
    if (systemState == 0) {
      if (currentMenuPosition < lastMenuPosition) {
        currentMenuPosition++;
        menu();
      }
    }
    else if (systemState == 1) {
      if (currentSettingsPosition < lastSettingsPosition) {
        canChangeLevel = false;
        settingsMatrixAnimation();
        Serial.println("down in settings");
        currentSettingsPosition++;
        settings();
      }
    }
    else if (systemState == 3) {
      if (xPosition < matrixSize - 1) {
        xPosition++;
      }
      else {
        xPosition = 0;
      }
    }  
  }
  // right movement
  if (joystickYValue > maxThreshold && joystickMoved == false) {
    joystickMoved = true;
    if (systemState == 1 && canChangeLevel == true) {
      if (currentSettingsPosition == 0) {
        if (lcdBrightnessLevel < 3) {
          lcdBrightnessLevel++;
        }
        analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel]);
        EEPROM.update(lcdBrightnessAdress, lcdBrightnessLevel);
        lcdBrightnessLevel = EEPROM.read(lcdBrightnessAdress);
      }
      lcd.setCursor(7, 1);
      lcd.print(lcdBrightnessLevel);

      if (currentSettingsPosition == 1 && canChangeLevel == true) {
        if (matrixBrightnessLevel < 3) {
          matrixBrightnessLevel++;
        }
        lc.setIntensity(0, matrixBrightnessValues[matrixBrightnessLevel]);
        EEPROM.update(matrixBrightnessAdress, matrixBrightnessLevel);
        matrixBrightnessLevel = EEPROM.read(matrixBrightnessAdress);
        lcd.setCursor(7, 1);
        lcd.print(matrixBrightnessLevel);
      }      
      
      if (currentSettingsPosition == 2 && canChangeLevel == true) {
        if (difficultyLevel < 3) {
          difficultyLevel++;
        }
        EEPROM.update(difficultyAdress, difficultyLevel);
        difficultyLevel = EEPROM.read(difficultyAdress);
        lcd.setCursor(7, 1);
        lcd.print(difficultyLevel); 
      }     
    }
    if (systemState == 3) {
      if (yPosition < matrixSize - 1) {
        yPosition++;
      }
      else {
        yPosition = 0;
      }
    }
  } 
  // left movement
  if (joystickYValue < minThreshold && joystickMoved == false)  {
    joystickMoved = true;
    if (systemState == 1 && canChangeLevel == true) {
      if (currentSettingsPosition == 0) {
        if (lcdBrightnessLevel > 1) {
          lcdBrightnessLevel--;
        }
        analogWrite(lcdBrightnessPin, lcdBrightnessValues[lcdBrightnessLevel]);
        EEPROM.update(lcdBrightnessAdress, lcdBrightnessLevel);
        lcdBrightnessLevel = EEPROM.read(lcdBrightnessAdress);
      }
      lcd.setCursor(7, 1);    
      lcd.print(lcdBrightnessLevel);

      if (currentSettingsPosition == 1 && canChangeLevel == true) {
        if (matrixBrightnessLevel > 1) {
          matrixBrightnessLevel--;
        }
        lc.setIntensity(0, matrixBrightnessValues[matrixBrightnessLevel]);
        EEPROM.update(matrixBrightnessAdress, matrixBrightnessLevel);
        matrixBrightnessLevel = EEPROM.read(matrixBrightnessAdress);
        lcd.setCursor(7, 1);
        lcd.print(matrixBrightnessLevel);
      }
      
      if (currentSettingsPosition == 2 && canChangeLevel == true) {
        if (difficultyLevel > 1) {
          difficultyLevel--;
        }
        EEPROM.update(difficultyAdress, difficultyLevel);
        difficultyLevel = EEPROM.read(difficultyAdress);
        lcd.setCursor(7, 1);
        lcd.print(difficultyLevel);
      }
    }
    if (systemState == 3) {
      if (yPosition > 0) {
        yPosition--;
      }
      else {
        yPosition = matrixSize - 1;
      }
    }
  } 
  lastJoystickReading = joystickReading; 

  if (xPosition != lastXPosition || yPosition != lastYPosition) {
    if (buttonPressed == true) {
      playMatrixChanged = true;
      if (playMatrix[xPosition][yPosition] == 0) {
        playMatrix[xPosition][yPosition] = 1;
        buttonPressed = false;        
      }          
      else {
        playMatrix[xPosition][yPosition] = 0;
        buttonPressed = false;
      } 
    }
    else {
      playMatrixChanged = true;
      playMatrix[lastXPosition][lastYPosition] = 0;
      playMatrix[xPosition][yPosition] = 1;
    }
  }    
  lastXPosition = xPosition;
  lastYPosition = yPosition;
     
}
