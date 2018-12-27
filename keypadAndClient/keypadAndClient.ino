/*
 *  This sketch sends a message to a TCP server
 *
 */

#include <WiFi.h>
#include <WiFiMulti.h>

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

WiFiMulti WiFiMulti;

void setup()
{
    Serial.begin(115200);
    delay(10);

    // We start by connecting to a WiFi network
    WiFiMulti.addAP("TP-LINK_232B", "12345678");

    Serial.println();
    Serial.println();
    Serial.print("Wait for WiFi... ");

    while(WiFiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    delay(500);
}
String code = "";

void loop()
{
    const uint16_t port = 80;
    const char * host = "192.168.0.101"; // ip or dns



    //Serial.print("connecting to ");
    //Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClient client;

    /*if (!client.connect(host, port)) {
        Serial.println("connection failed");
        Serial.println("wait 5 sec...");
        delay(5000);
        return;
    }*/

    char customKey = customKeypad.getKey();
    
  
    if (customKey != NO_KEY){
      if(customKey == '#'){
         client.connect(host, port);
          client.println(code);
          client.stop();
          Serial.println("Info has been sent");  
        }
        else{
          code += customKey;
          }
      Serial.println(customKey);      
    }



    Serial.println("Current code: " + code);
    // This will send the request to the server
    //client.print("Send this data to server");
    
    //read back one line from server
    //String line = client.readStringUntil('\r');
    //Serial.println("Current code: " + code);
    //client.println(line);

    //Serial.println("closing connection");
    //client.stop();

    //Serial.println("wait 5 sec...");
    //delay(5000);
}
