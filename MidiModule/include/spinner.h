#ifndef SPINNER_H
#define SPINNER_H

#include "midimodule.h"
#include "configtypes.h"

class Spinner : public MidiModule
{
public:
    Spinner();
    ~Spinner();
    void handlePulse();
    void readSensor() override;
    void checkPushes() override;
    void setInterrupts();
    virtual MidiCommand rpm2midi();
    volatile double rpm;
    virtual bool pinSetup(const int* pins, const int* modes, uint8_t len) override;
    // static void doPushesWrapper(void *pvParemeters);
private:
    uint8_t sPin;
    volatile uint32_t pulseCounter;
    int checkInterval = 1000;
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
};

#endif