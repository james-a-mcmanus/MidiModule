#include <Arduino.h>
#include "midicommand.h"
#include "midimodule.h"
#include "Spinner.h"
#include "configtypes.h"

BLEMidiConfig sConfig{8, "Spinner1"};
Spinner S;

void setup() {

    // debugging:
    Serial.begin(9600);
    while (!Serial){}
    Serial.println("Serial communication started");

    // todo this can all go in an initialise function.
    int pins[1] = {18};
    int modes[1] = {INPUT_PULLUP};
    S.pinSetup(pins, modes, 1); 
    S.setInterrupts();
    S.BLEMidiInit(&sConfig);
    S.setSendFunction(MidiModule::SendMode::BLE);
    BaseType_t sensRet, sendRet;
    TaskHandle_t sensHdl = NULL, sendHdl=NULL;
    sensRet = xTaskCreate(&Spinner::readSensorWrapper, "rpmCheck", 2048, (void *)&S, 1, &sensHdl);
    sendRet = xTaskCreate(&Spinner::sendWrapper, "sendBLE", 2048, (void *)&S, 1, &sendHdl);
    if (sensRet != pdPASS){
        Serial.println("error creating sensor task");
    }
    if (sendRet != pdPASS){
        Serial.println("error creating send task");
    }
}

void loop() {
    vTaskDelay(portMAX_DELAY);
}