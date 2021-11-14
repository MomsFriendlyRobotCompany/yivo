#pragma once
#include <Wire.h>
#include <stdint.h>

#include <Adafruit_LSM6DSOX.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor.h> // SENSORS_PRESSURE_SEALEVELHPA, SENSORS_GRAVITY_STANDARD


class gciLSM6DSOX {
public:
    gciLSM6DSOX(): found(false), soxFound(false), magFound(false), invg(1.0/SENSORS_GRAVITY_STANDARD) {;}

    void init(){
        if (sox.begin_I2C()) {
            soxFound = true;
    
            // Accelerometer ------------------------------------------
            sox.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
            sox.setAccelDataRate(LSM6DS_RATE_208_HZ);
    
            // Gyros ----------------------------------------------------
            sox.setGyroRange(LSM6DS_GYRO_RANGE_2000_DPS);
            sox.setGyroDataRate(LSM6DS_RATE_208_HZ);
        }

        // Magnetometer -----------------------------------------------------
        if (lis3mdl.begin_I2C()) {
          magFound = true;
          // lis3mdl.setPerformanceMode(LIS3MDL_ULTRAHIGHMODE); // 155 already does this
          // lis3mdl.setPerformanceMode(LIS3MDL_HIGHMODE); // 300 already does this
          lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
          lis3mdl.setDataRate(LIS3MDL_DATARATE_300_HZ); // sets LIS3MDL_HIGHMODE
          lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
        }

        found = magFound && soxFound;
    }

    void read(){
        if (soxFound) {
            sox.getEvent(&a,&g,&t);
    
            ax = a.acceleration.x * invg;
            ay = a.acceleration.y * invg;
            az = a.acceleration.z * invg;
    
            wx = g.gyro.x;
            wy = g.gyro.y;
            wz = g.gyro.z;
        }

        if (magFound){
            lis3mdl.getEvent(&mag);
    
            mx = mag.magnetic.x;
            my = mag.magnetic.y;
            mz = mag.magnetic.z;
        }
    }
    
    bool found;
    float ax, ay, az;
    float mx, my, mz;
    float wx, wy, wz;

protected:
    bool soxFound;
    bool magFound;
    Adafruit_LSM6DSOX sox; // accel and gyro
    Adafruit_LIS3MDL lis3mdl; // magnetometer
    const float invg;
    sensors_event_t a,g,t;
    sensors_event_t mag;
};

#include <Adafruit_DPS310.h>

class gciDPS310 {
public:
    gciDPS310(): found(false) {;}

    /* Sets up the sensor */
    void init(){
        if (dps.begin_I2C()) {
            dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
            dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);
            found = true;
        }
    }

    void read(){
        if (found){
            if (dps.temperatureAvailable() || dps.pressureAvailable()) {
                dps.getEvents(&temp_event, &pressure_event);
                temperature = temp_event.temperature;
                pressure = pressure_event.pressure;
            }
        }
    }

    bool found;
    float pressure;
    float temperature;
    
protected:
    Adafruit_DPS310 dps; // pressure / temperature
    sensors_event_t temp_event, pressure_event;
};
