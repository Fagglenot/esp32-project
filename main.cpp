#include <Keypad.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

// --- Configuration & Pin Definitions ---
int red_led = 13;
int green_led = 14;
int buzzer = 25;
const int servoPin = 26;
const int echo = 33;
const int trig = 32;
// SDA 21
// SCL 22

// Keypad Setup
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = { 19, 18, 5, 17 };
byte colPins[COLS] = { 16, 4, 27, 15 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change 0x27 to your LCD address if different

// --- Variables ---
String password = "123456"; // Default password
String inputPassword = "";
int attempts = 0;
bool isLocked = true;
bool systemActive = false;

unsigned long lastMoveTime = 0;

void setup() {
  Serial.begin(115200);
  
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  myServo.attach(servoPin);
  myServo.write(90); // Ensure safe is locked at start

  lcd.init();
  lcd.backlight();
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();
  lcd.noBacklight(); // Save power until ultrasonic wakes it up
}

void loop() {
  long distance = getDistance();

  // 1. Wake up system if movement within 10cm
  if (distance > 0 && distance <= 10 && !systemActive && isLocked) {
    activateSystem();
  }

  if (systemActive) {
    char key = keypad.getKey();
    
    if (key) {
      handleKeypress(key);
    }

    // 2. Logic for open door (Movement check to close)
    if (!isLocked) {
      checkAutoClose();
    }
  }
}

// --- Helper Functions ---

long getDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  return pulseIn(echo, HIGH) * 0.034 / 2;
}

void activateSystem() {
  systemActive = true;
  lcd.backlight();
  lcd.clear();
  lcd.print("Sistem Aktif");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print("Masukan Pass:");
}

void handleKeypress(char key) {
  // Reset password feature (Only if door is OPEN)
  if (key == '#' && !isLocked) {
    resetPasswordMode();
    return;
  }

  // Handle standard number input
  if (key != '#' && key != '*') {
    inputPassword += key;
    lcd.setCursor(inputPassword.length() - 1, 1);
    lcd.print("*"); // Mask password
  }

  // Check password once 6 digits are reached
  if (inputPassword.length() == 6) {
    if (inputPassword == password) {
      accessGranted();
    } else {
      accessDenied();
    }
  }
}

void accessGranted() {
  lcd.clear();
  lcd.print("Akses Diterima");
  digitalWrite(green_led, HIGH);
  myServo.write(0); // Open door
  isLocked = false;
  attempts = 0;
  inputPassword = "";
  lastMoveTime = millis();
}

void accessDenied() {
  attempts++;
  inputPassword = "";
  digitalWrite(red_led, HIGH);
  lcd.clear();
  lcd.print("Password Salah");
  delay(1000);
  digitalWrite(red_led, LOW);

  if (attempts >= 3) {
    lcd.clear();
    lcd.print("Akses Ditolak");
    digitalWrite(red_led, HIGH);
    tone(buzzer, 500); // Alarm sound
    delay(10000); // 10 second lockout
    noTone(buzzer);
    digitalWrite(red_led, LOW);
    attempts = 0;
    resetToInitial();
  } else {
    lcd.clear();
    lcd.print("Masukan Pass:");
  }
}

void checkAutoClose() {
  long currentDist = getDistance();

  if (currentDist > 0 && currentDist <= 10) {
    lastMoveTime = millis(); // Refresh timer if someone is there
  }

  // If 30 seconds pass without movement
  if (millis() - lastMoveTime > 30000) {
    myServo.write(90); // Lock door
    digitalWrite(green_led, LOW);
    isLocked = true;
    resetToInitial();
  }
}

void resetPasswordMode() {
  lcd.clear();
  lcd.print("Pass Baru (6):");
  inputPassword = "";
  while (inputPassword.length() < 6) {
    char key = keypad.getKey();
    if (key && key != '#' && key != '*') {
      inputPassword += key;
      lcd.setCursor(inputPassword.length() - 1, 1);
      lcd.print(key);
    }
  }
  password = inputPassword;
  inputPassword = "";
  lcd.clear();
  lcd.print("Pass Diganti!");
  delay(2000);
  lcd.clear();
  lcd.print("Pintu Terbuka");
}

void resetToInitial() {
  systemActive = false;
  lcd.clear();
  lcd.noBacklight();
  inputPassword = "";
}