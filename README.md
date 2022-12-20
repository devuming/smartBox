# smartBox
- 아두이노 Master
- 웹 서버에서 데이터 받아오고, 할일 모두 완료되었을 경우 선물상자 Control
- 초음파 센서로 거리 인식해 앞에 사람 있을 경우에 자동으로 Open, 없을 경우 자동으로 Close
---
- SmarTree.ino : Main, , Slave에 조도센서값 요청, 서보모터를 통한 선물상자 문 Contro;
- comWeb.ino : Wifi 연결, 서버에 데이터 요청
- UDPNTP_Time.ino : NTP서버에서 UDP 패킷으로 시간 받아옴
- ringCarol.ino : 수동부저로 캐롤 재생
