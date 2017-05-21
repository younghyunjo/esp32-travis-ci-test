#include <stdlib.h>
#include <string.h>

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "MQTTClient.h"
#include "analog_sensor.h"
#include "ds18b20.h"
#include "wwifi.h"

#define TAG "MQTT"

#define TEMERATURE_GPIO 0
#define LIGHT_ADC_PORT  ADC1_CHANNEL_4
#define SOUND_ADC_PORT  ADC1_CHANNEL_3

#define REPORT_INTERVAL_SEC 60

#define MQTT_WEBSOCKET 0  // 0=no 1=yes
#define MQTT_BUF_SIZE 512
#define MQTT_TIMEOUT_MS 2000

struct mqtt {
    char server[256];
    char user[128];
    char password[128];
    int port;
};
static struct mqtt *_m = NULL;

static unsigned char _tx_buffer[MQTT_BUF_SIZE] = {0,};
static unsigned char _rx_buffer[MQTT_BUF_SIZE] = {0,};

static void *light;
static void *sound;

static int _esp32_status_publish(struct mqtt *m, MQTTClient *c)
{
    char topic[100] = {0,};
    sprintf(topic, "v/a/g/%s/status", m->user);

    char *payload = "on,90";
    MQTTMessage message = {
        .qos = QOS1,
        .retained = false,
        .dup = false,
        .payload = (void*)payload,
        .payloadlen = strlen(payload),
    };

    return MQTTPublish(c, topic, &message);
}

static int _temperatuet_publish(struct mqtt *m, MQTTClient *c)
{
    char topic[100] = {0,};
    sprintf(topic, "v/a/g/%s/s/temperature-%s-0/status", m->user, m->user);

    char payload[256] = {0,};
    sprintf(payload, "%s", "on,90");

    MQTTMessage message = {
        .qos = QOS1,
        .retained = false,
        .dup = false,
        .payload = (void*)payload,
        .payloadlen = strlen(payload),
    };

    int ret = MQTTPublish(c, topic, &message);
    if (ret < 0) {
        ESP_LOGE(TAG, "MQTTPublish failed. topic:%s payload:%s",
                topic, payload);
        return ret;
    }

    memset(topic, 0, sizeof(topic));
    sprintf(topic, "v/a/g/%s/s/temperature-%s-0", m->user, m->user);

    time_t now = 0;
    time(&now);
    memset(payload, 0, sizeof(payload));
    sprintf(payload, "%ld000,%f", now, DS_get_temp());

    memset(&message, 0, sizeof(message));
    message.qos = QOS1;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)payload;
    message.payloadlen = strlen(payload);

    return MQTTPublish(c, topic, &message);
}

static int _light_publish(struct mqtt *m, MQTTClient *c)
{
    char topic[100] = {0,};
    sprintf(topic, "v/a/g/%s/s/light-%s-0/status", m->user, m->user);

    char payload[256] = {0,};
    sprintf(payload, "%s", "on,90");

    MQTTMessage message = {
        .qos = QOS1,
        .retained = false,
        .dup = false,
        .payload = (void*)payload,
        .payloadlen = strlen(payload),
    };

    int ret = MQTTPublish(c, topic, &message);
    if (ret < 0) {
        ESP_LOGE(TAG, "MQTTPublish failed. topic:%s payload:%s",
                topic, payload);
        return ret;
    }

    memset(topic, 0, sizeof(topic));
    sprintf(topic, "v/a/g/%s/s/light-%s-0", m->user, m->user);

    time_t now = 0;
    time(&now);
    memset(payload, 0, sizeof(payload));
    sprintf(payload, "%ld000,%d", now, analog_sensor_value(light));

    memset(&message, 0, sizeof(message));
    message.qos = QOS1;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)payload;
    message.payloadlen = strlen(payload);

    return MQTTPublish(c, topic, &message);
}

static int _mic_publish(struct mqtt *m, MQTTClient *c)
{
    char topic[100] = {0,};
    sprintf(topic, "v/a/g/%s/s/noise-%s-1/status", m->user, m->user);

    char payload[256] = {0,};
    sprintf(payload, "%s", "on,90");

    MQTTMessage message = {
        .qos = QOS1,
        .retained = false,
        .dup = false,
        .payload = (void*)payload,
        .payloadlen = strlen(payload),
    };

    int ret = MQTTPublish(c, topic, &message);
    if (ret < 0) {
        ESP_LOGE(TAG, "MQTTPublish failed. topic:%s payload:%s",
                topic, payload);
        return ret;
    }

    memset(topic, 0, sizeof(topic));
    sprintf(topic, "v/a/g/%s/s/noise-%s-1", m->user, m->user);

    time_t now = 0;
    time(&now);
    memset(payload, 0, sizeof(payload));
    sprintf(payload, "%ld000,%d", now, analog_sensor_value(sound));

    memset(&message, 0, sizeof(message));
    message.qos = QOS1;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)payload;
    message.payloadlen = strlen(payload);

    return MQTTPublish(c, topic, &message);
}

