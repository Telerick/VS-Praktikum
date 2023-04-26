//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"
#include "functional"

int main() {
    std::string containerName = std::getenv("CONTAINER_NAME");
    std::string myIP = std::getenv("MY_IPV4_ADDRESS");
    std::hash<std::string> nameHash;
    unsigned int seed = nameHash(containerName);
    srand(seed);
    Bank bank(fillPortfolio(), containerName, myIP);
    bank.registerToStockMarket();
    bank.receiveMessage();
    //std::cout << "OUTSIDE receiveMessage()" << std::endl;
};