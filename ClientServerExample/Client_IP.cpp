#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h> // for getaddrinfo()
#include <unistd.h>

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#define BUF_SIZE 1024

int main(int argc, char** argv) {
    int sockfd, n;
    socklen_t len;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;
    struct addrinfo hints, *res;

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // Resolve hostname to IP address
    int status = getaddrinfo("server", "8080", &hints, &res);
    if (status != 0) {
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memcpy(&servaddr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    // Send "Hello" message to server
    const char* hello = "CLIENTMESSAGE: Hello it's me, MARIO";
    sendto(sockfd, hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    std::cout << "Client: Hello message sent to server" << std::endl;

    // Wait for "Hello" message from server
    len = sizeof(servaddr);
    n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    std::cout << "Client : " << buffer << std::endl;

    close(sockfd);

    return 0;
}
