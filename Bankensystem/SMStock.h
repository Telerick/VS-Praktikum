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
        mt19937 rng(std::random_device{}());

        uniform_real_distribution<float> dist(-0.1, 0.1);
        this->price += dist(rng) * this->price;

        uniform_real_distribution<int> dist2(50, 500);

        return this->acronym + " " + std::to_string(this->price) + " " + std::to_string(dist2(rng));
    }


private:
    vector<string> addresses;
};


#endif //BANKENSYSTEM_SMSTOCK_H
