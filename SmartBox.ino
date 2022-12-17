#include<Wire.h>
//---- Sensor ----
//---- Wifi ----
#include "WiFiEsp.h"
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7);  //RX, TX
char ssid[] = "umin";            // your network SSID (name)
char pass[] = "umin1234";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char server[] = "15.164.111.44";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 30000L; // 60초 마다

WiFiEspClient client;

String todo = "";
String old_todo = "none";
String user_id = "1";
String message = "";
String old_message = "";

//---- Slave ----
void setup() {
  Wire.begin();
  
  Serial.begin(115200);     // initialize serial for debugging
  Serial1.begin(9600);      // initialize serial for ESP module
  // initialize ESP module
  WiFi.init(&Serial1);      // initialize ESP module

  // 와이파이 연결
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  
  printWifiStatus();
}
void loop() {
  requestEvent();
  delay(100);
}

void requestEvent(){
  // todo List 조회
  while (client.available()) {
    String line = client.readStringUntil('\r');
    
    if(line.indexOf(':') != -1){
      String header = line.substring(line.indexOf('\n') + 1, line.indexOf(':'));
      if(header.equals("MSG"))
      {
        message = line.substring(line.indexOf(':') + 1);
        Serial.println(message);
      }
    }
    else if(line.indexOf('#') != -1)
    {
      todo = line.substring(line.indexOf('#') + 1, line.length());
      Serial.print(todo);
    }
  }
  Serial.print(message);
  Wire.beginTransmission(8);
  Wire.print(message);
  Wire.endTransmission();
  if(message != "" && !message.equals(old_message))
  {
    old_message = message;
  }
  message = "";   

  // 변경된 경우에만 조명새로하기
  if(todo != "" && !todo.equals(old_todo))
  {
    old_todo = todo;
  }
  todo = "";    //  버퍼 비우기
  
  delay(100);

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    // httpRequest("/service/done/" + user_id);
    httpRequest("/service/message/" + user_id);
  }
}
