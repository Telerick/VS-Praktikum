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
#include "netdb.h"

#ifndef BANKENSYSTEM_BANK_H
#define BANKENSYSTEM_BANK_H

const int UDP_PORT = 8080;

class Bank {
public:
    Bank(std::vector<BStock *> stocks, std::string name)
            : portfolio(stocks), cashReserves(100000), outstandingLoans(30000), totalValue(0), name(name) {
        std::cout << "Start Constructor Bank" << std::endl;

        updateTotalValue();
        printBankInformation();

        std::cout << "End Constructor Bank" << std::endl;
    }

    void registerToStockMarket() {
        std::cout << "Start registerToStockMarket" << std::endl;

        int sockfdStockMarket;
        // Creating socket file descriptor for stock market
        if ((sockfdStockMarket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cerr << "Error: socket creation failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        sockaddr_in stockMarketAddr;

        const char *hostname = "stockMarket";
        struct hostent *stockMarket = gethostbyname(hostname);
        if (stockMarket == NULL) {
            std::cerr << "Error: could not resolve hostname" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Filling stockMarket information
        memset(&stockMarketAddr, 0, sizeof(stockMarketAddr));
        stockMarketAddr.sin_family = AF_INET;
        stockMarketAddr.sin_addr = *((struct in_addr *) stockMarket->h_addr);
        stockMarketAddr.sin_port = htons(UDP_PORT);

        std::cout << "Registering to Stock Market with IP: " << inet_ntoa(stockMarketAddr.sin_addr) << std::endl;

        std::string regMsg = createRegisterMessage();
        // Send a registration message with hostname and all stock acronyms to the stockMarket
        sendto(sockfdStockMarket, regMsg.c_str(), regMsg.length(), 0, (struct sockaddr *) &stockMarketAddr,
               sizeof(stockMarketAddr));

        //std::cout << "End registerToStockMarket" << std::endl;
    }

    void receiveMessage() {
        //std::cout << "Start receiveMessage" << std::endl;

        // Creating socket file descriptor for bank
        int sockfdBank;
        if ((sockfdBank = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            std::cerr << "Error: socket creation failed" << std::endl;
            exit(EXIT_FAILURE);
        }


        sockaddr_in bankAddr;
        // Filling bank information
        memset(&bankAddr, 0, sizeof(bankAddr));
        bankAddr.sin_family = AF_INET;
        bankAddr.sin_addr.s_addr = INADDR_ANY;
        bankAddr.sin_port = htons(UDP_PORT);

        // Bind the socket with the bank address
        if (bind(sockfdBank, (const struct sockaddr *) &bankAddr,
                 sizeof(bankAddr)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        //std::cout << this->name << " ready for receiving messages" << std::endl;

        while (true) {
            //std::cout << "Before receive" << std::endl;
            // wait for an incoming message
            std::string message;
            message.resize(1024); // allocate space for the received message
            socklen_t addrlen = sizeof(bankAddr);
            int nbytes = recvfrom(sockfdBank, &message[0], message.size(), 0, (struct sockaddr *) &bankAddr,
                                  &addrlen);
            if (nbytes < 0) {
                std::cerr << "Error receiving message" << std::endl;
                break;
            }
            //std::cout << "After receive" << std::endl;
            // split the message into its parts
            std::istringstream iss(message);
            std::string acronym;
            unsigned int price, amount;
            iss >> acronym >> price >> amount;

            // print the received message parts and source address
            std::cout << "Received " << nbytes << " bytes from " << inet_ntoa(bankAddr.sin_addr) << std::endl;
            std::cout << "Acronym: " << acronym << std::endl;
            std::cout << "Price: " << price << std::endl;
            std::cout << "Amount: " << amount << std::endl;

            updateStock(acronym, price);
            updateTotalValue();
            //printTotalValue();
            //std::cout << "INSIDE receiveMessage()" << std::endl;

        }
       // std::cout << "End receiveMessage" << std::endl;
    }

    std::string createRegisterMessage() {
        // Set first attribute of register message as bankname (which is equal to hostname)
        std::string regMsg = this->name + " ";

        // Set second argument as ttype (sub- or desubscribe // "sub" "desub")
        regMsg += "sub ";

        // Set third attribute of register message as number of stocks in portfolio
        regMsg += std::to_string(this->portfolio.size()) + " ";

        // Add all stock acronyms of portfolio to the register message
        for (auto stock: this->portfolio) {
            regMsg += stock->getAcronym() + " ";
        }

        std::cout << "regMes: " << regMsg << std::endl;
        return regMsg;
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

    void printBankName() {
        std::cout << "Bank name " + this->name << std::endl;
    }

    void printPortfolioData() {
        for (int i = 0; i < this->portfolio.size(); i++) {
            std::cout << i + 1 << " .Stock: " + this->portfolio[i]->getAcronym() << "\t" << "price: "
                      << this->portfolio[i]->getPrice() << "€" << "\t" << "Amount: " << this->portfolio[i]->getAmount()
                      << std::endl;
        }
    }

    void printBankInformation() {
        printBankName();
        std::cout << std::endl;
        printPortfolioData();
        std::cout << std::endl;
        printTotalValue();
    }

private:
    std::vector<BStock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    std::string name;
};

#endif // BANKENSYSTEM_BANK_H
