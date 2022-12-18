#include <Wire.h>
//---- Sensor ----
const int TRIG = 9;   // 초음파 보내는 핀
const int ECHO = 10;   // 초음파 받는 핀
float duration, distance;

//---- Wifi ----
#include "WiFiEsp.h"
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7);  //RX, TX
char ssid[] = "umin";          // your network SSID (name)
char pass[] = "umin1234";      // your network password
int status = WL_IDLE_STATUS;   // the Wifi radio's status
char server[] = "15.164.111.44";

unsigned long lastLightTime = 0;              // 조도센서 조회 호출
const unsigned long lightInterval = 120000L;  // 2분 마다

unsigned long lastConnectionTime = 0;          // 할일 조회 호출
const unsigned long postingInterval = 30000L;  // 60초 마다

WiFiEspClient client;
String user_id = "1";

#include <Servo.h>
Servo myServo;
bool open_flag = false;  // true : 열림, false : 닫힘
#define BUZZER 8

//---- Master : Wifi 값 조회해와 트리에 전달 ----
void setup() {
  Wire.begin();
  myServo.attach(4);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  // myServo.write(180);

  Serial.begin(115200);  // initialize serial for debugging
  Serial1.begin(9600);   // initialize serial for ESP module
  // initialize ESP module
  WiFi.init(&Serial1);  // initialize ESP module

  // 와이파이 연결
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");

  printWifiStatus();
}
void loop() {
  boxControl();

  // LDR : lightInterval 마다 데이터 요청
  if (millis() - lastLightTime > lightInterval) {
    Wire.requestFrom(1, 2);
    Serial.println();

    if (Wire.available() > 0) {
      Serial.print("LDR:");
      char update_flag = Wire.read();
      Serial.print(update_flag);

      if (update_flag == '1') {
        Serial.println("update");
        // todo date 업데이트 요청
        // httpRequest("/service/goals/update");
      }
      lastLightTime = millis();
    }
    delay(1000);
  }

  // todo List 조회
  while (client.available()) {
    String line = client.readStringUntil('\r');
    line.replace("\n", "");

    if (line.indexOf('#') != -1) {
      int isize = line.length();
      // 32byte 이상 데이터 전송 처리 : 오래 걸림 휴
      while (isize > 0) {
        String now = "";
        if (isize > 30) {
          now = line.substring(0, 30);
          line = line.substring(30);
          Serial.println(isize);
          isize = line.length();
        } else {
          now = line;
          isize = 0;
        }

        Wire.beginTransmission(1);
        Serial.print(now);
        Wire.print(now);
        Wire.endTransmission();
        delay(50);
      }
    }
    delay(10);
  }
  delay(100);

  // postingInterval 마다 데이터 조회
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest("/service/goals/" + user_id);
  }
}

// 상자 열기
void boxControl() { 
  // 초음파 센서 값 받아오기 
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);   // 초음파 보내기
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // 초음파 반사되어 돌려받은 시간 저장
  duration = pulseIn(ECHO, HIGH); 
  // 거리 계산 (cm 변환)
  distance = duration * 17.0 / 1000.0;

  Serial.print("distance");
  Serial.println(distance);
  delay(100);
  int open = 0;
  int close = 120;

  if (open_flag && distance < 22) {   // 닫혀있고, 앞에 뭐가 있는 경우에만
    myServo.attach(4);
    open_flag = !open_flag;
    // 90: open,  180: close
    for (int pos = close; pos > open; pos--) {
      myServo.write(pos);
      delay(15);
    }
    myServo.detach();
    // 징글벨 재생
    // jinglebell();
    delay(100);
  }
  else if(!open_flag && distance > 22){    // 열려있는데, 앞에 뭐가 없으면
    myServo.attach(4);
    open_flag = !open_flag;
    for (int pos = open; pos < close; pos++) {
      myServo.write(pos);
      delay(15);
    }
    myServo.detach();

    // 울면안돼 재생
    // noCry();
    delay(100);
  }
}

// 상자 닫기
void close() {
  for (int pos = 90; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  myServo.write(0);

  // 경고음 재생
  ring(2, 330);
}

//---- NTP ----
