//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"



int main() {
    srand(time(0));
    // Bank bank(fillPortfolio(), bankNames[rand() % 30], 8080);
    Bank bank(fillPortfolio(), std::getenv("CONTAINER_NAME"), 8080);
    bank.receiveMessage();
    std::cout << "OUTSIDE receiveMessage()" << std::endl;
    
};