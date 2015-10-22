/**
HTU21D / HPP828E031 driver for mbed.
- Includes RTOS hooks if RTOS is detected during compile.
Author: Kevin Braun
**/

#include "htu21d/htu21d.h"

double theTempIs = 0.0;
double theHumIs = 0.0;

//--------------------------------------------------------------------------------------------------------------------------------------//
//Contstructor

htu21d::htu21d(PinName sda, PinName scl) : _i2c(sda, scl) {
    _i2c.frequency(400000);
}

htu21d::htu21d(PinName sda, PinName scl, int i2cFrequency) : _i2c(sda, scl) {
    _i2c.frequency(i2cFrequency);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Destructor

htu21d::~htu21d() {
}
    
//--------------------------------------------------------------------------------------------------------------------------------------//
//generic routine to get temp or humidity from HTU21D. 
//Returns 14 bits of data (anded 0xFFFC) or 0000 if i2c timeout occurs.
//After a read temp or humidity command, HTU21D responds with NACKs until data is ready.
//NOTE: Use non-hold commands

uint16_t htu21d::getData(uint8_t reg) {
    int htu21 = 0;              //ACK flag
    int htu21cnt = 0;           //number of NACKs before ACK or timeout 
    uint16_t htu21data = 0;     //returned data

    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE); //HTU21Di2cWRITE = 0x80
    _i2c.write(reg);            //read temp, no hold
    _i2c.stop();

    if(htu21 == 0) return 0;    //HTU21T not responding
    do {
        htu21cnt++;

        _i2c.start();
        htu21 = _i2c.write(HTU21Di2cREAD);
        if(htu21 == 1) {
            htu21data = _i2c.read(1) << 8;
            htu21data |= _i2c.read(0) & 0xFC;
            _i2c.stop();
        }
        wait_us(1000);
    } while((htu21cnt < 100) && (htu21 == 0));  //htu21cnt takes 55 to get temp, 16 for humidity (at 1mS loops)
        
    if(htu21 == 0) return 0;    //HTU21D ACK response timed out
    return(htu21data);          //return 14 bit value
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//get temperature from HTU21D in degrees C. Returns with 255.0 if HTU21D had timed out.

float htu21d::getTemp() {
    uint16_t getData = htu21d::getData(HTU21DtempNOHOLD);
    if (getData == 0) return(255.0);                        //return with error
    double tempData = (double)getData / 65536.0;
    theTempIs = -46.85 + (175.72 * tempData);
    return(theTempIs);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//get humidity from HTU21D in percentage. Returns with 255.0 if HTU21D had timed out.

float htu21d::getHum() {
    uint16_t getData = htu21d::getData(HTU21DhumNOHOLD);
    if (getData == 0) return(255.0);                        //return with error
    double tempData = (double)getData / 65536.0;
    theHumIs = -6.0 + (125.0 * tempData);
    return(theHumIs);
}



