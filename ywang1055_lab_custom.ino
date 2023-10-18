/* Your Name & E-mail: Yishao Wang <ywang1055@ucr.edu>
* Lab Section: 21
* Assignment: Custom Lab Assignment - DDRPad                               
* Exercise Description: [optional - include for your own benefit]
*
* I acknowledge all content contained herein, excluding 
*   template or example code, is my own original work.
*
* Demo Link: 
*/
#include <Joystick.h>
#include <EEPROM.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// FSR
Joystick_ Joystick;
const int fsr0 = 18;
const int fsr1 = 19;
const int fsr2 = 20;
const int fsr3 = 21;
int fsr0_read;
int fsr1_read;
int fsr2_read;
int fsr3_read;
int fsr0_threashold = 300;
int fsr1_threashold = 300;
int fsr2_threashold = 300;
int fsr3_threashold = 300;

// Menu / LCD
byte menuCount = 0;
byte idleTimer = 0;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Tasks
typedef struct task {
  int state;
  unsigned long period;
  unsigned long elapsedTime;
  int (*TickFct)(int);
} task;

const unsigned short tasksNum = 2;
task tasks[tasksNum];

enum Pad_States { PAD_START };
int TickFct_Pad(int state);

enum Lcd_States { LCD_START, LCD_MENU, LCD_MENUCHECK };
int TickFct_Lcd(int state);

void setup() {
  pinMode(fsr0, INPUT_PULLUP);
  pinMode(fsr1, INPUT_PULLUP);
  pinMode(fsr2, INPUT_PULLUP);
  pinMode(fsr3, INPUT_PULLUP);
  Serial.begin(9600);
  Joystick.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(WHITE);

  unsigned char i = 0;
  tasks[i].state = PAD_START;
  tasks[i].period = 1;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_Pad;
  i++;
  tasks[i].state = LCD_START;
  tasks[i].period = 300;
  tasks[i].elapsedTime = 0;
  tasks[i].TickFct = &TickFct_Lcd;
}

int TickFct_Pad(int state){
  switch(state){ // state transition
    case PAD_START:
      state = PAD_START;
      break;
      }
   switch(state){ // state action
    case PAD_START:
      fsr0_read = analogRead(fsr0);
      fsr1_read = analogRead(fsr1);
      fsr2_read = analogRead(fsr2);
      fsr3_read = analogRead(fsr3);
      if(fsr0_read > fsr0_threashold){
        Joystick.setButton(0, 1);
      }
      else{
        Joystick.setButton(0, 0);
      }
      if(fsr1_read > fsr1_threashold){
        Joystick.setButton(1, 1);
      }
      else{
        Joystick.setButton(1, 0);
      }
      if(fsr2_read > fsr2_threashold){
        Joystick.setButton(2, 1);
      }
      else{
        Joystick.setButton(2, 0);
      }
      if(fsr3_read > fsr3_threashold){
        Joystick.setButton(3, 1);
      }
      else{
        Joystick.setButton(3, 0);
      }
      break;
  }
  return state;
}

void menuNav(){
  if(digitalRead(8) == HIGH){
    menuCount++;
  }
  if(menuCount > 5){
    menuCount = 0;
  }
  if(menuCount == 0){
    if(digitalRead(9) == HIGH){
      fsr0_threashold += 20;
    }
    else if(digitalRead(7) == HIGH){
      fsr0_threashold -= 20;
    }
  }
  if(menuCount == 1){
    if(digitalRead(9) == HIGH){
      fsr1_threashold += 20;
    }
    else if(digitalRead(7) == HIGH){
      fsr1_threashold -= 20;
    }
  }
  if(menuCount == 2){
    if(digitalRead(9) == HIGH){
      fsr2_threashold += 20;
    }
    else if(digitalRead(7) == HIGH){
      fsr2_threashold -= 20;
    }
  }
  if(menuCount == 3){
    if(digitalRead(9) == HIGH){
      fsr3_threashold += 20;
    }
    else if(digitalRead(7) == HIGH){
      fsr3_threashold -= 20;
    }
  }
}

int TickFct_Lcd(int state){
  switch(state){  //state transition
    case LCD_START:
      idleTimer = 0;
      if(digitalRead(7) == HIGH || digitalRead(8) == HIGH || digitalRead(9) == HIGH){
        state = LCD_MENU;
        }
      else{
        state = LCD_START;
        }
      break;
    case LCD_MENU:
      state = LCD_MENU;
      menuNav();
      if(digitalRead(7) == LOW && digitalRead(8) == LOW && digitalRead(9) == LOW){
        idleTimer++;
      }
      else{
        idleTimer = 0;
      }
      if(idleTimer >= 21){
        state = LCD_START;
      }
      break;
  }
  switch(state){  //state action
    case LCD_START:
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextSize(2);
      display.print("FSRDDR Pad");
      display.setCursor(0,30);
      display.setTextSize(1);
      display.print("by: Yishao Wang");
      display.setCursor(0,40);
      display.print("Press any button to \n");
      display.print("enter config");
      display.display(); 
      break;
    case LCD_MENU:
      display.clearDisplay();
      
      display.setTextSize(1);
      display.setCursor(10, 0);
      display.println("FSR0:");
      display.setCursor(60, 0);
      display.println(fsr0_threashold);
    
      display.setCursor(10, 10);
      display.println("FSR1:");
      display.setCursor(60, 10);
      display.println(fsr1_threashold);
    
      display.setCursor(10, 20);
      display.println("FSR2:");
      display.setCursor(60, 20);
      display.println(fsr2_threashold);
    
      display.setCursor(10, 30);
      display.println("FSR3:");
      display.setCursor(60, 30);
      display.println(fsr3_threashold);

      display.setCursor(10, 40);
      display.println("LED TYPE:");
      display.setCursor(70, 40);
      display.println("STATIC");

      display.setCursor(10, 50);
      display.println("PAD TYPE:");
      display.setCursor(70, 50);
      display.println("JOY");
      
      display.setCursor(2, (menuCount * 10));
      display.println(">");
    
      display.display();
      break;
  }
  return state;
}

void loop() {
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) {
     if ( (millis() - tasks[i].elapsedTime) >= tasks[i].period) {
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = millis(); // Last time this task was ran
     }
   }
  delay(1);
}
