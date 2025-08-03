#ifndef MIDI_MODULE_H
#define MIDI_MODULE_H

#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <BLEMidi.h>

#include "configtypes.h"
#include "safecircularbuffer.h"
#include "midicommand.h"

#define BUFFER_SIZE 20

// idea: support both direct midi and esp_now with segregated queues (for vis.).

class MidiModule{
public:
	MidiModule();
	~MidiModule();
	enum SendMode{BLE, ESP, SER, WIFI, NONE};
	bool BLEMidiInit(BLEMidiConfig* bmc);
	bool espNowInit(EspnowConfig* econfig);
	static void sendWrapper(void *pvParameters); // wrapper for attaching to a xtaskcreate.
	bool setSendFunction(SendMode sm); // set the function by which sendOne and sendAll operate.
	virtual bool pinSetup(const int* pins, const int* modes, uint8_t len); // can't see why this needs to be part of this class.
	virtual bool pinSetup(const PinConfigArray* pca, uint8_t len);
	static void receiveEspNowWrapper(void *pvParameters);
	static void readSensorWrapper(void *pvParemeters);
protected:
	int decodeMIDI(const char* data, uint8_t l); // decodes char array into midi structs and adds to send queue, returns number of midi packets added to queue.
	bool pushMidi(const MidiCommand& m); // pushes midi into the send queue.
	bool espSend(); // send from queue via esp-now
	bool bleMidiSend(); // send from queue via bt
	// bool send(); // send via send function.
	bool sendOne(); // send one from queue.
	bool sendAll(); // send all from queue.
	bool doNothing();
	virtual int receiveEspNow(const uint8_t* mac, const uint8_t* data, const int len); // 
	virtual void readSensor(); // override function for any sensor reads.
	virtual void checkPushes(); // override function for checking internal state and pushing to midi queue.
private:
	SendMode sm;
	SafeCircularBuffer<MidiCommand, BUFFER_SIZE> midiBuffer;
	esp_now_peer_info_t* peerInfo;
	bool econfig;
	bool mconfig;
	bool (MidiModule::*sendFunc)();
};

#endif