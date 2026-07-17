#include <SoftwareSerial.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Token Blynk asal milik peranti Door Lock awak
const String TOKEN = "GTxkCuax7Zk6lrE44r7tC8X_dzkWIjQ3";

// Konfigurasi pin WiFi: Pin 4 (RX Arduino -> TX ESP), Pin 2 (TX Arduino -> RX ESP)
SoftwareSerial EspSerial(4, 2); 
Servo myservo; 
LiquidCrystal_I2C lcd(0x27, 16, 2); 

void setup() {
  Serial.begin(9600);
  EspSerial.begin(9600); 
  
  // Initialize LCD Skrin
  lcd.init();
  lcd.backlight();
  
  // Set servo ke posisi diam (90 darjah untuk pergerakan 360)
  myservo.attach(3);   
  myservo.write(90);  
  delay(500);
  myservo.detach();    
  
  paparanSedia();
  
  Serial.println("--- MEMULAKAN SAMBUNGAN WIFI & BLYNK ---");
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("CONNECTING WIFI.");
  
  // Setup WiFi secara manual menggunakan arahan AT Command
  hantarAT("AT+RST", 2000);
  hantarAT("AT+CWMODE=1", 1000); 
  hantarAT("AT+CWJAP=\"tehais\",\"tehtarik\"", 5000); 
  hantarAT("AT+CIPMUX=0", 1000);
  
  lcd.clear();
  lcd.setCursor(3, 0); lcd.print("SYSTEM READY");
  delay(1500);
  
  paparanSedia();
}

// Fungsi memaparkan status utama pada LCD
void paparanSedia() {
  lcd.clear();
  lcd.setCursor(1, 0); lcd.print("SILA IMBAS KAD");
  lcd.setCursor(3, 1); lcd.print("NFC / RFID");
}

// Fungsi kitaran penuh untuk membuka dan mengunci semula pintu
void operasiBukaPintu() {
  // 1. AKSES DILULUSKAN
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("ACCESS GRANTED");
  lcd.setCursor(1, 1); lcd.print("PINTU DIBUKA  ");
  
  // 2. GERAKAN SERVO MEMBUKA PINTU (Pusing Kiri)
  myservo.attach(3);
  myservo.write(0);   
  delay(100);         // Delay laras fizikal 100ms
  myservo.write(90);  // Berhenti
  myservo.detach();   
  
  // 3. FASA PINTU TERBUKA
  Serial.println("⏳ Pintu terbuka... Menunggu 3 saat.");
  delay(3000); 
  
  // 4. GERAKAN SERVO MENUTUP PINTU (Pusing Kanan)
  Serial.println("🔒 Menutup Pintu Semula...");
  lcd.clear();
  lcd.setCursor(2, 0); lcd.print("MENUTUP PINTU");
  lcd.setCursor(4, 1); lcd.print("MENGUNCI...");
  
  myservo.attach(3);
  myservo.write(180); 
  delay(100);         // Delay laras fizikal 100ms
  myservo.write(90);  // Berhenti
  myservo.detach();  
  
  Serial.println("🔒 Pintu selesai dikunci.");
  delay(1000);
  paparanSedia();
}

void hantarAT(String arahan, int delayMasa) {
  EspSerial.println(arahan);
  delay(delayMasa);
  while(EspSerial.available()) {
    String respon = EspSerial.readString();
    Serial.println(respon); 
  }
}

void loop() {
  static unsigned long masaLalu = 0;
  
  // ========================================================
  // BAHAGIAN 1: PEMANTAUAN APLIKASI BLYNK (SETIAP 3 SAAT)
  // ========================================================
  if (millis() - masaLalu >= 3000) {
    masaLalu = millis();
    
    EspSerial.println("AT+CIPSTART=\"TCP\",\"blynk.cloud\",80");
    delay(300);
    
    // Request status Virtual Pin 1 (V1) dari pelayan cloud Blynk
    String urlGet = "GET /api/v1/get?token=" + TOKEN + "&V1 HTTP/1.1\r\nHost: blynk.cloud\r\n\r\n";
    EspSerial.print("AT+CIPSEND=");
    EspSerial.println(urlGet.length());
    delay(100);
    EspSerial.print(urlGet);
  }

  // Membaca maklum balas daripada pelayan Blynk Cloud
  if (EspSerial.available()) {
    String responServer = EspSerial.readString();
    Serial.println(responServer);
    
    // Jika data Virtual Pin mengandungi nilai "1" (Butang Blynk ditekan)
    if (responServer.indexOf("[\"1\"]") != -1) {
      Serial.println("🔓 Arahan diterima daripada Aplikasi Blynk!");
      operasiBukaPintu();
      
      // Set semula nilai Virtual Pin V1 ke status "0" (Auto-Off)
      EspSerial.println("AT+CIPSTART=\"TCP\",\"blynk.cloud\",80");
      delay(300);
      String urlUpdate = "GET /api/v1/update?token=" + TOKEN + "&V1=0 HTTP/1.1\r\nHost: blynk.cloud\r\n\r\n";
      EspSerial.print("AT+CIPSEND=");
      EspSerial.println(urlUpdate.length());
      delay(100);
      EspSerial.print(urlUpdate);
    }
  }

  // ========================================================
  // BAHAGIAN 2: SIMULASI IMBASAN KAD DARI LAPTOP (SERIAL MONITOR)
  // ========================================================
  if (Serial.available() > 0) {
    char ch = Serial.read(); 
    
    // Abaikan pembacaan Newline dan Carriage Return
    if (ch == '\n' || ch == '\r') return; 
    
    // Jika Kad Sah diimbas (Taip huruf S)
    if (ch == 'S' || ch == 's') {
      Serial.println("💳 Kad RFID Sah! Membaca Data...");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("SCANNING CARD...");
      lcd.setCursor(0, 1); lcd.print("READING UID: OK");
      delay(1000); 
      
      operasiBukaPintu();
    }
    // Jika Kad Tidak Dikenali diimbas (Taip apa-apa huruf lain)
    else {
      Serial.println("⚠️ Kad Tidak Dikenali!");
      lcd.clear();
      lcd.setCursor(0, 0); lcd.print("SCANNING CARD...");
      delay(800);
      
      lcd.clear();
      lcd.setCursor(1, 0); lcd.print("ACCESS DENIED!");
      lcd.setCursor(0, 1); lcd.print("UNAUTHORIZED CARD");
      
      // Lampu LCD berkelip 3 kali sebagai indikator amaran
      for(int i = 0; i < 3; i++) {
        lcd.noBacklight(); delay(200);
        lcd.backlight(); delay(200);
      }
      delay(2000); 
      paparanSedia();
    }
  }
}
