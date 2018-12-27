/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>
WiFiServer wifiServer(80);

#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WebServer.h>
#include <WiFiUdp.h>

const char* ssid = "TP-LINK_232B";
const char* password =  "12345678";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Set web server port number to 80
WebServer server(80);

String header;

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;
String timeWithoutSeconds;
String timeOfAlarm = "Nie ustawiono";


TFT_eSPI tft; 

void setTft()
{
  tft = TFT_eSPI();  // Invoke library
  tft.init();
  tft.setRotation(1);
  tft.setTextFont(7);
  tft.setTextSize(2);
  tft.setTextColor(TFT_BLUE);
}
void setWiFi()
{
  WiFi.begin(ssid, password);
  
}
void setTimeClient()
{
  timeClient.begin();
   //Set to our timezone
   timeClient.setTimeOffset(3600);
}
void handleRoot(){
  String temp = "<html>\
  <head>\
  <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>\
    <meta http-equiv='refresh' content='5'/>\
    <title>IoT Budzik</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
  <h1>IoT Budzik</h1>\
  <form action='/' method='POST'>\
  <select>\
  <option>00</option>\
  <option>01</option>\
  <option>02</option>\
  <option>03</option>\
  <option>04</option>\
  <option>05</option>\
  <option>06</option>\
  <option>07</option>\
  <option>08</option>\
  <option>09</option>\
  <option>10</option>\
  <option>11</option>\
  <option>12</option>\
  <option>13</option>\
  <option>14</option>\
  <option>15</option>\
  <option>16</option>\
  <option>17</option>\
  <option>18</option>\
  <option>19</option>\
  <option>20</option>\
  <option>21</option>\
  <option>22</option>\
  <option>23</option>\
  <option>24</option>\
  </select>\
  <select>\
  <option>00</option>\
  <option>01</option>\
  <option>02</option>\
  <option>03</option>\
  <option>04</option>\
  <option>05</option>\
  <option>06</option>\
  <option>07</option>\
  <option>08</option>\
   <option>09</option>\
  <option>10</option>\
  <option>11</option>\
  <option>12</option>\
  <option>13</option>\
  <option>14</option>\
  <option>15</option>\
  <option>16</option>\
  <option>17</option>\
  <option>18</option>\
  <option>19</option>\
  <option>20</option>\
  <option>21</option>\
  <option>22</option>\
  <option>23</option>\
  <option>24</option>\
  <option>25</option>\
  <option>26</option>\
  <option>27</option>\
  <option>28</option>\
  <option>29</option>\
  <option>30</option>\
  <option>31</option>\
  <option>32</option>\
  <option>33</option>\
  <option>34</option>\
  <option>35</option>\
  <option>36</option>\
  <option>37</option>\
  <option>38</option>\
  <option>39</option>\
  <option>40</option>\
  <option>41</option>\
  <option>42</option>\
  <option>43</option>\
  <option>44</option>\
  <option>45</option>\
  <option>46</option>\
  <option>47</option>\
  <option>48</option>\
  <option>49</option>\
  <option>50</option>\
  <option>51</option>\
  <option>52</option>\
  <option>53</option>\
  <option>54</option>\
  <option>55</option>\
  <option>56</option>\
  <option>57</option>\
  <option>58</option>\
  <option>59</option>\
  </select>\
  <button type='submit'>Zatwierdź</button>\
  </form>\
  </body>\
  </html>";

   server.send(200,"text/html",temp);
}
void setup()
{
    setTft();
     setWiFi();
      setTimeClient();
    server.on("/",handleRoot);

 server.begin();
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}

int value = 0;

void loop(){
 server.handleClient();
 WiFiClient client = wifiServer.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  }
