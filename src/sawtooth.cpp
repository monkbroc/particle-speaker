/* Generate audio with your Photon or Electron
 *
 * Speaker library by Julien Vanier <jvanier@gmail.com>
 * 
 */
#include "speaker.h"

SYSTEM_THREAD(ENABLED);

uint16_t bufferSize = 128;
Speaker speaker(bufferSize);

uint16_t audioFrequency = 22050; // Hz

uint16_t audioSignal = 0;
uint16_t signalFrequency = 1000; // Hz

void sawtoothWaveform(uint16_t *buffer)
{
  uint32_t signalLimit = ((uint32_t) audioFrequency << 16) / signalFrequency;

  for (uint16_t i = 0; i < bufferSize; i++) {
    buffer[i] = audioSignal;
    audioSignal += 5000;
    if (audioSignal > 50000) {
      audioSignal = 0;
    }
  }
}

void setup() {
  sawtoothWaveform(speaker.getBuffer());
  speaker.begin(audioFrequency);
}

void loop() {
  if (speaker.ready()) {
    sawtoothWaveform(speaker.getBuffer());
  }
}

