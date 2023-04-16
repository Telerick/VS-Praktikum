//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"
#include "functional"

int main() {
    std::string containerName = std::getenv("CONTAINER_NAME");
    std::hash<std::string> nameHash;
    unsigned int seed = nameHash(containerName);
    srand(seed);
    Bank bank(fillPortfolio(), containerName);
    bank.receiveMessage();
    std::cout << "OUTSIDE receiveMessage()" << std::endl;
};