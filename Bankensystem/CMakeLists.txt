cmake_minimum_required(VERSION 3.25)
project(Bankensystem)

set(CMAKE_CXX_STANDARD 14)

add_executable(Bankensystem main.cpp StockMarket.h Bank.h Stock.h stocksInitData.h BStock.h SMStock.h)
