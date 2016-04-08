//ADXL345 accelerometer function library
//Barrett Anderies
//Sep. 15, 2012

#include <Arduino.h>
#include <ADXL345.h>
#include <Wire.h>

ADXL345::ADXL345()
   {
     Wire.begin();
   }

//=====================Write=====================//

void ADXL345::setTHRESH_TAP(byte _THRESH_TAP)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_TAP_ADDRESS);       
       Wire.write(_THRESH_TAP);
       Wire.endTransmission();
   }

void ADXL345::setOFSX(byte _OFSX)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSX_ADDRESS);       
       Wire.write(_OFSX);
       Wire.endTransmission();
   }

void ADXL345::setOFSY(byte _OFSY)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSY_ADDRESS);       
       Wire.write(_OFSY);
       Wire.endTransmission();
   }

void ADXL345::setOFSZ(byte _OFSZ)
   {  
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSZ_ADDRESS);       
       Wire.write(_OFSZ);
       Wire.endTransmission();
   }

void ADXL345::setDUR(byte _DUR)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DUR_ADDRESS);       
       Wire.write(_DUR);
       Wire.endTransmission();
   }

void ADXL345::setLATENT(byte _LATENT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(LATENT_ADDRESS);       
       Wire.write(_LATENT);
       Wire.endTransmission();
   }

void ADXL345::setWINDOW(byte _WINDOW)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(WINDOW_ADDRESS);       
       Wire.write(_WINDOW);
       Wire.endTransmission();
   }

void ADXL345::setTHRESH_ACT(byte _THRESH_ACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_ACT_ADDRESS);       
       Wire.write(_THRESH_ACT);
       Wire.endTransmission();
   }

void ADXL345::setTHRESH_INACT(byte _THRESH_INACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_INACT_ADDRESS);       
       Wire.write(_THRESH_INACT);
       Wire.endTransmission();
   }

void ADXL345::setTIME_INACT(byte _TIME_INACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TIME_INACT_ADDRESS);       
       Wire.write(_TIME_INACT);
       Wire.endTransmission();
   }

void ADXL345::setACT_INACT_CTL(byte _ACT_INACT_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(ACT_INACT_CTL_ADDRESS);       
       Wire.write(_ACT_INACT_CTL);
       Wire.endTransmission();
   }

void ADXL345::setTHRESH_FF(byte _THRESH_FF)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_FF_ADDRESS);       
       Wire.write(_THRESH_FF);
       Wire.endTransmission();
   }

void ADXL345::setTIME_FF(byte _TIME_FF)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TIME_FF_ADDRESS);       
       Wire.write(_TIME_FF);
       Wire.endTransmission();
   }

void ADXL345::setTAP_AXES(byte _TAP_AXES)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TAP_AXES_ADDRESS);       
       Wire.write(_TAP_AXES);
       Wire.endTransmission();
   }

void ADXL345::setBW_RATE(byte _BW_RATE)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(BW_RATE_ADDRESS);       
       Wire.write(_BW_RATE);
       Wire.endTransmission();
   }

void ADXL345::setPOWER_CTL(byte _POWER_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(POWER_CTL_ADDRESS);       
       Wire.write(_POWER_CTL);
       Wire.endTransmission();
   }

void ADXL345::setINT_ENABLE(byte _INT_ENABLE)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(INT_ENABLE_ADDRESS);       
       Wire.write(_INT_ENABLE);
       Wire.endTransmission();
   }

void ADXL345::setINT_MAP(byte _INT_MAP)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(INT_MAP_ADDRESS);       
       Wire.write(_INT_MAP);
       Wire.endTransmission();
   }

void ADXL345::setDATA_FORMAT(byte _DATA_FORMAT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATA_FORMAT_ADDRESS);       
       Wire.write(_DATA_FORMAT);
       Wire.endTransmission();
   }
void ADXL345::setFIFO_CTL(byte _FIFO_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(FIFO_CTL_ADDRESS);       
       Wire.write(_FIFO_CTL);
       Wire.endTransmission();
   }

//======================Read=====================//


