#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Info.h"
#include "Cafe.h"

#define COFFEE_DATA "coffee.fl"
#define COFFEE_GARBAGE "coffee_garbage.txt"
#define COFFEE_SIZE sizeof(struct Coffee)
#define SIZE 10

struct Coffee linkLoop(FILE *database, struct Cafe* cafe, struct Coffee *previous);

int updateCafe(struct Cafe cafe, char* error);

void printCoffeeFromCafe(struct Coffee coffee) {
    printf("Coffee name: %s\n", coffee.name);
	printf("Coffee ID: %d\n", coffee.coffeeId);
	printf("Coffee price: %.2f\n", coffee.price);
}

void viewAll(struct Cafe cafe) {
	FILE* coffeeDb = fopen(COFFEE_DATA, "a+b");
	struct Coffee coffee;
	fseek(coffeeDb, cafe.firstCoffeeAddress, SEEK_SET);
	for (int i = 0; i < cafe.coffeeCount; i++) {
        printf ("#%d coffee from cafe:\n", i + 1);
		fread(&coffee, COFFEE_SIZE, 1, coffeeDb);
		fclose(coffeeDb);
		printCoffeeFromCafe(coffee);
        coffeeDb = fopen(COFFEE_DATA, "r+b");
		fseek(coffeeDb, coffee.nextAddress, SEEK_SET);
	}
	fclose(coffeeDb);
	return;
}

struct Coffee linkLoop(FILE *database, struct Cafe *cafe, struct Coffee *previous) {
    for (int i = 0; i < (*cafe).coffeeCount; i++) {
        fread(previous, COFFEE_SIZE, 1, database);
        fseek(database, (*previous).nextAddress, SEEK_SET);
    }
    return (*previous);
}

void reopenDatabase(FILE* database) {
	fclose(database);
	database = fopen(COFFEE_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Cafe cafe, struct Coffee coffee) {
	reopenDatabase(database);
	struct Coffee previous;
	fseek(database, cafe.firstCoffeeAddress, SEEK_SET);
    previous = linkLoop(database, &cafe, &previous);
    previous.nextAddress = coffee.selfAddress;
	fwrite(&previous, COFFEE_SIZE, 1, database);
}

void relinkAddresses(FILE* database, struct Coffee previous, struct Coffee coffee, struct Cafe* cafe) {
	if (coffee.selfAddress == cafe->firstCoffeeAddress) {
		if (coffee.selfAddress == coffee.nextAddress) {
            cafe->firstCoffeeAddress = -1;
		} else {
            cafe->firstCoffeeAddress = coffee.nextAddress;
		}
	} else {
		if (coffee.selfAddress == coffee.nextAddress) {
			previous.nextAddress = previous.selfAddress;
		}
		else {
			previous.nextAddress = coffee.nextAddress;
		}
		fseek(database, previous.selfAddress, SEEK_SET);
		fwrite(&previous, COFFEE_SIZE, 1, database);
	}
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Coffee* record) {
	long* deletedIds = (long * )malloc(garbageCount * sizeof(long));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%ld", deletedIds + i);
	}

	record->selfAddress = deletedIds[0];
	record->nextAddress = deletedIds[0];

	fclose(garbageZone);
	fopen(COFFEE_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %ld", deletedIds[i]);
	}

	free(deletedIds);
	fclose(garbageZone);
}

void noteDeletedCoffee(long address) {
    FILE* garbageZone = fopen(COFFEE_GARBAGE, "rb");
    int garbageCount;
    fscanf(garbageZone, "%d", &garbageCount);
    long* deletedAddresses = (long * )malloc(garbageCount * sizeof(long));

    for (int i = 0; i < garbageCount; i++) {
        fscanf(garbageZone, "%ld", deletedAddresses + i);
    }

    fclose(garbageZone);
    garbageZone = fopen(COFFEE_GARBAGE, "wb");
    fprintf(garbageZone, "%d", garbageCount + 1);

    for (int i = 0; i < garbageCount; i++) {
        fprintf(garbageZone, " %ld", deletedAddresses[i]);
    }

    fprintf(garbageZone, " %ld", address);
    free(deletedAddresses);
    fclose(garbageZone);
}

int getCoffee(struct Cafe cafe, struct Coffee* coffee, int coffeeId, char* error) {
    if (!cafe.coffeeCount) {
        strcpy(error, "The cafe is empty");
        return 0;
    }
    FILE* database = fopen(COFFEE_DATA, "rb");
	fseek(database, cafe .firstCoffeeAddress, SEEK_SET);
    fread(coffee, COFFEE_SIZE, 1, database);

    for (int i = 0; i < cafe.coffeeCount; i++) {
        if (coffee->coffeeId == coffeeId) {
            fclose(database);
            return 1;
        }
        fseek(database, coffee->nextAddress, SEEK_SET);
        fread(coffee, COFFEE_SIZE, 1, database);
    }
    strcpy(error, "No such coffee in database");
    fclose(database);
    return 0;
}

int insertCoffee(struct Cafe cafe, struct Coffee coffee, char* error) {
    coffee.exists = 1;
	FILE* database = fopen(COFFEE_DATA, "a+b");
	FILE* garbageZone = fopen(COFFEE_GARBAGE, "a+b");
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);
	if (garbageCount) {
		overwriteGarbageAddress(garbageCount, garbageZone, &coffee);
		reopenDatabase(database);
		fseek(database, coffee.selfAddress, SEEK_SET);
	} else {
		fseek(database, 0, SEEK_END);
		int dbSize = ftell(database);
        coffee.selfAddress = dbSize;
		coffee.nextAddress = dbSize;
	}
	fwrite(&coffee, COFFEE_SIZE, 1, database);
	if (!cafe.coffeeCount) {
		cafe.firstCoffeeAddress = coffee.selfAddress;
	} else {
		linkAddresses(database, cafe, coffee);
	}
	fclose(database);
	cafe.coffeeCount++;
    updateCafe(cafe, error);
	return 1;
}

int updateCoffee(struct Coffee coffee, int coffeeId) {
	FILE* database = fopen(COFFEE_DATA, "r+b");
	fseek(database, coffee.selfAddress, SEEK_SET);
	fwrite(&coffee, COFFEE_SIZE, 1, database);
	fclose(database);
	return 1;
}

void deleteCoffee(struct Cafe cafe, struct Coffee coffee, int coffeeId, char* error) {
	FILE* database = fopen(COFFEE_DATA, "r+b");
	struct Coffee previous;
	fseek(database, cafe.firstCoffeeAddress, SEEK_SET);
	do {
		fread(&previous, COFFEE_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}
	while (previous.nextAddress != coffee.selfAddress && coffee.selfAddress != cafe.firstCoffeeAddress);

	relinkAddresses(database, previous, coffee, &cafe);
    noteDeletedCoffee(coffee.selfAddress);
	coffee.exists = 0;

	fseek(database, coffee.selfAddress, SEEK_SET);
	fwrite(&coffee, COFFEE_SIZE, 1, database);
	fclose(database);

	cafe.coffeeCount--;
    updateCafe(cafe, error);
}
