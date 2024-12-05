//      >>>>>  T-I-N-Y  A-R-K-A-N-O-I-D for ATTINY85  GPL v3 <<<<
//                  Programmer: Daniel Champagne 2018
//                 Contact EMAIL: phoenixbozo@gmail.com
//           https://sites.google.com/view/arduino-collection

//  Tiny ARKANOID is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

//the code work at 16MHZ internal
//and use ssd1306xled Library for SSD1306 oled display 128x64
//Program chip with arduino uno as isp at 16 mhz internal!

#include <ssd1306xled.h>
#include "spritebank.h"

// var public
// fin var public

void setup() {
SSD1306.ssd1306_init();
pinMode(1,INPUT);
pinMode(4,OUTPUT);
pinMode(A0,INPUT); 
pinMode(A3,INPUT);}

void RsVarNewGame(GROUPE *VAR){
VAR->LEVELSPEED=16;
VAR->LEVEL=1;
VAR->live=3;
VAR->ANIMREFLECT=0;
LoadLevel(0,VAR);
}

void loop() {
GROUPE VARIABLE;
NEWGAME:;
Tiny_Flip(1,&VARIABLE);
while(1){
if (digitalRead(1)==0) {break;}}
RsVarNewGame(&VARIABLE);
Tiny_Flip(2,&VARIABLE);
PLAYMUSIC();
LoadLevel(VARIABLE.LEVEL-1,&VARIABLE);
goto ONE;
NEXTLEVEL:;
Sound(60,100);
Sound(80,100);
Sound(100,100);
Sound(120,100);
Sound(140,100);
if (VARIABLE.LEVELSPEED>8) {VARIABLE.LEVELSPEED=VARIABLE.LEVELSPEED-2;}
Tiny_Flip(2,&VARIABLE);
delay(400);
ResetVar(&VARIABLE);
VARIABLE.LEVEL++;
goto ONE;
RESTARTLEVEL:;
Sound(200,100);
Sound(150,100);
Sound(100,100);
Sound(50,100);
if (VARIABLE.live>0) {VARIABLE.live--;}else{goto NEWGAME;}
ONE:;
ResetBall(&VARIABLE);
while(1){
if (VARIABLE.Frame%8==0) {
if ((analogRead(A3)>=750)&&(analogRead(A3)<850)) {if (VARIABLE.TrackBaryDecal<7) {if (VARIABLE.TrackBaryDecal+(VARIABLE.TrackBary*8)<44){ VARIABLE.TrackBaryDecal++;}}else{VARIABLE.TrackBaryDecal=0;VARIABLE.TrackBary++;}}
if ((analogRead(A3)>500)&&(analogRead(A3)<750)) {if (VARIABLE.TrackBaryDecal>0) {if (VARIABLE.TrackBaryDecal+(VARIABLE.TrackBary*8)>4){ VARIABLE.TrackBaryDecal--;}}else{VARIABLE.TrackBaryDecal=7;VARIABLE.TrackBary--;}}
if ((VARIABLE.launch==0)&&(digitalRead(1)==LOW)) {VARIABLE.launch=1;}
if (VARIABLE.launch==0) {VARIABLE.Ballypos=((VARIABLE.TrackBary*8)+VARIABLE.TrackBaryDecal)+10;VARIABLE.SIMBallypos=VARIABLE.Ballypos;}
}
if ((VARIABLE.Frame%VARIABLE.LEVELSPEED==0)) {UpdateBall(&VARIABLE);} //1,2,4,8,
if (VARIABLE.Frame%32==0) {Tiny_Flip(0,&VARIABLE);}
if (VARIABLE.Frame==48) {
if (VARIABLE.ANIMREFLECT<3) {VARIABLE.ANIMREFLECT++;}
if (BallMissing(&VARIABLE)) {goto RESTARTLEVEL;}
if (CheckLevelEnded(&VARIABLE)) {goto NEXTLEVEL;}
}
if (VARIABLE.Frame<64) {VARIABLE.Frame++;}else{VARIABLE.Frame=1;}
}
}
////////////////////////////////// main end /////////////////////////////////

void PLAYMUSIC(void){
for (uint8_t t=0;t<92;t=t+2){
Sound(pgm_read_byte(&Music1[t]),(pgm_read_byte(&Music1[t+1])-100));
}}

uint8_t BallMissing(GROUPE *VAR){
if (VAR->Ballxpos<0) {return 1;}
return 0; 
}

uint8_t CheckLevelEnded(GROUPE *VAR){
uint8_t h,v,res=1;
for(v=0;v<5;v++){
for(h=0;h<6;h++){
if ((VAR->BlocsGrid[h][v]!=255)&&(VAR->BlocsGrid[h][v]!=5)) {res=0;}
}}
return res;
}

