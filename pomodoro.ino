#include <M5StickC.h>
#include <Arduino.h>

const int pomodoroTime = 25; //How long in minuites the pomodoros should be.
const int breakTime = 5; //How long in minuites the breaks should be.
const int longBreakTime = 30; //How long in minuites the final 'long break' should be.
const int pomodoroNumber = 4; //How many Pomodoros in a cycle.
int stage;
int countdown;
bool onBreak;
bool onFinal;
bool alarming;
String displayText = "";

RTC_TimeTypeDef TimeStruct;

void alarm(){
  digitalWrite(10, LOW); //Turns on LED
  delay(500);
  M5.Lcd.setCursor(25, 40);
  M5.Lcd.setTextSize(4);
  M5.Lcd.print("  :  ");
  digitalWrite(10, HIGH); //Turns off LED
}

void pomodoroStep(){
  onBreak = 0;
  countdown = pomodoroTime;
  displayText = "Focus";
}

void breakStep(){
  onBreak = 1;
  countdown = breakTime;
  displayText = "Break";
}

void longBreakStep(){
  onFinal = 1;
  countdown = longBreakTime;
  displayText = "Relax";
}

void drawPomodoro(int slot, bool fill){
  int pomodoroPosition = 150 - (17 * slot);
  if(fill){
    M5.Lcd.fillCircle(pomodoroPosition, 12, 6, RED);
    M5.Lcd.drawPixel(pomodoroPosition, 13, WHITE);
    M5.Lcd.drawPixel(pomodoroPosition + 2, 13, WHITE);
    M5.Lcd.drawPixel(pomodoroPosition + 2, 11, WHITE);
  } else {
    M5.Lcd.drawCircle(pomodoroPosition, 12, 6, RED);
  }
  M5.Lcd.drawPixel(pomodoroPosition, 7, GREEN);
  M5.Lcd.drawPixel(pomodoroPosition, 6, GREEN);
  M5.Lcd.drawPixel(pomodoroPosition, 5, GREEN);
  M5.Lcd.drawPixel(pomodoroPosition - 1, 6, GREEN);
  M5.Lcd.drawPixel(pomodoroPosition + 1, 6, GREEN);
}

void reset(){
  alarming = 0;
  delay(1000);
  TimeStruct.Hours = 0;
  TimeStruct.Minutes = 0;
  TimeStruct.Seconds = 0;
  M5.Rtc.SetTime(&TimeStruct);
}

void hardReset(){
  M5.Lcd.fillScreen(BLACK);
  stage = 0;
  onFinal = 0;
  pomodoroStep();
  for (int i = 0; i < pomodoroNumber; i++) {
    drawPomodoro(i,0);
  }
  reset();
}


void setup() {
  M5.begin();
  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  pinMode(10, OUTPUT);

  alarm();
  hardReset();
}

void loop() {
  int resetButton = digitalRead(39);
  if(resetButton == 0){
    hardReset();
  }
  int snoozeButton = digitalRead(37);
  if(snoozeButton == 0){
    reset();
  }

  if(TimeStruct.Minutes == countdown - 1 && TimeStruct.Seconds == 59 && !alarming){
    drawPomodoro(stage,1);
    if (onFinal){
      hardReset();
    } else if(onBreak){
      pomodoroStep();
      stage++;
    } else if(stage < (pomodoroNumber - 1)){
      breakStep();
    } else {
      longBreakStep();
    }
    alarming = 1;
  }

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(5, 5);
  M5.Lcd.printf("%s", displayText);
  M5.Rtc.GetTime(&TimeStruct);
  M5.Lcd.setCursor(25, 40);
  M5.Lcd.setTextSize(4);
  if(alarming){
      M5.Lcd.print("00 00");
      alarm();
  } else{
     M5.Lcd.printf("%02d:%02d", countdown - TimeStruct.Minutes - 1, 59 - TimeStruct.Seconds);
  }
  delay(500);
}
