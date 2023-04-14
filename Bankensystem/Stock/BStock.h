//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_BSTOCK_H
#define BANKENSYSTEM_BSTOCK_H

#include "Stock.h"


class BStock : public Stock{
public:
    BStock(std::string acronym, unsigned int price, unsigned int amount)
    : Stock(acronym, price), amount(amount) {
    }

    unsigned int getAmount() {
        return this->amount;
    }

    void setAmount(unsigned int amount) {
        this->amount = amount;
    }


private:
    unsigned int amount;
};


#endif //BANKENSYSTEM_BSTOCK_H