void UpdateBall(GROUPE *VAR){
VAR->TrackAngleOut=0;
for (uint8_t T1=0;T1<=6;T1++) {
RecupeBALLPosForSIM(VAR);
if (VAR->launch==0) goto FIN;
SimulMove(T1,VAR);
TestMoveBALL(VAR);
if (CheckCollisionBall(VAR)==0) {goto FIN;}
}
FIN:;
WriteBallMove(VAR);
}

void RecupeBALLPosForSIM(GROUPE *VAR){
VAR->SIMBallxpos=VAR->Ballxpos;
VAR->SIMBallypos=VAR->Ballypos;
VAR->SIMBallSpeedx=VAR->BallSpeedx;
VAR->SIMBallSpeedy=VAR->BallSpeedy;
}

void TestMoveBALL(GROUPE *VAR){
VAR->SIMBallxpos=VAR->SIMBallxpos+VAR->SIMBallSpeedx;
VAR->SIMBallypos=VAR->SIMBallypos+VAR->SIMBallSpeedy;
}

void SimulMove(uint8_t Sim,GROUPE *VAR){
switch(Sim){
  case (0):VAR->SIMBallSpeedx=VAR->BallSpeedx;VAR->SIMBallSpeedy=VAR->BallSpeedy;break;
  case (1):VAR->SIMBallSpeedx=-VAR->BallSpeedx;VAR->SIMBallSpeedy=VAR->BallSpeedy;break;
  case (2):VAR->SIMBallSpeedx=VAR->BallSpeedx;VAR->SIMBallSpeedy=-VAR->BallSpeedy;break;
  case (3):VAR->SIMBallSpeedx=-VAR->BallSpeedx;VAR->SIMBallSpeedy=-VAR->BallSpeedy;break;
  case (4):VAR->SIMBallSpeedx=-VAR->BallSpeedy;VAR->SIMBallSpeedy=-VAR->BallSpeedx;break;
  case (5):VAR->SIMBallxpos=VAR->Ballxpos+1;VAR->SIMBallypos=VAR->Ballypos;VAR->SIMBallSpeedx=-1;VAR->SIMBallSpeedy=1;break;
  case (6):VAR->SIMBallxpos=VAR->Ballxpos+1;VAR->SIMBallypos=VAR->Ballypos;VAR->SIMBallSpeedx=-1;VAR->SIMBallSpeedy=-1;break;
  default:break;
}}

uint8_t CheckCollisionBall(GROUPE *VAR){
if (VAR->SIMBallxpos>106) {return 1;}
if (VAR->SIMBallypos>59) {return 1;}
if (VAR->SIMBallypos<4) {return 1;}
if (CheckCollisionWithTRACKBAR(VAR)) {Sound(60,10);return 1;}
if (CheckCollisionWithBLOCK(VAR)) {return 1;}
return 0;
}

uint8_t CheckCollisionWithBLOCK(GROUPE *VAR){
RecupePositionOnGrid(VAR);
if ((VAR->Px==255)||(VAR->Py==255)) {return 0;}
if (VAR->BlocsGrid[VAR->Py][VAR->Px]==5) {Sound(210,50);VAR->ANIMREFLECT=0;return 1;}
if (VAR->BlocsGrid[VAR->Py][VAR->Px]==255) {return 0;}
Sound(150,10);
VAR->BlocsGrid[VAR->Py][VAR->Px]=255;
return 1;
}

void RecupePositionOnGrid(GROUPE *VAR){
VAR->Px=RecupeXPositionOnGrid(VAR);
VAR->Py=RecupeYPositionOnGrid(VAR);
}

uint8_t RecupeXPositionOnGrid(GROUPE *VAR){
if ((VAR->SIMBallxpos>=66)&&(VAR->SIMBallxpos<72))  return 0;
else if ((VAR->SIMBallxpos>=72)&&(VAR->SIMBallxpos<78)) return 1;
else if ((VAR->SIMBallxpos>=78)&&(VAR->SIMBallxpos<84)) return 2;
else if ((VAR->SIMBallxpos>=84)&&(VAR->SIMBallxpos<90))  return 3;
else if ((VAR->SIMBallxpos>=90)&&(VAR->SIMBallxpos<96))  return 4;  
return 255;
}

