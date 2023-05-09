#include <stdio.h>
#include "Structures.h"
#include "Cafe.h"
#include "Coffee.h"
#include "consolestream.h"

int main() {
	struct Cafe cafe;
	struct Coffee coffee;
	while (1) {
		int choice;
		int id;
		char error[51];
		printf("Enter the command:\n");
        printf("0 exit\n");
        printf("1 insert cafe\n");
        printf("2 get cafe\n");
        printf("3 update cafe\n");
        printf("4 delete cafe\n");
        printf("5 insert coffee\n");
        printf("6 get coffee\n");
        printf("7 update coffee\n");
        printf("8 delete coffee\n");
        printf("9 all information\n");
		printf("Enter the number to continue\n");
		scanf("%d", &choice);
		switch (choice) {
		case 0:
			return 0;
		case 1:
			inputCafe(&cafe);
			insertCafe(cafe);
			break;
		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			getCafe(&cafe, id, error) ? printCafe(cafe) : printf("Error: %s\n", error);
			break;
		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);
			cafe.id = id;
			inputCafe(&cafe);
			updateCafe(cafe, error) ? printf("Updated successfully\n") : printf("Error: % s\n", error);
			break;
		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteCafe(id, error) ? printf("Deleted successfully\n") : printf("Error: % s\n", error);
			break;
		case 5:
			printf("Enter cafe ID: ");
			scanf("%d", &id);
			if (getCafe(&cafe, id, error)) {
                    coffee.cafeId = id;
                    id = findNewId();
                    if (checkKeyPairUnique(cafe, id)) {
                        coffee.coffeeId = id;
                        inputCoffee(&coffee);
                        insertCoffee(cafe, coffee, error);
                    }
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 6:
			printf("Enter coffee ID: ");
			scanf("%d", &id);
			cafe = findMaster(id);
			if (getCafe(&cafe, cafe.id, error)) {
				if (getCoffee(cafe, &coffee, id, error)) printCoffee(coffee, cafe);
				else printf("Error: %s\n", error);
			}
			else {
                printf("Error: %s\n", error);
			}
			break;
		case 7:
			printf("Enter coffee ID: ");
			scanf("%d", &id);
			cafe = findMaster(id);
			if (getCafe(&cafe, cafe.id, error)) {
				if (getCoffee(cafe, &coffee, id, error)) {
					inputCoffee(&coffee);
					updateCoffee(coffee, id);
					printf("Updated successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 8:
			printf("Enter coffee ID: ");
			scanf("%d", &id);
			cafe = findMaster(id);
			if (getCafe(&cafe, cafe.id, error)) {
				if (getCoffee(cafe, &coffee, id, error)) {
					deleteCoffee(cafe, coffee, id, error);
					printf("Deleted successfully\n");
				}
				else {
					printf("Error: %s\n", error);
				}
			}
			else {
				printf("Error: %s\n", error);
			}
			break;
		case 9:
			info();
			break;
		default:
			printf("Invalid input\n");
		}
		printf("\n");
	}
}
