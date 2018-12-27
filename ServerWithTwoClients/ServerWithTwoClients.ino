#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

const char* ssid     = "TP-LINK_232B";
const char* password = "12345678";

String message;
String formattedDate;
String dayStamp;
String timeStamp;
String timeWithoutSeconds;
String timeOfAlarm = "Nie ustawiono";
String kod = "4444";

WiFiServer server(80);

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
void setTimeClient()
{
  timeClient.begin();
  //Set to our timezone
  timeClient.setTimeOffset(3600);
}
void setup()
{
  Serial.begin(115200);
  setTimeClient();
  setTft();

  delay(10);

  // We start by connecting to a WiFi network

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

String setHTML() {
  String temp = "<!DOCTYPE html><html>\
  <head><meta name='viewport' content='width=device-width', initial-scale=1>\
  <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>\
  </head>\
  <body>\
  <h1>IoT Budzik</h1>\
  <select id='hour'>\
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
  <select id ='minute'>\
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
  <button id = 'submit'>Zatwierdź</button>\
  </form>\
  </body>\
  </html>";
  /* <script>\
 var button = document.getElementById('submit'); \
  var selectHour = document.getElementById('hour'); \
  var selectMinute = document.getElementById('minute'); \
  var alarrmtTime; \
  button.onclick = function() {\
    alarmTime = selectHour.value + ':' + selectMinute.value;\
    alert(alarmTime) \
    var url = '/' + alarmTime; \
    window.location.replace(url); \
    < / script >";*/
    return temp;
  }
  void tryToTurnOffTheAlarm(String code){
    Serial.println("ENTERED tryToTurnOffTheAlarm");
    }
 void resolveMessage(){
    if(message.substring(0,4) == "CODE"){
      String code  = message.substring(4,message.length());
      tryToTurnOffTheAlarm(code);
      }
      else
      {
        String tmp  = message.substring(5,10);
        if(isdigit(tmp[0])){
          timeOfAlarm = tmp;
          } 
       }
    }
  void loop() {
    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
      Serial.println("New Client.");           // print a message out the serial port
      String currentLine = "";                // make a String to hold incoming data from the client
      while (client.connected()) {            // loop while the client's connected
        if (client.available()) {             // if there's bytes to read from the client,
          char c = client.read();
          message += c ;// read a byte, then
          //Serial.write(c);                    // print it out the serial monitor
          if (c == '\n') {                    // if the byte is a newline character

            // if the current line is blank, you got two newline characters in a row.
            // that's the end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();

              client.println(setHTML());

               String minute = "minute";
              String hour = "hour";
              client.println("<script>");
              client.println("var button = document.getElementById(\"submit\");");
              client.println("var selectHour = document.getElementById(\"hour\");");
              client.println("var selectMinute = document.getElementById(\"minute\");");
              client.println("var alarmTime;");

              client.println("button.onclick = function(){alarmTime = selectHour.value + ':' + selectMinute.value;alert(alarmTime);");
              client.println("var url = '/' +alarmTime; window.location.replace(url);};");

              client.println("</script>");


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
      //tft.println(header);
      Serial.println(message);

      resolveMessage();
      message = "";
      
      // close the connection:
      client.stop();
      Serial.println("Client Disconnected.");
    }

    tft.fillScreen(TFT_RED);

    tft.setCursor(2, 2, 2);

    while (!timeClient.update()) {
      timeClient.forceUpdate();
    }

    // The formattedDate comes with the following format:
    // 2018-05-28T16:00:13Z
    // We need to extract date and time
    formattedDate = timeClient.getFormattedDate();

    int indexOfT = formattedDate.indexOf("T");
    dayStamp = formattedDate.substring(0, indexOfT);

    timeStamp = formattedDate.substring(indexOfT + 1, formattedDate.length() - 1);

    timeWithoutSeconds = formattedDate.substring(indexOfT + 1, formattedDate.length() - 4);


    if (WiFi.status() == WL_CONNECTED) {
      tft.println(WiFi.localIP());
      tft.println("Connected");
      tft.print("DATE: ");
      tft.println(dayStamp);
      tft.print("HOUR: ");
      tft.println(timeStamp);
      tft.println(timeOfAlarm);
    }
    else {
      tft.println("NOT Connected");
    }


    //1sec delay
    delay(1000);
  }
