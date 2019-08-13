//===============================================================
//		 ---------------------------------		|
//		|copyright khairunarkham	  |		|
//		|email: khairunarkham97@gmail.com |		|
//               ---------------------------------		|
//===============================================================

#include <Ethernet.h> //library ethernet
#include <SPI.h>
#include <UbidotsEthernet.h>//library ubidots
#include <dht.h>
#define dataPin 7
#define pinSensor A1    //definisi analog PIN co mq 7
 #include <dht.h>       //library dht
#define dataPin 7       //definisi digital pin dht 22 suhu
#define ANALOGPIN A0    //definisi analog PIN nh3 mq 135
#include <Wire.h> 
#include <SPI.h>



dht DHT;


int hasil;
float t,ppm2,pwm,ppm;
float dingin,normal,panas;
float rendah,sedang,tinggi;
float rendah1,sedang1,tinggi1;
float aman,waspada,bahaya;
float rule1a, rule2a, rule3a, rule4a, rule5a, rule6a, rule7a,rule8a, rule9a,
rule1b, rule2b, rule3b, rule4b, rule5b, rule6b, rule7b,rule8b, rule9b;
float rule1x, rule2x, rule3x, rule4x, rule5x, rule6x, rule7x,rule8x, rule9x,
rule1y, rule2y, rule3y, rule4y, rule5y, rule6y, rule7y,rule8y, rule9y;
float def,def1;

const int pinBuzzer1 = 2; //definisi digital pin buzzer
const int pinRelay1 = 3;  //definisi digital pin relay

float RL = 1000; // 1000 Ohm
float Ro = 830; // 830 ohm ( SILAHKAN DISESUAIKAN) di dapat dari 2,863=RS/RO
float RLNH3 = 1000; // 2000 Ohm
float RoNH3 = 555; // 555 ohm ( SILAHKAN DISESUAIKAN) di dapat dari 4,504 =RS/RO


char const * TOKEN = "A1E-GOZqWfsXf4cHQJRSDWbglZEgi3AfDc"; // token pada Ubidots
char const * VARIABLE_LABEL_1 = "temperatureimitasi2"; // Variabel penyimpan Data
char const * VARIABLE_LABEL_2 = "NH3imitasi2"; // Variabel penyimpan Data
char const * VARIABLE_LABEL_3 = "COimitasi2"; // Variabel penyimpan Data
char const * VARIABLE_LABEL_4= "defNH3imitasi2"; // Variabel penyimpan Data
char const * VARIABLE_LABEL_5 = "defCOimitasi2"; // Variabel penyimpan Data
char const * VARIABLE_LABEL_6 = "hasilimitasi2"; // Variabel penyimpan Data 

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

Ubidots client(TOKEN);
// fuzzy pada Suhu
void SuhuDingin(){
  if (t <= 30){dingin =1;}
  else if (t >=30 && t <=45){dingin=(45-t)/15;}
  else if (t >= 45){dingin =0;}
  return dingin;
}
void SuhuNormal(){
   if (t <= 30){normal =0;}
  else if (t >=30 && t<=45){normal=(t-30)/15;}
  else if (t >=45 && t <=60){normal=(60-t)/15;}
  else if (t >= 60){normal =0;}  
}
void SuhuPanas(){
  if (t <=45 ){panas =0;}
  else if (t >=45 && t <=60){panas=(t-45)/15;}
  else if (t >= 60){panas =1;}
  
}
// fuzzy pada NH3
//mq135
void NHRendah(){
  if (ppm2 <= 4) {
    rendah =1;
  }else if (ppm2 >=4 && ppm2 <=8){
    rendah = (8-ppm2)/4;
  }else if (ppm2 >= 8){
    rendah = 0;
  }
}
void NHSedang(){
   if (ppm2 <= 4) {
    sedang = 0;
  }else if (ppm2 >= 4 && ppm2 <=8){
    sedang =(ppm2-4)/4;
  }else if (ppm2 >=8 && ppm2<=14){
    sedang = 1; 
  }else if (ppm2 >=14 && ppm2 <=18){
    sedang = (18-ppm2)/4;
  }else if (ppm2 >= 18){
    sedang = 0;
  }
}
void NHTinggi(){
   if (ppm2 <= 14) {
    tinggi=0;
  }else if (ppm2 >=14&& ppm2 <=18){
    tinggi = (ppm2-14)/4;
  }else if (ppm2 >= 18){
    tinggi = 1;
  }
}
// fuzzy pada CO
//mq7
void CORendah(){
  if (ppm <= 10) {
    rendah1=1;
  }else if (ppm >=10 && ppm <=20){
    rendah1 = (20-ppm)/10;
  }else if (ppm >= 20){
    rendah1 = 0;
  }
}
void COSedang(){
  if (ppm <= 10) {
    sedang1 = 0;
  }else if (ppm >= 10 && ppm <=20){
    sedang1 =(ppm-10)/10;
  }else if (ppm >=20 && ppm<=40){
    sedang1 = 1; 
  }else if (ppm >=40 && ppm <=50){
    sedang1 = (50-ppm)/10;
  }else if (ppm >= 50){
    sedang1 = 0;
  }
}
void COTinggi(){
  if (ppm <= 40) {
    tinggi1=0;
  }else if (ppm >=40 && ppm <=50){
    tinggi1 = (ppm-40)/10;
  }else if (ppm >= 50){
    tinggi1 = 1;
  }
}


