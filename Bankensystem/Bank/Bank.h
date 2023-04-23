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
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
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
        std::cout << "Start Constructor Bank" << this->name << std::endl;

        updateTotalValue();
        printBankInformation();

        std::cout << "End Constructor Bank" << this->name << std::endl;
    }

    void registerToStockMarket() {
        std::cout << "Start registration process for " << this->name << std::endl;

        int sockfd;
        // Creating socket file descriptor for stock market
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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
        sendto(sockfd, regMsg.c_str(), regMsg.length(), 0, (struct sockaddr *) &stockMarketAddr,
               sizeof(stockMarketAddr));

        std::cout << "Registration sent" << std::endl;
    }

    void receiveMessage() {
        std::cout << "Start receiveMessage" << std::endl;

        // Creating socket file descriptor for bank
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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
        if (bind(sockfd, (const struct sockaddr *) &bankAddr,
                 sizeof(bankAddr)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << this->name << " ready for receiving messages" << std::endl;

        while (true) {
            //std::cout << "Before receive" << std::endl;
            // wait for an incoming message
            std::string message;
            message.resize(1024); // allocate space for the received message
            socklen_t addrlen = sizeof(bankAddr);
            int nbytes = recvfrom(sockfd, &message[0], message.size(), 0, (struct sockaddr *) &bankAddr,
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
            std::cout << this->name << " received " << nbytes << " bytes from "
                      << inet_ntoa(bankAddr.sin_addr) << std::endl;
            std::cout << "TRANSACTION ->\tAcronym: " << acronym << "\t";
            std::cout << "Price: " << price << "\t";
            std::cout << "Amount: " << amount << std::endl;


            updateStock(acronym, price);
            //std::cout << "INSIDE receiveMessage()" << std::endl;

        }
        //std::cout << "End receiveMessage" << std::endl;
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

        std::cout << "Registration message: " << regMsg << std::endl;
        return regMsg;
    }

    unsigned int getValuePortfolio() {
        return this->totalValue;
    }

    void updateStock(const std::string &acronym, unsigned int price) {
        printTotalValue();
        for (auto &i: this->portfolio) {
            if (i->getAcronym() == acronym) {
                i->setPrice(price);
                std::cout << "Stock price for " << i->getAcronym() << " updated!" << std::endl;
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

    // HTTP GET & POST interface for web ui
    void bankInterface() {
        std::cout << "Start bankInterface" << std::endl;
        const int PORT = 8000;
        int server_socket, client_socket;
        struct sockaddr_in server_address, client_address;
        socklen_t client_address_len = sizeof(client_address);
        char BUFFER[1024] = {0};
        std::string response;

        // create socket
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket < 0) {
            std::cout << "error opening socket" << std::endl;
            exit(1);
        }

        // setup server address
        memset(&server_address, 0, sizeof(server_address));
        server_address.sin_family = AF_INET;
        server_address.sin_addr.s_addr = INADDR_ANY;
        server_address.sin_port = htons(PORT);

        // bind socket to address
        if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
            std::cout << "error on binding" << std::endl;
            exit(1);
        }

        // listen for incoming connections
        // maximum of 5 pending connections
        if (listen(server_socket, 5) < 0) {
            std::cout << "error on listening" << std::endl;
            exit(1);
        }


        // accept incoming connections and process requests
        while (true) {
            client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
            if (client_socket < 0) {
                std::cout << "error on accept connection" << std::endl;
                exit(1);
            }

            // lock mutex before accessing shared data
            //std::lock_guard<std::mutex> lock(mu);

            // read client request
            int request_size = read(client_socket, BUFFER, sizeof(BUFFER) - 1 );
            if (request_size < 0) {
                std::cout << "error reading from socket" << std::endl;
                exit(1);
            }

            // process GET request and response with bank value
            std::cout << strncmp(BUFFER, "GET /total HTTP/1.1", 18) << std::endl;
            if (strncmp(BUFFER, "GET /total HTTP/1.1", 18) == 0) {
                std::cout << "GET RESPONSE" << std::endl;
                // lock_guard ensures that the mutex is automatically unlocked when the scope is exited
                std::stringstream response_stream;
                response_stream << "HTTP/1.1 200 OK\r\n";
                response_stream << "Content-Type: text/html\r\n";
                response_stream << "Connection: close\r\n\r\n";
                response_stream << this->totalValue;
                response = response_stream.str();
                std::cout << "GET:" << response << std::endl;
            }
            // process POST request and update bank data
            else if(strncmp(BUFFER, "POST /update HTTP/1.1", 21) == 0) {
                // extract values from body
                std::string request(BUFFER);
                std::size_t pos = request.find("\r\n\r\n");
                if (pos != std::string::npos) {
                    std::string value_str = request.substr(pos + 4);
                    int new_value = std::stoi(value_str);

                    // lock mutex before modifying shared data
                    // this ensures that only one thread can write to totalValue at a time
                    //std::lock_guard<std::mutex> lock(mu);

                    // update value with data from POST request
                    this->totalValue = new_value;

                    // generate OK response
                    std::stringstream response_stream;
                    response_stream << "HTTP/1.1 200 OK\r\n";
                    response_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
                    response_stream << "Connection: close\r\n\r\n";
                    response = response_stream.str();
                } else {
                    response = "HTTP/1.1 400 Bad Request\r\n";
                }
            } else {
                response = "HTTP/1.1 400 Bad Request\r\n";
            }

            // send response to client
            int response_size = send(client_socket, response.c_str(), response.size(), 0);
            if (response_size < 0) {
                std::cout << "error writing to socket" << std::endl;
                exit(1);
            }

            // close client connection
            close(client_socket);
        }

        // close server connection
        close(server_socket);
    }

private:
    std::vector<BStock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    std::string name;
    std::mutex mu;
};

#endif // BANKENSYSTEM_BANK_H
