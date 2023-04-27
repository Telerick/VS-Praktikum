//
// Created by Erik Berg on 12.04.23.
//

#include "vector"
#include "../Stock/BStock.h"
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <netdb.h>

#include "sstream"
#include "netdb.h"

#ifndef BANKENSYSTEM_BANK_H
#define BANKENSYSTEM_BANK_H

const int UDP_PORT = 8080;
const int TCP_PORT = 8000;

class Bank
{
public:
    Bank(std::vector<BStock *> stocks, std::string name, std::string myIP)
        : portfolio(stocks), cashReserves(100000), outstandingLoans(30000), totalValue(0), name(name), myIP(myIP)
    {
        // std::cout << "Start Constructor Bank" << this->name << std::endl;

        updateTotalValue();
        printBankInformation();

        // std::cout << "End Constructor Bank" << this->name << std::endl;
    }

    Bank(const Bank &other)
        : portfolio(other.portfolio), cashReserves(other.cashReserves), outstandingLoans(other.outstandingLoans),
          totalValue(other.totalValue), name(other.name)
    {
        std::cout << "Copy Constructor Bank" << this->name << std::endl;
    }

    ////////////////////////////////////////////////////////////////////GETTER & SETTER//////////////////////////////////////////////////////////////

    unsigned int getTotalValue()
    {
        return this->totalValue;
    }

    void setTotalValue(unsigned int val)
    {
        this->totalValue = val;
    }

    std::vector<BStock *> getPortfolio()
    {
        return this->portfolio;
    }

    int getCashReserves()
    {
        return this->cashReserves;
    }

    void setCashReserves(int cashReserves)
    {
        this->cashReserves = cashReserves;
    }

    int getOutstandingLoans()
    {
        return this->outstandingLoans;
    }

    void setOutstandingLoans(int outstandingLoans)
    {
        this->outstandingLoans = outstandingLoans;
    }

    std::string getName()
    {
        return this->name;
    }

    void setName(std::string name)
    {
        this->name = name;
    }
    ////////////////////////////////////////////////////////////////////UPDATE values methods//////////////////////////////////////////////////////////////

    void updateStock(const std::string &acronym, unsigned int price)
    {
        printTotalValue();
        for (auto &i : this->portfolio)
        {
            if (i->getAcronym() == acronym)
            {
                i->setPrice(price);
                std::cout << "Stock price for " << i->getAcronym() << " updated!" << std::endl;
                updateTotalValue();
                printTotalValue();
                return;
            }
        }
    }

    void updateTotalValue()
    {
        this->totalValue = 0;

        for (auto &i : this->portfolio)
        {
            this->totalValue += (i->getAmount() * i->getPrice());
        }

        totalValue += cashReserves;
        totalValue -= outstandingLoans;
    }

    void updateCashreserves(int newCashreserves)
    {
        this->cashReserves = newCashreserves;
        updateTotalValue();
    }

    void updateOutstandingLoans(int newOutstandingLoans)
    {
        this->outstandingLoans = newOutstandingLoans;
        updateTotalValue();
    }

    void updateStockAmount(std::string acronym, int amount)
    {
        for (auto stock : this->portfolio)
        {
            if (stock->getAcronym() == acronym)
            {
                stock->setAmount((unsigned int)amount);
                updateTotalValue();
                return;
            }
        }
    }

    ////////////////////////////////////////////////////////////////////PRINT methods//////////////////////////////////////////////////////////////

    void printTotalValue()
    {
        std::cout << "Total Value " + this->name + ": " << this->totalValue << "€" << std::endl;
    }

    void printBankNameAndIP()
    {
        std::cout << "### " + this->name << " (" << this->myIP << ")"
                  << " ###" << std::endl;
    }

    void printPortfolioData()
    {
        for (int i = 0; i < this->portfolio.size(); i++)
        {
            std::cout << i + 1 << " .Stock: " + this->portfolio[i]->getAcronym() << "\t"
                      << "price: "
                      << this->portfolio[i]->getPrice() << "€"
                      << "\t"
                      << "Amount: " << this->portfolio[i]->getAmount()
                      << std::endl;
        }
    }

    void printBankInformation()
    {
        printBankNameAndIP();
        std::cout << std::endl;
        printPortfolioData();
        std::cout << std::endl;
        printTotalValue();
    }

private:
    std::vector<BStock *> portfolio;
    int cashReserves;
    int outstandingLoans;
    unsigned int totalValue;
    std::string name;
    std::string myIP;
};

#endif // BANKENSYSTEM_BANK_H