void ADXL345::readTHRESH_TAP(byte *_THRESH_TAP)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_TAP_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_THRESH_TAP = Wire.read();
   }

void ADXL345::readOFSX(byte *_OFSX)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSX_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_OFSX = Wire.read();
   }

void ADXL345::readOFSY(byte *_OFSY)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSY_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_OFSY = Wire.read();
   }

void ADXL345::readOFSZ(byte *_OFSZ)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(OFSZ_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_OFSZ = Wire.read();
   }

void ADXL345::readDUR(byte *_DUR)
   {       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DUR_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DUR = Wire.read();
   }

void ADXL345::readLATENT(byte *_LATENT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(LATENT_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_LATENT = Wire.read();
   }

void ADXL345::readWINDOW(byte *_WINDOW)
   { 
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(WINDOW_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_WINDOW = Wire.read();
   }

void ADXL345::readTHRESH_ACT(byte *_THRESH_ACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_ACT_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_THRESH_ACT = Wire.read();
   }

void ADXL345::readTHRESH_INACT(byte *_THRESH_INACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_INACT_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_THRESH_INACT = Wire.read();
   }

void ADXL345::readTIME_INACT(byte *_TIME_INACT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TIME_INACT_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_TIME_INACT = Wire.read();
   }

void ADXL345::readACT_INACT_CTL(byte *_ACT_INACT_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(ACT_INACT_CTL_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_ACT_INACT_CTL = Wire.read();
   }

void ADXL345::readTHRESH_FF(byte *_THRESH_FF)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(THRESH_FF_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_THRESH_FF = Wire.read();
   }

void ADXL345::readTIME_FF(byte *_TIME_FF)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TIME_FF_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_TIME_FF = Wire.read();
   }

void ADXL345::readTAP_AXES(byte *_TAP_AXES)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(TAP_AXES_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_TAP_AXES = Wire.read();
   }

void ADXL345::readACT_TAP_STATUS(byte *_ACT_TAP_STATUS)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(ACT_TAP_STATUS_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_ACT_TAP_STATUS = Wire.read();
   }

void ADXL345::readBW_RATE(byte *_BW_RATE)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(BW_RATE_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_BW_RATE = Wire.read();
   }

void ADXL345::readPOWER_CTL(byte *_POWER_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(POWER_CTL_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_POWER_CTL = Wire.read();
   }

void ADXL345::readINT_ENABLE(byte *_INT_ENABLE)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(INT_ENABLE_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_INT_ENABLE = Wire.read();
   }

void ADXL345::readINT_MAP(byte *_INT_MAP)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(INT_MAP_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_INT_MAP = Wire.read();
   }

void ADXL345::readINT_SOURCE(byte *_INT_SOURCE)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(INT_SOURCE_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_INT_SOURCE = Wire.read();
   }

void ADXL345::readDATA_FORMAT(byte *_DATA_FORMAT)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATA_FORMAT_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATA_FORMAT = Wire.read();
   }

void ADXL345::readDATAX(byte *_DATAX0, byte *_DATAX1)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAX0_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAX0= Wire.read();

       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAX1_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAX1= Wire.read();
   }

void ADXL345::readDATAY(byte *_DATAY0, byte *_DATAY1)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAY0_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAY0= Wire.read();

       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAY1_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAY1= Wire.read();
   }

void ADXL345::readDATAZ(byte *_DATAZ0, byte *_DATAZ1)
   { 
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAZ0_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAZ0= Wire.read();

       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(DATAZ1_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_DATAZ1= Wire.read();
   }

void ADXL345::readFIFO_CTL(byte *_FIFO_CTL)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(FIFO_CTL_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_FIFO_CTL = Wire.read();
   }

void ADXL345::readFIFO_STATUS(byte *_FIFO_STATUS)
   {
       Wire.beginTransmission(ADXL345_ADDRESS);
       Wire.write(FIFO_STATUS_ADDRESS);
       Wire.endTransmission();
       Wire.requestFrom(ADXL345_ADDRESS, 1);
       *_FIFO_STATUS = Wire.read();
   }
