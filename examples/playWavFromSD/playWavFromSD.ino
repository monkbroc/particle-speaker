// This #include statement was automatically added by the Particle IDE.
#include <SdFat.h>

// Generate audio with your Photon or Electron
//
// Speaker library by Julien Vanier <jvanier@gmail.com>
// 
// Example: playWavFromSD by ScruffR
//          requires manual import of SdFat library
//          via Particle.function "playWav" the file name of any WAV in the root directory of 
//          the SD card can be selected to play
//          Sending "ls" or "dir" will list the file names in root via USB Serial
//

//SYSTEM_THREAD(ENABLED);

#include <speaker.h>
//#include <SdFat.h>                            // will be auto-included when the library gets imported
#ifndef SdFat_h
#  error "library SdFat needs to imported and/or include statement above may need uncommenting"
#endif

SerialLogHandler logHandler(LOG_LEVEL_NONE, {   // Logging level for non-app messages
    { "app", LOG_LEVEL_INFO }                   // Logging level for application messages
});

// WAV header spec information:
// https://web.archive.org/web/20140327141505/
// https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
// http://www.topherlee.com/software/pcm-tut-wavformat.html
typedef struct wav_header {
  // RIFF Header
  char     riff_header[4];   // Contains "RIFF"
  uint32_t wav_size;         // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
  char     wave_header[4];   // Contains "WAVE"
    
  // Format Header
  char     fmt_header[4];    // Contains "fmt " (includes trailing space)
  uint32_t fmt_chunk_size;   // Should be 16 for PCM
  uint16_t audio_format;     // Should be 1 for PCM. 3 for IEEE Float
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;        // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
  uint16_t sample_alignment; // num_channels * Bytes Per Sample
  uint16_t bit_depth;        // Number of bits per sample
    
  // Data
  char     data_header[4];   // Contains "data"
  uint32_t data_bytes;       // Number of bytes in data. Number of samples * num_channels * sample byte size
  // uint8_t bytes[];        // Remainder of wave file is bytes
} WavHeader_t;

const int    SD_SS      = A2; 
const size_t BUFFERSIZE = 1024;
uint16_t     data[2][BUFFERSIZE];

SdFat        sd;
File         wavFile;
WavHeader_t  wh;

Speaker speaker(data[0], data[1], BUFFERSIZE);

void setup() {
  Particle.function("playWav", selectFile);
 
  if (sd.begin(SD_SS)) 
    Log.info("SD initialised");
  else
    Log.warn("failed to open card");
}

void loop() {
  if (speaker.ready()) {
    readChunk();
  }
}

int selectFile(const char* filename) {
  int retVal = 0;
  
  if (!strcmp("ls", filename) || !strcmp("dir", filename)) {
    sd.ls("/", LS_R);
    return 0;
  }
  
  if (wavFile.isOpen()) wavFile.close();
  
  wavFile = sd.open(filename);
  if (wavFile) {
    memset((uint8_t*)data, 0x80, sizeof(data)); // reset buffer to bias value 0x8080 (quicker via memset() than writing 0x8000 in a loop)
    if (sizeof(wh) == wavFile.read((uint8_t*)&wh, sizeof(wh))) {
      Log.printf("%s\n\r\t%.4s\r\n\tsize: %lu\r\n\t%.4s\r\n\t%.4s\r\n\tchunk size (16?): %lu\r\n\taudio format (1?): %u\r\n\tchannels: %u"
                , filename
                , wh.riff_header
                , wh.wav_size
                , wh.wave_header
                , wh.fmt_header
                , wh.fmt_chunk_size
                , wh.audio_format
                , wh.num_channels
                );
      // two chunks since Log only supports limited length output
      Log.printf("\r\n\tsample rate: %lu\r\n\tbyte rate: %lu\r\n\tsample alignment: %u\r\n\tbit depth: %u\r\n\t%.4s\r\n\tdata bytes: %u"
                , wh.sample_rate
                , wh.byte_rate
                , wh.sample_alignment
                , wh.bit_depth
                , wh.data_header
                , wh.data_bytes
                );
      retVal = wh.data_bytes;
      readChunk();
      speaker.begin(wh.sample_rate);
    }
  } 
  else {
    Log.error("%s not found", filename);
  }

  return retVal;
}

int readChunk() {
  int retVal = 0;
  if (wavFile.isOpen()) {
    uint16_t* wav = speaker.getBuffer();
    uint8_t   buf[BUFFERSIZE * 2 * wh.num_channels];
    int       n = wavFile.read(buf, sizeof(buf));
    if (n < sizeof(buf)) wavFile.close();               // when all data is read close the file
    
    memset((uint8_t*)wav, 0x80, BUFFERSIZE);            // reset buffer to bias value 0x8080
    for(int b = 0; b < n; b += wh.sample_alignment) {
      wav[retVal++] = *(uint16_t*)&buf[b] + 32768;      // convert int16_t to uin16_t with bias 0x8000
    }
  }
  else
    speaker.end();
    
  return retVal;
}
