#include "HardwareSerial.h"
#include "Arduino.h"

const byte unconnectedPin = A3; // used for the random seed
// LCD
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 2;
const byte d6 = 5;
const byte d7 = 4;
const byte lcdContrastPin = 3;
const byte lcdBrightnessPin = 6;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

const int lcdColumns = 16;
const int lcdRows = 2;

// arrows to be displayed on the lcd
const byte menuArrows[][8] = {
{ // up arrow
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B01110,
  B01110,
  B01110
},{
  // down arrow
  B01110,
  B01110,
  B01110,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100 
},{
  // right arrow
  B10000,
  B01000,
  B00100,
  B00011,
  B00011,
  B00100,
  B01000,
  B10000
},{
  // left arrow
  B00001,
  B00010,
  B00100,
  B11000,
  B11000,
  B00100,
  B00010,
  B00001
}
};

int upArrow = 0;
int downArrow = 1;
int rightArrow = 2;
int leftArrow = 3;

// Matrix 
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER

byte generatedMatrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};
byte playMatrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0}, 
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}  
};

// matrix shapes for different options in the menu
const byte menuShape[8] = {
  0b11111111,
  0b10000001,
  0b10111101,
  0b10100101,
  0b10100101,
  0b10111101,
  0b10000001,
  0b11111111
};

const byte playShape[8] = {
  0b00110000,
  0b00111000,
  0b00111100,
  0b00111110,
  0b00111110,
  0b00111100,
  0b00111000,
  0b00110000
};

const byte instructionsShape[8] = {
  0b00111100,
  0b00111100,
  0b00100100,
  0b00000100,
  0b00001000,
  0b00001000,
  0b00000000,
  0b00001000
};

const byte highscoreShape[8] = {
  0b00100100,
  0b00100100,
  0b01100110,
  0b01100110,
  0b00111100,
  0b00011000,
  0b00011000,
  0b00111100
};

const byte settingsShape[8] = {
  0b00111110,
  0b01111110,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000,
  0b00011000
};

const byte digit1[8] = {
  0b00010000,
  0b00110000,
  0b00010000,
  0b00010000,
  0b00010000,
  0b00010000,
  0b00010000,
  0b00111000
};

const byte digit2[8] = {
  0b00111000,
  0b01000100,
  0b00000100,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01111100
};

const byte digit3[8] = {
  0b00111000,
  0b01000100,
  0b00000100,
  0b00011000,
  0b00000100,
  0b00000100,
  0b01000100,
  0b00111000
};

const byte finalGame[8] = {
  0b00111100,
  0b01000010,
  0b10000001,
  0b10000101,
  0b10101001,
  0b10010001,
  0b01000010,
  0b00111100
};

const byte gameOver[8] = {
  0b00111100,
  0b01000010,
  0b10100101,
  0b10011001,
  0b10011001,
  0b10100101,
  0b01000010,
  0b00111100
};

// EEPROM
const int lcdBrightnessAdress = 0;
const int matrixBrightnessAdress = 1;
const int difficultyAdress = 2;

int lcdBrightnessLevel = 1;
int lcdBrightnessValues[] = {0, 60, 130, 200};

int matrixBrightnessLevel = 1;
int matrixBrightnessValues[] = {0, 1, 4, 7};

int difficultyLevel = 1;

int menuPosition = 0; // 0 => PLAY, 1 => INSTRUCTIONS, 2 => HIGHSCORE, 3 => SETTINGS, 4 => ABOUT 
int settingsPosition = 0; // 0 => LCD Brightness, 1 => Matrix brightness, 2 => Difficulty, 3 => Back to menu
const int firstMenuPosition = 0;
const int lastMenuPosition = 4;
int currentMenuPosition = 0;
const int firstSettingsPosition = 0;
const int lastSettingsPosition = 3;
int currentSettingsPosition = 0;
int systemState = 0; // 0 => in menu, 1 => in settings, 2 => in game, 3 => during gameplay

void initialMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, true); // turns on LED at col, row
      delay(25);
    }
  }
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, false); // turns off LED at col, row
      delay(25);
    }
  }
}

// animations
void initialAnimation() {
  lcd.begin(lcdColumns, lcdRows);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("REMEMBER");
  lcd.setCursor(0, 1);
  lcd.print("Test your memory");
  initialMatrixAnimation();
}

// matrix animations for menu
void playMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, playShape[row]);
  }   
}

void instructionsMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, instructionsShape[row]);
  }   
}

void highscoreMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, highscoreShape[row]);
  }   
}

void settingsMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, settingsShape[row]);
  }   
}

void aboutMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, menuShape[row]);
  }  
}

void fullMatrix() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, true); // turns on LED at col, row
    }
  }
}

void startGameMatrixAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, digit3[row]);
  }
  delay(1000);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, digit2[row]);
  } 
  delay(1000);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, digit1[row]);
  } 
  delay(1000);
}

void menu() {
  lcd.createChar(1, menuArrows[downArrow]);
  lcd.createChar(2, menuArrows[upArrow]);
  
  lcd.clear();
  lcd.setCursor(4, 0); 
  lcd.print("MAIN MENU");

  if (systemState == 0) {
    if (currentMenuPosition == 0) {
      playMatrixAnimation();
      lcd.setCursor(0, 1);
      lcd.print("PLAY");
      
      lcd.setCursor(15, 1);
      lcd.write(1);     
    }    
    else if (currentMenuPosition == 1) {
      instructionsMatrixAnimation();
      lcd.setCursor(0, 1);
      lcd.print("INSTRUCTIONS");
      
      lcd.setCursor(15, 0);    
      lcd.write(2);
      lcd.setCursor(15, 1);
      lcd.write(1);
    }
    else if (currentMenuPosition == 2) {
      highscoreMatrixAnimation();    
      lcd.setCursor(0, 1);
      lcd.print("HIGHSCORE");
      
      lcd.setCursor(15, 0);    
      lcd.write(2);
      lcd.setCursor(15, 1);
      lcd.write(1);      
    }
    else if (currentMenuPosition == 3) {
      settingsMatrixAnimation();
      lcd.setCursor(0, 1);
      lcd.print("SETTINGS");
      
      lcd.setCursor(15, 0);    
      lcd.write(2);
      lcd.setCursor(15, 1);
      lcd.write(1);  
    }
    else if (currentMenuPosition == 4) {
      aboutMatrixAnimation();
      lcd.setCursor(0, 1);
      lcd.print("ABOUT");
      
      lcd.setCursor(15, 1);    
      lcd.write(2); 
    }
  }
}

void about() {
  int scrollDelay = 400;
  int remainingText = 24;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Title: REMEMBER"); 
  lcd.setCursor(1, 1);
  lcd.print("Author: Roman Andrei,Git:andreiroman103"); 

  for (int i = 0; i < remainingText; i++) { 
    lcd.scrollDisplayLeft();
    delay(scrollDelay);
  } 
  delay(scrollDelay);
  menuPosition = 4; 
  menu();
}

void instructions() {
  int scrollDelay = 400;
  int remainingText = 24;  

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Pay attention to the shape that appears"); 
  for (int i = 0; i < remainingText; i++) { 
    lcd.scrollDisplayLeft();
    delay(scrollDelay);
  }  
  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("      Draw the shape using the joystick"); 
  for (int i = 0; i < remainingText; i++) { 
    lcd.scrollDisplayLeft();
    delay(scrollDelay);
  }
  delay(scrollDelay);
  menuPosition = 1; 
  menu();  
}

void highscore() { 
}

void settings() {
  systemState = 1;
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("SETTINGS");
  
  if (systemState == 1) {
    if (currentSettingsPosition == 0) {
      lcd.setCursor(0, 1);
      lcd.print("LCD Brightness");
      
      lcd.setCursor(15, 1);
      lcd.write(1);
    } 
    else if (currentSettingsPosition == 1) {
      lcd.setCursor(0, 1);
      lcd.print("Matrix bright");
      
      lcd.setCursor(15, 0);    
      lcd.write(2);
      lcd.setCursor(15, 1);
      lcd.write(1);
    }
    else if (currentSettingsPosition == 2) {  
      lcd.setCursor(0, 1);
      lcd.print("Difficulty");
      
      lcd.setCursor(15, 0);    
      lcd.write(2);
      lcd.setCursor(15, 1);
      lcd.write(1);      
    }
    else if (currentSettingsPosition == 3) {
      lcd.setCursor(0, 1);
      lcd.print("Back to menu");
      
      lcd.setCursor(15, 1);    
      lcd.write(2);  
    } 
  } 
}

void lcdBrightness() {  
  lcd.createChar(3, menuArrows[rightArrow]);
  lcd.createChar(4, menuArrows[leftArrow]);
  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("LCD Brightness");
  
  lcd.setCursor(5, 1);
  lcd.write(4); 

  lcd.setCursor(7, 1);
  lcd.print(lcdBrightnessLevel);

  lcd.setCursor(9, 1);
  lcd.write(3);
}

