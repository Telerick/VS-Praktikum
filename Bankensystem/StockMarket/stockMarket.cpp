#include "StockMarket.h"
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>



#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#define BUF_SIZE 1024

//
// Created by Till Langen on 14.04.23.
//

//Starts the Server to
int startServer() {
    int sockfd, n;
    socklen_t len;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr, cliaddr;
    struct addrinfo hints, *res;

    // Creating socket file descriptor fd
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // socket(domain, type, protocol)
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // Resolve hostname to IP address
    int status = getaddrinfo("bankErik", "8080", &hints, &res);
    if (status != 0) {
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memcpy(&servaddr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    // // Filling server information
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_port = htons(8080);
    // servaddr.sin_addr.s_addr = INADDR_ANY;  // ALLE VERFÜGBARE SCHNITTSTELLE

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return -1;
    }

    // Wait for "Hello" message from client
    len = sizeof(cliaddr);
    n = recvfrom(sockfd, (char *) buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
    buffer[n] = '\0';
    std::cout << "Server: " << buffer << std::endl;

    // Send "Hello" message to client
    const char *hello = "SERVERMESSAGE: Hello, Mario. It's me Bowser. I have Peach, come and get her";

    sendto(sockfd, hello, strlen(hello), MSG_CONFIRM, (const struct sockaddr *) &cliaddr, len);
    std::cout << "Server: Hello message sent to client" << std::endl;

    close(sockfd);

    return 0;
}

int sendMessage(std::string message) {
    int sockfd, n;
    socklen_t len;
    char buffer[BUF_SIZE];
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = INADDR_ANY; // listen to all network adresses

    // Send message to server
    sendto(sockfd, message.c_str(), message.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
    std::cout << message << std::endl;

    close(sockfd);

    return 0;
}

int sendMessage2(std::string message) {
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
    int status = getaddrinfo("bankErik", "8080", &hints, &res);
    if (status != 0) {
        std::cerr << "Error resolving hostname: " << gai_strerror(status) << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memcpy(&servaddr, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);

    sendto(sockfd, message.c_str(), message.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));


    // Wait for "Hello" message from server
    len = sizeof(servaddr);
    n = recvfrom(sockfd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
    buffer[n] = '\0';
    std::cout << "Client : " << buffer << std::endl;

    close(sockfd);

    return 0;
}


int main() {
    srand(time(0)); //seed for random numbers

    StockMarket stockMarket1;
    stockMarket1.initData();
    stockMarket1.printStockMarket();

    while(true){
        std::cout << "Preparing message" << std::endl;
        std::string message = stockMarket1.generateTransaction();
        sendMessage2(message);
        std::cout << "Stockmarket before sleep" << std::endl;
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
        std::cout << "Stockmarket next loop" << std::endl;

    }

/*
    std::thread first (startServer);     // spawn new thread that calls foo()
    std::thread second (startClient);

    // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes
*/

    return 0;
}
