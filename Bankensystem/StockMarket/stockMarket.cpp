#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sstream>
#include <netdb.h>
#include "StockMarket.h"

#ifndef MSG_CONFIRM
#define MSG_CONFIRM 0x800
#endif

#define BUF_SIZE 1024

//
// Created by Till Langen on 14.04.23.
//

//
// global variables
//
std::map<std::string, std::vector<std::string> > map;

std::mutex mu;

//
// functions
//
void addStock(std::string stock){
    if(map.find(stock) != map.end()){
        return; //if stock is already a key of the map
    }
    std::vector<std::string> v1;
    map[stock] = v1;
}

void addSubscriber(std::string stock, std::string ip){
    if(map.find(stock) == map.end()){
        return; //if stock is not key of the map
    }

    for (int i = 0; i < map[stock].size(); ++i) {
        if(map[stock][i] == ip){
            return;
        }
    }

    map[stock].push_back(ip);
}

void removeSubscriber(std::string stock, std::string ip){
    if(map.find(stock) == map.end()){
        return; //if stock is not key of the map
    }

    for (int i = 0; i < map[stock].size(); ++i) {
        if(map[stock][i] == ip){
            map[stock].erase(map[stock].begin()+i);
            return;
        }
    }

}

std::vector<std::string> getSubscriber(std::string stock){
    if(map.find(stock) == map.end()){
        return std::vector<std::string>(); //if stock is not key of the map
    }
    return map[stock];
}

void printVector(std::vector<std::string> v){
    for (const auto & i : v) {
        std::cout << i << ", ";
    }
}

void printMap(){
    for (const auto& [key, value] : map){
        std::cout << '[' << key << "] = ";
        printVector(value);
        std::cout << ";" << std::endl;
    }
}

void fillMap(){
    addStock("LSFT");
    addStock("TC19");
    addStock("MNSW");
    addStock("MFC");
    addStock("GDAG");
    addStock("TB2");
    addStock("JOI");
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

    close(sockfd);

    return 0;
}

void startSubscribeServer(){
    int sockfd;
    struct sockaddr_in servaddr;

    // Creating socket file descriptor fd
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);    // socket(domain, type, protocol)
    if (sockfd < 0) {
        std::cerr << "Creating server socket failed" << std::endl;
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = INADDR_ANY;  // ALLE VERFÜGBARE SCHNITTSTELLE

    // Bind the socket with the server address
    if (::bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Binding socket with server address failed" << std::endl;
        return;
    }

    while(true){
        std::string message;
        message.resize(BUF_SIZE);
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));

        // Wait for message from client
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, &message[0], message.size(), MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        //buffer[n] = '\0';
        if (n>0){
            std::cout << "Received message: " << message << std::endl;

            // split the message into its parts
            std::istringstream iss(message);
            std::string acronym, ip;
            iss >> acronym >> ip;

            if(acronym == "stop"){
                return;
            }

            mu.lock();
            addSubscriber(acronym, inet_ntoa(cliaddr.sin_addr)); //could use cliaddr.sin_addr
            mu.unlock();

            std::cout << "Subscriber list changed:" << std::endl;

            printMap();
        }
    }

    close(sockfd);
}

void transactionThread(){
    StockMarket stockMarket1;
    stockMarket1.initData();
    stockMarket1.printStockMarket();

    while(true){
        std::string message = stockMarket1.generateTransaction();

        // split the message into its parts
        std::istringstream iss(message);
        std::string acronym;
        unsigned int price, amount;
        iss >> acronym >> price >> amount;

        std::cout << "New generated transaction: " << message << std::endl;

        mu.lock();
        std::vector<std::string> addresses = getSubscriber(acronym);

        for (int i = 0; i < addresses.size(); ++i) {
            sendMessage(addresses[i]);
            std::cout << "Send message to: " << addresses[i] << std::endl;
        }
        mu.unlock();

        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(2));
    }
}

//
// olf functions (may delete later)
//
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

int sendMessageOld(std::string message) {
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

int sendMessageOld2(std::string message) {
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
    fillMap();

    std::thread t(transactionThread);
    std::thread t2(startSubscribeServer);

    t.join();
    t2.join();

    return 0;
}
