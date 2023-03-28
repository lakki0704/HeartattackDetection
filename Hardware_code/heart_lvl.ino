#include <CircularBuffer.h>
#include <MAX30100.h>
#include <MAX30100_BeatDetector.h>
#include <MAX30100_Filters.h>
#include <MAX30100_PulseOximeter.h>
#include <MAX30100_Registers.h>
#include <MAX30100_SpO2Calculator.h>

#include <Wire.h>
// #include "MAX30100_PulseOximeter.h"
// #define BLYNK_PRINT Serial
// #include <Blynk.h>
#include <WiFi.h>
// #include <BlynkSimpleEsp32.h>
 
#define REPORTING_PERIOD_MS 1000

/****************************************
 * Define Constants For AD822 ECG Sensor
 ****************************************/
#define VARIABLE_LABEL "sensor" // Assing the variable label
#define DEVICE_LABEL "esp32" // Assig the device label
 
#define SENSOR A0 // Set the A0 as SENSOR

char ssid[] = "timestone";
char pass[] = "yadukanet";

char payload[100];
char topic[150];
// Space to store values to send
char str_sensor[10];

PulseOximeter pox;
float BPM, SpO2;
uint32_t tsLastReport = 0;
 
// AD822 ECG Functions:
void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  Serial.write(payload, length);
  Serial.println(topic);
}


void onBeatDetected()
{
    Serial.println("Beat Detected!");
}

void setup()
{
    Serial.begin(115200);
    /* ------------------------------------
                FOR AD822 ECG
    ---------------------------------------*/

    pinMode(SENSOR, INPUT);
 
    Serial.println();

    /* ----------------------------------------------------------
                      FOR PULSE OXYMETER MAX 30100
    -------------------------------------------------------------*/
    
    pinMode(19, OUTPUT);
    // Blynk.begin(auth, ssid, pass);
 
    Serial.print("Initializing Pulse Oximeter..");
 
    if (!pox.begin())
    {
         Serial.println("FAILED");
         for(;;);
    }
    else
    {
         Serial.println("SUCCESS");
         pox.setOnBeatDetectedCallback(onBeatDetected);
    }
 
        pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop()
{
    /* --------------------------------
              ECG Sensor:
    ------------------------------------*/
    sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    sprintf(payload, "%s", ""); // Cleans the payload
    sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label

    float sensor = analogRead(SENSOR); 

      /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
    dtostrf(sensor, 4, 2, str_sensor);
    
    sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor);
    Serial.print(str_sensor);
    delay(500);
    /*----------------------------------
              FOR PULSEOXYMETER
    -------------------------------------*/
    pox.update();
    // Blynk.run();
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
 
        // Blynk.virtualWrite(V3, BPM);
        // Blynk.virtualWrite(V4, SpO2);
 
        tsLastReport = millis();
    }
}