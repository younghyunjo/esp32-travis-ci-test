#ifndef _M_ADC_H_
#define _M_ADC_H_

#include "driver/adc.h"

adc_bits_width_t m_analog_config_width_get(void);
adc_atten_t m_analog_atten_get(adc1_channel_t channel);
void m_analog_senor_value_set(int value);


#endif //#ifndef _M_ADC_H_
