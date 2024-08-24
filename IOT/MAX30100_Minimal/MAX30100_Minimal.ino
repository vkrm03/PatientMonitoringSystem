#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#define REPORTING_PERIOD_MS     1000



PulseOximeter pox;
uint32_t tsLastReport = 0;
int sp = 0;
int hr = 0;

byte Heart[] = {
B00000,
B01010,
B11111,
B11111,
B01110,
B00100,
B00000,
B00000
};


void onBeatDetected() {
    Serial.println("Beat!");
}

void setup() {
    Serial.begin(9600);

    Serial.print("Initializing pulse oximeter..");

    
    if (!pox.begin()) {
        Serial.println("FAILED");
        
        for(;;);
    } else {
        Serial.println("SUCCESS");

    }

 
  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        hr = pox.getHeartRate();
        sp = pox.getSpO2();
        
        Serial.print("HEART RATE:");
        Serial.print(hr);
        Serial.write(0);
        Serial.print(" ");
        Serial.print("HEART RATE:");
        Serial.print(hr);

        Serial.print("SpO2 LEVELS:");
        Serial.print(sp);
        Serial.print("%   ");
        Serial.print("bpm / SpO2:");
        Serial.print(sp);
        Serial.println("%");

        tsLastReport = millis();
    }
}