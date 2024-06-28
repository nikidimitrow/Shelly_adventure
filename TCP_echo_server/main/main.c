#include <string.h>
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcp_server.h"
#include "wifi_init.h"

static const char *TAG = "main";

void app_main(void) {
    // Initialize NVS (non-volatile storage)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "NVS Flash init");

    // Initialize Wi-Fi
    ret = wifi_init();
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Wi-Fi init don");
        start_tcp_server();  // Start the TCP server
    } else {
        ESP_LOGE(TAG, "Wi-Fi init failed");
    }
}
