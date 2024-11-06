#include <Wire.h>
#include <ESP_I2S.h>
#include <BluetoothA2DPSink.h>
#include <LiquidCrystal_I2C.h>
// #include "DebugTools.h"


#define I2C_SCL 17
#define I2C_SDA 16

#define I2S_SCK 22
#define I2S_WS 25
#define I2S_SD 26

#define LCD_I2C_ADDRESS 0x27

I2SClass i2s;
BluetoothA2DPSink a2dp_sink(i2s);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 20, 4);

void setup() 
{
  Serial.begin(115200);

  Wire.begin(I2C_SDA, I2C_SCL);

  i2s.setPins(I2S_SCK, I2S_WS, I2S_SD);
  i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH)
  
  lcd.begin();

  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST 
  | ESP_AVRC_MD_ATTR_ALBUM | ESP_AVRC_MD_ATTR_PLAYING_TIME);
  a2dp_sink.start("AurioSystem");
}

void loop() 
{

}

