// MIT
// Kevin Walchko (c) 2020
// -----------------------------------------------------------
// accel/gyro: https://adafruit.github.io/Adafruit_LSM6DS/html/class_adafruit___l_s_m6_d_s.html
// mag: https://adafruit.github.io/Adafruit_LIS3MDL/html/class_adafruit___l_i_s3_m_d_l.html
// pressure[lps22]: https://adafruit.github.io/Adafruit_LPS2X/html/_adafruit___l_p_s2_x_8h.html
// pressure[bpm390]: https://github.com/adafruit/Adafruit_BMP3XX/blob/master/bmp3_defs.h
// conversions: https://github.com/adafruit/Adafruit_Sensor/blob/master/Adafruit_Sensor.h
// TFmini: https://github.com/hideakitai/TFmini
// -----------------------------------------------------------

#include <Wire.h>
#include "Arduino.h"
#include "TFmini.h"
#include "packer.hpp"
#include "sensor.hpp"
#include "rate.hpp"

// sensors
TFmini tfmini;   // distance
gciLSM6DSOX imu; // accel / gyro / mag
gciDPS310 pres;  // pressure / temperature

void setup(void) {
    Serial.begin(1000000); // 1Mbps
    Serial.setTimeout(1);
    while (!Serial)
        delay(1000);

    // lidar
    Serial1.begin(TFmini::DEFAULT_BAUDRATE);
    tfmini.attach(Serial1);

    Wire.setClock(400000); // 400 kHz

    // accel / gyro / mag
    imu.init();

    // pressure / temperature
    pres.init();
}

Packer packer(&Serial);
//LoopRate looprate;
float distance = 0.0f;

void loop() {
    int numChar = Serial.available();

    if (numChar > 0){
        int b = Serial.read();

//        // Enable displaying loop rate
//        if (b == 'h'){
//            looprate.toggle();
//        }
//        // update or print debugging loop rate info
//        looprate.update();
        
        if (b == 'g') {
            imu.read();
            pres.read();
            if (tfmini.available()) distance = tfmini.getDistance();
        
            // Send data
            // [FF,FF,msgLen, ...., EE,EE]
            packer.begin();
            if (imu.found) packer.frame(0xFE, imu.ax,imu.ay,imu.az); // 3
            if (imu.found) packer.frame(0xFD, imu.wx,imu.wy,imu.wz); // 3
            if (imu.found) packer.frame(0xFC, imu.mx,imu.my,imu.mz); // 3 
            
            if (pres.found) packer.frame(0xFB, pres.temperature, pres.pressure); // 2
            packer.frame(0xF7, distance);   // 1
            packer.end();
        }
    }
}
