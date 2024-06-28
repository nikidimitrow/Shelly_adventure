#include "wifi_init.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"

// Wi-Fi credentials and authentication mode
#define WIFI_SSID "nikidimitrow"
#define WIFI_PASS "NobodyWouldUseThis" // A joke
#define WIFI_AUTH_MODE WIFI_AUTH_WPA2_PSK
/* https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/wifi.html */

static const char *TAG = "wifi_init";

// Function to initialize Wi-Fi
esp_err_t wifi_init(void) 
{
    
    // Init the network interface
    ESP_ERROR_CHECK(esp_netif_init());
    // Default event loop
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    //  Wi-Fi station interface
    esp_netif_create_default_wifi_sta();
    
    //Configure Wi-Fi with default
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();   
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Wi-Fi mode to station (client)
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    
    // Set Wi-Fi configuration
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

    //Start the server
    ESP_ERROR_CHECK(esp_wifi_start());
    
    // Try to connect to the Wi-Fi network
    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "Wi-Fi failed to connect");
        return ret;
    }

    //Did we manage to finished fine?
    ESP_LOGI(TAG, "Wi-Fi is done %s", WIFI_SSID);
    return ESP_OK;
}
