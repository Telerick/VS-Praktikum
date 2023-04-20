#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h> // for getaddrinfo()
#include <unistd.h>

#include <chrono>

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#define BUF_SIZE 1024

int main(int argc, char** argv) {
    std::cout << "Client: START" << std::endl;
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


    char buffersend[BUF_SIZE];
    std::memset(buffersend, 0, BUF_SIZE);
    //std::chrono::time_point<std::chrono::system_clock> start_time = std::chrono::system_clock::now();
    auto start_time = std::chrono::high_resolution_clock::now();
   // std::sprintf(buffersend, "%ld", std::chrono::duration_cast<std::chrono::milliseconds>(start_time.time_since_epoch()).count());

    // Send "Hello" message to server
    const char* hello = "CLIENTMESSAGE: Hello it's me, MARIO";
        if (sendto(sockfd, buffersend, std::strlen(buffersend), 0, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Failed to send data\n";
        return 1;
    }
    std::cout << "Client: Sent message to server" << std::endl;

    struct sockaddr_in reply_address;
    socklen_t reply_address_length = sizeof(reply_address);
    std::memset(&reply_address, 0, reply_address_length);
    std::memset(buffersend, 0, BUF_SIZE);
    if (recvfrom(sockfd, buffersend, BUF_SIZE, 0, (struct sockaddr*)&reply_address, &reply_address_length) < 0) {
        std::cerr << "Failed to receive data\n";
        return 1;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    long rtt = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    std::cout << "RTT: " << rtt << " mikroseconds\n";

    close(sockfd);

    return 0;
}
