#include "speaker.h"        
#include "sound.h"

const int audioFrequency = 22050; // Hz

Speaker speaker((uint16_t*)sound, sizeof(sound) / sizeof(sound[0]));
void setup() {
  speaker.begin(audioFrequency);
}
