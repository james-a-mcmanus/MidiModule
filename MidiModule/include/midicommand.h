#ifndef MIDICOMMAND_H
#define MIDICOMMAND_H

typedef struct __attribute__((packed)) {
	uint8_t com; // 0 = note off, 1 = note on, 3 = hit note on and off.
	uint8_t chan;
	uint8_t note;
	uint8_t vel;
} MidiCommand;

#endif