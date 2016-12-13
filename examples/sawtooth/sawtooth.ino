/* Generate audio with your Photon or Electron
 *
 * Speaker library by Julien Vanier <jvanier@gmail.com>
 * 
 */
#include "speaker.h"

SYSTEM_THREAD(ENABLED);

/* Configure audio output */
uint16_t bufferSize = 128;
Speaker speaker(bufferSize);

uint16_t audioFrequency = 22050; // Hz

/* Configure signal parameters */
uint16_t amplitude = 50000; // signal goes from 0 to 65535
uint16_t frequency = 1000; // Hz
uint32_t audioSignal = 0; // current value of the audio signal

/* Generate a sawtooth signal into the Speaker buffer.
 * A sawtooth goes up until a limit, then goes back to 0.
 */
void generateSawtooth(uint16_t *buffer)
{
  uint32_t increment = ((uint32_t) frequency * amplitude) / audioFrequency;

  for (uint16_t i = 0; i < bufferSize; i++) {
    buffer[i] = audioSignal;
    audioSignal += increment;
    if (audioSignal > amplitude) {
      audioSignal -= amplitude;
    }
  }
}

/* Particle function to change the volume of the sawtooth signal by
 * changing the maximum value.
 */
int changeAmplitude(String arg)
{
  long num = arg.toInt();
  if (num >= 0 && num < 65535)
  {
    amplitude = (uint16_t) num;
    if (audioSignal > amplitude) {
      audioSignal = 0;
    }
  }
  return amplitude;
}

/* Particle function to change the sawtooth frequency */
int changeFrequency(String arg)
{
  long num = arg.toInt();
  if (num > 0 && num < audioFrequency)
  {
    frequency = (uint16_t) num;
  }
  return frequency;
}

/* Set up the speaker */
void setup() {
  Particle.function("amp", changeAmplitude);
  Particle.function("freq", changeFrequency);

  // Write initial data in the audio buffer. Not mandatory, but if you
  // don't do this you'll start with 1 buffer of silence.
  generateSawtooth(speaker.getBuffer());

  // Start the audio output
  speaker.begin(audioFrequency);
}

/* Generate more audio data when needed */
void loop() {
  // Call ready as often as possible to avoid the speaker playing the same buffer multiple times.
  if (speaker.ready()) {
    // Write new audio into the buffer returned by getBuffer
    generateSawtooth(speaker.getBuffer());
  }
}

