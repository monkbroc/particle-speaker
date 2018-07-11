#pragma once

// Speaker library by Julien Vanier <jvanier@gmail.com>
//   Adaptions by ScruffR
//     - make double buffering optinal
//     - option to provide "external" buffer(s) to the library
//       which allows to play PCM data stored in flash without copying 

#if PLATFORM_ID == 6 || PLATFORM_ID == 10

#include "application.h"

class Speaker
{
public:
    // Constructor.
    Speaker(uint16_t bufferSize, bool dblBuffer = true);
    Speaker(uint16_t *buffer, uint16_t bufferSize);                       // single external buffer
    Speaker(uint16_t *buffer0, uint16_t *buffer1, uint16_t bufferSize);   // two external buffers for double buffering
    ~Speaker();

    // Start playback
    void begin(uint16_t audioFrequency);

    // Stop playback
    void end();

    // Is a buffer ready to be filled
    bool ready();

    // The next buffer to fill with audio data
    uint16_t *getBuffer();
private:
    void setupHW(uint16_t audioFrequency);
    uint16_t timerAutoReloadValue(uint16_t audioFrequency);
    uint8_t currentBuffer();

    bool privateBuffer;
    uint8_t numBuffers;
    uint16_t bufferSize;
    uint8_t lastBuffer;
    uint16_t audioFrequency;

    /* Double buffers for audio data */
    uint16_t *buffer[2];
};

#else // PLATFORM_ID == 6 || PLATFORM_ID == 10

#error "The speaker library is only compatible with the Photon and Electron."

#endif // PLATFORM_ID == 6 || PLATFORM_ID == 10
