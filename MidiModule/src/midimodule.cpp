#include "midimodule.h"

MidiModule::MidiModule() : sm(SendMode::NONE), econfig(nullptr), mconfig(nullptr), sendFunc(nullptr) {
	setSendFunction(SendMode::NONE);
}

MidiModule::~MidiModule(){
}

bool MidiModule::BLEMidiInit(BLEMidiConfig* bmc){
	if (!bmc){ return false; }
	std::string deviceName(bmc->name, bmc->len);
	BLEMidiServer.begin(deviceName);
	mconfig = true;
	return true;
}

bool MidiModule::espNowInit(EspnowConfig* ec){
	if (!ec){ return false; }
	if (esp_now_init() != ESP_OK) { return false; } // need esp_wifi start?
	esp_wifi_set_channel(ec->channel, WIFI_SECOND_CHAN_NONE);
	peerInfo = new esp_now_peer_info_t;
	memcpy(peerInfo->peer_addr, ec->peerMac, 6);
	peerInfo->channel = ec->channel;
	peerInfo->encrypt = false;
	if (esp_now_add_peer(peerInfo) != ESP_OK){ return false; }
	econfig = true;
	return true;
}

bool MidiModule::pinSetup(const int* pins, const int* modes, uint8_t len){
	for (int i = 0; i < len; i++){
		pinMode(pins[i], modes[i]);
	}
	return true;
}

bool MidiModule::pinSetup(const PinConfigArray* pca, uint8_t len){
	if (!pca || (len != pca->numPins*sizeof(PinConfig))){
		return false;
	}
	for (int i = 0; i < pca->numPins; i++){
		pinMode(pca->configs[i].pin, pca->configs[i].mode);
	}
	return true;
}

int MidiModule::decodeMIDI(const char* data, uint8_t l){
	// read in a char array into midi and save in queue.
	if (l % sizeof(MidiCommand) != 0){
		return -1;
	}

	if (reinterpret_cast<uintptr_t>(data) % alignof(MidiCommand) != 0) {
		return -2; // alignment error;
	}

	const MidiCommand* midiData = reinterpret_cast<const MidiCommand*>(data);
	int numel = l / sizeof(MidiCommand);
	int pushed = 0;

	for (int i = 0; i < numel; i++){
		if (pushMidi(midiData[i])) { pushed++; };
	}
	
	return pushed;
}

bool MidiModule::pushMidi(const MidiCommand& m){
	return midiBuffer.push(m);
}

bool MidiModule::espSend(){
	if (midiBuffer.isEmpty()){ return false; }
	MidiCommand m;
	if (!midiBuffer.pop(m)){ return false; };
	esp_err_t result = esp_now_send(peerInfo->peer_addr, (uint8_t *)&m, sizeof(MidiCommand));
	if (result == ESP_OK) {
		return true;
	}
	else { 
		return false;
	}
}

bool MidiModule::bleMidiSend(){
	if (midiBuffer.isEmpty()){ return false; }
	MidiCommand m;
	if (!midiBuffer.pop(m)) { return false; };
	switch (m.com){
	case 0:
		BLEMidiServer.noteOff(m.chan, m.note, m.vel);
		break;
	case 1:
		BLEMidiServer.noteOn(m.chan, m.note, m.vel);
		break;
	}
	return true;
}

bool MidiModule::sendOne(){
	if (sendFunc){
		(this->*sendFunc)();
	}
	return true;
}

bool MidiModule::sendAll(){
	if (sendFunc){
		do ((this->*sendFunc)()); while (true);
	}
	return true;
}

void MidiModule::readSensorWrapper(void *pvParameters){
	MidiModule* THIS = (MidiModule*)pvParameters;
	THIS->readSensor();
}

void MidiModule::sendWrapper(void *pvParameters){
	MidiModule* obj = static_cast<MidiModule*>(pvParameters);
	obj->sendOne(); // then needs to wait.
}

int MidiModule::receiveEspNow(const uint8_t* mac, const uint8_t* data, const int len){
	// default behaviour do nothing.
	// who's got control of this data?.
	return 0;
}

void MidiModule::readSensor(){
	// default behaviour no sensor to read.
	return;
}

void MidiModule::checkPushes(){
	return;
}

bool MidiModule::doNothing(){
	return true;
}

bool MidiModule::setSendFunction(SendMode sm){
	switch (sm){
	case SendMode::BLE:
		if (!mconfig) { return false; }
		sendFunc = &MidiModule::bleMidiSend;
		break;
	case SendMode::ESP:
		if (!econfig) { return false; }
		sendFunc = &MidiModule::espSend;
		break;
	default:
		sendFunc = &MidiModule::doNothing;
		return false; // unimplemented.
	}
	return true;
}