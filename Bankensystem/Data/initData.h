//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_INITDATA_H
#define BANKENSYSTEM_INITDATA_H

#include "../Stock/BStock.h"
#include "../Stock/SMStock.h"
#include <set> // für std::set

std::vector<SMStock *> stocksInit = {new SMStock("TC19", 60), new SMStock("LSFT", 200), new SMStock("MNSW", 300),
                                     new SMStock("MFC", 180), new SMStock("GDAG", 120), new SMStock("TB2", 230),
                                     new SMStock("JOI", 120), new SMStock("UKR", 190), new SMStock("MKA", 310),
                                     new SMStock("FCB", 60), new SMStock("BVB", 200), new SMStock("B04", 300),
                                     new SMStock("S04", 180), new SMStock("LFC", 120), new SMStock("FRA", 230),
                                     new SMStock("HSV", 70), new SMStock("SVW", 110), new SMStock("M05", 50)};

std::vector <std::string> bankNames = {"liliBank", "softBank", "goodBank", "rickBank", "lionBank", "norbiBank",
                                       "felixBank", "hagenBank", "fishBank", "sandBank", "parkBank", "germanBank",
                                       "lolBank", "miauBank", "wuffBank", "blubbBank", "mountainBank", "lalaBank",
                                       "crazyBank", "peterBank", "michaelBank", "spardaBank", "postBank", "inselBank",
                                       "richBank", "poorBank", "bestBank", "badBank", "funnyBank", "monkeyBank"};

std::vector<BStock *> fillPortfolio() {
    std::cout << "Start fillPortfolio" << std::endl;
    std::vector < BStock * > stocks;
    std::set<int> selectedIndices;

    int numOfDifferentStocks = rand() % 7 + 4;

    while (stocks.size() < numOfDifferentStocks) {
        int index = rand() % stocksInit.size();
        unsigned int stockAmount = rand() % 451 + 50;
        if (selectedIndices.count(index) == 0) {
            selectedIndices.insert(index);
            stocks.push_back(new BStock(stocksInit[index]->getAcronym(), stocksInit[index]->getPrice(), stockAmount));
        }
    }

    std::cout << "End fillPortfolio" << std::endl;
    return stocks;
}

std::vector<SMStock *> fillStockMarket() {
    std::cout << "fillStockMarket" << std::endl;
    return stocksInit;
}

std::string getRandomBankName() {
    return bankNames[rand() % stocksInit.size()];
}

#endif // BANKENSYSTEM_INITDATA_H
