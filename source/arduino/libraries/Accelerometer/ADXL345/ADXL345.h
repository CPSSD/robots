//ADXL345 accelerometer function library
//Barrett Anderies
//Sep. 15, 2012

#include <Arduino.h>
#ifndef ADXL345_h
#define ADXL345_h

//================================Register Addresses===============================//

#define ADXL345_ADDRESS 0x1D                                                     //ADXL345 address (the SEVEN bit device address) the W/R bit is tacked on by the Wire.h library

#define THRESH_TAP_ADDRESS 0x1D                                                  //Tap threshold register
#define OFSX_ADDRESS 0x1E                                                        //X-axis offset register  
#define OFSY_ADDRESS 0x1F                                                        //Y-axis offset register
#define OFSZ_ADDRESS 0x20                                                        //Z-axis offset register
#define DUR_ADDRESS 0x21                                                         //Tap duration register
#define LATENT_ADDRESS 0x22                                                      //Tap Latency register
#define WINDOW_ADDRESS 0x23                                                      //Tap window register
#define THRESH_ACT_ADDRESS 0x24                                                  //Activity threshold register
#define THRESH_INACT_ADDRESS 0x25                                                //Inactivity threshold register
#define TIME_INACT_ADDRESS 0x26                                                  //Inactivity time register
#define ACT_INACT_CTL_ADDRESS 0x27                                               //Axis enable control for activity and inactivity detection register
#define THRESH_FF_ADDRESS 0x28                                                   //Free-fall threshold register
#define TIME_FF_ADDRESS 0x29                                                     //Free-fall time register
#define TAP_AXES_ADDRESS 0x2A                                                    //Axis control for single tap/double tap register 
#define ACT_TAP_STATUS_ADDRESS 0x2B                                              //Source of single tap/double tap register
#define BW_RATE_ADDRESS 0x2C                                                     //Data rate and power mode control register
#define POWER_CTL_ADDRESS 0x2D                                                   //Power-saving features control register
#define INT_ENABLE_ADDRESS 0x2E                                                  //Interrupt enable control register
#define INT_MAP_ADDRESS 0x2F                                                     //Interrupt mapping control register
#define INT_SOURCE_ADDRESS 0x30                                                  //Source of interrupts register
#define DATA_FORMAT_ADDRESS 0x31                                                 //Data format control register
#define DATAX0_ADDRESS 0x32                                                      //X-Axis Data 0 register                                                      
#define DATAX1_ADDRESS 0x33                                                      //X-Axis Data 1 register
#define DATAY0_ADDRESS 0x34                                                      //Y-Axis Data 0 register
#define DATAY1_ADDRESS 0x35                                                      //Y-Axis Data 1 register
#define DATAZ0_ADDRESS 0x36                                                      //Z-Axis Data 0 register
#define DATAZ1_ADDRESS 0x37                                                      //Z-Axis Data 1 register
#define FIFO_CTL_ADDRESS 0x38                                                    //FIFO control register
#define FIFO_STATUS_ADDRESS 0x39                                                 //FIFO status register

class ADXL345 {
      public:
      ADXL345();

      //Write

      void setTHRESH_TAP(byte _THRESH_TAP);
      void setOFSX(byte _OFSX);
      void setOFSY(byte _OFSY);
      void setOFSZ(byte _OFSZ);
      void setDUR(byte _DUR);
      void setLATENT(byte _LATENT);
      void setWINDOW(byte _WINDOW);
      void setTHRESH_ACT(byte _THRESH_ACT);
      void setTHRESH_INACT(byte _THRESH_INACT);
      void setTIME_INACT(byte _TIME_INACT);
      void setACT_INACT_CTL(byte _ACT_INACT_CTL);
      void setTHRESH_FF(byte _THRESH_FF);
      void setTIME_FF(byte _TIME_FF);
      void setTAP_AXES(byte _TAP_AXES);
      void setBW_RATE(byte _BW_RATE);
      void setPOWER_CTL(byte _POWER_CTL);
      void setINT_ENABLE(byte _INT_ENABLE);
      void setINT_MAP(byte _INT_MAP);
      void setDATA_FORMAT(byte _DATA_FORMAT);
      void setFIFO_CTL(byte _FIFO_CTL);

      //Read

      void readTHRESH_TAP(byte *_THRESH_TAP);
      void readOFSX(byte *_OFSX);
      void readOFSY(byte *_OFSY);
      void readOFSZ(byte *_OFSZ);
      void readDUR(byte *_DUR);
      void readLATENT(byte *_LATENT);
      void readWINDOW(byte *_WINDOW);
      void readTHRESH_ACT(byte *_THRESH_ACT);
      void readTHRESH_INACT(byte *_THRESH_INACT);
      void readTIME_INACT(byte *_TIME_INACT);
      void readACT_INACT_CTL(byte *_ACT_INACT_CTL);
      void readTHRESH_FF(byte *_THRESH_FF);
      void readTIME_FF(byte *_TIME_FF);
      void readTAP_AXES(byte *_TAP_AXES);
      void readACT_TAP_STATUS(byte *_ACT_TAP_STATUS);
      void readBW_RATE(byte *_BW_RATE);
      void readPOWER_CTL(byte *_POWER_CTL);
      void readINT_ENABLE(byte *_INT_ENABLE);
      void readINT_MAP(byte *_INT_MAP);
      void readINT_SOURCE(byte *_INT_SOURCE);
      void readDATA_FORMAT(byte *_DATA_FORMAT);
      void readDATAX(byte *_DATAX0, byte *_DATAX1);
      void readDATAY(byte *_DATAY0, byte *_DATAY1);
      void readDATAZ(byte *_DATAZ0, byte *_DATAZ1);
      void readFIFO_CTL(byte *_FIFO_CTL);
      void readFIFO_STATUS(byte *_FIFO_STATUS);

      private:
      void writeADXL345(byte _register, byte _val);
      void readADXL345(byte _register);
};

#endif
