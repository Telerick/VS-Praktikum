//
// Created by Erik Berg on 12.04.23.
//
#include <iostream>
#include "SMStock.h"

#ifndef BANKENSYSTEM_STOCKMARKET_H
#define BANKENSYSTEM_STOCKMARKET_H

class StockMarket {

public:

    const string getName() {
        return Name;
    }

    void setName(const string &name) {
        Name = name;
    }

    const vector<SMStock *> &getStocksOnMarket() {
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
            std::cout << stocksOnMarket[i]->getAcronym() << endl;
        }
    }

    string generateTransaction(){
        int index = rand()%stocksOnMarket.size();
        return stocksOnMarket[index]->createRandomizedStockMessage();
    }

private:
    string Name;
    vector<SMStock *> stocksOnMarket;
};


#endif //BANKENSYSTEM_STOCKMARKET_H
