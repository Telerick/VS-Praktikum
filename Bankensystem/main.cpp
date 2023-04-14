#include <iostream>
#include "Bank.h"
#include "stocksInitData.h"

int main() {
    // initial stock values & bank
    srand(time(0));
    Bank liliBank(fillPortfolio(), "liliBank", 8080);
    Bank softBank(fillPortfolio(), "softBank", 8080);
    Bank goodBank(fillPortfolio(), "goodBank", 8080);
    Bank rickBank(fillPortfolio(), "rickBank", 8080);
    Bank lionBank(fillPortfolio(), "lionBank", 8080);
    Bank norbiBank(fillPortfolio(), "norbiBank", 8080);
    Bank felixBank(fillPortfolio(), "felixBank", 8080);

    rickBank.updateStock("TC19", 289);

}

