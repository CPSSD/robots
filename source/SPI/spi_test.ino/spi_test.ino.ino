#include "spi.h"

void setup() {
  SPI_wrapper wrapper = new SPI_wrapper();
  Serial.begin(9600);
  Serial.write("Starting up!");
}

void loop() {
  int recv = wrapper.get_data_received();
  if(recv != NULL) {
    Serial.write(recv);
  }
}
