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
