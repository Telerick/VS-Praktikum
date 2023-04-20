#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
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

    // Creating socket file descriptor fd
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // socket(domain, type, protocol)
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(8080);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // Wait for "Hello" message from client
    std::cout << "Server: Waiting for message" << std::endl;
    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *) buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';
    std::cout << "Server: " << buffer << std::endl;

    // Send "Hello" message to client
    const char *hello = "SERVERMESSAGE: Hello, Mario. It's me Bowser. I have Peach, come and get her";

    sendto(sockfd, hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
    std::cout << "Hello message sent to client" << std::endl;

    close(sockfd);

    return 0;

}