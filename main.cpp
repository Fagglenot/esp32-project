#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;
const int servo = 26;
const int echo = 19;
const int trig = 18;
int myValue;
int chances = 3;
bool legit = true;
void setup() {
  myservo.attach(servo);
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
}

void loop() {
  long duration, distance;
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  distance = (duration*0.034/ 2);
  lcd.clear();
  if(distance > 10){
    myservo.write(0);
    return;
  }
  while(chances!=0){
    lcd.setCursor(0,0);
    if(legit) lcd.print("Enter your passcode:");
    while (Serial.available() == 0) {
      delay(10);
    }
    String input = Serial.readStringUntil('\n');
    input.trim();
    myValue = input.toInt();
    while (Serial.available()) Serial.read();
    legit = false;
    lcd.setCursor(0,1);
    lcd.print(myValue);
    if(myValue == 189023){
      myservo.write(90);
      lcd.setCursor(0,2);
      lcd.print("Access Granted");
      delay(5000);
      lcd.clear();
      break;
    }
    else{
      chances--;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Wrong Passcode");
      lcd.setCursor(0,1);
      lcd.print("You have ");
      lcd.print(chances);
      lcd.print(" tries left");
      delay(2000);
      lcd.clear();
      legit = true;
      }
  }
  if(chances==0){
    lcd.clear();
    lcd.print("No tries left");
    lcd.setCursor(0,1);
    lcd.print("Access Denied");
    delay(5000);
    lcd.clear();
    legit = false;
    }
  
}


