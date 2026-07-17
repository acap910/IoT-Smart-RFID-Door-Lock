# IoT Smart RFID Door Lock System

## 👥 Group Members
* AFIQ AFFENDI (2023448758) 
* ASYRAF MUQRI (2023437202) 
* MUHAMMAD RAFIQI AIMAN (2023410832)
* ROMY AZRIL HAZEEQ (2023438624) 

---

## 📝 Project Overview
This repository contains the source code for an IoT-based Smart RFID Door Lock System, developed as part of a mobile computing Final Year Project. The system utilizes an Arduino Uno as the main microcontroller to manage access control via local authentication and remote cloud monitoring.

## 🛠️ Hardware Components
* **Arduino Uno** – Main microcontroller unit
* **Continuous Rotation Servo Motor (360°)** – Electronic door lock actuator
* **ESP-01 WiFi Module** – Enables IoT cloud connectivity
* **LCD Display with I2C Driver (0x27)** – Displays real-time system status

## 📱 System Integration & Features
* **Blynk Cloud**: Allows remote monitoring and access control via a smartphone application using Virtual Pin 1 (`V1`).
* **LCD Visual Feedback**: Displays current system operations such as `PLEASE SCAN CARD`, `ACCESS GRANTED` (authorized entry), or `ACCESS DENIED` (unauthorized attempt).
* **NFC/RFID Simulation**: Manages secure access logic directly through the Serial Monitor using custom commands (`S` for Access Granted, any other character for Access Denied).

## 🔧 Pin Configuration
* **Servo Motor**: Connected to Pin D3
* **ESP-01 Module**: Connected via SoftwareSerial on Pin D4 (RX) and Pin D2 (TX)
* **LCD I2C**: Connected to SDA and SCL pins
