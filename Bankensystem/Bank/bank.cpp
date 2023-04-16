//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../Data/initData.h"

int main() {
    srand(time(0));
    Bank bank(fillPortfolio(), getRandomBankName());
    bank.registerToStockMarket();
    bank.receiveMessage();
};