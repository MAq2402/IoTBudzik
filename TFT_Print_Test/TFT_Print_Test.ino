/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin comnenctions are correct by
 editting the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */

#include<Keypad.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>




TFT_eSPI tft = TFT_eSPI();  // Invoke library
 
const byte n_rows = 4;
const byte n_cols = 3;
 
char keys[n_rows][n_cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte colPins[n_rows] = {3, 4, 5, 6};
byte rowPins[n_cols] = {0, 1, 2};
 
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols); 

void setup(void) {  
  tft.init();
  tft.setRotation(1);
  
  Serial.begin(115200);
}

void loop() {
  
  tft.fillScreen(TFT_RED);
  tft.setTextFont(7);
  tft.setTextSize(4);
  tft.setTextColor(TFT_BLUE);
  tft.setCursor(2, 2, 2);
  
 
 
   char myKey = myKeypad.getKey();
 
  if (myKey != NULL){
    tft.println(myKey);
  }

  
  //delay(1000);

 
  delay(10000);
}
