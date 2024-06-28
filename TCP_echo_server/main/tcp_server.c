#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lwip/sockets.h"
#include "tcp_server.h"

#define PORT 6666
#define MAX_CLIENTS 5
#define BUFFER_SIZE 128
#define TASK_STACK_SIZE 4096
#define TASK_PRIORITY 3 // Server and client tasks are set to the same priority
#define PENDING_CONNECTION 1

static const char *TAG = "tcp_server";

// Structure to hold client information
typedef struct 
{
    int sock;
    int message_count;
} tcp_client_info_t;

// Array to hold client information
static tcp_client_info_t clients[MAX_CLIENTS];
static int client_count = 0;
static SemaphoreHandle_t client_mutex;

// Function to handle communication with a single client
static void handle_client(void *arg) 
{
    tcp_client_info_t *client = (tcp_client_info_t *)arg;
    char rx_buffer[BUFFER_SIZE];
    int len;

    while (1) 
    {
        len = recv(client->sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (len < 0) 
        {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        } else if (len == 0) 
        {
            ESP_LOGI(TAG, "Connection closed");
            break;
        } else 
        {
            rx_buffer[len] = '\0';

            if (strncmp(rx_buffer, "STATS", 5) == 0) 
            {
                char stats[BUFFER_SIZE];
                snprintf(stats, sizeof(stats), "Client count: %d, Message Count: %d\n", client_count, client->message_count);
                send(client->sock, stats, strlen(stats), 0);
            } else 
            {
                client->message_count++;
                send(client->sock, rx_buffer, len, 0);
            }
        }
    }

    // Close the client's socket
    close(client->sock);
    // Take semaphore
    xSemaphoreTake(client_mutex, portMAX_DELAY);
    //Remove one client
    client_count--;
    // Release semaphore
    xSemaphoreGive(client_mutex);
    // Delted the task
    vTaskDelete(NULL);
}

// Fnc to manage incoming clients
static void tcp_server_task(void *pvParameters) 
{
    int addr_family = AF_INET;
    int ip_protocol = IPPROTO_IP;

    while (1) 
    {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);

        // Socket for listening for a connections
        int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0) 
        {
            ESP_LOGE(TAG, "Not able to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket done");

        // Bind the socket
        int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG, "Not able to bind: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket bound is done at port %d", PORT);

        // Start listen for a connection
        err = listen(listen_sock, PENDING_CONNECTION);
        if (err != 0) {
            ESP_LOGE(TAG, "Failed during listen: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket start listen");

        while (1) 
        {
            struct sockaddr_in6 source_addr;
            socklen_t addr_len = sizeof(source_addr);
            // Accept a the sock
            int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);

            if (sock < 0) 
            {
                ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
                break;
            }

            ESP_LOGI(TAG, "Socket accepted ip address");

            xSemaphoreTake(client_mutex, portMAX_DELAY);
            
            if (client_count < MAX_CLIENTS) 
            {
                tcp_client_info_t *client = &clients[client_count++];
                client->sock = sock;
                client->message_count = 0;
                xTaskCreate(handle_client, "handle_client", TASK_STACK_SIZE, client, TASK_PRIORITY, NULL);
            } else 
            {
                ESP_LOGE(TAG, "Not able to process, too much client");
                close(sock);
            }

            xSemaphoreGive(client_mutex);
        }

        if (listen_sock != -1) 
        {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(listen_sock, 0);
            close(listen_sock);
        }
    }

    vTaskDelete(NULL);
}

// Start the TCP server
void start_tcp_server(void) 
{
    client_mutex = xSemaphoreCreateMutex();
    if (client_mutex == NULL) 
    {
        ESP_LOGE(TAG, "Failed to create client mutex");
        return;
    }
    
    xTaskCreate(tcp_server_task, "tcp_server_task", TASK_STACK_SIZE, NULL, TASK_PRIORITY, NULL);
}
