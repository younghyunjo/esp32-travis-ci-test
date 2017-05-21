#ifndef _ANALOG_SENSOR_H_
#define _ANALOG_SENSOR_H_

#include "driver/adc.h"

enum analog_sensor_max_voltage {
    ANALOG_SENSOR_MAX_VOLTAGE_1_1,
    ANALOG_SENSOR_MAX_VOLTAGE_1_5,
    ANALOG_SENSOR_MAX_VOLTAGE_2_2,
    ANALOG_SENSOR_MAX_VOLTAGE_3_9,
};

int analog_sensor_value(void *instance);

void* analog_sensor_alloc(adc1_channel_t channel, 
        enum analog_sensor_max_voltage voltage);
void analog_sensor_free(void* instance);

void analog_sensor_init(void);
void analog_sensor_cleanup(void);

#endif //#ifndef _ANALOG_SENSOR_H_
