//
// Created by Erik Berg on 12.04.23.
//
#include <iostream>
#include "../Stock/SMStock.h"

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

    void initData(){
        stocksOnMarket.push_back(new SMStock("LSFT", 200));
        stocksOnMarket.push_back(new SMStock("TC19", 60));
        stocksOnMarket.push_back(new SMStock("MNSW", 300));
        stocksOnMarket.push_back(new SMStock("MFC", 180));
        stocksOnMarket.push_back(new SMStock("GDAG", 120));
        stocksOnMarket.push_back(new SMStock("TB2", 230));
        stocksOnMarket.push_back(new SMStock("JOI", 90));
    }

    void printStockMarket(){
        for (int i = 0; i < stocksOnMarket.size(); ++i) {
            std::cout << stocksOnMarket[i]->getAcronym() << std::endl;
        }
    }

    std::string generateTransaction(){
        int index = rand()%stocksOnMarket.size();
        return stocksOnMarket[index]->createRandomizedStockMessage();
    }

private:
    std::string Name;
    std::vector<SMStock *> stocksOnMarket;
};


#endif //BANKENSYSTEM_STOCKMARKET_H
