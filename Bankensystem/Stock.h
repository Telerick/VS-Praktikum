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

using namespace std;

class Stock {
public:
    Stock(string acronym, unsigned int price)
            : acronym(acronym), price(price) {
    }

    string getAcronym() {
        return this->acronym;
    }

    void setAcronym(string acronym) {
        this->acronym = acronym;
    }

    unsigned int getPrice() {
        return this->price;
    }

    void setPrice(unsigned int price) {
        this->price = price;
    }


protected:
    string acronym;
    unsigned int price;
};


#endif //BANKENSYSTEM_STOCK_H
