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
#include "../Data/initData.h"

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
std::map <std::string, std::vector<std::string>> map;
std::map <std::string, std::string> ipBankMap;

std::mutex mu;

bool rttActive = false;
auto start_time = std::chrono::high_resolution_clock::now();
auto end_time = std::chrono::high_resolution_clock::now();
struct sockaddr_in rttaddr;

long averageRtt;
int averageRttCnt = 0;

//
// functions
//
void addStock(std::string stock) {
    if (map.find(stock) != map.end()) {
        return; //if stock is already a key of the map
    }
    std::vector <std::string> v1;
    map[stock] = v1;
}

void mapBankToIP(std::string ip, std::string bankName){
    ipBankMap[ip] = bankName;
    std::cout << "ADD to IP-Bank Map: " << "{" << ip << "}" << "{" << bankName << "}"  << "PROOF" << ipBankMap[ip] << std::endl;
}

void addSubscriber(std::string stock, std::string ip) {
    if (map.find(stock) == map.end()) {
        return; //if stock is not key of the map
    }

    for (int i = 0; i < map[stock].size(); ++i) {
        if (map[stock][i] == ip) {
            return;
        }
    }

    map[stock].push_back(ip);
}

void removeSubscriber(std::string stock, std::string ip) {
    if (map.find(stock) == map.end()) {
        return; //if stock is not key of the map
    }

    for (int i = 0; i < map[stock].size(); ++i) {
        if (map[stock][i] == ip) {
            map[stock].erase(map[stock].begin() + i);
            return;
        }
    }

}

std::vector <std::string> getSubscriber(std::string stock) {
    if (map.find(stock) == map.end()) {
        return std::vector<std::string>(); //if stock is not key of the map
    }
    return map[stock];
}

void printVector(std::vector <std::string> v, std::map <std::string, std::string> bankName) {
    for (const auto &ip: v) {
        std::cout << ip;
        std::cout << "{" << bankName[ip] << "},";
    }
}

void printMap() {
    for (const auto &[key, value]: map) {
        std::cout << '[' << key << "] = ";
        printVector(value, ipBankMap);
        std::cout << ";" << std::endl;
    }
}

void fillMap() {
    for (int i = 0; i < stocksInit.size(); ++i) {
        addStock(stocksInit[i]->getAcronym());
    }
}

int sendMessage(std::string message, std::string ip, bool needAck) {
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
    servaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(needAck) { //if this is the message for the rtt
        rttActive = true; //activate the rtt function
        rttaddr = servaddr; //save the ip addr for later comparison
        start_time = std::chrono::high_resolution_clock::now(); //save start time for RTT
    }

    // Send message to server
    sendto(sockfd, message.c_str(), message.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr,
           sizeof(servaddr));

    std::cout << "Send message to: " << ip << std::endl;
    close(sockfd);
    return 0;
}

void startSubscribeServer() {
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

    while (true) {
        std::string message;
        message.resize(BUF_SIZE);
        struct sockaddr_in cliaddr;
        memset(&cliaddr, 0, sizeof(cliaddr));

        // Wait for message from client
        socklen_t len = sizeof(cliaddr);
        int n = recvfrom(sockfd, &message[0], message.size(), MSG_WAITALL, (struct sockaddr *) &cliaddr, &len);
        //buffer[n] = '\0';
        if (n > 0) {
            mu.lock();
            //std::cout << "Received message: " << message << std::endl;

            // split the message into its parts
            std::istringstream iss(message);
            std::string bankname, type;
            iss >> bankname >> type;

            if (type == "sub") {
                int numberStocks = 0;
                iss >> numberStocks;
                for (int i = 0; i < numberStocks; ++i) {
                    std::string tmpAcronym;
                    iss >> tmpAcronym;
                    //mu.lock();
                    addSubscriber(tmpAcronym, inet_ntoa(cliaddr.sin_addr)); //could use cliaddr.sin_addr
                    //mu.unlock();
                }
                mapBankToIP(inet_ntoa(cliaddr.sin_addr), bankname);
                std::cout << "Subscriber list changed:" << std::endl;
                printMap();
            } else if (type == "desub") {
                //TO DO desub function
                std::cout << "Subscriber list changed:" << std::endl;
                printMap();
            } else if (type == "stop") {
                mu.unlock();
                return;
            } else if (type == "ACK"){
                //std::cout << "Received ACK message, feature of measuring RTT will be supported in a future version. Stay tuned!" << std::endl;
                if(rttaddr.sin_addr.s_addr == cliaddr.sin_addr.s_addr && rttActive) { //check if response is from correct server (rtt just for one message)
                    end_time = std::chrono::high_resolution_clock::now();
                    long rtt = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count(); //calculate rtt
                    std::cout << "RTT: " << rtt << " microseconds" << std::endl;
                    if(averageRttCnt>0){
                        averageRtt = ((averageRtt*averageRttCnt)+rtt)/(++averageRttCnt);
                    } else {
                        averageRttCnt++;
                        averageRtt = rtt;
                    }
                    std::cout << "New average RTT: " << averageRtt << " microseconds, calculated with " << averageRttCnt << " values" << std::endl;
                }
            } else {
                std::cout << "No valid message type" << std::endl;
                //cout variables
            }
            mu.unlock();
        }
    }

    close(sockfd);
}

void transactionThread() {
    StockMarket stockMarket1;
    stockMarket1.initData();
    stockMarket1.printStockMarket();

    while (true) {
        std::string message = stockMarket1.generateTransaction();

        // split the message into its parts
        std::istringstream iss(message);
        std::string acronym;
        unsigned int price, amount;
        iss >> acronym >> price >> amount;

        std::cout << "New generated transaction: " << message << std::endl;

        mu.lock();
        std::vector <std::string> addresses = getSubscriber(acronym);

        for (int i = 0; i < addresses.size(); ++i) {
            if(i==addresses.size()-1){
                sendMessage(message + "true ", addresses[i], true); //send Message with RTT tracking just for the last message (only supported for one message)
            } else{
                sendMessage(message + "false ", addresses[i], false);
            }
            //std::cout << "Send message to: " << addresses[i] << std::endl;
        }
        mu.unlock();

        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(5));
    }
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
