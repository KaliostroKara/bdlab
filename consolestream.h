#pragma once
#include <string.h>
#include <stdio.h>
#include "Cafe.h"
#include "Structures.h"

void inputCafe(struct Cafe* cafe) {
    int price;
    float rating;
	while (getchar()!='\n'){}
	printf("Enter cafe\'s name: ");
	gets(cafe->name);
	printf("Enter cafe\'s address: ");
	gets(cafe->address);
	printf("Enter link to cafe\'s website: ");
	gets(cafe->website);
	printf("Enter cafe\'s description: ");
	gets(cafe->description);
	printf("Enter price score of the cafe (integer from 1 to 5): ");
	scanf("%d", &price);
	cafe->price = price;
	printf("Enter rating of the cafe (float from 1 to 5): ");
	scanf("%f", &rating);
	cafe->rating = rating;
}

void printCafe(struct Cafe cafe) {
	printf("Cafe\'s name: %s\n", cafe.name);
	printf("Cafe\'s address: %s\n", cafe.address);
	printf("Link to cafe\'s website: %s\n", cafe.website);
	printf("Cafe\'s description: %s\n", cafe.description);
	printf("Cafe\'s price: %d\n", cafe.price);
	printf("Cafe\'s rating: %.1f\n", cafe.rating);
}

void inputCoffee(struct Coffee* coffee) {
    char name[32];
    float price;
    char description[256];
    int isMain;
    int cafeId;
    int categoryId;
    while (getchar()!='\n'){}
	printf("Enter coffee name: ");
	gets(coffee->name);
	printf("Enter coffee price: ");
	scanf("%f", &price);
	coffee->price = price;
	printf("This coffee is the main product? (1 - Yes / 0 - No): ");
	scanf("%d", &isMain);
	coffee->isMain = isMain;
	printf("Enter coffee category ID: ");
	scanf("%d", &categoryId);
	coffee->categoryId = categoryId;
	while (getchar()!='\n'){}
	printf("Enter coffee description: ");
	gets(coffee->description);
}

void printCoffee(struct Coffee coffee, struct Cafe cafe) {
	printf("Cafe name: %s\n", cafe.name);
	printf("Coffee name: %s\n", coffee.name);
	printf("Coffee ID: %d\n", coffee.coffeeId);
	printf("Coffee category ID: %d\n", coffee.categoryId);
	printf("Coffee price: %.2f\n", coffee.price);
	printf("Is the main product (1 - Yes / 0 - No): %d\n", coffee.isMain);
	printf("Coffee description: %s\n", coffee.description);
}
