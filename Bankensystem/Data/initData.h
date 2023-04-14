//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_INITDATA_H
#define BANKENSYSTEM_INITDATA_H

#include "../Stock/BStock.h"
#include "../Stock/SMStock.h"

std::vector<BStock *> fillPortfolio() {
    std::vector<BStock *> stocksInit;

    stocksInit.push_back(new BStock("LSFT", 200, 0));
    stocksInit.push_back(new BStock("TC19", 60, 0));
    stocksInit.push_back(new BStock("MNSW", 300, 0));
    stocksInit.push_back(new BStock("MFC", 180, 0));
    stocksInit.push_back(new BStock("GDAG", 120, 0));
    stocksInit.push_back(new BStock("TB2", 230, 0));
    stocksInit.push_back(new BStock("JOI", 90, 0));

    std::vector<BStock *> stocks;

    int numOfDifferentStocks = rand() % 5 + 2;

    for (int i = 0; i < numOfDifferentStocks; i++) {

        unsigned int stockAmount = rand() % 951 + 50;
        stocks.push_back(stocksInit[i]);
        stocksInit[i]->setAmount(stockAmount);

    }
    return stocks;
}

std::vector<SMStock *> fillStocketMarket() {
    std::vector<SMStock *> stocksInit;

    stocksInit.push_back(new SMStock("LSFT", 200));
    stocksInit.push_back(new SMStock("TC19", 60));
    stocksInit.push_back(new SMStock("MNSW", 300));
    stocksInit.push_back(new SMStock("MFC", 180));
    stocksInit.push_back(new SMStock("GDAG", 120));
    stocksInit.push_back(new SMStock("TB2", 230));
    stocksInit.push_back(new SMStock("JOI", 90));

    return stocksInit;
}

std::vector<std::string> bankNames = {"liliBank", "softBank", "goodBank", "rickBank", "lionBank", "norbiBank",
                                      "felixBank", "hagenBank", "fishBank", "sandBank", "parkBank", "germanBank",
                                      "lolBank", "miauBank", "wuffBank", "blubbBank", "mountainBank", "lalaBank",
                                      "crazyBank", "peterBank", "michaelBank", "spardaBank", "postBank", "inselBank",
                                      "richBank", "poorBank", "bestBank", "badBank", "funnyBank","monkeyBank" };



#endif // BANKENSYSTEM_INITDATA_H
