#include "DebugTools.h"


void dt::cb::on_data_received() 
{
  Serial.println("Data packet received");
}

void dt::cb::print_audio_data(const uint8_t* data, uint32_t len) 
{
  Serial.print("Received audio packet, length: ");
  Serial.println(len);
  for (uint32_t i = 0; i < len; i++) 
  {
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void dt::cb::print_avrc_metadata(uint8_t data1, const uint8_t* data2) 
{
  Serial.printf("AVRC metadata rsp: attribute id 0x%x, %s\n", data1, data2);
}