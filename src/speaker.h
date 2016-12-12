#pragma once

// Speaker library by Julien Vanier <jvanier@gmail.com>

#if PLATFORM_ID == 6 || PLATFORM_ID == 10

#include "application.h"

class Speaker
{
public:
    // Constructor.
    Speaker(uint16_t bufferSize);
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

    uint16_t bufferSize;
    uint16_t audioFrequency;

    /* Double buffers for audio data */
    uint16_t *buffer0;
    uint16_t *buffer1;
};

#else // PLATFORM_ID == 6 || PLATFORM_ID == 10

#error "The speaker library is only compatible with the Photon and Electron."

#endif // PLATFORM_ID == 6 || PLATFORM_ID == 10
