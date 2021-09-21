//ライブラリ一覧----------------
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <DS3231.h>
#include <Wire.h>
#include <FlexiTimer2.h>
//#include <LiquidCrystal.h>//デバッグ用
//----------------------------


//グローバル変数一覧-----------------------------------------
bool h12;
bool PM;
bool Century;
char data1, data2, data3, data4, data5, data6, data7, data8;//桁数字データ
char mode;//モード変更
char p1,p2,p3,p4,p5,p6,p7,p8;//ドットの表示指示
char d1,d2,d3,d4,d5,d6,d7,d8;//数字ロジックの電源供給駆動ピン指示
char mode1; //機能切り替え変数
char number1, number2, number3, number4, number5, number6, number7, number8, number9;//乱数データ
char count2;//スイッチのカウントチャタリング防止
unsigned long dm1, dm2;//大バージェンスメーター用の変数
static char count3;//1度ループのための変数

const char number_logic_a = 2;//数字用デコーダ入力a
const char number_logic_b = 3;//数字用デコーダ入力b
const char number_logic_c = 4;//数字用デコーダ入力c
const char number_logic_d = 5;//数字用デコーダ入力d
const char digit_logic_a = A0;//桁用デコーダ入力a
const char digit_logic_b = A1;//桁用デコーダ入力b
const char digit_logic_c = A2;//桁用デコーダ入力c

const char disap = 6;//数字ロジックの電源供給トランジスタ駆動ピン!!注意LCDを使う際には無効化せよ注意!!
const char dot_point = 1;//ドット表示のための出力ピン
const char sw1 = 0;//機能切り替えスイッチ
//------------------------------------------------




//GPS・ソフトウェアシリアル通信設定-------------------
const int PIN_RX = 8, PIN_TX = 9;//RX受信ピン、TX送信ピン
TinyGPSPlus gps;
SoftwareSerial ss(PIN_RX, PIN_TX);
//------------------------------------------------



//LCD設定-----------------------------------------
//LiquidCrystal lcd( 6, 7, 10, 11, 12, 13 );
//------------------------------------------------



//RTC設定-------------------------------------------
DS3231 Clock;
//-------------------------------------------------




//文字定義-----------------------------------------
#define SEGMENT_ON HIGH
#define SEGMENT_OFF LOW
//------------------------------------------------




//数字toデコーダ-------------------------------------
const int digits[] = {
    0x0,  // 0
    0x1,  // 1
    0x2,  // 2
    0x3,  // 3
    0x4,  // 4
    0x5,  // 5
    0x6,  // 6
    0x7,  // 7
    0x8,  // 8
    0x9,  // 9
    0xa   //何も表示しない
};
//--------------------------------------------------




//初期セットアップ------------------------------
void setup() {
  //lcd.begin(16,2);//デバッグ用
  //lcd.clear();//デバッグ用
  Wire.begin();
  ss.begin(9600);

  pinMode(number_logic_a,OUTPUT);
  pinMode(number_logic_b,OUTPUT);
  pinMode(number_logic_c,OUTPUT);
  pinMode(number_logic_d,OUTPUT);

  pinMode(digit_logic_a,OUTPUT);
  pinMode(digit_logic_b,OUTPUT);
  pinMode(digit_logic_c,OUTPUT);

  pinMode(sw1,INPUT);
  pinMode(dot_point,OUTPUT);
  pinMode(disap,OUTPUT);

  FlexiTimer2::set(8,1/10000,digit);//タイマー2割り込みms単位
  FlexiTimer2::start();

}
//-----------------------------------------------------------





