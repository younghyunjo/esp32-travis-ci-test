
#include "driver/adc.h"
#include "esp_err.h"

static adc_bits_width_t _adc_bit_width;
static adc_atten_t _adc_atten[ADC1_CHANNEL_MAX];
static int _adc_value;


esp_err_t adc1_config_width(adc_bits_width_t width_bit)
{
    _adc_bit_width = width_bit;
    return ESP_OK;
}

adc_bits_width_t m_analog_config_width_get(void)
{
    return _adc_bit_width;
}

esp_err_t adc1_config_channel_atten(adc1_channel_t channel, adc_atten_t atten)
{
    _adc_atten[channel] = atten;
}

adc_atten_t m_analog_atten_get(adc1_channel_t channel)
{
    return _adc_atten[channel];
}

int adc1_get_voltage(adc1_channel_t channel)
{
    return _adc_value;
}

void m_analog_senor_value_set(int value)
{
    _adc_value = value;
}
