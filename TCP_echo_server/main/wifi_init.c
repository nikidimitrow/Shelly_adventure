#include "wifi_init.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

#define WIFI_SSID "nikidimitrow"
#define WIFI_PASS "NobodyWouldUseThis"
#define WIFI_AUTH_MODE WIFI_AUTH_WPA2_PSK
/* https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html */

static const char *TAG = "wifi_init";

esp_err_t wifi_init(void) 
{
    
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    
    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_MODE,
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Wi-Fi failed to connect");
        return ret;
    }

    ESP_LOGI(TAG, "Wi-Fi is done %s", WIFI_SSID);
    return ESP_OK;
}
