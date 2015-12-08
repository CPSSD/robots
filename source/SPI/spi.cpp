#include "spi.h"
#include <SPI.h>
#include "Arduino.h"

using namespace std;


SPI_wrapper::SPI_wrapper() {
    SPISettings(100000, MSBFIRST, SPI_MODE0);
    SPI.begin();

       
    SPCR |= bit(SPE);
    pinMode(MISO, OUTPUT);
    SPI.attachInterrupt();

    currently_receiving = false;
}


SPI_wrapper::~SPI_wrapper() {
    SPI.end();
}


void SPI_wrapper::enqueue_data_for_transfer(int data) {
    data_out.push(data);
}


void SPI_wrapper::enqueue_data_received(int data) {
    data_in.push(data);
}


int SPI_wrapper::get_data_for_transfer() {
    int return_me = data_out.front();
    data_out.pop();

    return return_me;
}


int SPI_wrapper::get_data_received() {
    int return_me = data_in.front();
    data_in.pop();

    return return_me;
}


void SPI_wrapper::dispatch_response() {
    //TODO: Call other functions here.
}


ISR(SPI_STC_vect) {
    int data_just_received = SPDR;

    // Command not begun yet.
    if(data_just_received == 255 && !currently_receiving) {
        currently_receiving = true;

        return;
    }

    SPI_wrapper::enque_data_received(data_just_received);  // grab byte from SPI Data Register
    SPDR = get_data_for_transfer();
}
