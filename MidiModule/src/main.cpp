#include <Arduino.h>
#include "midicommand.h"
#include "midimodule.h"
#include "Spinner.h"
#include "configtypes.h"

BLEMidiConfig sConfig{1, "Spinner1"};

Spinner S(17, &sConfig);

void setup() {
    xTaskCreate(&Spinner::readSensorWrapper, "rpmCheck", 2048, (void *)&S, 1, nullptr);
}

void loop() {

}