void fuzifikasi(){
  SuhuDingin();
  SuhuNormal();
  SuhuPanas();
  CORendah();
  COSedang();
  COTinggi();
  NHRendah();
  NHSedang();
  NHTinggi();
}
//deklarasi niloai minimal
float Min (float a, float b) {
if (a < b) {
return a;
}
else if (b < a) {
return b;
}
else return a;
}

// rule fuzzy mamdani
void rule(){
  fuzifikasi();
  //rule 1 jika suhu dingin dan NH rendah maka aman
  float x1 = Min (dingin,rendah);
  rule1a = 6 - (6 * x1);
  //rule 2 jika suhu normal dan NH rendah maka aman
  float x2 = Min (normal,rendah);
    rule2a = 6 - (6 * x2);
  //rule 3 jika suhu panas dan NH rendah maka waspada
  float x3 = Min (panas,rendah);
    rule3a = 3 + (3* x3);
    rule3b = 9 - (3 * x3);
  //rule 4 jika suhu dingin dan NH sedang maka waspada
  float x4 = Min (dingin,sedang);
    rule4a = 3 + (3 * x4);
    rule4b = 9 - (3 * x4);
  //rule 5 jika suhu normal dan NH sedang maka waspada
   float x5 = Min (normal,sedang);
    rule5a = 3 + (3 * x5);
    rule5b = 9 - (3 * x5);
  //rule 6 jika suhu panas dan NH sedang maka bahaya
   float x6 = Min (panas,sedang);
    rule6a = 6 + (6 * x6);
  //rule 7 jika suhu dingin dan NH tinggi maka bahaya
   float x7 = Min (dingin,tinggi);
    rule7a = 6 + (6 * x7);
  //rule 8 jika suhu normal dan NH tinggi maka bahaya
   float x8 = Min (normal,tinggi);
    rule8a = 6 + (6 * x8);
  //rule 9 jika suhu panas dan NH tinggi maka bahaya
   float x9 = Min (panas,tinggi);
    rule9a = 6 + (6 * x9);
    
 //defuzzyfikasi nh3 dengan suhu
 def = ((x1 * rule1a)+(x2 * rule2a) + (x3 * rule3a)+(x3 *rule3b)+(x4*rule4a)+(x4*rule4b)+(x5*rule5a)+(x5*rule5b)+(x6*rule6a)+(x7*rule7a)+(x8*rule8a)+(x9*rule9a))/(x1+x2+x3+x3+x4+x4+x5+x5+x6+x7+x8+x9);

//=================================================================
 //rule 1 jika suhu dingin dan co rendah maka aman
  float y1 = Min (dingin,rendah1);
  rule1x = 6 - (6 * y1);
  //rule 2 jika suhu normal dan co rendah maka aman
  float y2 = Min (normal,rendah1);
    rule2x = 6 - (6 * y2);
  //rule 3 jika suhu panas dan co rendah maka waspada
  float y3 = Min (panas,rendah1);
    rule3x = 3 + (3 * y3);
    rule3y = 9 - (3 * y3);
  //rule 4 jika suhu dingin dan co sedang maka waspada
  float y4 = Min (dingin,sedang1);
    rule4x = 3 + (3 * y4);
    rule4y = 9 - (3 * y4);
  //rule 5 jika suhu normal dan co sedang maka waspada
   float y5 = Min (normal,sedang1);
    rule5x = 3 + (3 * y5);
    rule5y = 9 - (3 * y5);
  //rule 6 jika suhu panas dan co sedang maka bahaya
   float y6 = Min (panas,sedang1);
    rule6x = 6 + (6 * y6);
  //rule 7 jika suhu dingin dan co tinggi maka bahaya
   float y7 = Min (dingin,tinggi1);
    rule7x = 6 + (6 * y7);
  //rule 8 jika suhu normal dan co tinggi maka bahaya
   float y8 = Min (normal,tinggi1);
    rule8x = 6 + (6 * y8);
  //rule 9 jika suhu panas dan co tinggi maka bahaya
   float y9 = Min (panas,tinggi1);
    rule9x = 6 + (6 * y9);

//defuzzyfikasi CO dengan suhu
   def1 = ((y1 * rule1x)+(y2 * rule2x) + (y3 * rule3x)+(y3 *rule3y)+(y4*rule4x)+(y4*rule4y)+(y5*rule5x)+(y5*rule5y)+(y6*rule6x)+(y7*rule7x)+(y8*rule8x)+(y9*rule9x))/(y1+y2+y3+y3+y4+y4+y5+y5+y6+y7+y8+y9);
 
 }

