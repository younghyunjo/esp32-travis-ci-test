       #include <string.h>
#include "bt.h"
#include "bt_trace.h"
#include "bt_types.h"
#include "btm_api.h"
#include "bta_api.h"
#include "bta_gatt_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"

#define GATTC_TAG "GATTC_DEMO"

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0

/* nordic */
esp_bd_addr_t target_addr = {0xee, 0x2f, 0x5f, 0x19, 0xd7, 0x72};
/*swithcer */
//esp_bd_addr_t target_addr = {0xee, 0xf6, 0x45, 0x70, 0x73, 0x4b};
/*swithcer HOME*/
//esp_bd_addr_t target_addr = {0xc8, 0xa3, 0x11, 0x9c, 0xd0, 0x82};

void (*unregistered)(void);
static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_PASSIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30
};

static esp_gatt_srvc_id_t _switcher_switch_service_id = {
    .id = {
        .uuid = {
            .len = ESP_UUID_LEN_16,
            .uuid = {.uuid16 = 0x150b,},
        },
        .inst_id = 0,
    },
    .is_primary = true,
};

static esp_gatt_id_t _swticher_switch_service_onoff_char_id = {
    .uuid = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = 0x15ba,},
    },
    .inst_id = 0,
};

static esp_gatt_if_t gif;
static uint16_t _conn_id = 0;
static uint8_t _cmd;

void switcher_move(uint8_t cmd)
{
    esp_ble_gattc_write_char(
            gif,
            _conn_id,
            &_switcher_switch_service_id,
            &_swticher_switch_service_onoff_char_id,
            sizeof(cmd),
            (uint8_t*)&cmd,
            ESP_GATT_WRITE_TYPE_RSP,
            ESP_GATT_AUTH_REQ_NONE);
}

