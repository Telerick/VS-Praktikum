//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_STOCK_H
#define BANKENSYSTEM_STOCK_H

#include "string"
#include "vector"
#include "random"
#include <cstdlib>
#include <ctime>

class Stock {
public:
    Stock(std::string acronym, unsigned int price)
            : acronym(acronym), price(price) {
    }

    std::string getAcronym() {
        return this->acronym;
    }

    void setAcronym(std::string acronym) {
        this->acronym = acronym;
    }

    unsigned int getPrice() {
        return this->price;
    }

    void setPrice(unsigned int price) {
        this->price = price;
    }

protected:
    std::string acronym;
    unsigned int price;
};

#endif // BANKENSYSTEM_STOCK_H
