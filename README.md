# REMEMBER

### Task Requirements

Create a game using 8x8 LED Matrix and LCD. The game must have a menu controlled by the joystick.

### Game Description
REMEMBER is a memory game in which the player must replicate the shapes that appear at the start of each level. There are 3 levels of difficulty:
- Level 1: the shapes are small and can spawn in a 4x4 square in the center of the matrix
- Level 2: the shapes are bigger and can spawn in a 4x8 rectangle
- Level 3: the shapes are the same size as in the second level but now the player has 3 lives. 

If the player presses the button for too many times during the game, he loses a life. If there are no lives left, the player loses the game
For each playthrough, there are 5 levels the player has to pass through. The score increases depending on how many mistakes the player makes (if he has no mistakes the score increases by 20, if he has less than 3 mistakes the score increases by 10).

### How to play
At the beginning of each level there is a 3 seconds timer. After this timer the generated shape appears and stays on the matrix for 3 seconds (for difficulty level 1) and for 2 seconds (for difficulty level 2 and 3). Then, the player can move the joystick to control a point on the matrix. Using this point, the player tries to replicate the shape, pressing the joystick button to light up the points he wants. If the player moves over a point that is already on, it will turn off so the player doesn't waste the button presses (they count for the third level of difficulty).
To go to the next level, the player has to press the joystick button for a longer period.

### Components used
- 1x 8x8 LED Matrix
- 1x MAX7219 Matrix driver
- 1x 16x2 LCD
- 1x electrolytic capacitor(10 μF)
- 1x ceramic capacitor(104 pF)
- wires
- resistors

### Bugs
- When the menu is first opened and the user scrolls down it jumps to the third menu option, not the second as it should
- When the users opens the settings menu, the first option is immediately opened (it should only open the settings menu)
- Sometimes the matrix and the lcd do not show what they should (probably because of system noise)

### Picture of the setup
![Matrix Game Setup](https://user-images.githubusercontent.com/34553466/208913165-209e8d35-c3d2-4d73-91a4-3affd80c224a.jpg)
