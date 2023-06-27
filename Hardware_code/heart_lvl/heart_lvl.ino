#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <Wire.h>

#include <CircularBuffer.h>
#include <MAX30100.h>
#include <MAX30100_BeatDetector.h>
#include <MAX30100_Filters.h>
#include <MAX30100_PulseOximeter.h>
#include <MAX30100_Registers.h>
#include <MAX30100_SpO2Calculator.h>

#include <MAX30100.h>

#include <ThingSpeak.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
// #include <PubSubClient.h>
 
//------- WI-FI details ----------//
char ssid[] = "timestone"; //SSID here
char pass[] = "yadukanet"; // Passowrd here
//--------------------------------//                                // Put your wifi password here
// #define TOKEN "BBFF-YKxITsj1YPeTMxw7mq8lvYFBpXnCxD"         // Put your Ubidots' TOKEN
// #define MQTT_CLIENT_NAME "myecgsensor"                       // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                                            //it should be a random and unique ascii string and different from all other devices
 
/****************************************
 * Define Constants for Thinspeak
 ****************************************/
unsigned long Channel_ID = 2088062; // Your Channel ID
const char * myWriteAPIKey = "K73FZPHDDC8BD3J0"; //Your write API key

const int spo_field = 1;
const int bpm_field = 2;
const int ecg_field = 3;
WiFiClient client;
/****************************************
 * Define Constants For AD822 ECG Sensor
 ****************************************/
#define VARIABLE_LABEL "myecg" // Assing the variable label
#define DEVICE_LABEL "esp8266" // Assig the device label
 
#define SENSOR A0 // Set the A0 as SENSOR
// char payload[100];
// char topic[150];
// Space to store values to send
char str_sensor[10];

/****************************************
 * Define Constants For MAX30100 BPM/POX Sensor
 ****************************************/
// Connections : SCL PIN - D1 , SDA PIN - D2 , INT PIN - D0

#define REPORTING_PERIOD_MS 1000
PulseOximeter pox;
 
float BPM, SpO2;
uint32_t tsLastReport = 0;

// const unsigned char bitmap [] PROGMEM=
// {
// 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x18, 0x00, 0x0f, 0xe0, 0x7f, 0x00, 0x3f, 0xf9, 0xff, 0xc0,
// 0x7f, 0xf9, 0xff, 0xc0, 0x7f, 0xff, 0xff, 0xe0, 0x7f, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xf0,
// 0xff, 0xf7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0xff, 0xe7, 0xff, 0xf0, 0x7f, 0xdb, 0xff, 0xe0,
// 0x7f, 0x9b, 0xff, 0xe0, 0x00, 0x3b, 0xc0, 0x00, 0x3f, 0xf9, 0x9f, 0xc0, 0x3f, 0xfd, 0xbf, 0xc0,
// 0x1f, 0xfd, 0xbf, 0x80, 0x0f, 0xfd, 0x7f, 0x00, 0x07, 0xfe, 0x7e, 0x00, 0x03, 0xfe, 0xfc, 0x00,
// 0x01, 0xff, 0xf8, 0x00, 0x00, 0xff, 0xf0, 0x00, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x3f, 0xc0, 0x00,
// 0x00, 0x0f, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
// };

void onBeatDetected()
{
    Serial.println("Beat Detected!");
}
void setup()
{
    Serial.begin(9600);
    /* ------------------------------------
                FOR AD822 ECG
    ---------------------------------------*/
    // Serial.begin(115200);
    // WiFi.mode(WIFI_STA);
    // WiFi.begin(ssid,pass);
    // Assign the pin as INPUT 
    pinMode(SENSOR, INPUT);
  
    Serial.println();
    Serial.print("Waiting for WiFi...");
    
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Waiting for WiFi...");
      delay(5000);
    }
    
    Serial.println("");
    Serial.println("WiFi Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    ThingSpeak.begin(client);
    // client.setServer(mqttBroker, 1883);
    // client.setCallback(callback);
    

    /* ----------------------------------------------------------
                      FOR PULSE OXYMETER MAX 30100
    -------------------------------------------------------------*/
    pinMode(16, OUTPUT);
    
 
    Serial.print("Initializing Pulse Oximeter..");
 
    // if (!pox.begin())
    // {
    //      Serial.println("FAILED");
    //      for(;;);
    // }
    // else
    // {
    //      Serial.println("SUCCESS");
    //      pox.setOnBeatDetectedCallback(onBeatDetected);
    // }

}

void loop()
{
    // internet();
    
    /* --------------------------------
              ECG Sensor:
    ------------------------------------*/
    // sprintf(topic, "%s%s", "/v1.6/devices/", DEVICE_LABEL);
    // sprintf(payload, "%s", "");                   // Cleans the payload
    // sprintf(payload, "{\"%s\":", VARIABLE_LABEL); // Adds the variable label

    float myecg = analogRead(SENSOR);

    /* 4 is mininum width, 2 is precision; float value is copied onto str_sensor*/
    dtostrf(myecg, 4, 2, str_sensor);
    Serial.println("EGG: ");
    Serial.print(myecg);

    // sprintf(payload, "%s {\"value\": %s}}", payload, str_sensor); // Adds the value
    // Serial.println("Payload: ");
    // Serial.print(payload);
    ThingSpeak.writeField(Channel_ID, ecg_field, myecg, myWriteAPIKey);
    delay(10);

    /*----------------------------------
              FOR PULSEOXYMETER
    -------------------------------------*/
    pox.update();
 
    BPM = pox.getHeartRate();
    SpO2 = pox.getSpO2();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS)
    {
        Serial.print("Heart rate:");
        Serial.print(BPM);
        Serial.print(" bpm / SpO2:");
        Serial.print(SpO2);
        Serial.println(" %");
        ThingSpeak.writeField(Channel_ID, bpm_field, BPM, myWriteAPIKey);
        ThingSpeak.writeField(Channel_ID, spo_field, SpO2, myWriteAPIKey);

        tsLastReport = millis();
    }

}

void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(5000);
      Serial.print(".");
    }
  }
}