//数字表示用プログラム----------------------------------------------------------------
void flash(char n){
  digitalWrite(number_logic_a , digits[n] & 0b00000001 ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(number_logic_b , digits[n] & 0b00000010 ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(number_logic_c , digits[n] & 0b00000100 ? SEGMENT_ON : SEGMENT_OFF);
  digitalWrite(number_logic_d , digits[n] & 0b00001000 ? SEGMENT_ON : SEGMENT_OFF);
}
//-------------------------------------------------------------------------------------





//桁割り込み用プログラム-----------------------------
void digit(){
  char digit_sw;
  char ichi_keta, ni_keta, san_keta, yon_keta, go_keta, 
  roku_keta, nana_keta, hachi_keta;
  int lap = 1500;

  for(digit_sw = 0; digit_sw < 9; digit_sw++){

  switch(digit_sw){
    
  case 0:
  digitalWrite(digit_logic_a,LOW);
  digitalWrite(digit_logic_b,LOW);
  digitalWrite(digit_logic_c,LOW);
  digitalWrite(dot_point,p1);
  flash(data1);
  delayMicroseconds(lap);
  break;

  case 1:
  digitalWrite(digit_logic_a,HIGH);
  digitalWrite(digit_logic_b,LOW);
  digitalWrite(digit_logic_c,LOW);
  digitalWrite(dot_point,p2);
  flash(data2);
  delayMicroseconds(lap);
  break;

  case 2:
  digitalWrite(digit_logic_a,LOW);
  digitalWrite(digit_logic_b,HIGH);
  digitalWrite(digit_logic_c,LOW);
  digitalWrite(dot_point,p3);
  flash(data3);
  delayMicroseconds(lap);
  break;

  case 3:
  digitalWrite(digit_logic_a,HIGH);
  digitalWrite(digit_logic_b,HIGH);
  digitalWrite(digit_logic_c,LOW);
  digitalWrite(dot_point,p4);
  flash(data4);
  delayMicroseconds(lap);
  break;

  case 4:
  digitalWrite(digit_logic_a,LOW);
  digitalWrite(digit_logic_b,LOW);
  digitalWrite(digit_logic_c,HIGH);
  digitalWrite(dot_point,p5);
  flash(data5);
  delayMicroseconds(lap);
  break;

  case 5:
  digitalWrite(digit_logic_a,HIGH);
  digitalWrite(digit_logic_b,LOW);
  digitalWrite(digit_logic_c,HIGH);
  digitalWrite(dot_point,p6);
  flash(data6);
  delayMicroseconds(lap);
  break;

  case 6:
  digitalWrite(digit_logic_a,LOW);
  digitalWrite(digit_logic_b,HIGH);
  digitalWrite(digit_logic_c,HIGH);
  digitalWrite(dot_point,p7);
  flash(data7);
  delayMicroseconds(lap);
  break;

  case 7:
  digitalWrite(digit_logic_a,HIGH);
  digitalWrite(digit_logic_b,HIGH);
  digitalWrite(digit_logic_c,HIGH);
  digitalWrite(dot_point,p8);
  flash(data8);
  delayMicroseconds(lap);
  break;
  
  case 8:
if1:
if( dm2 >= dm1 + 100 && dm2 <= dm1 + 110){
  goto num2;
}

if2:
if( dm2 >= dm1 + 110 && dm2 <= dm1 + 120){
  goto num3;
}

if3:
if( dm2 >= dm1 + 120 && dm2 <= dm1 + 130){
  goto num4;
}

if4:
if( dm2 >= dm1 + 130 && dm2 <= dm1 + 140){
  goto num5;
}

if5:
if( dm2 >= dm1 + 140 && dm2 <= dm1 + 150){
  goto num6;
}

if6:
if( dm2 >= dm1 + 150 && dm2 <= dm1 + 160){
  goto num7;
}

if7:
if( dm2 >= dm1 + 160 && dm2 <= dm1 + 170){
  goto num8;
}

if8:
if( dm2 >= dm1 + 170 && dm2 <= dm1 + 171){
    number8 = random(0,2);
}

if9:
if( dm2 >= dm1 + 171 && dm2 <= dm1 + 400){
    goto sw_hantei;
}

if10:
if( dm2 >= dm1 +400 && dm2 <= dm1 + 401){
  count3 = 0;
}
num1:         number5 = random(0,10);
num2:         number7 = random(0,10);
num3:         number2 = random(0,10);
num4:         number4 = random(0,10);
num5:         number6 = random(0,10);
num6:         number1 = random(0,10);
num7:         number3 = random(0,10);
num8:         number8 = random(0,10);




sw_hantei:
    if (digitalRead(sw1) == 0){
      count2 = count2 + 1;
    }
    if(count2 > 4){
    mode1 = mode1 + 1;
    count2 = 0;
    }
  break;

  default:
  break;
  }
  }
}
//------------------------------------------------





//RTC書き込みプログラム------------------------------
void rtc_write(){

  char s,m,h,d,mon;
  int y;
        s = gps.time.second();
        m = gps.time.minute();
        h = gps.time.hour() + 9;
        d = gps.date.day();
        mon = gps.date.month();
        y = gps.date.year() - 2000;

        if(h > 23){
          d = d+1;
        }


        if(h > 12){
          h = h-12;
          if(h >= 12){
            h = h - 12;
          }
        }

  if(s != 0);{
    if(h != 9){
        Clock.setSecond(s);
        Clock.setMinute(m);
        Clock.setHour(h);
        Clock.setDate(d);
        Clock.setMonth(mon);
        Clock.setYear(y);
    }
      }
}
//--------------------------------------------





//LCD出力用プログラム------------------------------------
/*デバッグ用
void display_rtc(){

    String r;
    r = Clock.getYear() + 2000;
    r += "/";
    r += Clock.getMonth(Century);
    r += "/";
    r += Clock.getDate();
    lcd.setCursor(0,0);
    lcd.print(r);
    
    String s;
    s = "nowtime=";
    s += Clock.getHour(h12,PM);
    s += ":";
    s += Clock.getMinute();
    s += ":";
    s += Clock.getSecond();
    lcd.setCursor(0,1);
    lcd.print(s);
}
*/
//-----------------------------------------------------



//乱数プログラム1-----------------------------------------
/*
void ransuu(){
  char t1 = 1;
    data1 = random(0,9);
      delay(t1);
    data2 = random(0,9);
      delay(t1);
    data3 = random(0,9);
      delay(t1);
    data4 = random(0,9);
      delay(t1);
    data5 = random(0,9);
      delay(t1);
    data6 = random(0,9);
      delay(t1);
    data7 = random(0,9);
      delay(t1);
    data8 = random(0,9);
}
*/
//-----------------------------------------------------





//gps時刻取得プログラム----------------------------------
void gps_gettime(){
    while (ss.available() > 0){
    if (gps.encode(ss.read())){
    }
    }
   if(gps.time.isUpdated()){
    rtc_write();
    FlexiTimer2::start();
   }
}
//---------------------------------------------



//メイン関数---------------------------------------
void loop() {

  char tt;
  long sec;//arduinoが起動してからの時間を記録するための変数
  char dot_flag;//ドット表示用のための変数

    switch(mode1){

    case 0://時刻モード
    gps_gettime();
    data1 = Clock.getSecond() % 10;
    data2 = Clock.getSecond() / 10;
    data3 = 10;
    data4 = Clock.getMinute() % 10;
    data5 = Clock.getMinute() / 10;
    data6 = 10;
    tt = Clock.getHour(h12,PM);
    if(tt > 12){
      tt = tt - 12;
    }
    data7 = tt % 10;
    data8 = tt / 10;
    p1 = p2 = p4 = p5 = p7 = p8 = 0;
    if(data1 % 2 == 1){
    p3 = p6 = 1;
    }
    else{
    p3 = p6 = 0;
    }
    if(Clock.getSecond() == 10 || Clock.getSecond() == 11 || Clock.getSecond() == 12){
    FlexiTimer2::stop();
    }
    break;


    

    case 1://日付モード
    gps_gettime();
    data1 = Clock.getDate() % 10;
    data2 = Clock.getDate() / 10;
    data3 = Clock.getMonth(Century) % 10;
    data4 = Clock.getMonth(Century) / 10;
    data5 = Clock.getYear() % 10;
    data6 = Clock.getYear() / 10;
    data7 = 0;
    data8 = 2;
    dot_flag = Clock.getSecond() % 10;
    if(dot_flag % 2 == 1){
    p3 = p5 = 1;
    }
    else{
    p3 = p5 = 0;
    }
    p1 = p2 = p4 = p6 = p7 = p8 = 0;
    if(Clock.getSecond() == 10 || Clock.getSecond() == 11 || Clock.getSecond() == 12){
    FlexiTimer2::stop();
    }
    break;



    case 2://arduinoが起動してからの時間 milli()関数を使用
    sec = millis() / 100;
    data1 = (((((((sec % 10000000) % 1000000) % 100000) % 10000) % 1000) % 100) % 10);
    data2 = (((((((sec % 10000000) % 1000000) % 100000) % 10000) % 1000) % 100) / 10);
    data3 = ((((((sec % 10000000) % 1000000) % 100000) % 10000) % 1000) / 100);
    data4 = (((((sec % 10000000) % 1000000) % 100000) % 10000) / 1000);
    data5 = ((((sec % 10000000) % 1000000) % 100000) / 10000);
    data6 = (((sec % 10000000) % 1000000) / 100000);
    data7 = ((sec % 10000000) / 1000000);
    data8 = sec / 10000000;
    p1 = p2 = p3 = p4 = p5 = p6 = p7 = p8 = 0;
    break;




    case 3://乱数決定モード
    if(count3 == 0){
    dm1 = millis() / 10;
    count3 = 1;
    }
    dm2 = millis() / 10;
    p1 = p2 = p3 = p4 = p5 = p6 = p8 = 0;
    p7 = 1;
    data1 = number1;
    data2 = number2;
    data3 = number3;
    data4 = number4;
    data5 = number5;
    data6 = number6;
    data7 = 10;
    data8 = number8;
    break;



    case 4://モードを時計モードへ移行
    count3 = 0;
    dm1 = 0;
    dm2 = 0;
    mode1 = 0;
    break;


    
    
    default:
    mode1 = 0;

    //display_rtc();//デバッグ用
    }
  }
//--------------------------------------------------
