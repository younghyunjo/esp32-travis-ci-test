#include <stdbool.h>

#include "apps/sntp/sntp.h"
#include "esp_log.h"

#define TAG "NTP"

static bool _ntp_initialized = false;

void ntp_init(void)
{
    if (_ntp_initialized)
        return;

    ESP_LOGI(TAG, "Initializing NTP");

    sntp_setoperatingmode(SNTP_OPMODE_POLL);

    sntp_setservername(0, "pool.ntp.org");
#if SNTP_MAX_SERVERS > 2
    sntp_setservername(1, "time.google.com");
    sntp_setservername(2, "time.windows.com");
#endif

    sntp_init();
    _ntp_initialized = true;
}