uint8_t RecupeYPositionOnGrid(GROUPE *VAR){
if ((VAR->SIMBallypos>=8)&&(VAR->SIMBallypos<16)) return 0;
else if ((VAR->SIMBallypos>=16)&&(VAR->SIMBallypos<23))  return 1;
else if ((VAR->SIMBallypos>=23)&&(VAR->SIMBallypos<31))  return 2;
else if ((VAR->SIMBallypos>=31)&&(VAR->SIMBallypos<40))  return 3;
else if ((VAR->SIMBallypos>=40)&&(VAR->SIMBallypos<48))  return 4; 
else if ((VAR->SIMBallypos>=48)&&(VAR->SIMBallypos<55))  return 5;   
return 255;
}

uint8_t CheckCollisionWithTRACKBAR(GROUPE *VAR){
uint8_t TRACK=(VAR->TrackBary*8)+VAR->TrackBaryDecal;
if ((VAR->SIMBallxpos>6)||(VAR->SIMBallxpos<5)) {return 0;}
if (TRACK>VAR->SIMBallypos) {return 0;}
if ((TRACK+16)<VAR->SIMBallypos) {return 0;}
VAR->TrackAngleOut=(((VAR->SIMBallypos-TRACK)*200)/16)-100;
return 1;
}

void WriteBallMove(GROUPE *VAR){
float CORECTIONY=(VAR->SIMBallSpeedy)+(VAR->TrackAngleOut/100.00);
if (CORECTIONY<-1) {CORECTIONY=-1;}
if (CORECTIONY>1) {CORECTIONY=1;}
VAR->Ballxpos=VAR->SIMBallxpos;
VAR->Ballypos=VAR->SIMBallypos;
VAR->BallSpeedx=VAR->SIMBallSpeedx;
VAR->BallSpeedy=CORECTIONY;
VAR->BALLyDecal=RecupeDecalageY(VAR->Ballypos-1);
VAR->Ypos=((VAR->Ballypos-1)/8);
}

void Tiny_Flip(uint8_t render0_picture1,GROUPE *VAR){
uint8_t y,x; 
for (y = 0; y < 8; y++){ 
    SSD1306.ssd1306_send_command(0xb0 + y);
    SSD1306.ssd1306_send_command(0x00);
    SSD1306.ssd1306_send_command(0x10);
    SSD1306.ssd1306_send_data_start();
for (x = 0; x < 128; x++){
if (render0_picture1==0) {
SSD1306.ssd1306_send_byte(Block(x,y,VAR)|Ball(x,y,VAR)|TrackBar(x,y,VAR)|background(x,y)|PannelLive(x,y,VAR)|PannelLevel(x,y,VAR));
}else if (render0_picture1==1) {
SSD1306.ssd1306_send_byte(pgm_read_byte(&MAIN[x+(y*128)]));
}else if (render0_picture1==2) {
SSD1306.ssd1306_send_byte(background(x,y));
}}
SSD1306.ssd1306_send_data_stop();
}}

uint8_t PannelLevel(uint8_t X,uint8_t Y,GROUPE *VAR){
if ((Y<5)||(Y>6)||(X<117)||(X>123)) return 0x00;
#define VAl10 (VAR->LEVEL/10)
#define VAl01 (VAR->LEVEL-(VAl10*10))
if (Y==5) {return (pgm_read_byte(&DIGITAL[(X-117)+(VAl10*7)]));}
else if (Y==6) {return (pgm_read_byte(&DIGITAL[(X-117)+(VAl01*7)]));}
return 0x00;
}

uint8_t Block(uint8_t X,uint8_t Y,GROUPE *VAR){
uint8_t XValue=255;
if ((X>=67)&&(X<97)&&(Y>=1)&&(Y<=6)) {
if ((X>=67)&&(X<73)) XValue=0;
else if ((X>=73)&&(X<79)) XValue=1;
else if ((X>=79)&&(X<85)) XValue=2;
else if ((X>=85)&&(X<91)) XValue=3;
else if ((X>=91)&&(X<97)) XValue=4;
if (XValue==255) return 0x00;
//uint8_t XValue=((X-67)/6);
if (VAR->BlocsGrid[(Y-1)][XValue]==255) return 0x00;
if (VAR->BlocsGrid[(Y-1)][XValue]==5) {return pgm_read_byte(&BLOCKREFLECT[((X-67)-(XValue*6))+(VAR->ANIMREFLECT*6)])|pgm_read_byte(&BLOCK[((X-67)-(XValue*6))+((VAR->BlocsGrid[(Y-1)][XValue])*6)]);}
return pgm_read_byte(&BLOCK[((X-67)-(XValue*6))+((VAR->BlocsGrid[(Y-1)][XValue])*6)]);
}return 0x00;}

uint8_t RecupeDecalageY(uint8_t Valeur){
while(Valeur>7){Valeur=Valeur-8;}
return Valeur;
}

