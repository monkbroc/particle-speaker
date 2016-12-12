# Speaker

Generate audio output for a speaker for Particle devices (Photon, Electron)

## Usage

Connect a speaker amplifier like [this one from Adafruit](https://www.adafruit.com/product/2130) to the digital to analog converter `DAC` pin of a Photon or Electron and run this code to play a sawtooth wave.

```
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
```

See [complete example](examples/sawtooth/sawtooth.ino) in the examples directory.

## Reference

### `Speaker`

`Speaker speaker(bufferSize);`

Creates a speaker object with 2 buffers of the same size (double buffering).

While the library plays the sound in one buffer your application fills the second one.

The larger the buffer, the more delay there will be in between your application filling a buffer and it being played. The shorter the buffer, the less time your code has to fill the next buffer. Short buffers work best for real-time audio synthesis and longer buffer for playback from an SD card.

### `begin`

`speaker.begin(audioFrequency);`

Sets up the `DAC` pin and `TIM6` timer to trigger at the correct audio freqency. Common frequencies are 44100 Hz, 22050 Hz, 11025 Hz and 8000 Hz.

Starts playing the content of the buffer immediately so you may want to fill the audio buffer before calling `speaker.begin`. The buffer is zero by default so not filling the buffer first would still be OK.

_Note: Do not call `analogWrite(DAC, ...);` when using this library since it completely takes over the DAC peripheral.`

### `end`

`speaker.end();`

Stops the audio playback.

### `ready`

`bool readyForMoreAudio = speaker.ready();`

Returns `true` if the library has a buffer ready to be filled with audio samples.

### `getBuffer`

`uint16_t *buffer = speaker.getBuffer();`

Returns a pointer to an array of `bufferSize` audio samples.

You must only write to this array when `speaker.ready()` is `true`.

## Resources

This library uses the `DAC1` digital to analog converter, `TIM6` basic timer and the `DMA1` direct memory access channel.

## License
Copyright 2016 Julien Vanier
Released under the MIT license
