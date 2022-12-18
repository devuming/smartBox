int Tones_3[7] = {130, 146, 164, 174, 195, 220, 246};       // 3옥타브 계이름 (도 ~ 시)
int Tones_4[8] = {261, 294, 330, 349, 392, 440, 494, 523};  // 4옥타브 계이름(도 ~ 도)

// 수동부저 tone
void ring(int time, int note)
{
  int ms = 0;   // 지속시간
  switch(time){
    case 16:  ms = 80;  break;    // 16분 음표
    case 8:  ms = 160;  break;    // 8분 음표
    case 6:  ms = 240;  break;    // 점4분 음표
    case 4:  ms = 320;  break;    // 4분 음표
    case 2:  ms = 640;  break;    // 2분 음표
    default: ms = time;  break;
  }
  // 소리 울리기
  tone(BUZZER, note);
  delay(ms); noTone(BUZZER); delay(100);  
}

//- 울면 안돼
void noCry(){
  // 울면 안돼 : 미파 솔 솔
  ring(8, Tones_4[2]);
  ring(8, Tones_4[3]);
  ring(4, Tones_4[4]);
  ring(2, Tones_4[4]);
  delay(300);

  // 울면 안돼 : 라시도도
  ring(8, Tones_4[5]);
  ring(8, Tones_4[6]);
  ring(4, Tones_4[7]);
  ring(2, Tones_4[7]);
  delay(300);
  
  // 산타할아버지는 : 미파솔솔솔솔 솔
  ring(8, Tones_4[2]);
  ring(8, Tones_4[3]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[4]);
  ring(4, Tones_4[4]);

  // 우는 아이에겐 : 라솔파파파 파
  ring(8, Tones_4[5]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[3]);
  ring(8, Tones_4[3]);
  ring(4, Tones_4[3]);
  ring(4, Tones_4[3]);
  delay(100);

  // 선-물을 안주신데요 : 미솔도미레 파파 시도
  ring(4, Tones_4[2]);
  ring(4, Tones_4[4]);
  ring(4, Tones_4[0]);
  ring(4, Tones_4[2]);

  ring(4, Tones_4[1]);
  ring(4, Tones_4[3]);
  ring(4, Tones_4[3]);
  ring(4, Tones_3[6]);

  ring(2, Tones_4[0]);
  delay(2000);

}

//- 징글벨
void jinglebell(){
  // 종소리
  ring(8, Tones_4[2]);
  ring(8, Tones_4[2]);
  ring(4, Tones_4[2]);

  //울려라
  ring(8, Tones_4[2]);
  ring(8, Tones_4[2]);
  ring(4, Tones_4[2]);

  // 종소리 울려
  ring(8, Tones_4[2]);
  ring(8, Tones_4[4]);
  ring(6, Tones_4[0]); 
  ring(16, Tones_4[1]);
  ring(4, Tones_4[2]);
  delay(300); 

  // 우리썰매 : 파파파파 파미미미 미레레도
  ring(8, Tones_4[3]);
  ring(8, Tones_4[3]);
  ring(8, Tones_4[3]);
  ring(8, Tones_4[3]);

  ring(8, Tones_4[3]);
  ring(8, Tones_4[2]);
  ring(8, Tones_4[2]);
  ring(8, Tones_4[2]);

  ring(8, Tones_4[2]);
  ring(8, Tones_4[1]);
  ring(8, Tones_4[1]);
  ring(8, Tones_4[0]);
  
  // 레 솔
  ring(4, Tones_4[1]);
  ring(4, Tones_4[4]);
  delay(1000);
}

void rudolph(){
  // 루돌프사슴코는
  // 솔라 솔미 도 
  ring(8, Tones_4[4]);
  ring(4, Tones_4[5]);
  ring(8, Tones_4[4]);
  ring(4, Tones_4[2]);
  ring(4, Tones_4[7]);
  // 라 솔
  ring(4, Tones_4[5]);
  ring(2, Tones_4[4]);
  delay(200);
  // 매우반짝이는 코
  // 솔라솔라솔 도 
  ring(8, Tones_4[4]);
  ring(8, Tones_4[5]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[5]);
  ring(4, Tones_4[4]);
  ring(4, Tones_4[7]);
  // 시
  ring(2, Tones_4[6]);
  delay(200);
  // 만일네가봤다면
  // 파솔 파레 시
  ring(8, Tones_4[3]);
  ring(4, Tones_4[4]);
  ring(8, Tones_4[3]);
  ring(4, Tones_4[1]);
  ring(4, Tones_4[6]);
  // 라 솔
  ring(4, Tones_4[5]);
  ring(2, Tones_4[4]);
  delay(100);
  // 불붙는다 했겠지
  // 솔라솔라솔라
  ring(8, Tones_4[4]);
  ring(8, Tones_4[5]);
  ring(8, Tones_4[4]);
  ring(8, Tones_4[5]);
  ring(4, Tones_4[4]);
  ring(4, Tones_4[5]);
  // 미
  ring(2, Tones_4[2]);
  delay(2000);
}