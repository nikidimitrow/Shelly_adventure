#include "unity.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

// Define constants for the tests
#define SERVER_IP "127.0.0.1" // there's no place like home
#define SERVER_PORT 666
#define TEST_MESSAGE "Hey, Shelly!"
#define BUFFER_SIZE 128

// Variables to hold the socket descriptor for setup and teardown
static int test_sock;

void setUp(void) 
{
    // Create a socket for the test
    test_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, test_sock, "Socket creation failed");

    // Configure the server address
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(SERVER_PORT);

    // Connect to the server
    int err = connect(test_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    TEST_ASSERT_EQUAL_MESSAGE(0, err, "Socket connection failed");
}

void tearDown(void) 
{
    // Close the socket after the test
    close(test_sock);
}

void test_tcp_echo_server(void) 
{
    // Send a test message
    int len = send(test_sock, TEST_MESSAGE, strlen(TEST_MESSAGE), 0);
    TEST_ASSERT_EQUAL_MESSAGE(strlen(TEST_MESSAGE), len, "Send message failed");

    // Receive the echoed message
    char rx_buffer[BUFFER_SIZE];
    len = recv(test_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, len, "Receive failed");

    // Null-terminate the received data
    rx_buffer[len] = '\0';

    // Check if the echoed message is the same as the sent message
    TEST_ASSERT_EQUAL_STRING(TEST_MESSAGE, rx_buffer);
}

int main(void) 
{
    UNITY_BEGIN();
    RUN_TEST(test_tcp_echo_server);
    return UNITY_END();
}
