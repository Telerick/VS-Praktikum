//
// Created by Erik Berg on 12.04.23.
//
#include <iostream>
#include "../Data/initData.h"
#include "../Stock/SMStock.h"
#include <cstring>

#ifndef BANKENSYSTEM_STOCKMARKET_H
#define BANKENSYSTEM_STOCKMARKET_H

class StockMarket {

public:
    const std::string getName() {
        return Name;
    }

    void setName(const std::string &name) {
        Name = name;
    }

    const std::vector<SMStock *> &getStocksOnMarket() {
        return stocksOnMarket;
    }

    void initData() {
        stocksOnMarket = fillStockMarket();
    }

    void printStockMarket() {
        for (int i = 0; i < stocksOnMarket.size(); ++i) {
            std::cout << stocksOnMarket[i]->getAcronym() << std::endl;
        }
    }

    std::string generateTransaction() {
        int index = rand() % stocksOnMarket.size();
        return stocksOnMarket[index]->createRandomizedStockMessage();
    }

private:
    std::string Name;
    std::vector<SMStock *> stocksOnMarket;
};

#endif // BANKENSYSTEM_STOCKMARKET_H
