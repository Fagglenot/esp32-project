#include <Arduino.h>
#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SPI.h>
#include <MFRC522.h>

// LCD dan Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo myservo;

// Pin definitions (PERBAIKI)
const int servo = 26;
const int echo = 19;
const int trig = 18;
const int RST_PIN = 4;    // PINDAH dari 22
const int SS_PIN = 5;     // PINDAH dari 21

// Keypad setup (4x4)
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27};
byte colPins[COLS] = {25, 33, 32, 15};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// RFID setup
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Password dan UID RFID yang diizinkan
const String password = "189023";
byte authorizedUID[] = {0xDE, 0xAD, 0xBE, 0xEF}; // Ganti dengan UID kartu Anda

String inputPassword = "";
int chances = 3;
bool systemLocked = false;
unsigned long lockTime = 0;
const unsigned long LOCK_DURATION = 30000; // 30 detik lockout

void setup() {
  myservo.attach(servo);
  lcd.init();
  lcd.backlight();
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  
  myservo.write(0); // Posisi terkunci
  
  Serial.println("System Ready");
  Serial.println("Waiting for authentication...");
}

void loop() {
  // Cek jarak ultrasonik
  long distance = getDistance();
  
  // Reset jika tidak ada orang di depan brankas
  if (distance > 10) {
    myservo.write(0);
    inputPassword = "";
    lcd.clear();
    return;
  }
  
  // Cek apakah sistem terkunci
  if (systemLocked) {
    if (millis() - lockTime < LOCK_DURATION) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("System Locked");
      lcd.setCursor(0, 1);
      lcd.print("Wait ");
      lcd.print((LOCK_DURATION - (millis() - lockTime)) / 1000);
      lcd.print("s");
      delay(1000);
      return;
    } else {
      // Reset setelah lockout selesai
      systemLocked = false;
      chances = 3;
      inputPassword = "";
    }
  }
  
  // Tampilkan menu utama
  displayMainScreen();
  
  // Cek RFID
  if (checkRFID()) {
    grantAccess("RFID");
    delay(5000);
    inputPassword = "";
    return;
  }
  
  // Cek input keypad
  char key = keypad.getKey();
  if (key) {
    handleKeypadInput(key);
  }
}

long getDistance() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH);
  return (duration * 0.034 / 2);
}

void displayMainScreen() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 100) {
    lcd.setCursor(0, 0);
    lcd.print("Enter PIN/RFID:");
    lcd.setCursor(0, 1);
    
    // Tampilkan password dengan bintang
    for (int i = 0; i < inputPassword.length(); i++) {
      lcd.print("*");
    }
    for (int i = inputPassword.length(); i < 6; i++) {
      lcd.print(" ");
    }
    
    lastUpdate = millis();
  }
}

void handleKeypadInput(char key) {
  if (key == '#') {
    // Tombol Enter
    checkPassword();
  } else if (key == '*') {
    // Tombol Clear
    inputPassword = "";
    lcd.clear();
  } else if (key >= '0' && key <= '9') {
    // Input angka
    if (inputPassword.length() < 6) {
      inputPassword += key;
      Serial.println("Input: " + inputPassword);
    }
  }
}

void checkPassword() {
  if (inputPassword == password) {
    grantAccess("Keypad");
    chances = 3; // Reset chances
  } else {
    denyAccess();
  }
  inputPassword = "";
}

bool checkRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  
  // Cek apakah UID cocok
  bool authorized = true;
  if (mfrc522.uid.size != sizeof(authorizedUID)) {
    authorized = false;
  } else {
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] != authorizedUID[i]) {
        authorized = false;
        break;
      }
    }
  }
  
  // Print UID untuk debugging
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
  
  mfrc522.PICC_HaltA();
  
  if (authorized) {
    chances = 3; // Reset chances
    return true;
  } else {
    denyAccess();
    return false;
  }
}

void grantAccess(String method) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Granted");
  lcd.setCursor(0, 1);
  lcd.print("via " + method);
  
  myservo.write(90); // Buka kunci
  
  Serial.println("Access granted via " + method);
  
  delay(5000);
  myservo.write(0); // Kunci kembali
  lcd.clear();
}

void denyAccess() {
  chances--;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong Password!");
  lcd.setCursor(0, 1);
  
  if (chances > 0) {
    lcd.print("Tries left: ");
    lcd.print(chances);
    Serial.println("Wrong password. Tries left: " + String(chances));
  } else {
    lcd.print("Access Denied!");
    systemLocked = true;
    lockTime = millis();
    Serial.println("System locked for 30 seconds");
  }
  
  delay(2000);
  lcd.clear();
}