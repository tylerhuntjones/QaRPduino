/*
 * KK4OIX's 'QaRPduino-CW' transmitter - Arduino Source Code - Version 0.1.1
 * Copyright (C) 2013, Tyler H. Jones - me@tylerjones.me (February 2013)
 * http://tylerjones.me
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

//#include <LiquidCrystal.h>
#include <Morse.h>

#define BTN1 13
#define BTN3 12
#define LED_BUSY 9
#define LED_READY 8

//
// Usage: morse( <pin number>, <speed WPM>, <1=beep, 0=PTT> )  i.e. Speaker=beep, LED=PTT
//        sendmsg( "<text-to-send>" )
//
Morse morse(9, 12, 0);

boolean isBusy = false;
//LiquidCrystal lcd(11, 10, 9, 8, 7, 6); //RS, EN, D0, D1, D2, D3

int btn1_lastState = 0;
int btn3_lastState = 0;

//Serial data buffer variables and defines
#define MAX_DATA_LEN 120
#define SOP "["
#define EOP "]"
boolean SerialActive = false;
int Pointer = 0;
char SerialData[MAX_DATA_LEN+1];
char PreviousSerialData[MAX_DATA_LEN+1];


void setup() {
  Serial.begin(9600);

  pinMode(BTN1, INPUT);
  pinMode(BTN3, INPUT);
  pinMode(LED_BUSY, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  
  Serial.println("KK4OIX's QaRPduino-CW Tranmitter - Sofware Verion 0.1.1");
  
  setLEDReady(HIGH);
  
  /*
  lcd.begin(20, 2);
  lcd.noCursor();
  lcd.noBlink();
  lcd.noAutoscroll();
  delay(5);
  lcd.clear();
  lcd.print("Booting up");
  delay(5);
  for(int i=0;i<10;i++) {
    lcd.setCursor(i,1);
    lcd.print(".");  
    delay(200); 
  }
  */
}

void loop() {
  
  while(Serial.available() > 0) {
    char datain = Serial.read();
    if(datain == '[') {
      SerialActive = true;
      Pointer = 0;
      SerialData[Pointer] = '\0';
    } else if(datain == ']') {
      SerialActive = false;
      break;
    } else if(Pointer >= MAX_DATA_LEN) {
      Serial.println("The data sent to the Arduino was too long! MAX 120 chars!!!");
      SerialActive = false;
      break;
    } else {
      if(Pointer < MAX_DATA_LEN - 1 && SerialActive) {
        SerialData[Pointer] = datain;
        Pointer++;
        SerialData[Pointer] = '\0';
      } 
    }
  }
  if(!SerialActive && Pointer > 0 && SerialData[Pointer-1] != '\0') {
     Serial.println("Processing received data...");
     Serial.println("Encoding Morse code from received data:");
     Serial.println((String)SerialData);
     SendCode(SerialData);
     delay(50*Pointer);
     for(int i=0;i<Pointer;i++) {
        PreviousSerialData[i] = SerialData[i];
        SerialData[i] = '\0';
     }
     for(int j=Pointer;j<MAX_DATA_LEN;j++) {
       PreviousSerialData[j] = '\0';  
     }
     Pointer = 0;
     Serial.println("Sent Morse code message to transmitter.");
  }  
   /* 
    do {
      SerialBuffer[BufferLen] = Serial.read();
      if(SerialBuffer[BufferLen] == MSG_BREAK_CHAR) break;
    } (BufferLen++ < MAX_BUFFER_LEN);
    
    
  }*/
  
  int btn1_State = digitalRead(BTN1);
  int btn3_State = digitalRead(BTN3);
  if(btn1_State == HIGH || btn3_State == HIGH) {
    if(btn1_State != btn1_lastState) {
      if(btn1_State == HIGH) {
        Serial.println("Button 1 PRESSED!");
        Serial.println("Sending example Morse code string: 'HELLO WORLD'...");
        SendCode();        
      } else {
        Serial.println("Button 1 RELEASED!");
      }
    }
    if(btn3_State != btn3_lastState) {
      if(btn3_State == HIGH) {
        Serial.println("Button 3 PRESSED!");
        //Serial.readln();
        SendCode("MORSE CODE TEST.");
       } else {
        Serial.println("Button 3 RELEASED!");
       }
      }
    btn3_lastState = btn3_State;
    btn1_lastState = btn1_State;  }
      
  delay(5);
  
}

void setLEDReady(int stat) {
  if(stat == HIGH) {
    digitalWrite(LED_READY, HIGH);
    digitalWrite(LED_BUSY, LOW); 
  } else {
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, LOW);  
  }
}

void SendCode() {
  SendCode("HELLO WORLD!");
}

void SendCode(char *msg) {
  setLEDReady(LOW);
  isBusy=true;
  morse.sendmsg(msg);
  delay(2000);
  isBusy=false;
  setLEDReady(HIGH); 
}
