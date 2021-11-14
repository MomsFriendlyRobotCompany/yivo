#pragma once

#include <stdint.h>
#include "Arduino.h"

class Rate {
public:
    Rate(uint16_t msec){
        period = msec;
        last = millis();
    }

    void sleep(){
        /* Calls millis() and blocks for the period */
        unsigned long now = millis();
        unsigned long delta = now - last;
        if (delta >= period) {
            last = now;
            return;
        }
        delay(delta);
        last = millis();
    }

    bool check(){
        /* Non-blocking, checks to see if period time has elapsed */
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

class LoopRate {
public:
    LoopRate(): hzToggle(false) {;}
    void toggle(){
        hzToggle = !hzToggle;

        if (hzToggle){
            start = micros();
            count = 1;
        }
    }

    void update(){
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
    }

protected:
    unsigned long count;
    unsigned long start;
    bool hzToggle;
    float hz;
};
