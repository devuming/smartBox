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

unsigned long lastConnectionTime = 0;           // 할일 조회 호출
const unsigned long postingInterval = 30000L; // 60초 마다

WiFiEspClient client;
String user_id = "1";

// String todo = "todo";
// String old_todo = "old_todo";
// String message = "message";
// String old_message = "old_todo";

//---- Master ----
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

  // todo List 조회
  while (client.available()) {
    String line = client.readStringUntil('\r');
    line.replace("\n", "");
    
    if(line.indexOf('#') != -1){
      int isize = line.length();
      while(isize > 0)
      {
       String now = "";
        if (isize > 30){
          now = line.substring(0, 30);
          line = line.substring(30);
          Serial.println(isize);
          isize = line.length();
        }
        else{
          now = line;
          isize = 0;
        }
          
        Wire.beginTransmission(1);
        Serial.print(now);
        Wire.print(now);
        // Wire.println("#hello");
        Wire.endTransmission();
        delay(50);
      }
      // // Serial.println(line);
      // if (line.length() > 30)
      // {
      //   Serial.print("hey");
      //   Serial.println(line.length());

      //   Wire.beginTransmission(1);
      //   for(int i = 0; i < line.length(); i++){
      //   Serial.print(line[i]);
      //   Wire.write(line[i]);
      //   }
      //   // Serial.print(line.substring(0, 30));
      //   // Wire.print(line.substring(0, 30));
      //   // Wire.endTransmission(1);
      //   // delay(100);
      //   // Wire.beginTransmission(1);
      //   // Serial.println(line.substring(30));
      //   // Wire.println(line.substring(30));
      //   Wire.endTransmission(1);
      // }
      // else{
      //   Wire.beginTransmission(1);
      //   Wire.println(line);
      //   // Wire.println("#hello");
      //   Wire.endTransmission();
      // }
    }
    delay(10);
  }
  delay(100);

  // postingInterval 마다 데이터 조회
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest("/service/goals/" + user_id);
  }
}
