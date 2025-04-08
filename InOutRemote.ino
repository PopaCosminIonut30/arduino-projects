#include <Wire.h>
#include <WiFi.h>
#include "mpu_x.h"
#include <ESPAsyncWebServer.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// Configurare pentru WiFi (pentru SoftAP)
const char* ssid = "ESP32_Web_Server";  // SSID-ul rețelei create de ESP32
const char* password = "123456789";  // Parola pentru rețea

const int IMU_ADDR_ON_BUS = 0x68;  // Adresa I2C a senzorului IMU

MPUx imu(IMU_ADDR_ON_BUS);

float z_axis, x_axis, y_axis; 
float threshold = 1.0;
int gyroX, gyroY, gyroZ;  // Variabile pentru valorile giroscopului

// Definire pin buzzer
#define BUZZER_PIN 13

// Configurare pentru LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adresa LCD-ului, 16 coloane, 2 linii

int buzzerFrequency = 1000;  // Frecvența inițială a buzzer-ului (în Hz)

AsyncWebServer server(80);  // Crează serverul web pe portul 80

void setup() {
  // Inițializare Serial Monitor
  Serial.begin(115200);
  Wire.begin();  // Inițializare I2C
  imu.init();  // Inițializare IMU
  
  // Setarea ESP32 ca Access Point
  WiFi.softAP(ssid, password);  // Crează rețeaua Wi-Fi (SSID: ESP32_Web_Server, parola: 123456789)

  // Citire IP acces point
  IPAddress IP = WiFi.softAPIP();
  Serial.print("IP Address: ");
  Serial.println(IP);

  // Configurare LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready");

  // Setare pin buzzer ca OUTPUT
  pinMode(BUZZER_PIN, OUTPUT);

  // Crearea serverului web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body>";
    html += "<h1>Control Buzzer</h1>";
    html += "<button onclick=\"location.href='/volumeup'\">Volume Up</button><br><br>";
    html += "<button onclick=\"location.href='/volumedown'\">Volume Down</button><br><br>";

    // Citirea datelor de la IMU
    imu.readAcceleration(x_axis,y_axis,z_axis);
    imu.readGyroscope(gyroX, gyroY, gyroZ);  // Citește valorile accelerometrului  // Citește valorile giroscopului

    html += "<h2>Accelerometer Data</h2>";
    html += "<p>X Axis: " + String(x_axis) + "</p>";
    html += "<p>Y Axis: " + String(y_axis) + "</p>";
    html += "<p>Z Axis: " + String(z_axis) + "</p>";

    html += "<h2>Gyroscope Data</h2>";
    html += "<p>Gyro X: " + String(gyroX) + "</p>";
    html += "<p>Gyro Y: " + String(gyroY) + "</p>";
    html += "<p>Gyro Z: " + String(gyroZ) + "</p>";

    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/volumeup", HTTP_GET, [](AsyncWebServerRequest *request){
    buzzerFrequency = 2000;  // Crește frecvența pentru a face sunetul mai tare
    tone(BUZZER_PIN, NOTE_C4);  // Sunetul pentru Volume Up
    lcd.clear();
    lcd.print("Volume Up");
    delay(1000);
    request->send(200, "text/html", "<html><body><h1>Volume Up</h1><a href='/'>Back</a></body></html>");
  });

  server.on("/volumedown", HTTP_GET, [](AsyncWebServerRequest *request){
    buzzerFrequency = 500;  // Scade frecvența pentru a face sunetul mai încet
    tone(BUZZER_PIN, NOTE_C3);  // Sunetul pentru Volume Down
    lcd.clear();
    lcd.print("Volume Down");
    delay(1000);
    request->send(200, "text/html", "<html><body><h1>Volume Down</h1><a href='/'>Back</a></body></html>");
  });

  server.begin();
}

void loop() {
  // Citirea datelor de la accelerometru și giroscop
  imu.readAcceleration(x_axis,y_axis,z_axis);
  IMU.readGyroscope(gyroX, gyroY, gyroZ);
  // Dacă accelerometrul detectează mișcarea în sus (z-axis > threshold), se face "Volume Up"
  if (z_axis > threshold) {
    buzzerFrequency = 2000;
    tone(BUZZER_PIN, NOTE_C4);  // Sunetul pentru Volume Up
    lcd.clear();
    lcd.print("Volume Up");
  }
  // Dacă accelerometrul detectează mișcarea în jos (z-axis < -threshold), se face "Volume Down"
  else if (z_axis < -threshold) {
    buzzerFrequency = 500;
    tone(BUZZER_PIN, NOTE_C3);  // Sunetul pentru Volume Down
    lcd.clear();
    lcd.print("Volume Down");
  }

  // Detectare rotație pe giroscop pentru a schimba canalul
  if (gyroX > 50 || gyroY > 50 || gyroZ > 50) {  // Schimbă canalul dacă există o rotație mare
    lcd.clear();
    lcd.print("Switch Channel");
  }

  delay(100);  // Așteaptă puțin înainte de a citi din nou
}
