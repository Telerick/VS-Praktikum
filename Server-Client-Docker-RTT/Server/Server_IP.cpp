#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#define BUF_SIZE 1024

int main(int argc, char** argv) {
    int sockfd, n;
    socklen_t len;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Get server hostname
    const char* hostname = "server";
    struct hostent* server = gethostbyname(hostname);
    if (server == NULL) {
        std::cerr << "Error: could not resolve hostname" << std::endl;
        return -1;
    }

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr = *((struct in_addr*) server->h_addr);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Server Bind failed" << std::endl;
        return -1;
    }

    // Wait for "Hello" message from client
    std::cout << "Server: Waiting for message" << std::endl;
    char bufecho[BUF_SIZE];
    std::memset(buffer, 0, BUF_SIZE);
    struct sockaddr_in client_address;
    socklen_t client_address_length = sizeof(client_address);
    std::memset(&client_address, 0, client_address_length);
    if (recvfrom(sockfd, bufecho, BUF_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length) < 0) {
        std::cerr << "Failed to receive data\n";
        return 1;
    }

    // Send "Hello" message to client

    std::cout << "Received message: " << bufecho << '\n';

    if (sendto(sockfd, bufecho, std::strlen(bufecho), 0, (struct sockaddr*)&client_address, client_address_length) < 0) {
        std::cerr << "Failed to send data\n";
        return 1;
    }

    close(sockfd);

    return 0;
}
