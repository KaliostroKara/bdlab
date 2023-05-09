#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "Coffee.h"

int getCafe(struct Cafe* cafe, int id, char* error);

void loopThroughCafes(int indAmount, struct Cafe* cafe, char *dummy, int *cafeCount, int *coffeeCount);

int checkFileExists(FILE* indexTable, FILE* database, char* error) {
	if (indexTable == NULL || database == NULL) {
		strcpy(error, "DB files do not exits");
		return 0;
	}
	return 1;
}

int checkIndexExists(FILE* indexTable, char* error, int id) {
	fseek(indexTable, 0, SEEK_END);
	long indexTableSize = ftell(indexTable);
	if (indexTableSize == 0 || id * sizeof(struct Indexer) > indexTableSize) {
		strcpy(error, "no such ID in table");
		return 0;
	}
	return 1;
}



int checkRecordExists(struct Indexer indexer, char* error) {
	if (!indexer.exists) {
		strcpy(error, "the record has been deleted");
		return 0;
	}
	return 1;
}

int checkKeyPairUnique(struct Cafe cafe, int coffeeId) {
	FILE* coffeeDb = fopen(COFFEE_DATA, "a+b");
	struct Coffee coffee;
	fseek(coffeeDb, cafe.firstCoffeeAddress, SEEK_SET);
	for (int i = 0; i < cafe.coffeeCount; i++) {
		fread(&coffee, COFFEE_SIZE, 1, coffeeDb);
		fclose(coffeeDb);
		if (coffee.coffeeId == coffeeId) {
			return 0;
		}
        coffeeDb = fopen(COFFEE_DATA, "r+b");
		fseek(coffeeDb, coffee.nextAddress, SEEK_SET);
	}
	fclose(coffeeDb);
	return 1;
}

struct Cafe findMaster(int id){
    struct Cafe cafe;
    struct Coffee coffee;
    for (int j = 1; j <= SIZE; ++j){
            char error[51];
            if (getCafe(&cafe, j, error) == 1){
            if (getCoffee(cafe, &coffee, id, error) == 1){
                return cafe;
            }
            }
    }
    return cafe;
}

int findNewId(){
    struct Cafe cafe;
    struct Coffee coffee;
    for (int i = 1; i <= SIZE; ++i){
        int fl = 1;
        for (int j = 1; j <= SIZE; ++j){
            char error[51];
            if (getCafe(&cafe, j, error) == 1){
            if (getCoffee(cafe, &coffee, i, error) == 1){
                fl = 0;
                break;
            }
            }
        }
        if (fl == 1) return i;
    }
}

void info() {
	FILE* indexTable = fopen("cafe.ind", "rb");
	if (indexTable == NULL) {
		printf("Error: database files do not exist\n");
		return;
	}
	int cafeCount = 0;
	int coffeeCount = 0;
	fseek(indexTable, 0, SEEK_END);
	int indAmount = ftell(indexTable) / sizeof(struct Indexer);
	struct Cafe cafe;
	char dummy[51];
    loopThroughCafes(indAmount, &cafe, dummy, &cafeCount, &coffeeCount);
    fclose(indexTable);
	printf("Total cafes: %d\n", cafeCount);
	printf("Total coffees: %d\n", coffeeCount);
}

void loopThroughCafes(int indAmount, struct Cafe* cafe, char* dummy, int* cafeCount, int* coffeeCount) {
    for (int i = 1; i <= indAmount; i++) {
        if (getCafe(cafe, i, dummy)) {
            (*cafeCount)++;
            (*coffeeCount) += (*cafe).coffeeCount;
            printf("Cafe #%d has %d coffees\n", i, (*cafe).coffeeCount);
            printf("Cafe name: %s\n", (*cafe).name);
            viewAll(*cafe);
            printf("\n");
        }
    }
}
