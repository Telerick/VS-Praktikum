//
// Created by Erik Berg on 12.04.23.
//

#include "vector"
#include "BStock.h"
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


using namespace std;

#ifndef BANKENSYSTEM_BANK_H
#define BANKENSYSTEM_BANK_H

class Bank {
public:
    Bank(vector<BStock *> stocks, string name, int udpPort)
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

        // Filling server information
        servaddr.sin_family = AF_INET; // IPv4
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(udpPort);

        // Bind the socket with the server address
        if (::bind(sockfd, (const struct sockaddr *) &servaddr,
                   sizeof(servaddr)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
    }


    unsigned int getValuePortfolio() {
        return this->totalValue;
    }

    void updateStock(const string &acronym, unsigned int price) {
        for (auto &i: this->portfolio) {
            if (i->getAcronym() == acronym) {
                i->setPrice(price);
                cout << "Stockvalue updated!" << endl;
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
        cout << "Total Value " + this->name + ": " << this->totalValue << "€" << endl;
    }

private:
    vector<BStock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    string name;
    int sockfd;
};


#endif //BANKENSYSTEM_BANK_H