static void _gattc_app_cb_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    uint16_t conn_id = 0;
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

    switch (event) {
        case ESP_GATTC_REG_EVT:
            ESP_LOGI(GATTC_TAG, "REG_EVT\n");
            esp_ble_gap_set_scan_params(&ble_scan_params);
            break;
        case ESP_GATTC_OPEN_EVT:
            conn_id = p_data->open.conn_id;
            _conn_id = p_data->open.conn_id;

            //memcpy(gl_profile_tab[PROFILE_A_APP_ID].remote_bda, p_data->open.remote_bda, sizeof(esp_bd_addr_t));
            ESP_LOGI(GATTC_TAG, "ESP_GATTC_OPEN_EVT conn_id %d, if %d, status %d, mtu %d\n", conn_id, gattc_if, p_data->open.status, p_data->open.mtu);
            if (p_data->open.status != 0) {

                ESP_LOGI(GATTC_TAG,"0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:\n", 
                        p_data->open.remote_bda[0], 
                        p_data->open.remote_bda[1], 
                        p_data->open.remote_bda[2], 
                        p_data->open.remote_bda[3], 
                        p_data->open.remote_bda[4], 
                        p_data->open.remote_bda[5]);
                //esp_ble_gattc_open(gif, p_data->open.remote_bda, true);
                esp_ble_gattc_open(gif, target_addr, true);
                break;
            }

#if 0
            ESP_LOGI(GATTC_TAG, "REMOTE BDA  %02x:%02x:%02x:%02x:%02x:%02x\n",
                    gl_profile_tab[PROFILE_A_APP_ID].remote_bda[0], gl_profile_tab[PROFILE_A_APP_ID].remote_bda[1], 
                    gl_profile_tab[PROFILE_A_APP_ID].remote_bda[2], gl_profile_tab[PROFILE_A_APP_ID].remote_bda[3],
                    gl_profile_tab[PROFILE_A_APP_ID].remote_bda[4], gl_profile_tab[PROFILE_A_APP_ID].remote_bda[5]
                    );
#endif
            esp_ble_gattc_search_service(gattc_if, conn_id, NULL);
            break;
        case ESP_GATTC_CLOSE_EVT:
            ESP_LOGI(GATTC_TAG, "closed status:%d conn_id:%d reason:%d\n", 
                    param->close.status,
                    param->close.conn_id,
                    param->close.reason);
            esp_ble_gattc_app_unregister(gattc_if);
            unregistered();

            break;
        case ESP_GATTC_SEARCH_RES_EVT: {
                                           esp_gatt_srvc_id_t *srvc_id = &p_data->search_res.srvc_id;
                                           conn_id = p_data->search_res.conn_id;
                                           ESP_LOGI(GATTC_TAG, "SEARCH RES: conn_id = %x\n", conn_id);
                                           if (srvc_id->id.uuid.len == ESP_UUID_LEN_16) {
                                               ESP_LOGI(GATTC_TAG, "UUID16: %x\n", srvc_id->id.uuid.uuid.uuid16);
                                           } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_32) {
                                               ESP_LOGI(GATTC_TAG, "UUID32: %x\n", srvc_id->id.uuid.uuid.uuid32);
                                           } else if (srvc_id->id.uuid.len == ESP_UUID_LEN_128) {
                                               ESP_LOGI(GATTC_TAG, "UUID128: %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n", srvc_id->id.uuid.uuid.uuid128[0],
                                                       srvc_id->id.uuid.uuid.uuid128[1], srvc_id->id.uuid.uuid.uuid128[2], srvc_id->id.uuid.uuid.uuid128[3],
                                                       srvc_id->id.uuid.uuid.uuid128[4], srvc_id->id.uuid.uuid.uuid128[5], srvc_id->id.uuid.uuid.uuid128[6],
                                                       srvc_id->id.uuid.uuid.uuid128[7], srvc_id->id.uuid.uuid.uuid128[8], srvc_id->id.uuid.uuid.uuid128[9],
                                                       srvc_id->id.uuid.uuid.uuid128[10], srvc_id->id.uuid.uuid.uuid128[11], srvc_id->id.uuid.uuid.uuid128[12],
                                                       srvc_id->id.uuid.uuid.uuid128[13], srvc_id->id.uuid.uuid.uuid128[14], srvc_id->id.uuid.uuid.uuid128[15]);
                                           } else {
                                               ESP_LOGE(GATTC_TAG, "UNKNOWN LEN %d\n", srvc_id->id.uuid.len);
                                           }
                                           break;
                                       }
        case ESP_GATTC_SEARCH_CMPL_EVT:
                                       conn_id = p_data->search_cmpl.conn_id;
                                       ESP_LOGI(GATTC_TAG, "SEARCH_CMPL: conn_id = %x, status %d\n", conn_id, p_data->search_cmpl.status);
                                       //esp_ble_gattc_get_characteristic(gattc_if, conn_id, &_switcher_switch_service_id, NULL);
                                       switcher_move(_cmd);
                                       break;
        case ESP_GATTC_GET_CHAR_EVT:
                                       if (p_data->get_char.status != ESP_GATT_OK) {
                                           break;
                                       }
                                       ESP_LOGI(GATTC_TAG, "GET CHAR: conn_id = %x, status %d\n", p_data->get_char.conn_id, p_data->get_char.status);
                                       ESP_LOGI(GATTC_TAG, "GET CHAR: srvc_id = %04x, char_id = %04x\n", p_data->get_char.srvc_id.id.uuid.uuid.uuid16, p_data->get_char.char_id.uuid.uuid.uuid16);

                                       esp_ble_gattc_get_characteristic(gattc_if, conn_id, &_switcher_switch_service_id, &p_data->get_char.char_id);
                                       break;
        case ESP_GATTC_REG_FOR_NOTIFY_EVT:
                                       break;
        case ESP_GATTC_NOTIFY_EVT:
                                       ESP_LOGI(GATTC_TAG, "NOTIFY: len %d, value %08x\n", p_data->notify.value_len, *(uint32_t *)p_data->notify.value);
                                       break;
        case ESP_GATTC_WRITE_CHAR_EVT:
                                       ESP_LOGI(GATTC_TAG, "WRITE: status %d\n", p_data->write.status);
                                       esp_ble_gattc_close(gattc_if, param->write.conn_id);
                                       break;
        default:
                                       break;
    }
}

