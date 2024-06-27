idf_component_register(SRCS "tcp_echo_server.c" "test_tcp_echo_server.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_event esp_netif freertos lwip unity esp_wifi) 

# Enable Unity testing framework
if(EXISTS "$ENV{IDF_PATH}/components/test_framework/unity")
    list(APPEND COMPONENT_REQUIRES unity)
endif()


