#include <iostream>
#include <winsock2.h>     // For Winsock functions
#include <ws2tcpip.h>     // For advanced TCP/IP functions
#include <string>         // For using std::string
#pragma comment(lib, "ws2_32.lib")  // Link Winsock library

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    SOCKET client_socket;
    sockaddr_in server_addr;
    const int port = 1234;
    int sequence_number = 0;  // Initialize the sequence number

    // Create UDP socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Convert IP address (using inet_addr)

    std::string message;
    while (true) {
        // Clear the message before getting new input
        message.clear();

        // Get user input
        std::cout << "Enter message: ";
        std::getline(std::cin, message);  // Using std::getline to handle input properly

        // Send message to the server
        int server_addr_size = sizeof(server_addr);
        sendto(client_socket, message.c_str(), message.size(), 0, (sockaddr*)&server_addr, server_addr_size);

        // Print socket ID and sequence number
        std::cout << "Socket ID: " << client_socket << ", Sequence Number: " << sequence_number << std::endl;

        // If the message is "EOF", wait for the server to send "GOODBYE" and exit
        if (message == "EOF") {
            char buffer[1024];
            int received_bytes = recvfrom(client_socket, buffer, sizeof(buffer), 0, nullptr, nullptr);
            if (received_bytes == SOCKET_ERROR) {
                std::cerr << "Error receiving message" << std::endl;
                break;
            }
            buffer[received_bytes] = '\0';  // Null-terminate the received message
            std::cout << "Server: " << buffer << std::endl;
            break;
        }

        // Increment the sequence number for each message
        sequence_number++;
    }

    // Close the client socket
    closesocket(client_socket);  // Use closesocket() instead of close() in Winsock
    WSACleanup();  // Cleanup Winsock
    return 0;
}
