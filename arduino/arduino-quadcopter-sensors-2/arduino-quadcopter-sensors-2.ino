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

#include <Adafruit_Sensor.h>
#include <Adafruit_DPS310.h>
#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h> // SENSORS_PRESSURE_SEALEVELHPA, SENSORS_GRAVITY_STANDARD

TFmini tfmini;
Adafruit_LIS3MDL lis3mdl; // magnetometer
//GCI_BMP3XX bmp;           // pressure
Adafruit_DPS310 dps;      // pressure
Adafruit_LSM6DSOX sox;    // accels/gyros
//hp_BH1750 bh1750;         // light
//Adafruit_MLX90640 mlx;    // ir camera
//float frame[32*24];       // buffer for full frame of temperatures
//float m[16]; // dummy array for sensor data storage
//byte const* p = reinterpret_cast<byte const *>(m);
//byte const* pmlx = reinterpret_cast<byte const *>(frame);
sensors_event_t a;
sensors_event_t g;
sensors_event_t tmp;
sensors_event_t mag;
float ax, ay, az;
float mx, my, mz;
float wx, wy, wz;
float pres, alt;
float temp;
float distance = 0.0f;
unsigned long start = 0;
unsigned int count = 1;
unsigned int mlxCount = 1;
const float invg = 1.0/SENSORS_GRAVITY_STANDARD;
float lux = 0.0;

bool soxFound = false;
bool lisFound = false;
bool bmpFound = false;
bool bh1750Found = false;
bool mlxFound = false;
bool dps310Found = false;

bool accelFound = false;
bool gyroFound = false;
bool magFound = false;
bool lightFound = false;
bool irFound = false;
bool pressFound = false;
bool lidarFound = false;


uint8_t messageLength = 56; // count [EE,EE] at end

Packer packer(&Serial);

// static float f[3];
// static byte bf[13];
//
// void frame(byte id, float a){
//     byte const* p = reinterpret_cast<byte const *>(f);
//     f[0] = a;
//     bf[0] = id;
//     memcpy(&bf[1], p, 4);
//     Serial.write(bf, 5);
// }
//
// void frame(byte id, float a, float b){
//     byte const* p = reinterpret_cast<byte const *>(f);
//     f[0] = a;
//     f[1] = b;
//     bf[0] = id;
//     memcpy(&bf[1], p, 8);
//     Serial.write(bf, 9);
// }
//
// void frame(const byte id, const float a, const float b, const float c){
//     byte const* p = reinterpret_cast<byte const *>(f);
//     f[0] = a;
//     f[1] = b;
//     f[2] = c;
//     bf[0] = id;
//     memcpy(&bf[1], p, 12);
//     Serial.write(bf, 13);
// }

void setup(void) {
    Serial.begin(1000000); // 1Mbps
//    Serial.begin(115200);
    Serial.setTimeout(1);
    while (!Serial)
        delay(1000); // will pause Zero, Leonardo, etc until serial console opens

    Serial1.begin(TFmini::DEFAULT_BAUDRATE);
    tfmini.attach(Serial1);
    lidarFound = true;

    Wire.setClock(400000); // 400 kHz

    if (sox.begin_I2C()) {
        soxFound = true;
        accelFound = true;
        gyroFound = true;
        messageLength += 24; // 6f * 4B = 24B
        messageLength += 1; // header

        // Accelerometer ------------------------------------------
        sox.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
        sox.setAccelDataRate(LSM6DS_RATE_208_HZ);

        // Gyros ----------------------------------------------------
        sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
        sox.setGyroDataRate(LSM6DS_RATE_208_HZ);
    }

    // Magnetometer -----------------------------------------------------
      if (lis3mdl.begin_I2C()) {
          lisFound = true;
          magFound = true;
          messageLength += 12; // 3f * 4B = 12 B
//          messageLength += 1; // header - this is captured with the accel/gyro
          // lis3mdl.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE); // 155 already does this
          // lis3mdl.setPerformanceMode(LIS3MDL_HIGHMODE); // 300 already does this
          lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
          lis3mdl.setDataRate(LIS3MDL_DATARATE_300_HZ); // sets LIS3MDL_HIGHMODE
          lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
      }

    // Pressure/Temperature -----------------------------------------------
//    if (bmp.begin_I2C()) {
//        bmpFound = true;
//        messageLength += 8; // 2f * 4B = 8B
//        messageLength += 1; // header
//
//        // Set up oversampling and filter initialization
//        // Datasheet, table 9, pg 17
//        if (1) {
//            // Drone
//            // Mode: Normal
//            // OS Pres: x8
//            // OS Temp: x1
//            // IIR: 2 -> 3
//            // Sampling: 50Hz
//            // RMS Noise [cm]: 11
//            bmp.setTemperatureOversampling(BMP3_NO_OVERSAMPLING);
//            bmp.setPressureOversampling(BMP3_OVERSAMPLING_8X);
//            bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
//            bmp.setOutputDataRate(BMP3_ODR_50_HZ);
//        }
//        else {
//            // Indoor nav
//            // Mode: Normal
//            // OS Pres: x16
//            // OS Temp: x2
//            // IIR: 4 -> 15
//            // Sampling: 25Hz
//            // RMS Noise [cm]: 5
//            bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_2X);
//            bmp.setPressureOversampling(BMP3_OVERSAMPLING_16X);
//            bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_15);
//            bmp.setOutputDataRate(BMP3_ODR_25_HZ);
//        }
//    }
    if (dps.begin_I2C()) {
        dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
        dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
        dps310Found = true;
        pressFound = true;
    }

    // Light ----------------------------------------------------------------
//    if (bh1750.begin(BH1750_TO_GROUND)) {
//        bh1750Found = true;
//        messageLength += 4; // 1f * 4B = 4B
//        messageLength += 1; // header
//        bh1750.calibrateTiming();  // calibrate the timings, about 855ms with a bad chip
//        bh1750.start();            // start the first measurement
//    }

//    // LIDAR
//    if (tfmini.available()) {
//        distance = tfmini.getDistance();
//    }

    // IR -------------------------------------------------------------------
    // WARNING: this driver is buggy and has a beat that slows everything
    //          down for some reason.
//    if (mlx.begin(MLX90640_I2CADDR_DEFAULT, &Wire)) {
//        mlxFound = true;
//        //mlx.setMode(MLX90640_INTERLEAVED);
//        mlx.setMode(MLX90640_CHESS);
//        mlx.setResolution(MLX90640_ADC_18BIT);
//        mlx.setRefreshRate(MLX90640_64_HZ);
//    }

}

