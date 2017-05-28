#include <CppUTest/TestHarness.h>
#include <CppUTest/MemoryLeakDetectorNewMacros.h>
#include <CppUTest/MemoryLeakDetectorMallocMacros.h>
#include <CppUTestExt/MockSupport.h>


extern "C" {
    #include "analog_sensor.h"
    #include "m_adc.h"
}

TEST_GROUP(analog_sensor_init)
{
    void teardown()
    {
        analog_sensor_cleanup();
    }
};

TEST(analog_sensor_init, init_set_analog_width_to_12bit)
{
    //when
    analog_sensor_init();

    //then
    adc_bits_width_t width = m_analog_config_width_get();
    CHECK_EQUAL(ADC_WIDTH_12Bit, width);

    //cleanup
    analog_sensor_cleanup();
}


TEST(analog_sensor_init, try_alloc_analog_sensor_even_though_the_module_not_initialized)
{
    //when
    void *instance = analog_sensor_alloc(ADC1_CHANNEL_7, ANALOG_SENSOR_MAX_VOLTAGE_3_9);

    //then
    CHECK_EQUAL(NULL, instance);
}

TEST_GROUP(analog_sensor)
{
    void setup() {
        analog_sensor_init();
    }

    void teardown() {
        analog_sensor_cleanup();
    }
};

TEST(analog_sensor, call_value_with_null_instance)
{
    int ret = analog_sensor_value(NULL);

    CHECK_EQUAL(-1, ret);
}

TEST(analog_sensor, value)
{
    const int mock_value = 10;
    //given
    void *instance = analog_sensor_alloc(ADC1_CHANNEL_0, ANALOG_SENSOR_MAX_VOLTAGE_3_9);
    m_analog_senor_value_set(mock_value);

    //when
    int value = analog_sensor_value(instance);

    //then
    CHECK_EQUAL(mock_value, value);
}

TEST(analog_sensor, attenuation_level_set)
{
    //given

    //when
    void *instance = analog_sensor_alloc(ADC1_CHANNEL_7, ANALOG_SENSOR_MAX_VOLTAGE_1_1);

    //then
    adc_atten_t atten = m_analog_atten_get(ADC1_CHANNEL_7);
    CHECK_EQUAL(ADC_ATTEN_0db, atten);

    //cleanup
    analog_sensor_free(instance);
}

TEST(analog_sensor, alloc)
{
    //given
    adc1_channel_t adc_channel = ADC1_CHANNEL_7;

    //when
    void* instance = analog_sensor_alloc(adc_channel, ANALOG_SENSOR_MAX_VOLTAGE_1_5);

    //then
    CHECK(instance != NULL);

    //cleanup
    analog_sensor_free(instance);
}

TEST(analog_sensor, try_alloc_the_channel_already_allocated)
{
    //given
    adc1_channel_t adc_channel = ADC1_CHANNEL_7;
    void *instance0 = analog_sensor_alloc(adc_channel, ANALOG_SENSOR_MAX_VOLTAGE_2_2);

    //when
    void* instance = analog_sensor_alloc(adc_channel, ANALOG_SENSOR_MAX_VOLTAGE_2_2);

    //then
    CHECK_EQUAL(NULL, instance);

    //cleanup
    analog_sensor_free(instance0);
}

TEST(analog_sensor, max_1_1_vol_attenuation_level_set)
{
    //given
    adc1_channel_t adc_channel = ADC1_CHANNEL_7;

    //when
    void *instance0 = analog_sensor_alloc(adc_channel, ANALOG_SENSOR_MAX_VOLTAGE_1_1);

    //then
    adc_atten_t atten = m_analog_atten_get(adc_channel);
    CHECK_EQUAL(ADC_ATTEN_0db , atten)

    //cleanup
    analog_sensor_free(instance0);
}

TEST(analog_sensor, max_2_2_vol_attenuation_level_set)
{
    //given
    adc1_channel_t adc_channel = ADC1_CHANNEL_7;

    //when
    void *instance0 = analog_sensor_alloc(adc_channel, ANALOG_SENSOR_MAX_VOLTAGE_2_2);

    //then
    adc_atten_t atten = m_analog_atten_get(adc_channel);
    CHECK_EQUAL(ADC_ATTEN_6db , atten)

    //cleanup
    analog_sensor_free(instance0);
}

TEST(analog_sensor, free_with_null_expect_no_segment_fault)
{
    analog_sensor_free(NULL);
}