void matrixBrightness() {
  lcd.createChar(3, menuArrows[rightArrow]);
  lcd.createChar(4, menuArrows[leftArrow]);
  
  fullMatrix();
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Matrix Bright");
  
  lcd.setCursor(5, 1);
  lcd.write(4); 

  lcd.setCursor(7, 1);
  lcd.print(matrixBrightnessLevel);

  lcd.setCursor(9, 1);
  lcd.write(3);  
}

void difficulty() {
  lcd.createChar(3, menuArrows[rightArrow]);
  lcd.createChar(4, menuArrows[leftArrow]);
  
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Difficulty");
  
  lcd.setCursor(5, 1);
  lcd.write(4); 

  lcd.setCursor(7, 1);
  lcd.print(difficultyLevel);

  lcd.setCursor(9, 1);
  lcd.write(3);    
}

void backToMenu() {
  systemState = 0;
  currentMenuPosition = 3;
  currentSettingsPosition = 0;
  menu();    
}

void updateGeneratedMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, generatedMatrix[row][col]);
    }
  }
}

void resetGeneratedMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      generatedMatrix[row][col] = 0;
    }
  }  
}

void updatePlayMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, playMatrix[row][col]);
    }
  }
}

void resetPlayMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, false);
      playMatrix[row][col] = 0;
    }
  }  
}

byte xPosition = 0;
byte yPosition = 0;
byte lastXPosition = 0;
byte lastYPosition = 0;
int randomXPosition = 0;
int randomYPosition = 0;
int lowerXBound = 0;
int upperXBound = 8;
int lowerYBound = 0;
int upperYBound = 8;
bool generatedMatrixChanged = false;
bool playMatrixChanged = false;
int shapeShowTime = 0;
int nrMaxClicks = 10; // the maximum number of button presses you can use for each level on the 3rd difficulty

void generateShape() {
  int generatedPoints = 0;
  int shapeSize = 0;
  int row = 0;
  int col = 0;
  int rowLimit = 0;
  int colLimit = 0;
  if (difficultyLevel == 1) {
    lowerXBound = 2;
    upperXBound = 6;
    lowerYBound = 2;
    upperYBound = 6;
    shapeSize = 8;
    row = 2;
    col = 2;
    rowLimit = 6;
    colLimit = 6; 
    shapeShowTime = 3000;   
  }
  else if (difficultyLevel == 2) {
    lowerXBound = 0;
    upperXBound = 8;
    lowerYBound = 2;
    upperYBound = 6;
    shapeSize = 8;
    row = 0;
    col = 2;
    rowLimit = 8;
    colLimit = 6;
    shapeShowTime = 2000;     
  }
  else if (difficultyLevel == 3) {
    lowerXBound = 0;
    upperXBound = 8;
    lowerYBound = 2;
    upperYBound = 6;
    shapeSize = 8;
    row = 0;
    col = 2;
    rowLimit = 8;
    colLimit = 6;
    shapeShowTime = 2000;     
  }
  for (int i = row; i < rowLimit; i++) {
    for (int j = col; j < colLimit; j++) {
      if (generatedPoints != shapeSize) {
        randomXPosition = random(lowerXBound, upperXBound);
        randomYPosition = random(lowerYBound, upperYBound);
        generatedMatrix[randomXPosition][randomYPosition] = 1;
        generatedMatrixChanged = true;
        generatedPoints++;
      }      
    }
  }
}

void updateGame() {
  if (generatedMatrixChanged == true) {
    updateGeneratedMatrix();
    generatedMatrixChanged = false;    
  }
  if (playMatrixChanged == true) {
    updatePlayMatrix();
  }
}

int score = 0;
int level = 1;
int lives = 3;

void updateLCDInGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level:");
  lcd.print(level);
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);
  lcd.setCursor(10, 1);
  lcd.print("Diff:");
  lcd.print(difficultyLevel);
  if (difficultyLevel == 3) {
    lcd.setCursor(9, 0);
    lcd.print("Lives:");
    lcd.print(lives);
  }
}

void lcdFinalGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Congratulations");
  lcd.setCursor(4, 1);
  lcd.print("Score:");
  lcd.print(score);  
}

void matrixFinalGameAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, finalGame[row]);
  }
}

void lcdGameOver() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("You lost");  
}

void matrixGameOverAnimation() {
  lc.clearDisplay(0);
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, gameOver[row]);
  }  
}

void play() {
  if (level <= 5) { 
    startGameMatrixAnimation();
    generateShape();
    updateGame();
    delay(shapeShowTime);
    resetPlayMatrix();
    updateGame();
    systemState = 3;
  }
}