void setup (){
  Wire.begin();
  Serial.begin(9600);
  
   pinMode(pinBuzzer1, OUTPUT);
  pinMode(pinRelay1, OUTPUT);
   
  if (!Ethernet.begin(mac)) {
    Serial.println(F("failed"));
  } else {
    Serial.println(Ethernet.localIP());
  }
  delay(50);
  Serial.println(F("Ready"));
  delay(1000);
}


void loop (){ 
  Ethernet.maintain();
  //mq135 gas NH3 kalibrasi
float sensorValue = analogRead(ANALOGPIN); // membaca nilai ADC dari sensor
 float VRLNH3= sensorValue*5.00/1024;  // mengubah nilai ADC ( 0 - 1023 ) menjadi nilai voltase ( 0 - 5.00 volt )
 float RsNH3 = ( 5.00 * RLNH3 / VRLNH3 ) - RLNH3;
  ppm2 = 100 * pow(RsNH3 / RoNH3,-1.53); // ppm = 100 * ((rs/ro)^-1.53);
 
//mq7 gas co kalibrasi
float sensorvalue = analogRead(pinSensor); // membaca nilai ADC dari sensor
 float VRL= sensorvalue*5.00/1024;  // mengubah nilai ADC ( 0 - 1023 ) menjadi nilai voltase ( 0 - 5.00 volt )
 float Rs = ( 5.00 * RL / VRL ) - RL;
 ppm = 100 * pow(Rs / Ro,-1.53); // ppm = 100 * ((rs/ro)^-1.53);

  rule();
  analogWrite(7,def);
   analogWrite(6,def1);
   int readData = DHT.read22(dataPin);
   float t = DHT.temperature;
   int hasil2=hasil;


if (def>9 || def1>9){
  digitalWrite(pinRelay1, LOW);
     hasil= 2;
    
    digitalWrite(pinBuzzer1, HIGH);
  }else if (def>=4 && def<=9 || def1>4 && def1<=9){
    digitalWrite(pinRelay1,LOW);
    hasil= 1;
      digitalWrite(pinBuzzer1, LOW);
  } else{
     hasil= 0;
      digitalWrite(pinRelay1, HIGH);
      digitalWrite(pinBuzzer1, LOW);
  }
  
 client.add(VARIABLE_LABEL_1, t);
  client.add(VARIABLE_LABEL_2, ppm2);
  client.add(VARIABLE_LABEL_3, ppm);
  client.add(VARIABLE_LABEL_4, def);
  client.add(VARIABLE_LABEL_5, def1);
 client.sendAll();
 client.add(VARIABLE_LABEL_6, hasil2);
  client.sendAll();
  delay(1000);
}
