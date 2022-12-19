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
const unsigned long postingInterval = 30000L;  // 30초 마다

WiFiEspClient client;
String user_id = "1";
bool all_done = false;    // 모든 할일 완료여부

//---- NTP-UDP 요청 ----
#include "WiFiEspUdp.h"

unsigned long lastNTPRequest = 0;                 // 시간 조회
const unsigned long NTPRequestInterval = 60000L;  // 60초 마다
bool is0004 = false;                              // 00시 00분 ~ 04시 59분 인지 확인
char timeServer[] = "time.nist.gov";  // NTP server
unsigned int localPort = 2390;        // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;  // NTP timestamp is in the first 48 bytes of the message
const int UDP_TIMEOUT = 2000;    // timeout in miliseconds to wait for an UDP packet to arrive

byte packetBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets
WiFiEspUDP Udp;                   // UDP instance

//---- Actuator -----
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
  Udp.begin(localPort);
}
void loop() {
  if(all_done){   // 모두 완료한 경우에만 선물박스 작동 가능
    boxControl();
  }
  else{
    myServo.write(0);
    myServo.detach();
  }

  // 현재시간 조회
  if (millis() - lastNTPRequest > NTPRequestInterval) {
    requestNTPTime();
    lastNTPRequest = millis();
  }
  
  // 00:00 ~ 04:59 AM 이면
  if(is0004){
    // 조도센서값 요청
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
          httpRequest("/service/goals/update");
        }
        lastLightTime = millis();
      }
      delay(1000);
    }
  }

  // todo List 조회
  while (client.available()) {
    String line = client.readStringUntil('\r');
    line.replace("\n", "");

    if (line.indexOf('#') != -1) {  // 웹서비스 요청 Response 받기
      int isize = line.length();
      checkAllDone(line);           // 할일 모두 완료했는지 체크
      Serial.println("Good Data");

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
    else{
      Serial.println("Bad Data");
      Serial.println(line);
    }
  }
  delay(100);

  // postingInterval 마다 데이터 조회
  if (millis() - lastConnectionTime > postingInterval) {
    // httpRequest("/service/goals/" + user_id);
    httpRequest("/service/goals/connected");
  }
}

// 할일 모두 완료했는지 체크
void checkAllDone(String res){
  // Response 문자열 형식 : led색상10진수|총갯수,완료갯수|메세지,positive메세지갯수
  if(res.indexOf('|') == -1) return;
  // {led색상10진수} 제거
  res = res.substring(res.indexOf('|') + 1);
  if(res.indexOf('|') == -1) return;
  // {메세지,positive메세지갯수} 제거
  res = res.substring(0, res.indexOf('|'));
  if(res.indexOf(',') == -1) return;

  //{갯수,완료갯수}
  int comma_idx = res.indexOf(',');
  int total_cnt = res.substring(0, comma_idx).toInt();
  int done_cnt = res.substring(comma_idx + 1).toInt();

  if(total_cnt > 0 && total_cnt == done_cnt)   // 등록된 할일이 있고, 모두 완료한 경우
  {
    all_done = true;
  }
  else{
    all_done = false;
  }
  Serial.print("all done:");
  Serial.println(all_done);
}

// 상자 열기
void boxControl() { 
  // 초음파 센서 측정
  digitalWrite(TRIG, LOW);
  digitalWrite(ECHO, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);   // 초음파 보내기
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);  
  
  duration = pulseIn(ECHO, HIGH);       // 초음파 반사되어 돌려받은 시간 저장
  distance = duration * 17.0 / 1000.0;  // 거리 계산 (cm 변환)

  delay(1000);
  int open = 0;
  int close = 100;

  if (!open_flag && distance < 20) {   // 닫혀있고, 앞에 뭐가 있는 경우에만
    Serial.print("distance : ");
    Serial.print(distance);
    Serial.println(" (open) ");
    myServo.attach(4);
    open_flag = !open_flag;
    // 90: open,  180: close
    for (int pos = close; pos > open; pos--) {
      myServo.write(pos);
      delay(15);
    }
    myServo.detach();
    // 징글벨 재생
    jinglebell();
    delay(100);
  }
  else if(open_flag && distance > 50){    // 열려있는데, 앞에 뭐가 없으면 닫기
    Serial.print("distance : ");
    Serial.print(distance);
    Serial.println(" (close) ");
    myServo.attach(4);
    open_flag = !open_flag;
    for (int pos = open; pos < close; pos++) {
      myServo.write(pos);
      delay(15);
    }
    myServo.detach();

    // 울면안돼 재생
    noCry();
    delay(100);
  }
}
