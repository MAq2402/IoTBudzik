/* @file CustomKeypad.pde
|| @version 1.0
|| @author Alexander Brevig
|| @contact alexanderbrevig@gmail.com
||
|| @description
|| | Demonstrates changing the keypad size and key values.
|| #
*/
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //four columns
//define the cymbols on the buttons of the keypads
char keys[ROWS][COLS] = {
 {'1','2','3'},
 {'4','5','6'},
 {'7','8','9'},
 {'*','0','#'}
};
//byte rowPins[ROWS] = {12, 14, 27, 25}; //connect to the row pinouts of the keypad
///byte colPins[COLS] = {2, 15, 13}; //connect to the column pinouts of the keypad

byte rowPins[ROWS] = {12, 14, 27, 25}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {26, 15, 13}; //connect to the column pinouts of the keypad

//byte rowPins[ROWS] = {24, 27, 14, 12}; //connect to the row pinouts of the keypad
//byte colPins[COLS] = {13, 15, 2}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Serial.begin(9600);
}
  
void loop(){
  char customKey = customKeypad.getKey();
  
  if (customKey != NO_KEY){
    Serial.println(customKey);
  }
}
