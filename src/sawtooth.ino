#include "speaker.h"

uint16_t bufferSize = 128;
Speaker speaker(bufferSize);

void setup() {
  uint16_t audioFrequency = 11025; // Hz
  speaker.begin(audioFrequency);
}

uint16_t audioSignal = 0;

void loop() {
  if (speaker.ready()) {
    uint16_t *buffer = speaker.getBuffer();
    for (uint16_t i = 0; i < bufferSize; i++) {
      buffer[i] = audioSignal;
      audioSignal++;
      if (audioSignal > 1023) {
        audioSignal = 0;
      }
    }
  }
}

