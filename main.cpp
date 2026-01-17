#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
Servo myservo;
const int servo = 26;
const int echo = 19;
const int trig = 18;
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
  lcd.setCursor(0,0);
  lcd.print("Enter your passcode:");
  int myValue = Serial.parseInt();
  lcd.setCursor(0,1);
  lcd.print(myValue);
  if(myValue == 189023){
    myservo.write(90);
    lcd.print("Access Granted");
    delay(5000);
    lcd.clear();
  }
}

