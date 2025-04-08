#include <Wire.h>
#include <MPU6050.h>  
#include <Arduino.h> 
#include "pitches.h"
const int gameTones[] = { NOTE_G3, NOTE_C4, NOTE_E4, NOTE_G5};


#define BUZZER_PIN 13

MPU6050 mpu;  // Crează un obiect MPU6050

float z_axis;  // Variabilă pentru valoarea axei Z a accelerometrului
float threshold = 1.0;  // Prag pentru detectarea mișcărilor (poate fi ajustat)

int buzzerFrequency = 1000;  // Frecvența inițială a buzzer-ului (în Hz)
int buzzerVolume = 255;  // Intensitatea (volumul) buzzer-ului (0-255)

void setup() {
  Serial.begin(115200); // Inițializează monitorul serial
  Wire.begin(); // Inițializează bus-ul I2C
  mpu.initialize(); // Inițializează senzorul MPU6050
  pinMode(BUZZER_PIN, OUTPUT);  // Setează pinul buzzer-ului ca OUTPUT

  // Configurare PWM pentru buzzer 
    // Canalul 0, frecvența de 1000 Hz, rezoluție 8 biți
  ledcAttach(BUZZER_PIN, 0, 8);  // Atașează pinul buzzer-ului canalului 0, rezoluția 8 biți
  ledcWrite(0, buzzerVolume);  // Setează intensitatea inițială
  Serial.println("INFO: Buzzer initialized");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);  // Citește valorile accelerometrului
  z_axis = az;  // Folosește valoarea axei Z pentru detecția mișcărilor

  // Detectează mișcarea sus (volume up)
  if (z_axis > threshold) {
    Serial.println("Volume Up");
    buzzerFrequency = 2000;  // Crește frecvența pentru a face sunetul mai tare
    buzzerVolume = 255;  // Setează volumul maxim
    tone(BUZZER_PIN, NOTE_C4) ; // Modifică volumul buzzer-ului
    delay(1000);  // Așteaptă 1 secundă pentru a evita repetarea mesajului prea rapid
  }
  // Detectează mișcarea jos (volume down)
  else if (z_axis < -threshold) {
    Serial.println("Volume Down");
    buzzerFrequency = 500;  // Scade frecvența pentru a face sunetul mai încet
    buzzerVolume = 128;  // Setează volumul mediu
    tone(BUZZER_PIN, NOTE_C3);
    delay(1000);  // Așteaptă 1 secundă pentru a evita repetarea mesajului prea rapid
  }

  delay(100);  // Așteaptă puțin înainte de a citi din nou
}
