#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Info.h"
#include "Coffee.h"

#define CAFE_IND "cafe.ind"
#define CAFE_DATA "cafe.fl"
#define CAFE_GARBAGE "cafe_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define CAFE_SIZE sizeof(struct Cafe)

void noteDeletedCafe(int id) {
	FILE* garbageZone = fopen(CAFE_GARBAGE, "rb");
	int garbageAmount;
	fscanf(garbageZone, "%d", &garbageAmount);
	int* deleteIds = (int *)malloc(garbageAmount * sizeof(int));

	for (int i = 0; i < garbageAmount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}

	fclose(garbageZone);
	garbageZone = fopen(CAFE_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageAmount + 1);

	for (int i = 0; i < garbageAmount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	fprintf(garbageZone, " %d", id);
	free(deleteIds);
	fclose(garbageZone);
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Cafe* record) {
	int* deleteIds = (int*)malloc(garbageCount * sizeof(int));
	for (int i = 0; i < garbageCount; i++) {
		fscanf(garbageZone, "%d", deleteIds + i);
	}
	record->id = deleteIds[0];

	fclose(garbageZone);
	fopen(CAFE_GARBAGE, "wb");
	fprintf(garbageZone, "%d", garbageCount - 1);

	for (int i = 1; i < garbageCount; i++) {
		fprintf(garbageZone, " %d", deleteIds[i]);
	}

	free(deleteIds);
	fclose(garbageZone);
}

int insertCafe(struct Cafe record) {
	FILE* indexTable = fopen(CAFE_IND, "a+b");
	FILE* database = fopen(CAFE_DATA, "a+b");
	FILE* garbageZone = fopen(CAFE_GARBAGE, "a+b");
	struct Indexer indexer;
	int garbageCount = 0;
	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount) {
		overwriteGarbageId(garbageCount, garbageZone, &record);
		fclose(indexTable);
		fclose(database);
		indexTable = fopen(CAFE_IND, "r+b");
		database = fopen(CAFE_DATA, "r+b");
		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);
	} else {
		long indexerSize = INDEXER_SIZE;
		fseek(indexTable, 0, SEEK_END);
		if (ftell(indexTable)) {
			fseek(indexTable, -indexerSize, SEEK_END);
			fread(&indexer, INDEXER_SIZE, 1, indexTable);
			record.id = indexer.id + 1;
		} else {
			record.id = 1;
		}
	}
	record.firstCoffeeAddress = -1;
	record.coffeeCount = 0;
	fwrite(&record, CAFE_SIZE, 1, database);

	indexer.id = record.id;
	indexer.address = (record.id - 1) * CAFE_SIZE;
	indexer.exists = 1;
	printf("Cafe id is %d\n", record.id);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
	fclose(database);

	return 1;
}

int getCafe(struct Cafe* cafe, int id, char* error) {
	FILE* indexTable = fopen(CAFE_IND, "rb");
	FILE* database = fopen(CAFE_DATA, "rb");
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	struct Indexer indexer;
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fread(cafe, sizeof(struct Cafe), 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int updateCafe(struct Cafe cafe, char* error) {
	FILE* indexTable = fopen(CAFE_IND, "r+b");
	FILE* database = fopen(CAFE_DATA, "r+b");
    struct Indexer indexer;
    int id = cafe.id;
	if (!checkFileExists(indexTable, database, error)) {
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	if (!checkRecordExists(indexer, error)) {
		return 0;
	}
	fseek(database, indexer.address, SEEK_SET);
	fwrite(&cafe, CAFE_SIZE, 1, database);
	fclose(indexTable);
	fclose(database);
	return 1;
}

int deleteCafe(int id, char* error) {
	FILE* indexTable = fopen(CAFE_IND, "r+b");
    struct Cafe cafe;
    struct Indexer indexer;
    if (indexTable == NULL) {
		strcpy(error, "database files are not created yet");
		return 0;
	}
	if (!checkIndexExists(indexTable, error, id)) {
		return 0;
	}
    getCafe(&cafe, id, error);
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fread(&indexer, INDEXER_SIZE, 1, indexTable);
	indexer.exists = 0;
	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);
	fclose(indexTable);
    noteDeletedCafe(id);

	if (cafe.coffeeCount) {
		FILE* coffeeDb = fopen(COFFEE_DATA, "r+b");
		struct Coffee coffee;
		fseek(coffeeDb, cafe.firstCoffeeAddress, SEEK_SET);
		for (int i = 0; i < cafe.coffeeCount; i++) {
			fread(&coffee, COFFEE_SIZE, 1, coffeeDb);
			fclose(coffeeDb);
            deleteCoffee(cafe, coffee, coffee.coffeeId, error);
            coffeeDb = fopen(COFFEE_DATA, "r+b");
			fseek(coffeeDb, coffee.nextAddress, SEEK_SET);
		}
		fclose(coffeeDb);
	}
	return 1;
}
