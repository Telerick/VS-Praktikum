#include "StockMarket.h"
#include "stocksInitData.h"
#include <thread>
#include <chrono>

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


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

    // Creating socket file descriptor fd
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // socket(domain, type, protocol)
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = INADDR_ANY;  // ALLE VERFÜGBARE SCHNITTSTELLE

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

int sendMessage(string message) {
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
    cout << message << endl;

    close(sockfd);

    return 0;
}

int main() {
    srand(time(0)); //seed for random numbers

    StockMarket stockMarket1;
    stockMarket1.initData();
    stockMarket1.printStockMarket();

    while(true){
        string message = stockMarket1.generateTransaction();
        sendMessage(message);
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(5));
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
