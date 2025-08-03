#ifndef CONFIGTYPES_H
#define CONFIGTYPES_H

#include <stddef.h>
#include <stdint.h>

typedef struct  __attribute__((packed)) {
	int id; // 0 = pinConfigArray, 1 = sendConfig, 
			// 2 = bluetoothMidiConfig, 3 = wifiConfig, 4-255 = custom configs.
	size_t len;
	char* data;
} Config;

typedef struct  __attribute__((packed)) {
	int pin;
	int mode;
} PinConfig;

typedef struct  __attribute__((packed)) {
	int numPins;
	PinConfig* configs;
} PinConfigArray;

typedef struct  __attribute__((packed)) {
	int mode; // 0 = bluetooth midi, 1 = esp_now, 2 = usb midi
} SendConfig;

typedef struct  __attribute__((packed)) {
	uint8_t channel; // wifi channel not mini channel.
	uint8_t peerMac[6];
} EspnowConfig;

typedef struct  __attribute__((packed)) {
	size_t len;
	char* name;
} BLEMidiConfig;

typedef struct  __attribute__((packed)) {
	// tbi maybe...
} wifiConfig;

#endif
