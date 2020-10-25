#include <Adafruit_LSM6DS33.h>
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_LPS2X.h>
#include "AHRS_Madgwick.h"

Madgwick ahrs;

const lsm6ds_data_rate_t accel_data_rate = LSM6DS_RATE_104_HZ;
const lsm6ds_accel_range_t accel_range   = LSM6DS_ACCEL_RANGE_2_G;
const lsm6ds_data_rate_t gyro_data_rate  = LSM6DS_RATE_104_HZ;
const lsm6ds_gyro_range_t gyro_range     = LSM6DS_GYRO_RANGE_2000_DPS;
const lis3mdl_dataRate_t mag_data_rate   = LIS3MDL_DATARATE_40_HZ;
const lps22_rate_t pressure_data_rate    = LPS22_RATE_10_HZ;

unsigned long time_stamp = 0;

Adafruit_LIS3MDL mag;  // magnetometer
Adafruit_LPS22 pt;     // pressure/temperature
Adafruit_LSM6DS33 imu; // accels/gyros

sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
sensors_event_t me;
sensors_event_t pres;

class Rate {
public:
    Rate(uint16_t msec){
        period = msec;
        last = millis();
    }

    void sleep(){
        unsigned long now = millis();
        unsigned long delta = now - last;
        if (delta >= period) {
            last = now;
            return;
        }
        delay(delta);
        last = millis();
    }

    bool fire(){
        unsigned long now = millis();
        unsigned long delta = now - last;
        if (delta >= period) {
            last = now;
            return true;
        }
        return false;
    }

protected:
    unsigned long period;
    unsigned long last;
};

Rate magnetic(25);
Rate prestemp(100);

void setup(void) {
    // Serial.begin(115200);
    // Serial.begin(230400);
    Serial.begin(1000000);

    while (!Serial)
        delay(1000); // will pause until serial console opens

    Serial.println("Hello!");

    Wire.setClock(400000); // not sure this works?

    if (!imu.begin_I2C()) {
        Serial.println("Failed to find LSM6DS33 chip");
        while (1) delay(1000);
    }

    Serial.println("LSM6DS33 Found!");

    imu.setAccelRange(accel_range);
    imu.setAccelDataRate(accel_data_rate);
    imu.setGyroRange(gyro_range);
    imu.setGyroDataRate(gyro_data_rate);

    // Magnetometer -----------------------------------------------------
    if (! mag.begin_I2C()) {
        while (1) {
            delay(1000);
            Serial.println("LIS3MDL NOT Found!");
        }
    }
    Serial.println("LIS3MDL Found!");

    mag.setPerformanceMode(LIS3MDL_MEDIUMMODE);
    mag.setOperationMode(LIS3MDL_CONTINUOUSMODE);
    mag.setDataRate(mag_data_rate);
    mag.setRange(LIS3MDL_RANGE_4_GAUSS);

    // Pressure/Temperature -----------------------------------------------
    if (!pt.begin_I2C()) {
        Serial.println("Failed to find LPS22 chip");
        while (1) {
            delay(1000);
            Serial.println("LPS22 NOT Found!");
        }
    }
    Serial.println("LPS22 Found!");
    pt.setDataRate(pressure_data_rate);

    time_stamp = millis();
}

void loop() {
    imu.getEvent(&accel, &gyro, &temp);

    ahrs.update(
        gyro.gyro.x,gyro.gyro.y,gyro.gyro.z,
        accel.acceleration.x,accel.acceleration.y,accel.acceleration.z,
        me.magnetic.x,me.magnetic.y,me.magnetic.z
    );

    float w,x,y,z;
    ahrs.getQuaternion(&w,&x,&y,&z);
    // Serial.print("Quat: [");
    Serial.print(w);
    Serial.print(", ");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    // Serial.println("]");

    if (prestemp.fire()){
        pt.getEvent(&pres, &temp); // get pressure/temp

        // Serial.print("PT: ");
        // Serial.print(pres.pressure);
        // Serial.print(" hPa, ");
        // Serial.print(temp.temperature);
        // Serial.println(" C");
    }

    // // Display the results (acceleration is measured in m/s^2)
    // Serial.print("A: [");
    // Serial.print(accel.acceleration.x);
    // Serial.print(", ");
    // Serial.print(accel.acceleration.y);
    // Serial.print(", ");
    // Serial.print(accel.acceleration.z);
    // Serial.println("] m/s^2 ");
    //
    // // Display the results (rotation is measured in rad/s)
    // Serial.print("G: [");
    // Serial.print(gyro.gyro.x);
    // Serial.print(", ");
    // Serial.print(gyro.gyro.y);
    // Serial.print(", ");
    // Serial.print(gyro.gyro.z);
    // Serial.println("] radians/s ");

    if (magnetic.fire()) {
        mag.getEvent(&me);

        // Serial.print("M: [");
        // Serial.print(me.magnetic.x);
        // Serial.print(", ");
        // Serial.print(me.magnetic.y);
        // Serial.print(", ");
        // Serial.print(me.magnetic.z);
        // Serial.println("] uT ");
    }

    unsigned long step = millis();
    // Serial.print("TS:");
    Serial.print(",");
    Serial.print(step - time_stamp);
    // Serial.println(" msec --------------------------");
    time_stamp = step;
    Serial.println();
}
