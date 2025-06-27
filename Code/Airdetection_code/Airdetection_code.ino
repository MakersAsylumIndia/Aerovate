#define BLYNK_TEMPLATE_ID "TMPL3GTtOOQG1"
#define BLYNK_TEMPLATE_NAME "Air Quality Detector"
#define BLYNK_AUTH_TOKEN "Qdd8QblaecCAwsMk4JNQrXhNrnMwr9nD"

#include <SPI.h>
#include "LCD_Driver.h"
#include "GUI_Paint.h"
#include <DHT.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// WiFi credentials
char ssid[] = "Makers Asylum";
char pass[] = "Makeithappen";

// Sensor pin assignments
#define MQ2_PIN   34   // Smoke
#define MQ7_PIN   32   // CO
#define MQ135_PIN 33   // Harmful Gases
#define DHT_PIN   4    // DHT sensor

// Pin assignments
const int MQ135_analogPin = 33;
const int MQ7_analogPin   = 32;
const int MQ2_analogPin   = 34;
const int dhtPin          = 4;       // Digital pin for DHT

#define DHTTYPE DHT11  // Or DHT22
DHT dht(DHT_PIN, DHTTYPE);

BlynkTimer timer;

void sendSensorData() {
  int smoke = analogRead(MQ2_PIN);     // MQ-2
  int co = analogRead(MQ7_PIN);        // MQ-7
  int gas = analogRead(MQ135_PIN);     // MQ-135
  float temperature = dht.readTemperature();  // °C

  // Send data to Blynk
  Blynk.virtualWrite(V0, smoke);       
  Blynk.virtualWrite(V1, co);          
  Blynk.virtualWrite(V2, gas);         
  Blynk.virtualWrite(V3, temperature); 

  if (smoke > 750) { 
  Blynk.logEvent("smoke_alert", "⚠ Smoke level dangerously high!");
  }


  // Debug print
  Serial.print("Smoke: "); Serial.print(smoke);
  Serial.print(" | CO: "); Serial.print(co);
  Serial.print(" | Gas: "); Serial.print(gas);
  Serial.print(" | Temp: "); Serial.println(temperature);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  pinMode(LED_BUILTIN, OUTPUT);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  pinMode(MQ2_PIN, INPUT);
  pinMode(MQ7_PIN, INPUT);
  pinMode(MQ135_PIN, INPUT);

  timer.setInterval(1000L, sendSensorData);  // Read every second
  
  Config_Init();
  LCD_Init();
  LCD_SetBacklight(1000);  // Max backlight (0–1000)

  dht.begin();
  analogReadResolution(12);  // ESP32: 12-bit ADC (0–4095)

  Paint_NewImage(LCD_WIDTH, LCD_HEIGHT, 0, WHITE);  // width, height, rotation, bg color
  Paint_Clear(WHITE);
  Paint_SetRotate(0);  // Try 0, 1, 2, or 3 if orientation is wrong
  Paint_DrawString_EN(10, 20, "Harmful Gas:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 100, "Smoke Levels:", &Font24, WHITE, BLACK);
  Paint_DrawString_EN(10, 180, "CO Levels:", &Font24, WHITE, BLACK);

}

void loop() {
  Blynk.run();
  timer.run();

  int MQ135sensorValue = analogRead(MQ135_analogPin);
  int MQ7sensorValue   = analogRead(MQ7_analogPin);
  int MQ2sensorValue   = analogRead(MQ2_analogPin);
  float temperature = dht.readTemperature();

  Serial.print("MQ135: ");
  Serial.print(MQ135sensorValue);
  Serial.print(" | MQ7: ");
  Serial.print(MQ7sensorValue);
  Serial.print(" | MQ2: ");
  Serial.print(MQ2sensorValue);
  Serial.print(" | Temp: ");
  Serial.print(temperature);

  if (MQ2sensorValue > 500) { 
  Blynk.logEvent("smoke_alert", "⚠ Smoke level dangerously high!");
}

  if (MQ135sensorValue>=1500 || MQ2sensorValue>=3500 || MQ7sensorValue>=700){
    digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on
  }

  if (MQ135sensorValue<1500 || MQ2sensorValue<3500 || MQ7sensorValue<700){
    digitalWrite(LED_BUILTIN, LOW);  // turn the LED off
  }

  if (MQ135sensorValue<1500){
    Serial.print(" | Harmful gas Level: 🟩");
    Paint_DrawRectangle(10, 50, 10 + 220, 50 + 40, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if ((MQ135sensorValue>=1500 && MQ135sensorValue<2000)){
    Serial.print(" | Harmful gas Level: 🟨");
    Paint_DrawRectangle(10, 50, 10 + 220, 50 + 40, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if (MQ135sensorValue>=2000){
    Serial.print(" | Harmful gas Level: 🟥");
    Paint_DrawRectangle(10, 50, 10 + 220, 50 + 40, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  
  if (MQ2sensorValue<2000){
    Serial.print(" | Smoke Level: 🟩");
    Paint_DrawRectangle(10, 130, 10 + 220, 130 + 40, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if ((MQ2sensorValue>=2000 && MQ2sensorValue<3500)){
    Serial.print(" | Smoke Level: 🟨");
    Paint_DrawRectangle(10, 130, 10 + 220, 130 + 40, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if (MQ2sensorValue>=3500){
    Serial.print(" | Smoke Level: 🟥");
    Paint_DrawRectangle(10, 130, 10 + 220, 130 + 40, RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }


  if (MQ7sensorValue<700){
    Serial.print(" | Carbon Monoxide Level: 🟩");
    Paint_DrawRectangle(10, 210, 10 + 220, 210 + 40, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if ((MQ7sensorValue>=700 && MQ7sensorValue<1000)){
    Serial.print(" | Carbon Monoxide Level: 🟨");
    Paint_DrawRectangle(10, 210, 10 + 220, 210 + 40, YELLOW, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }
  else if (MQ7sensorValue>=1000){
    Serial.print(" | Carbon Monoxide Level: 🟥");
    Paint_DrawRectangle(10, 210, 10 + 220, 210 + 40, GREEN, DOT_PIXEL_1X1, DRAW_FILL_FULL);
  }

  Serial.println();
  delay(1000);
}