static void mqtt_message_handler(MessageData *md) 
{
}

void mqtt_task(void *pvParameters)
{
    MQTTClient client;
    struct mqtt* m = pvParameters;

    int ret;
    Network network;

    while(1) {
        ESP_LOGD(TAG,"Wait for WiFi ...");

        wwifi_connection_waiting();
        ESP_LOGI(TAG, "Connected to AP");

        ESP_LOGI(TAG, "Start MQTT Task ...");

        NetworkInit(&network);
        network.websocket = MQTT_WEBSOCKET;

        ESP_LOGD(TAG,"NetworkConnect %s:%d ...",m->server, m->port);
        ret = NetworkConnect(&network, m->server, m->port);
        if (ret != 0) {
            ESP_LOGI(TAG, "NetworkConnect not SUCCESS: %d", ret);
            goto exit;
        }
        ESP_LOGD(TAG,"MQTTClientInit  ...");
        MQTTClientInit(&client, &network, MQTT_TIMEOUT_MS, 
                _tx_buffer, MQTT_BUF_SIZE,
                _rx_buffer, MQTT_BUF_SIZE);

        char client_id[30];
        MQTTString clientId = MQTTString_initializer;
        sprintf(client_id, "%s", m->user);
        ESP_LOGI(TAG,"MQTTClientInit  %s",client_id);
        clientId.cstring = client_id;

        MQTTString username = MQTTString_initializer;
        username.cstring = m->user;

        MQTTString password = MQTTString_initializer;
        password.cstring = m->password;

        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.clientID          = clientId;
        data.willFlag          = 0;
        data.MQTTVersion       = 4; // 3 = 3.1 4 = 3.1.1
        data.keepAliveInterval = 60;
        data.cleansession      = 1;
        data.username          = username;
        data.password          = password;

        ESP_LOGI(TAG,"MQTTConnect  ...");
        ret = MQTTConnect(&client, &data);
        if (ret != SUCCESS) {
            ESP_LOGI(TAG, "MQTTConnect not SUCCESS: %d", ret);
            goto exit;
        }

        ESP_LOGI(TAG, "MQTTSubscribe  ...");
        ret = MQTTSubscribe(&client, "v/a/g/001100110011/req", QOS1, 
                mqtt_message_handler);
        if (ret != SUCCESS) {
            ESP_LOGI(TAG, "MQTTSubscribe: %d", ret);
            goto exit;
        }

        while(1) {
            esp_task_wdt_feed();

            _esp32_status_publish(m, &client);
            _light_publish(m, &client);
            _mic_publish(m, &client);
            _temperatuet_publish(m, &client);

            if (ret != SUCCESS) {
                ESP_LOGI(TAG, "MQTTPublish: %d", ret);
                goto exit;
            }

            vTaskDelay(2000 / portTICK_PERIOD_MS);

            ESP_LOGI(TAG, "MQTTYield  ...");
            //ret = MQTTYield(&client, (data.keepAliveInterval+1)*1000);
            ret = MQTTYield(&client, REPORT_INTERVAL_SEC * 1000);
            if (ret != SUCCESS) {
                ESP_LOGI(TAG, "MQTTYield: %d", ret);
                goto exit;
            }
            ESP_LOGI(TAG, "wake up\n");

        }
exit:
        MQTTDisconnect(&client);
        NetworkDisconnect(&network);
        ESP_LOGI(TAG, "Starting again!");
    }

    esp_task_wdt_delete();
    vTaskDelete(NULL);
}

void mqtt_start(char* server, char* user, char* password, int port)
{
    if (_m) {
        ESP_LOGW(TAG, "mqtt task already started");
        return;
    }

    _m = calloc(1, sizeof(struct mqtt));
    memcpy(_m->server, server, strlen(server));
    memcpy(_m->user, user, strlen(user));
    memcpy(_m->password, password, strlen(password));
    _m->port = port;

    xTaskCreate(&mqtt_task, "mqtt_task", 12288, _m, 5, NULL);
}

/*
 * Actually this code must be placed in other file.
 * I am sorry.
 */
void mqtt_init(void)
{
    analog_sensor_init();

    DS_init(TEMERATURE_GPIO);
    light = analog_sensor_alloc(LIGHT_ADC_PORT, ANALOG_SENSOR_MAX_VOLTAGE_3_9);
    sound = analog_sensor_alloc(SOUND_ADC_PORT, ANALOG_SENSOR_MAX_VOLTAGE_3_9);
}
