//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_STOCK_H
#define BANKENSYSTEM_STOCK_H

#include "string"
#include "vector"

using namespace std;

class Stock {
public:
    Stock(string acronym, unsigned int price, unsigned int amount)
            : acronym(acronym), price(price), amount(amount) {

    }

    explicit Stock(const string &acronym) : acronym(acronym) {}

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

    unsigned int getAmount() {
        return this->amount;
    }

    void setAmount(unsigned int amount) {
        this->amount = amount;
    }

    void randomizeStock() {
        srand(time(nullptr));
        this->price = rand() % 251 + 50;
        this->amount = rand() % 801 + 200;
    }


private:
    string acronym;
    unsigned int price;
    unsigned int amount;
};


#endif //BANKENSYSTEM_STOCK_H
