#include <Arduino.h>
#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"

using namespace audio_tools;

typedef int16_t sound_t;                                  // sound will be represented as int16_t (with 2 bytes)
const uint16_t sample_rate = 44100; 
bool removePoppingFromSoundBeginning = false;
bool removePoppingFromSoundEnd = true;
uint8_t from_channels = 1;
uint8_t to_channels = 2;

AnalogAudioStream analogInputStream; 
A2DPStream a2dpOutputStream;                  

ChannelFormatConverterStreamT<sound_t> increaseChannelsStream(analogInputStream);

ConverterAutoCenter<sound_t> autoCenterConverter;
// SilenceRemovalConverter this can be used to add sound threshold so that we don't send very quiet sounds 

PoppingSoundRemover<sound_t> poppingSoundRemover(to_channels, removePoppingFromSoundBeginning, removePoppingFromSoundEnd); 
ConverterScaler<sound_t> volumeScalerConverter(2, 0, INT16_MAX);
MultiConverter<sound_t> firstConverters(autoCenterConverter, volumeScalerConverter);
MultiConverter<sound_t> converters(firstConverters, poppingSoundRemover);

StreamCopy copier(a2dpOutputStream, increaseChannelsStream);

// Arduino Setup
void setup(void) {
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Error);

  // RX automatically uses port 0 with pin GPIO34
  auto cfgRx = analogInputStream.defaultConfig(RX_MODE);
  cfgRx.sample_rate = sample_rate;
  cfgRx.channels = from_channels;
  analogInputStream.begin(cfgRx);

  increaseChannelsStream.begin(from_channels, to_channels);

  auto cfgA2DP = a2dpOutputStream.defaultConfig(TX_MODE);
  cfgA2DP.name = "JBL Flip 4";
  cfgA2DP.auto_reconnect = true;
  a2dpOutputStream.begin(cfgA2DP);
  a2dpOutputStream.setVolume(1.0);
}

// Arduino loop  
void loop() {
  copier.copy(converters);
}
