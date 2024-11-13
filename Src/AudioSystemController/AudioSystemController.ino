#include <Wire.h>
#include <ESP_I2S.h>
#include <BluetoothA2DPSinkQueued.h>
#include <LiquidCrystal_I2C.h>
#include "DebugTools.h"


#define I2C_SCL 17
#define I2C_SDA 16

#define I2S_SCK 22
#define I2S_WS 25
#define I2S_SD 26

#define LCD_I2C_ADDRESS 0x27

I2SClass i2s;
BluetoothA2DPSinkQueued a2dp_sink(i2s);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 20, 4);

String currentSongTitle;
String currentSongArtist;
String currentSongAlbum;

struct ScrollState 
{
  int scrollIndex = 0;
  unsigned long previousMillis = 0;
};
ScrollState titleScrollState;
ScrollState artistScrollState;
ScrollState albumScrollState;


void setup() 
{
  Serial.begin(115200);

  Wire.setPins(I2C_SDA, I2C_SCL);
  lcd.init();                      
  lcd.backlight();

  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST 
  | ESP_AVRC_MD_ATTR_ALBUM);
  a2dp_sink.set_avrc_rn_track_change_callback([](uint8_t *id){
    currentSongTitle = "";
    lcd.clear();
    currentSongArtist = "";
    lcd.clear();
    currentSongAlbum = "";
    lcd.clear();
  });
  a2dp_sink.set_avrc_metadata_callback([](uint8_t metadata_id, const uint8_t *metadata_value){
    switch (metadata_id) 
    {
      case ESP_AVRC_MD_ATTR_TITLE:
        currentSongTitle = (const char*)metadata_value;
        break;
      case ESP_AVRC_MD_ATTR_ARTIST:
        currentSongArtist = (const char*)metadata_value;
        break;
      case ESP_AVRC_MD_ATTR_ALBUM:
        currentSongAlbum = (const char*)metadata_value;
        break;
      default:
        break;
    }
  });
  i2s.setPins(I2S_SCK, I2S_WS, I2S_SD);
  i2s.begin(I2S_MODE_STD, 44100, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO, I2S_STD_SLOT_BOTH);
  a2dp_sink.start("AurioSystem");
}

void loop() 
{
  if (a2dp_sink.is_connected())
  {
    printFormattedSongData();
  }
  else
  { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Connected");
  }
}

String repeatString(String text, int times) 
{
  String result = "";
  for (int i = 0; i < times; i++)
    result += text;
  return result;
}

void displayStaticAndScrollData(String dataText, int row, int displayWidth, int interval, ScrollState &state) 
{
  if (dataText.length() > displayWidth) 
  {
    dataText += "  ";
    unsigned long currentMillis = millis();
    if (currentMillis - state.previousMillis >= interval) 
    {
      state.previousMillis = currentMillis;

      lcd.setCursor(0, row);
      lcd.print(repeatString(" ", displayWidth));

      String displayText;
      if (state.scrollIndex + displayWidth <= dataText.length())
        displayText = dataText.substring(state.scrollIndex, state.scrollIndex + displayWidth);
      else
        displayText = dataText.substring(state.scrollIndex) + dataText.substring(0, (state.scrollIndex + displayWidth) % dataText.length());

      lcd.setCursor(0, row);
      lcd.print(displayText);

      state.scrollIndex = (state.scrollIndex + 1) % dataText.length();
    }
  } 
  else 
  {
    lcd.setCursor(0, row);
    lcd.print(dataText);
  }
}

void printFormattedSongData()
{
  displayStaticAndScrollData(currentSongTitle, 0, 20, 250, titleScrollState);
  displayStaticAndScrollData(currentSongArtist, 1, 20, 250, artistScrollState);
  displayStaticAndScrollData(currentSongAlbum, 2, 20, 250, albumScrollState);
}