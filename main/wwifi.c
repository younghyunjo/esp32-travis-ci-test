#include <string.h>

#include "esp_event_loop.h"
#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "wwifi.h"

#define TAG "WWIFI"

#define CONNECTED_BIT BIT0

/* defined by linker script($IDF_PATH/components/esp32/ld/esp32.common.ld) */
extern struct wwifi_event __start_wwifi_event_hook_list[];
extern int __wwifi_event_hook_list_size[];

EventGroupHandle_t _wifi_event_group;

static void _wwifi_event_handler(enum wwifi_event_id id, system_event_t *event)
{
    ESP_LOGD(TAG, "start:%p\n", __start_wwifi_event_hook_list);
    ESP_LOGD(TAG, "id:%d\n", id);
    ESP_LOGD(TAG, "size:%p\n", __wwifi_event_hook_list_size);

    struct wwifi_event *e = __start_wwifi_event_hook_list;
    const int nr_hook = ((int)__wwifi_event_hook_list_size) / sizeof(struct wwifi_event);
    for (int i=0; i<nr_hook ;i++, e++) {
        if (e->id == id && e->cb) {
            e->cb(e->id, NULL);
        }
    }
}

static esp_err_t _event_handler(void *ctx, system_event_t *event)
{
    ESP_LOGD(TAG, "event_id:%d,0x%x\n", event->event_id, event->event_id);

    enum wwifi_event_id wwifi_event_id = WWIFI_EVENT_NONE;
    switch(event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGD(TAG, "STARTED");
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "CONNECTED");
            xEventGroupSetBits(_wifi_event_group, CONNECTED_BIT);
            wwifi_event_id = WWIFI_EVENT_STA_CONNTECTED;
#if 0
            uint32_t ip0 = 0, ip1 = 0, ip2 = 0, ip3 = 0;
            ip3 = (event->event_info.got_ip.ip_info.ip.addr >> 24) & 0xff;
            ip2 = (event->event_info.got_ip.ip_info.ip.addr >> 16) & 0xff;
            ip1 = (event->event_info.got_ip.ip_info.ip.addr >> 8) & 0xff;
            ip0 = (event->event_info.got_ip.ip_info.ip.addr) & 0xff;
            ESP_LOGD(TAG, "%d.%d.%d.%d", ip0, ip1, ip2, ip3);
#endif
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            ESP_LOGE(TAG, "DISCONNECTED");
            xEventGroupClearBits(_wifi_event_group, CONNECTED_BIT);
            wwifi_event_id = WWIFI_EVENT_DISCONNTECTED;

            ESP_LOGE(TAG, "TRY RECONNECT");
            esp_wifi_connect();
            break;
        default:
            break;
    }

    if (wwifi_event_id != WWIFI_EVENT_NONE) {
        _wwifi_event_handler(wwifi_event_id, event);
    }

    return ESP_OK;
}

void wwifi_connection_waiting(void)
{
    xEventGroupWaitBits(_wifi_event_group, CONNECTED_BIT,
            false, true, portMAX_DELAY);
}

esp_err_t wwifi_init(char* ssid, char* password)
{
    esp_err_t err;
    wifi_config_t wifi_config;

    if (strlen(ssid) > sizeof(wifi_config.sta.ssid)) {
        ESP_LOGE(TAG, "ssid is too long(%zd). maximum len is %zd", 
                strlen(ssid), sizeof(wifi_config.sta.ssid));
        err = ESP_FAIL;
        return err;
    }

    if (strlen(password) > sizeof(wifi_config.sta.password)) {
        ESP_LOGE(TAG, "password is too long(%zd). maximum len is %zd", 
             strlen(password), sizeof(wifi_config.sta.password));
        err = ESP_FAIL;
        return err;
    }

    tcpip_adapter_init();
    _wifi_event_group = xEventGroupCreate();

    err = esp_event_loop_init(_event_handler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_event_loop_init faiiled(%d, 0x%x)", err, err);
        return err;
    }

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed(%d, 0x%x)", err, err);
        return err;
    }

    err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_storage failed(%d, 0x%x)", err, err);
        return err;
    }

    err = esp_wifi_set_mode(WIFI_MODE_STA);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_mode failed(%d, 0x%x)", err, err);
        return err;
    }

    memset(&wifi_config, 0, sizeof(wifi_config));
    memcpy(wifi_config.sta.ssid, ssid, strlen(ssid));
    memcpy(wifi_config.sta.password, password, strlen(password));
    err = esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_set_config failed(%d, 0x%x)", err, err);
        return err;
    }

    ESP_LOGI(TAG, "ssid:%s", wifi_config.sta.ssid);

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed(%d, 0x%x)", err, err);
        return err;
    }

    return err;
}
