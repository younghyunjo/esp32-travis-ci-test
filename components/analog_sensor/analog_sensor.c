#include <stdbool.h>

#include "analog_sensor.h"
#include "esp_log.h"

#define TAG "ANALOG_SENSOR"

static bool _initialized = false;

struct analog_sensor {
    adc1_channel_t channel;
    bool allocated;
};
static struct analog_sensor _analog_sensor[ADC1_CHANNEL_MAX];

int analog_sensor_value(void *instance)
{
    if (!instance) {
        return -1;
    }

    struct analog_sensor *a = instance;

    return adc1_get_voltage(a->channel);
}

void* analog_sensor_alloc(adc1_channel_t channel, 
        enum analog_sensor_max_voltage voltage)
{
    if (!_initialized) {
        ESP_LOGE(TAG, "not initialized\n");
        return NULL;
    }

    adc_atten_t atten;
    switch (voltage) {
    case ANALOG_SENSOR_MAX_VOLTAGE_1_1:
        atten = ADC_ATTEN_0db;
        break;
    case ANALOG_SENSOR_MAX_VOLTAGE_1_5:
        atten = ADC_ATTEN_2_5db;
        break;
    case ANALOG_SENSOR_MAX_VOLTAGE_2_2:
        atten = ADC_ATTEN_6db;
        break;
    case ANALOG_SENSOR_MAX_VOLTAGE_3_9:
        atten = ADC_ATTEN_11db;
        break;
    default:
        ESP_LOGE(TAG, "invalid voltage\n");
        return NULL;
    }

    if (_analog_sensor[channel].allocated) {
        ESP_LOGE(TAG, "alread allocated\n");
        return NULL;
    }

    adc1_config_channel_atten(channel, atten);

    _analog_sensor[channel].allocated = true;
    _analog_sensor[channel].channel = channel;
    return &_analog_sensor[channel];
}

void analog_sensor_free(void *instance)
{
    if (!_initialized) {
        ESP_LOGE(TAG, "not initialized\n");
        return;
    }

    if (!instance)
        return;

    struct analog_sensor *a = instance;

    a->allocated = false;
}

void analog_sensor_init(void)
{
    if (_initialized) {
        return;
    }

    //adc1_config_width(ADC_WIDTH_12Bit);
    adc1_config_width(ADC_WIDTH_12Bit);

    _initialized =  true;
}

void analog_sensor_cleanup(void)
{
    _initialized = false;
}
