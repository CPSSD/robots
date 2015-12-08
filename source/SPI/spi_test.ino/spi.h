#ifndef SPI_h
#define SPI_h

#include "Arduino.h"

using namespace std;


class SPI_wrapper {
    SPI_wrapper();
    ~SPI_wrapper();


    public:
        void enqueue_data(int data);

    private:
        static bool currently_receiving;

        static queue<int> data_out;
        static queue<int> data_in;

        void dispatch_response();
        
};

#endif
