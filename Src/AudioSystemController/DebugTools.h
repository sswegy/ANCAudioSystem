#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include <Arduino.h>


namespace dt // debug tools
{
  namespace cb // callbacks
  {
    void on_data_received();
    void read_data_stream(const uint8_t *data, uint32_t length);
    void print_audio_data(const uint8_t* data, uint32_t len);
    void print_avrc_metadata(uint8_t data1, const uint8_t* data2);
  }
}

#endif