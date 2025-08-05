#include "spinner.h"

Spinner* spinPtr = nullptr; // global pointer for accessing class in ISR.
void IRAM_ATTR isrBridge() {
  if (spinPtr) spinPtr->handlePulse();
}

Spinner::Spinner(){
    // TODO: implement setup from serial. 
    spinPtr = this;
    setSensorInterval(checkInterval);
    setSendInterval(100);
}

Spinner::~Spinner(){

}

bool Spinner::pinSetup(const int* pins, const int* modes, uint8_t len){
    sPin = pins[0];
    return MidiModule::pinSetup(pins, modes, len);
}

void Spinner::setInterrupts(){
    attachInterrupt(digitalPinToInterrupt(sPin), isrBridge, FALLING);
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
    checkPushes();
}


void Spinner::checkPushes(){
    // check current RPM and generate MIDI.
    if (rpm > 0){
        // Serial.println(rpm);
        pushMidi(rpm2midi());
    }
}

MidiCommand Spinner::rpm2midi(){
    return MidiCommand{3,1,60,127};//(uint8_t)this->rpm};
}