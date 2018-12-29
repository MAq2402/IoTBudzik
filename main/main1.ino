/*  
 Test the tft.print() viz embedded tft.write() function

 This sketch used font 2, 4, 7

 Make sure all the display driver and pin comnenctions are correct by
 editting the User_Setup.h file in the TFT_eSPI library folder.

 #########################################################################
 ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
 #########################################################################
 */


#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "TP-LINK_232B";
const char* password =  "12345678";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Set web server port number to 80
WiFiServer server(80);

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
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
}
void setTimeClient()
{
  timeClient.begin();
   //Set to our timezone
   timeClient.setTimeOffset(3600);
}
void setup(void) {  

 setTft();
 setWiFi();
 setTimeClient();

 server.begin();
   
}
void manageClient(WiFiClient client){
   if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        tft.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
        
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
           
            client.println("</head>");
            
            // Web Page Heading
            client.println("<body><h1>IoT Budzik</h1>");

          
            
            client.println("<select id=\"hour\">");
            client.println("<option>00</option>");
            client.println("<option>01</option>");
            client.println("<option>02</option>");
            client.println("<option>03</option>");
            client.println("<option>04</option>");
            client.println("<option>05</option>");
            client.println("<option>06</option>");
            client.println("<option>07</option>");
            client.println("<option>08</option>");
            client.println("<option>09</option>");
            client.println("<option>10</option>");
            client.println("<option>11</option>");
            client.println("<option>12</option>");
            client.println("<option>13</option>");
            client.println("<option>14</option>");
            client.println("<option>15</option>");
            client.println("<option>16</option>");
            client.println("<option>17</option>");
            client.println("<option>18</option>");
            client.println("<option>19</option>");
            client.println("<option>20</option>");
            client.println("<option>21</option>");
            client.println("<option>22</option>");
            client.println("<option>23</option>");
            client.println("</select>");

          
             //   client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            client.println("<select id=\"minute\">");
            client.println("<option>00</option>");
            client.println("<option>01</option>");
            client.println("<option>02</option>");
            client.println("<option>03</option>");
            client.println("<option>04</option>");
            client.println("<option>05</option>");
            client.println("<option>06</option>");
            client.println("<option>07</option>");
            client.println("<option>08</option>");
            client.println("<option>09</option>");
            client.println("<option>10</option>");
            client.println("<option>11</option>");
            client.println("<option>12</option>");
            client.println("<option>13</option>");
            client.println("<option>14</option>");
            client.println("<option>15</option>");
            client.println("<option>16</option>");
            client.println("<option>17</option>");
            client.println("<option>18</option>");
            client.println("<option>19</option>");
            client.println("<option>20</option>");
            client.println("<option>21</option>");
            client.println("<option>22</option>");
            client.println("<option>23</option>");
            client.println("<option>24</option>");
            client.println("<option>25</option>");
            client.println("<option>26</option>");
            client.println("<option>27</option>");
            client.println("<option>28</option>");
            client.println("<option>29</option>");
            client.println("<option>30</option>");
            client.println("<option>31</option>");
            client.println("<option>32</option>");
            client.println("<option>33</option>");
            client.println("<option>34</option>");
            client.println("<option>35</option>");
            client.println("<option>36</option>");
            client.println("<option>37</option>");
            client.println("<option>38</option>");
            client.println("<option>39</option>");
            client.println("<option>40</option>");
            client.println("<option>41</option>");
            client.println("<option>42</option>");
            client.println("<option>43</option>");
            client.println("<option>44</option>");
            client.println("<option>45</option>");
            client.println("<option>46</option>");
            client.println("<option>47</option>");
            client.println("<option>48</option>");
            client.println("<option>49</option>");
            client.println("<option>50</option>");
            client.println("<option>51</option>");
            client.println("<option>52</option>");
            client.println("<option>53</option>");
            client.println("<option>54</option>");
            client.println("<option>55</option>");
            client.println("<option>56</option>");
            client.println("<option>57</option>");
            client.println("<option>58</option>");
            client.println("<option>59</option>");
            client.println("</select>");
            String submit = "submit";
            client.println("<button id="+submit+">submit</button>");
            
            client.println("</body></html>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            //client.println("<p>GPIO 26 - State " + output26State + "</p>");
            // If the output26State is off, it displays the ON button       
            //if (output26State=="off") {
           //   client.println("<p><a href=\"/26/on\"><button class=\"button\">ON</button></a></p>");
            //} else {
            //  client.println("<p><a href=\"/26/off\"><button class=\"button button2\">OFF</button></a></p>");
           // } 
               
            // Display current state, and ON/OFF buttons for GPIO 27  
            //client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // If the output27State is off, it displays the ON button       
            //if (output27State=="off") {
            //  client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            //} else {
            //  client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            //}

            String minute = "minute";
            String hour = "hour";
            client.println("<script>");
            client.println("var button = document.getElementById(\"submit\");");
            client.println("var selectHour = document.getElementById(\"hour\");");
            client.println("var selectMinute = document.getElementById(\"minute\");");
            client.println("var alarmTime;");
            //String localIp = WiFi.localIP();
            client.println("button.onclick = function(){alarmTime = selectHour.value + ':' + selectMinute.value;alert(alarmTime);");
            client.println("var url = '/' +alarmTime; window.location.replace(url);};");
            //timeOfAlarm = client.read("alarmTime");
            timeOfAlarm = client.read();             // read a byte, then
            //tft.write(timeOfAlarm);
            delay(1000);  


            //client.println("if(document.URL.includes('/')){");
            //client.println("var url = document.URL + 'XD';");
            //client.println("window.location.replace(url);}");
            
            client.println("</script>");
            
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
  }
   // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
void loop() {

  WiFiClient client = server.available();

  manageClient(client); 
  
  tft.fillScreen(TFT_RED);
 
  tft.setCursor(2, 2, 2);
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

   // The formattedDate comes with the following format:
  // 2018-05-28T16:00:13Z
  // We need to extract date and time
  formattedDate = timeClient.getFormattedDate();

  int indexOfT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, indexOfT);

  timeStamp = formattedDate.substring(indexOfT+1, formattedDate.length()-1);

  timeWithoutSeconds = formattedDate.substring(indexOfT+1, formattedDate.length()-4);
 
 
   if(WiFi.status() == WL_CONNECTED) {
   tft.println(WiFi.localIP());
    tft.println("Connected");
     tft.print("DATE: ");
     tft.println(dayStamp);
      tft.print("HOUR: ");
     tft.println(timeStamp);
     tft.println(timeOfAlarm);
   }
   else{
     tft.println("NOT Connected");
   }


  //1sec delay
  delay(1000);

 
  
}
