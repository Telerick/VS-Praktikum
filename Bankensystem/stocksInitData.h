//
// Created by Erik Berg on 12.04.23.
//

#ifndef BANKENSYSTEM_STOCKSINITDATA_H
#define BANKENSYSTEM_STOCKSINITDATA_H

#include "BStock.h"
#include "SMStock.h"
#include <vector>

vector<BStock *> fillPortfolio() {
    vector<BStock *> stocksInit;

    stocksInit.push_back(new BStock("LSFT", 200, 0));
    stocksInit.push_back(new BStock("TC19", 60, 0));
    stocksInit.push_back(new BStock("MNSW", 300, 0));
    stocksInit.push_back(new BStock("MFC", 180, 0));
    stocksInit.push_back(new BStock("GDAG", 120, 0));
    stocksInit.push_back(new BStock("TB2", 230, 0));
    stocksInit.push_back(new BStock("JOI", 90, 0));

    vector<BStock *> stocks;

    mt19937 rng(std::random_device{}());
    uniform_real_distribution<int> dist(2, 6);
    int random = dist(rng);

    for (int i = 0; i < random; i++) {

        uniform_real_distribution<int> dist2(200, 1000);
        stocks.push_back(stocksInit[i]);
        stocksInit[i]->setAmount(dist2(rng));

    }
    return stocks;
}

vector<SMStock *> fillStocketMarket() {
    vector<SMStock *> stocksInit;

    stocksInit.push_back(new SMStock("LSFT", 200));
    stocksInit.push_back(new SMStock("TC19", 60));
    stocksInit.push_back(new SMStock("MNSW", 300));
    stocksInit.push_back(new SMStock("MFC", 180));
    stocksInit.push_back(new SMStock("GDAG", 120));
    stocksInit.push_back(new SMStock("TB2", 230));
    stocksInit.push_back(new SMStock("JOI", 90));


}

#endif // BANKENSYSTEM_STOCKSINITDATA_H
