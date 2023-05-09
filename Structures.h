#pragma once

struct Cafe {
	int id;
	char name[32];
	char address[32];
	char website[32];
	char description[256];
	int price;
	int coffeeCount;
    float rating;
    long firstCoffeeAddress;
};

struct Coffee {
    int coffeeId;
    char name[32];
    float price;
    char description[256];
    int isMain;
    int cafeId;
    int categoryId;
	int exists;
	long selfAddress;
    long nextAddress;
};

struct Indexer {
	int id;
	int address;
	int exists;
};
