// NTP UDP 패킷 전송
void sendNTPpacket(char *ntpSrv)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(ntpSrv, 123); //NTP requests are to port 123

  Udp.write(packetBuffer, NTP_PACKET_SIZE);

  Udp.endPacket();
}

// NTP 시간 조회
void requestNTPTime(){
  sendNTPpacket("time.nist.gov"); // send an NTP packet to a time server
  
  // wait for a reply for UDP_TIMEOUT miliseconds
  unsigned long startMs = millis();
  while (!Udp.available() && (millis() - startMs) < UDP_TIMEOUT) {}

  Serial.println(Udp.parsePacket());
  if (Udp.parsePacket()) {
    Serial.println("packet received");
    Udp.read(packetBuffer, NTP_PACKET_SIZE);    // 받은 패킷 버퍼로 읽어들이기

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.println(secsSince1900);

    // 1900.01.01 부터 계산
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    epoch += 32400;   // 한국시간 + 32400

    //- 시
    int hour = (epoch  % 86400L) / 3600;
    if(hour >= 0 && hour <= 5)
    {
      is0004 = true;    // 새벽 00:00~04:59 여부 업데이트
      Serial.print(hour);
      Serial.println(" AM");
    }else{
      is0004 = false;
    }
    // HH:mm:ss
    Serial.print("Time : ");       // UTC is the time at Greenwich Meridian (GMT)
    
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if (((epoch % 3600) / 60) < 10) { // 0 패딩
      Serial.print('0');  
    }
    //- 분
    int minute = (epoch  % 3600) / 60;
    Serial.print(minute); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if (minute < 10) {  // 0 패딩
      Serial.print('0');
    }
    //- 초
    Serial.println(epoch % 60);
  }

  delay(10);
}