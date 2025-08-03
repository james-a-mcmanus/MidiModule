#ifndef SPINNER_H
#define SPINNER_H

#include "midimodule.h"
#include "configtypes.h"

class Spinner : public MidiModule
{
public:
    Spinner(const uint8_t sensorPin, BLEMidiConfig* confi);
    ~Spinner();
    void handlePulse();
    void readSensor() override;
    void checkPushes() override;
    virtual MidiCommand rpm2midi();
    volatile double rpm;
private:
    uint8_t sPin;
    volatile uint32_t pulseCounter;
    int checkInterval = 1000;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};

#endif