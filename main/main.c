#include <string.h>
#include <stdlib.h>

#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "mqtt.h"
#include "ntp.h"
#include "wwifi.h"

#define TAG "APP"

#if 1
#define SSID        "NO_RUN"
#define PASSWORD    "1qaz2wsx"
#else
#define SSID        "IO"
#define PASSWORD    "!bobbers7"
#endif

#define MQTT_SERVER "dmqtt.thingplus.net"
#define MQTT_USER   "001100110011"
#define MQTT_PASS   "h2lDgM_9UZkRVtHtTbCyoC1cLb8="
#define MQTT_PORT   8883

static void _ntp_start(enum wwifi_event_id id, struct wwifi_event_desc* d)
{
    ntp_init();
}

wwifi_event_hook(ntp_start, WWIFI_EVENT_STA_CONNTECTED, _ntp_start, NULL);

static void _cpu_clock_set(void)
{
#ifdef CPU_FREQ_160MHZ
    ESP_LOGI(TAG, "Setup CPU run as 160MHz\n");
    SET_PERI_REG_BITS(RTC_CLK_CONF, RTC_CNTL_SOC_CLK_SEL, 0x1, 
            RTC_CNTL_SOC_CLK_SEL_S);
    WRITE_PERI_REG(CPU_PER_CONF_REG, 0x01);
    ESP_LOGI(TAG, "Setup CPU run as 160MHz - Done\n");
#endif
}


void app_main(void)
{
    _cpu_clock_set();
    nvs_flash_init();

    wwifi_init(SSID, PASSWORD);

    mqtt_init();
    mqtt_start(MQTT_SERVER, MQTT_USER, MQTT_PASS, MQTT_PORT);
}
