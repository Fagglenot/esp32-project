### **📌 TABEL PIN FINAL (TANPA KONFLIK)**

| Komponen | Pin | ESP32 GPIO |
|----------|-----|------------|
| **LCD I2C** | SDA | GPIO 21 |
| | SCL | GPIO 22 |
| | VCC | 5V |
| | GND | GND |
| **Servo** | Signal | GPIO 26 |
| | VCC | 5V (eksternal) |
| | GND | GND |
| **Ultrasonic** | TRIG | GPIO 18 |
| | ECHO | GPIO 19 |
| | VCC | 5V |
| | GND | GND |
| **RFID MFRC522** | RST | **GPIO 4** ⬅️ Dipindah |
| | SDA/SS | **GPIO 5** ⬅️ Dipindah |
| | MOSI | GPIO 23 |
| | MISO | GPIO 19 |
| | SCK | GPIO 18 |
| | 3.3V | 3.3V |
| | GND | GND |
| **Keypad** | Row 1 | GPIO 13 |
| | Row 2 | GPIO 12 |
| | Row 3 | GPIO 14 |
| | Row 4 | GPIO 27 |
| | Col 1 | GPIO 25 |
| | Col 2 | GPIO 33 |
| | Col 3 | GPIO 32 |
| | Col 4 | GPIO 15 |

---

## **⚡ KONEKSI POWER**

### **Dari ESP32:**
| Pin ESP32 | Ke Komponen |
|-----------|-------------|
| **5V** | LCD VCC, Ultrasonic VCC |
| **3.3V** | RFID VCC |
| **GND** | Semua GND komponen (bisa paralel) |

### **Power Supply Eksternal (Rekomendasi):**
```
Power 5V Eksternal → Servo VCC
Power GND → Servo GND + ESP32 GND (common ground!)
```

⚠️ **PENTING:** Jika menggunakan power eksternal untuk servo, **WAJIB** hubungkan GND power eksternal ke GND ESP32 (common ground).

---

## **🔌 DIAGRAM SEDERHANA**
```
ESP32 Pin Layout:
┌─────────────────┐
│  GND  ─────────┼──→ Semua GND komponen
│  3.3V ─────────┼──→ RFID VCC
│  5V   ─────────┼──→ LCD, Ultrasonic
│                │
│  21 (SDA) ────┼──→ LCD SDA
│  22 (SCL) ────┼──→ LCD SCL
│                │
│  26   ─────────┼──→ Servo Signal
│  18   ─────────┼──→ Ultrasonic TRIG, RFID SCK
│  19   ─────────┼──→ Ultrasonic ECHO, RFID MISO
│  23   ─────────┼──→ RFID MOSI
│  5    ─────────┼──→ RFID SS
│  4    ─────────┼──→ RFID RST
│                │
│  13,12,14,27 ─┼──→ Keypad Rows
│  25,33,32,15 ─┼──→ Keypad Cols
└─────────────────┘
