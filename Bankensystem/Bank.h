//
// Created by Erik Berg on 12.04.23.
//

#include "vector"
#include "Stock.h"
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

#define MAXLINE 1024

using namespace std;

#ifndef BANKENSYSTEM_BANK_H
#define BANKENSYSTEM_BANK_H

class Bank {
public:
    Bank(vector<Stock *> stocks, string name, int udpPort)
            : portfolio(stocks), cashReserves(100000), outstandingLoans(30000), totalValue(0), name(name) {
        updateTotalValue();
        printTotalValue();


        char buffer[MAXLINE];
        const char *hello = "Hello from server";
        struct sockaddr_in servaddr, cliaddr;

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
        bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr));

        socklen_t len;
        int n;

        len = sizeof(cliaddr); //len is value/result

        n = recvfrom(sockfd, (char *) buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *) &cliaddr,
                     &len);
        buffer[n] = '\0';
        printf("Client : %s\n", buffer);
    }


    unsigned int getValuePortfolio() {
        return this->totalValue;
    }

    void updateStock(string acronym, unsigned int price) {
        for (int i = 0; i < this->portfolio.size(); i++) {
            if (this->portfolio[i]->getAcronym() == acronym) {
                this->portfolio[i]->setPrice(price);
                cout << "Stockvalue updated!" << endl;
                updateTotalValue();
                printTotalValue();
                return;
            }
        }
    }

    void updateTotalValue() {
        for (int i = 0; i < this->portfolio.size(); i++) {
            this->totalValue += (this->portfolio.at(i)->getAmount() * this->portfolio.at(i)->getPrice());
        }
        totalValue += cashReserves;
        totalValue -= outstandingLoans;
    }

    void printTotalValue() {
        cout << "Total Value " + this->name + ": " << this->totalValue << "€" << endl;
    }

private:
    vector<Stock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    string name;
    int sockfd;
};


#endif //BANKENSYSTEM_BANK_H
