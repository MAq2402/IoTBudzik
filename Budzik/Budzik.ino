

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
  </body>\
  </html>";
  /*<form action='/set' method='post'>\
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
  <button type='submit'>Zatwierdź</button>\*/
 

   server.send(200,"text/html",temp);
}
void handleSet(){
   String temp = "<html>\
   <head>\
   <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>\
  <body>\
    <title>Add User Site</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
   <form action='/set' method='POST'>\
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
void setup(void) {  
 Serial.begin(115200);
 setTft();
 setWiFi();
 setTimeClient();
 server.on("/",handleRoot);
 server.on("/set",handleSet);
 

 server.begin();
 
}
void loop() {

  server.handleClient();

//  manageClient(client); 
  
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
