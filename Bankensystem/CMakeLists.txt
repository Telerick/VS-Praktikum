cmake_minimum_required(VERSION 3.25)
project(Bankensystem)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bankensystem main.cpp StockMarket.h Bank/Bank.h Stock.h initData.h BStock.h SMStock.h Bank/bank.cpp)
