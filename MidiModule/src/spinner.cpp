#include "spinner.h"

Spinner* spinPtr = nullptr; // global pointer for accessing class in ISR.
void IRAM_ATTR isrBridge() {
  if (spinPtr) spinPtr->handlePulse();
}

Spinner::Spinner(const uint8_t sensorPin, BLEMidiConfig* config){
    // TODO: implement setup from serial. 
    sPin = sensorPin;
    int pins[] = {sensorPin};
    int modes[] = {INPUT_PULLUP}; // CHECK
    pinSetup(pins, modes, 1); // TODO: support multiple digireads.
    BLEMidiInit(config);
    setSendFunction(SendMode::BLE);
    spinPtr = this;
}

Spinner::~Spinner(){

}

void Spinner::handlePulse(){
    portENTER_CRITICAL(&mux);
    pulseCounter++;
    portEXIT_CRITICAL(&mux);
}

// calculates RPM
void Spinner::readSensor(){
    portENTER_CRITICAL(&mux);
    rpm = pulseCounter * 60 * (1000.0/checkInterval);
    pulseCounter = 0;
    portEXIT_CRITICAL(&mux);
}

void Spinner::checkPushes(){
    // check current RPM and generate MIDI.
    pushMidi(rpm2midi());

}

MidiCommand Spinner::rpm2midi(){
    return MidiCommand{0,0,60, (uint8_t)this->rpm};
}