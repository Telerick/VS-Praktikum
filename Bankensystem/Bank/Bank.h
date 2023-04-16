//
// Created by Erik Berg on 12.04.23.
//

#include "vector"
#include "../Stock/BStock.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>


#include <netdb.h>

#include "sstream"

#define STOCK_MARKET_PORT 8080 //TODO: Change (eventually)
#define STOCK_MARKET_IP "127.0.0.1" //TODO: Change

#ifndef BANKENSYSTEM_BANK_H
#define BANKENSYSTEM_BANK_H

class Bank {
public:
    Bank(std::vector<BStock *> stocks, std::string name, int udpPort)
            : portfolio(stocks), cashReserves(100000), outstandingLoans(30000), totalValue(0), name(name) {
        updateTotalValue();
        printTotalValue();

        sockaddr_in servaddr, cliaddr;

        // Creating socket file descriptor
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        memset(&servaddr, 0, sizeof(servaddr));
        memset(&cliaddr, 0, sizeof(cliaddr));

        // Get Bank hostname
        const char* hostname = std::getenv("CONTAINER_NAME");
        //std::cout << "HOSTNAME: " << hostname << std::endl;
        struct hostent* server = gethostbyname(hostname);
        if (server == NULL) {
            std::cerr << "Error: could not resolve hostname" << std::endl;
            return;
        }

        // Filling server information
        servaddr.sin_family = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(udpPort);

            // Bind the socket with the server address
        if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            std::cerr << "Bank Bind failed" << std::endl;
            return;
        }

        std::cout << this->name << " ready for receiving messages" << std::endl;

        /* send a message with all acronyms to the stockMarket
        std::string acronymMsg = "";
        for (auto stock : stocks) {
            acronymMsg += stock->getAcronym() + " ";
        }
        struct sockaddr_in stockMarketAddr;
        memset(&stockMarketAddr, 0, sizeof(stockMarketAddr));
        stockMarketAddr.sin_family = AF_INET;
        stockMarketAddr.sin_port = htons(STOCK_MARKET_PORT);
        stockMarketAddr.sin_addr.s_addr = inet_addr(STOCK_MARKET_IP);
        sendto(sockfd, acronymMsg.c_str(), acronymMsg.length(), 0, (struct sockaddr *)&stockMarketAddr, sizeof(stockMarketAddr));


        // start the receive thread
        std::thread recvThread(&Bank::receiveMessage, this, sockfd);

        // detach the thread to let it run in the background
        recvThread.detach();
        */
    }


    void receiveMessage() {
        while (true) {
            std::cout << "FIRE 1" << std::endl;
            // wait for an incoming message
            std::string message;
            message.resize(1024); // allocate space for the received message
            struct sockaddr_in src_addr;
            socklen_t addrlen = sizeof(src_addr);
            std::cout << "FIRE 2" << std::endl;
            int nbytes = recvfrom(this->sockfd, &message[0], message.size(), MSG_WAITALL, (struct sockaddr *) &src_addr, &addrlen);

            if (nbytes < 0) {
                std::cerr << "Error receiving message" << std::endl;
                break;
            }
            std::cout << "FIRE 3" << std::endl;
            // split the message into its parts
            std::istringstream iss(message);
            std::string acronym;
            unsigned int price, amount;
            iss >> acronym >> price >> amount;

            // print the received message parts and source address
            std::cout << "Received " << nbytes << " bytes from " << inet_ntoa(src_addr.sin_addr) << std::endl;
            std::cout << "Acronym: " << acronym << std::endl;
            std::cout << "Price: " << price << std::endl;
            std::cout << "Amount: " << amount << std::endl;

            updateStock(acronym, price);
            updateTotalValue();
            printTotalValue();
            std::cout << "INSIDE receiveMessage()" << std::endl;
            
        }
    }


    unsigned int getValuePortfolio() {
        return this->totalValue;
    }

    void updateStock(const std::string &acronym, unsigned int price) {
        for (auto &i: this->portfolio) {
            if (i->getAcronym() == acronym) {
                i->setPrice(price);
                std::cout << "Stock price updated!" << std::endl;
                updateTotalValue();
                printTotalValue();
                return;
            }
        }
    }

    void updateTotalValue() {
        for (auto &i: this->portfolio) {
            this->totalValue += (i->getAmount() * i->getPrice());
        }
        totalValue += cashReserves;
        totalValue -= outstandingLoans;
    }

    void printTotalValue() {
        std::cout << "Total Value " + this->name + ": " << this->totalValue << "€" << std::endl;
    }

private:
    std::vector<BStock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    std::string name;
    int sockfd;
};


#endif //BANKENSYSTEM_BANK_H