uint8_t Ball(uint8_t X,uint8_t Y,GROUPE *VAR){
#define BALLXPOS (VAR->Ballxpos-1)
#define BALLYPOS (VAR->Ballypos-1)
 if (Y<VAR->Ypos) return 0x00;
 if (Y>(VAR->Ypos+1)) return 0x00;
 if ((X-uint8_t(BALLXPOS))<0) return 0x00;
 if (X<BALLXPOS) return 0x00;
 if (X>BALLXPOS+2) return 0x00;
if (VAR->BALLyDecal==0)  {
if (Y==VAR->Ypos ) {return (pgm_read_byte(&BALL[(X-uint8_t(BALLXPOS))]));}
  }else{
uint8_t DECAL=RecupeDecalageY(BALLYPOS);
if (Y==VAR->Ypos) { return SplitSpriteDecalageY(DECAL,pgm_read_byte(&BALL[(X-uint8_t(BALLXPOS))]),1);}
if (Y==(VAR->Ypos)+1) { return SplitSpriteDecalageY(DECAL,pgm_read_byte(&BALL[(X-uint8_t(BALLXPOS))]),0);}
}return 0x00;}

uint8_t SplitSpriteDecalageY(uint8_t decalage,uint8_t Input,uint8_t UPorDOWN){
if (UPorDOWN) {
return Input<<decalage;
}else{
return Input>>(8-decalage); 
}}

uint8_t TrackBar(uint8_t X,uint8_t Y,GROUPE *VAR){
if (X>6) return 0;
if (X<3) return 0;
if (Y>=(3+VAR->TrackBary)) return 0;
if (Y<(VAR->TrackBary)) return 0;
if (VAR->TrackBaryDecal==0){if (Y!=VAR->TrackBary+2){ return (pgm_read_byte(&TRACKBAR[(X-3)+((Y-VAR->TrackBary)*4)]));
}}else{
if (Y==VAR->TrackBary) {return SplitSpriteDecalageY(VAR->TrackBaryDecal,pgm_read_byte(&TRACKBAR[(X-3)]),1);}
else if (Y==VAR->TrackBary+1) {return SplitSpriteDecalageY(VAR->TrackBaryDecal,pgm_read_byte(&TRACKBAR[(X-3)]),0)|SplitSpriteDecalageY(VAR->TrackBaryDecal,pgm_read_byte(&TRACKBAR[(X-3)+4]),1);}
else if ((Y==VAR->TrackBary+2)&&(VAR->TrackBaryDecal!=0)) {return SplitSpriteDecalageY(VAR->TrackBaryDecal,pgm_read_byte(&TRACKBAR[(X-3)+4]),0);}
}return 0x00;}

uint8_t PannelLive(uint8_t X,uint8_t Y,GROUPE *VAR){
if ((Y<1)||(Y>VAR->live)||(X>121)||(X<119)) return 0x00;
return (pgm_read_byte(&LIVE[X-119]));}

uint8_t background(uint8_t X,uint8_t Y){
if (((Y>0)&&(Y<7)&&(X<106))) return 0;
return (pgm_read_byte(&back[X+(Y*128)]));}

void Sound(uint8_t freq,uint8_t dur){
if (freq==0) {delay(dur);goto END;}
for (uint8_t t=0;t<dur;t++){
if (freq!=0)digitalWrite(4,HIGH); 
for (uint8_t t=0;t<(255-freq);t++){
_delay_us(1);}
digitalWrite(4,LOW);
for (uint8_t t=0;t<(255-freq);t++){
_delay_us(1); }}
END:;
}

void LoadLevel(uint8_t Level,GROUPE *VAR){
uint8_t a,b;
for(b=0;b<5;b++){
for(a=0;a<6;a++){
VAR->BlocsGrid[a][b]=pgm_read_byte(&LEVEL[(Level*30)+b+(a*5)]);
}}}

void ResetVar(GROUPE *VAR){
uint8_t f=VAR->LEVEL;
while(1){if (f>4) {f=f-5;}else{break;}}
LoadLevel(f,VAR);
VAR->launch=0;}

void ResetBall(GROUPE *VAR){
VAR->ANIMREFLECT=0;
VAR->TrackBary=2;
VAR->TrackBaryDecal=4;
VAR->Ballxpos=8;
VAR->SIMBallxpos=8;
VAR->Ballypos=32;
VAR->SIMBallypos=32;
VAR->BallSpeedx=1;
VAR->SIMBallSpeedx=1;
VAR->BallSpeedy=.50;
VAR->SIMBallSpeedy=.50;
VAR->launch=0;
}


