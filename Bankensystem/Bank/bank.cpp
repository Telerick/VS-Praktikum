//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"

int main() {
    srand(time(0));
<<<<<<< HEAD
    // Bank bank(fillPortfolio(), bankNames[rand() % 30], 8080);
    Bank bank(fillPortfolio(), std::getenv("CONTAINER_NAME"), 8080);
=======
    Bank bank(fillPortfolio(), getRandomBankName());
    bank.registerToStockMarket();
>>>>>>> 6083334fe2c6e5bfc61bef16b717d3c9698d1a9d
    bank.receiveMessage();
    std::cout << "OUTSIDE receiveMessage()" << std::endl;
    
};