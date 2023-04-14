//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_SMSTOCK_H
#define BANKENSYSTEM_SMSTOCK_H

#include "Stock.h"

class SMStock : public Stock {
public:
    SMStock(string acronym, unsigned int price)
            : Stock(acronym, price) {
    }

    string createRandomizedStockMessage() {

        srand(time(0));
        float courseChangeFactor = ((float)rand()/(float)RAND_MAX) * 0.38 - 0.19;

        this->price += (unsigned int)(courseChangeFactor * this->price);

        unsigned int stockAmount = rand() % 451 + 50;

        return this->acronym + " " + std::to_string(this->price) + " " + std::to_string(stockAmount);
    }


private:
    vector<string> addresses;
};


#endif //BANKENSYSTEM_SMSTOCK_H