static void esp_gattc_cb(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param)
{
    ESP_LOGI(GATTC_TAG, "EVT %d, gattc if %d\n", event, gattc_if);

    /* If event is register event, store the gattc_if for each profile */
    if (event == ESP_GATTC_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            gif = gattc_if;
        } else {
            ESP_LOGI(GATTC_TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id, 
                    param->reg.status);
            return;
        }
    }

    /* If the gattc_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gattc_if == gif) {
                _gattc_app_cb_handler(event, gattc_if, param);
            }
        }
    } while (0);
}

static void _ble_gap_cb(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    ESP_LOGI(GATTC_TAG, "GAP EVT %d\n", event);
    switch (event) {
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: 
        {
            uint32_t duration = 30;
            esp_ble_gap_start_scanning(duration);
        }
        break;
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "Scan start failed\n");
        }
        break;
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: 
        {
            int ret = esp_ble_gattc_open(gif, target_addr, true);
            ESP_LOGI(GATTC_TAG, "open result:%d\n", ret);
        }

        break;
    case ESP_GAP_BLE_SCAN_RESULT_EVT:
        {
            //ESP_LOGI(GATTC_TAG, "scan_rst.search_evt:%d\n", param->scan_rst.search_evt);
            switch(param->scan_rst.search_evt) 
            {
                case ESP_GAP_SEARCH_INQ_RES_EVT:
                    {
                        uint8_t *adv_name = NULL;
                        uint8_t adv_name_len = 0;
                        esp_ble_gap_cb_param_t *scan_result = (esp_ble_gap_cb_param_t *)param;
                        adv_name = esp_ble_resolve_adv_data(scan_result->scan_rst.ble_adv,
                                ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
#if 0
                        ESP_LOGI(GATTC_TAG,"0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:\n", 
                                scan_result->scan_rst.bda[0], 
                                scan_result->scan_rst.bda[1], 
                                scan_result->scan_rst.bda[2], 
                                scan_result->scan_rst.bda[3], 
                                scan_result->scan_rst.bda[4], 
                                scan_result->scan_rst.bda[5]);
                        //ESP_LOGI(GATTC_TAG, "dev_type:%x\n", scan_result->scan_rst.dev_type);
                        //ESP_LOGI(GATTC_TAG, "ble_addr_type:%x\n", scan_result->scan_rst.ble_addr_type);
                        //ESP_LOGI(GATTC_TAG, "ble_evt_type:%x\n", scan_result->scan_rst.ble_evt_type);
#endif
#if 0
                        for (int j = 0; j < adv_name_len; j++) {
                            ESP_LOGI(GATTC_TAG, "%c", adv_name[j]);
                        }
#endif
                        if (adv_name != NULL && 
                                memcmp(scan_result->scan_rst.bda, target_addr, sizeof(target_addr)) == 0) {
                            ESP_LOGI(GATTC_TAG, "CONNECT TEST SWITCHER\n");
                            esp_ble_gap_stop_scanning();
                            //esp_ble_gattc_open(gif, target_addr, true);
                        }
                        break;
                    }
                default:
                    break;
            }
        }
        break;
    default:
        break;
    }
}

void ble_pairing(uint8_t cmd)
{
    _cmd = cmd;
    esp_ble_gattc_app_register(PROFILE_A_APP_ID);
}

esp_err_t ble_task(void *pvParameters)
{
    unregistered = pvParameters;

    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t ret;

    ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "esp_bt_controller_init failed\n");
        return ret;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BTDM);
    if (ret != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "esp_bt_controller_enable failed. %x\n", ret);
        return ret;
    }

    ret = esp_bluedroid_init();
    if (ret != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "esp_bluedroid_init failed\n");
        return ret;
    }

    ret = esp_bluedroid_enable();
    if (ret != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "esp_bluedroid_enable failed\n");
        return ret;
    }

    esp_err_t status;
    if ((status = esp_ble_gap_register_callback(_ble_gap_cb)) != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "gap register error, error code = %x\n", status);
        return -1;
    }

    if ((status = esp_ble_gattc_register_callback(esp_gattc_cb)) != ESP_OK) {
        ESP_LOGE(GATTC_TAG, "gattc register error, error code = %x\n", status);
        return -1;
    }
    return 0;
}
