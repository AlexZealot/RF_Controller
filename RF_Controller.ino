/*
 * Довольно простой вариант реализации пульта дистанционного управления на базе nRF24L01
 */
#include "Button.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//#define __DEBUG__

#define   PIN_BUTTON_1    5
#define   PIN_BUTTON_2    4
#define   PIN_BUTTON_3    6
#define   PIN_BUTTON_4    7
#define   PIN_SWITCH_1    8
#define   PIN_SWITCH_2    1
#define   PIN_J1_BUT      3
#define   PIN_J2_BUT      2
#define   PIN_POT_1       A5
#define   PIN_POT_2       A4
#define   PIN_J1_V        A2
#define   PIN_J1_H        A3
#define   PIN_J2_V        A1
#define   PIN_J2_H        A0

RF24 radio(9,10); // Для Uno-Nano-Pro Mini
Button But1{PIN_BUTTON_1}, But2{PIN_BUTTON_2}, But3{PIN_BUTTON_3}, But4{PIN_BUTTON_4}, Joy1{PIN_J1_BUT}, Joy2{PIN_J2_BUT};

#ifdef __DEBUG__
bool  flag = false;
bool  lastFlag  = false;
#endif

byte address[6] = {"Zpipe"}; 

struct  _Joistick {
  byte  button = 0;
  int   V = 0;
  int   H = 0;
};

struct DataBuf {
  byte              Button1 = 0;
  byte              Button2 = 0;
  byte              Button3 = 0;
  byte              Button4 = 0;
  byte              Switch1 = 0;
  byte              Switch2 = 0;
  int               Pot1    = 0;
  int               Pot2    = 0;
  _Joistick         J1;
  _Joistick         J2;
};

DataBuf datapack;

void setup() {
  /*настройка радиоканала*/
  SetUpRadio();

  /*настройка пинов*/
  SetUpPins();

  #ifdef __DEBUG__

  Serial.begin(9600);
  
  #endif
}

void loop() {
  //читаем данные
  ReadInputs();

  #ifndef __DEBUG__
  //шлём данные на приёмник
  TransmitData();
  #endif

  #ifdef __DEBUG__

  Serial.print("Button 1 \t"); Serial.print(datapack.Button1);
  Serial.print("\tButton 2 \t"); Serial.print(datapack.Button2);
  Serial.print("\tButton 3 \t"); Serial.print(datapack.Button3);
  Serial.print("\tButton 4 \t"); Serial.print(datapack.Button4);
  Serial.print("\tSwitch 1 \t"); Serial.print(datapack.Switch1);
  Serial.print("\tSwitch 2 \t"); Serial.print(datapack.Switch2);
  Serial.print("\tPot 1 \t"); Serial.print(datapack.Pot1);
  Serial.print("\tPot 2 \t"); Serial.print(datapack.Pot2);
  Serial.print("\tJ1B \t"); Serial.print(datapack.J1.button);
  Serial.print("\tJ1V \t"); Serial.print(datapack.J1.V);
  Serial.print("\tJ1H \t"); Serial.print(datapack.J1.H);
  Serial.print("\tJ2B \t"); Serial.print(datapack.J2.button);
  Serial.print("\tJ2V \t"); Serial.print(datapack.J2.V);
  Serial.print("\tJ2H \t"); Serial.print(datapack.J2.H);
  Serial.println();
  
  #endif
}

void TransmitData(){
  radio.write(&datapack, sizeof(datapack));
}


void ReadInputs(){
  unsigned long T = millis();
  But1.update(T); 
  But2.update(T); 
  But3.update(T); 
  But4.update(T); 
  Joy1.update(T); 
  Joy2.update(T); 

  //кнопки
  datapack.Button1  = But1.isDown();
  datapack.Button2  = But2.isDown();
  datapack.Button3  = But3.isDown();
  datapack.Button4  = But4.isDown();

  //переключатели
  datapack.Switch1  = !digitalRead(PIN_SWITCH_1);
  datapack.Switch2  = !digitalRead(PIN_SWITCH_2);

  //потенциометры
  datapack.Pot1     = 1023 - analogRead(PIN_POT_1);
  datapack.Pot2     = 1023 - analogRead(PIN_POT_2);

  //джойстики
  datapack.J1.button  = Joy1.isDown();
  datapack.J1.V       = analogRead(PIN_J1_V);
  datapack.J1.H       = analogRead(PIN_J1_H);

  //КОСТЫЛЬ ПО ПРИЧИНЕ КОРЯВЫХ ПОТЕНЦИОМЕТРОВ ДЖОЙСТИКОВ
  /*if (datapack.J1.H <= 9) datapack.J1.H = map(datapack.J1.H, 0, 9, 0, 512);
  else datapack.J1.H = map(datapack.J1.H, 280, 10, 1023, 513);*/

  if (datapack.J1.H > 1023) datapack.J1.H = 1023;
  if (datapack.J1.H < 0) datapack.J1.H = 0;
  //КОНЕЦ КОСТЫЛЯ
  //ВНЕЗАПНО, ЗАРАБОТАЛО НОРМАЛЬНО!
 
  datapack.J2.button  = Joy2.isDown();
  datapack.J2.V       = analogRead(PIN_J2_V);
  datapack.J2.H       = analogRead(PIN_J2_H);

  /*МЁРТВЫЕ ЗОНЫ ДЖОЙСТИКОВ*/
  if (datapack.J1.V >= 500 && datapack.J1.V <= 600)
    datapack.J1.V       = 513;
  if (datapack.J1.H >= 500 && datapack.J1.H <= 600)
    datapack.J1.H       = 513;
  if (datapack.J2.V >= 500 && datapack.J2.V <= 600)
    datapack.J2.V       = 513;
  if (datapack.J2.H >= 500 && datapack.J2.H <= 600)
    datapack.J2.H       = 513;
  /*МЁРТВЫЕ ЗОНЫ ДЖОЙСТИКОВ*/  
}

void SetUpPins(){
  pinMode(PIN_SWITCH_1, INPUT_PULLUP);
  pinMode(PIN_SWITCH_2, INPUT_PULLUP);  
  pinMode(PIN_POT_1   , INPUT);
  pinMode(PIN_POT_2   , INPUT);
  pinMode(PIN_J1_V    , INPUT);
  pinMode(PIN_J1_H    , INPUT);
  pinMode(PIN_J2_V    , INPUT);
  pinMode(PIN_J2_H    , INPUT);  
}

void SetUpRadio(){
  radio.begin(); 
  radio.setAutoAck(1);
  radio.setRetries(0,15);

  radio.openWritingPipe(address);
  radio.setChannel(0x30);

  radio.setPALevel (RF24_PA_MAX); //RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

  radio.powerUp();
  radio.stopListening();  
}
