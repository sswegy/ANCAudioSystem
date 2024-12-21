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

#define PLAY 18
#define NEXT 19
#define PREV 21

#define AUX_LEFT 34
#define AUX_RIGHT 35

#define LCD_I2C_ADDRESS 0x27
#define LCD_DISPLAY_WIDTH 20
#define DISPLAY_SCROLL_INTERVAL_MS 250

#define BUTTON_DEBOUNCE_DELAY_MS 50

I2SClass i2s;
BluetoothA2DPSinkQueued a2dp_sink(i2s);
LiquidCrystal_I2C lcd(LCD_I2C_ADDRESS, 20, 4);

String currentSongTitle;
String currentSongArtist;
String currentSongAlbum;

esp_avrc_playback_stat_t playbackState;

struct ScrollState 
{
  int scrollIndex = 0;
  unsigned long previousMillis = 0;
};
ScrollState titleScrollState;
ScrollState artistScrollState;
ScrollState albumScrollState;

struct ButtonState 
{
  bool currentState;
  bool lastReading;
  unsigned long lastDebounceTime; 
};
ButtonState playButtonState = {false, false, 0};
ButtonState nextButtonState = {false, false, 0};
ButtonState prevButtonState = {false, false, 0};


void setup() 
{
  Serial.begin(115200);

  pinMode(PLAY, INPUT);
  pinMode(NEXT, INPUT);
  pinMode(PREV, INPUT);

  Wire.setPins(I2C_SDA, I2C_SCL);
  lcd.init();                      
  lcd.backlight();

  a2dp_sink.set_avrc_rn_track_change_callback([](uint8_t *id){
    currentSongTitle = "                    ";
    currentSongArtist = "                    ";
    currentSongAlbum = "                    ";
    lcd.clear();
  });
  a2dp_sink.set_avrc_metadata_attribute_mask(ESP_AVRC_MD_ATTR_TITLE | ESP_AVRC_MD_ATTR_ARTIST 
  | ESP_AVRC_MD_ATTR_ALBUM);
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
  a2dp_sink.set_avrc_rn_playstatus_callback([](esp_avrc_playback_stat_t playback_state){
    playbackState = playback_state;
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
    if (debounceButton(PLAY, BUTTON_DEBOUNCE_DELAY_MS, playButtonState))
    {
      if (playbackState == ESP_AVRC_PLAYBACK_PLAYING)
        a2dp_sink.pause();
      else if (playbackState == ESP_AVRC_PLAYBACK_PAUSED)
        a2dp_sink.play();
    }
    else if (debounceButton(NEXT, BUTTON_DEBOUNCE_DELAY_MS, nextButtonState))
      a2dp_sink.next();
    else if (debounceButton(PREV, BUTTON_DEBOUNCE_DELAY_MS, prevButtonState))
      a2dp_sink.previous();
  }
  else
  { 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Connected");
  }
}

bool debounceButton(uint8_t buttonPin, unsigned long debounceDelay, ButtonState &state) 
{
  bool reading = digitalRead(buttonPin);
  if (reading != state.lastReading)
    state.lastDebounceTime = millis();

  if ((millis() - state.lastDebounceTime) > debounceDelay)
    if (reading != state.currentState)
      state.currentState = reading;

  state.lastReading = reading;
  return state.currentState;
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
  displayStaticAndScrollData(currentSongTitle, 0, LCD_DISPLAY_WIDTH, DISPLAY_SCROLL_INTERVAL_MS, titleScrollState);
  displayStaticAndScrollData(currentSongArtist, 1, LCD_DISPLAY_WIDTH, DISPLAY_SCROLL_INTERVAL_MS, artistScrollState);
  displayStaticAndScrollData(currentSongAlbum, 2, LCD_DISPLAY_WIDTH, DISPLAY_SCROLL_INTERVAL_MS, albumScrollState);
}