/*
[0xFF,0xFF]: start
0xFE: accel
0xFD: gyro
0xFC: mag
0xFB: temperature, pressure
0xFA:
0xF9: light
0xF8: MLX90640 IR camera
0xF7: lidar
0xF6-0xF3: unused
0xF2: quaternion
0xF1: velocity
0xF0: position
[0xEE,0xEE]: end
*/
//
//const uint8_t header[2] = {0xFF,0xFF};
//const uint8_t ender[2] = {0xEE,0xEE};
float hz = 0.0;
bool hzToggle = false;

void loop() {
    int numChar = Serial.available();
    int b = 0;
    if (numChar > 0) b = Serial.read();

    // Enable displaying loop rate
    if (b == 'h'){
        hzToggle = !hzToggle;
        start = micros();
        count = 1;
    }
    // update or print debugging loop rate info
    if (hzToggle){
        if (count % 10 == 0){
            unsigned long now = micros();
            hz = 1000000.0*float(count+1)/float(now - start);
            Serial.println(hz,2);
            count = 1;
            start = now;
        }
        else {
            count += 1;
        }
    }

//    if (mlxFound && 0){
//        if (mlxCount >= 100){
//            mlx.getFrame(frame);
//            mlxCount = 0;
//        }
//        else {
//            mlxCount +=1;
//        }
//        if (b == 'm') {
//            Serial.write(header, 2);
//            Serial.write(0xf8);
//            Serial.write(pmlx, sizeof(frame));
//            Serial.write(ender, 2);
//            return;
//        }
//    }

    if (soxFound) {
        sox.getEvent(&a,&g,&tmp);

        ax = a.acceleration.x * invg;
        ay = a.acceleration.y * invg;
        az = a.acceleration.z * invg;

        wx = g.gyro.x;
        wy = g.gyro.y;
        wz = g.gyro.z;
    }

    if (lisFound){
        lis3mdl.getEvent(&mag);

        mx = mag.magnetic.x;
        my = mag.magnetic.y;
        mz = mag.magnetic.z;
    }

//    if (bmpFound) {
//        bmp.performReading();
//        pres = bmp.pressure;
//        temp = bmp.temperature; // C
//    }

    if (dps310Found){
        sensors_event_t temp_event, pressure_event;
        if (dps.temperatureAvailable() || dps.pressureAvailable()) {
            dps.getEvents(&temp_event, &pressure_event);
            temp = temp_event.temperature;
            pres = pressure_event.pressure;
        }
    }

//    if (bh1750Found){
//       if (bh1750.hasValue() == true) {    // non blocking reading
//           lux = bh1750.getLux();
//           bh1750.start();
//       }
//   }

    // LIDAR
    if (tfmini.available()) {
        distance = tfmini.getDistance();
    }

    // Send data
    // [FF,FF,msgLen, ...., EE,EE]
    if (b == 'g') {
//        Serial.write(header, 2);
//        Serial.write(messageLength);

        packer.begin();
        if (accelFound) packer.frame(0xFE, ax,ay,az);   // 3
        if (gyroFound) packer.frame(0xFD, wx,wy,wz);    // 3
        if (magFound) packer.frame(0xFC, mx,my,mz);     // 3 
        if (pressFound) packer.frame(0xFB, temp, pres); // 2
//        if (lightFound) packer.frame(0xF9, lux);        // 1
        if (lidarFound) packer.frame(0xF7, distance);   // 1
        packer.end();

//        Serial.write(ender, 2);
    }
}
