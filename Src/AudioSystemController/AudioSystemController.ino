#include <ESP_I2S.h>
#include <BluetoothA2DPSink.h>
// #include "DebugTools.h"

#define I2S_SCK 22
#define I2S_WS 25
#define I2S_SDOUT 26
I2SClass i2s;
BluetoothA2DPSink a2dp_sink(i2s);

void setup() 
{
  Serial.begin(115200);

  i2s.setPins(I2S_SCK, I2S_WS, I2S_SDOUT);
  if (!i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)) 
    Serial.println("\n|FAIL| Initialize I2S");
  else
    Serial.println("\n|SUCCESS| Initialize I2S");
  
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST 
  | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_PLAYING_TIME);
  a2dp_sink.start("Chevrolet Cruze Audio System");
}

void loop() 
{

}

