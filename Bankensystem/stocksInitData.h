//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_STOCKSINITDATA_H
#define BANKENSYSTEM_STOCKSINITDATA_H

#include "Stock.h"
#include <vector>

vector<Stock *> fillPortfolio() {
    vector<Stock *> stocksInit;

    stocksInit.push_back(new Stock("LSFT", 200, 0));
    stocksInit.push_back(new Stock("TC19", 60, 0));
    stocksInit.push_back(new Stock("MNSW", 300, 0));
    stocksInit.push_back(new Stock("MFC", 180, 0));
    stocksInit.push_back(new Stock("GDAG", 120, 0));
    stocksInit.push_back(new Stock("TB2", 230, 0));
    stocksInit.push_back(new Stock("JOI", 90, 0));

    vector<Stock *> stocks;

    int random = rand() % 5 + 2; // + 2 für Mindestanzahl von 3 Aktientypen

    for (int i = 0; i < random; i++) {
        int randomAmount = rand() % 801 + 200;
        stocksInit[i]->setAmount(randomAmount);
        stocks.push_back(stocksInit[i]);
    }
    return stocks;
}

#endif // BANKENSYSTEM_STOCKSINITDATA_H
