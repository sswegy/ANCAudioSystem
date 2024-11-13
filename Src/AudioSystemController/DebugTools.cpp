#include "DebugTools.h"


void dt::cb::on_data_received() 
{
  Serial.println(F("Data packet received"));
}

void dt::cb::read_data_stream(const uint8_t *data, uint32_t length) 
{
    int16_t *values = (int16_t*) data;
    for (int j = 0; j < length / 2; j += 2)
    {
      Serial.print(values[j]);
      Serial.print(",");
      Serial.println(values[j+1]);
    }
}

void dt::cb::print_avrc_metadata(uint8_t data1, const uint8_t* data2) 
{
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);
}