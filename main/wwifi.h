#ifndef _WWIFI_H_
#define _WWIFI_H_

#include "esp_err.h"

enum wwifi_event_id {
    WWIFI_EVENT_NONE = -1,
    WWIFI_EVENT_STA_CONNTECTED,
    WWIFI_EVENT_DISCONNTECTED,
};

struct wwifi_event_desc {
};

struct wwifi_event {
    enum wwifi_event_id id;
    void (*cb)(enum wwifi_event_id, struct wwifi_event_desc*);
    void* private_data;
};

#define wwifi_event_hook(name, wwifi_event_id, callback, user_data) \
    static struct wwifi_event name __used __section("wwifi_event_hook_list") = {\
        .id = wwifi_event_id,   \
        .cb = callback, \
        .private_data = user_data   \
    };

void wwifi_connection_waiting(void);
esp_err_t wwifi_init(char* ssid, char* password);

#endif //#ifndef _WWIFI_H_
