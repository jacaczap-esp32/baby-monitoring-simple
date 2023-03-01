#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"

typedef int16_t sound_t;                                  // sound will be represented as int16_t (with 2 bytes)
const uint16_t sample_rate = 44100;
const uint8_t microphone_pin = A6; 
AnalogAudioStream in; 
A2DPStream out;                                           // A2DP output
StreamCopy copier(out, in); // copy in to out

#define USE_AUDIO_LOGGING false

ConverterAutoCenter<int16_t> converter;

// Arduino Setup
void setup(void) {
  Serial.begin(115200);

  // RX automatically uses port 0 with pin GPIO34
  auto cfgRx = in.defaultConfig(RX_MODE);
  cfgRx.sample_rate = sample_rate;
  cfgRx.channels = 1;
  in.begin(cfgRx);

  // We send the test signal via A2DP - so we conect to the MyMusic Bluetooth Speaker
  auto cfgA2DP = out.defaultConfig(TX_MODE);
  cfgA2DP.name = "JBL Flip 4";
  cfgA2DP.auto_reconnect = true;
  out.begin(cfgA2DP);
  out.setVolume(1);
  Serial.println("A2DP is connected now...");
}

// Arduino loop  
void loop() {
  copier.copy(converter);
}
