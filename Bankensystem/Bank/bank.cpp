//
// Created by Erik Berg on 14.04.23.
//

#include "Bank.h"
#include "../initData.h"



int main() {
    srand(time(0));
    Bank bank(fillPortfolio(), bankNames[rand() % 30], 8080);
};