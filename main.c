#include <stdio.h>
#include <stdlib.h>
#define MAX_NAME_LENGTH 4

/*
	Verner Frank
	Finn Storm
	Thorbjorn Toft
	Johan Rohde
	Sofus Eskildsen
	Rasmus Ovesen
	Gregers Aagaard
	Kjeld Truelsen
	Viktor Kjærsgaard
	Frithjof Ovesen
 */

typedef struct class{
	char name[MAX_NAME_LENGTH];
} class;

typedef struct subject{
	char name[MAX_NAME_LENGTH];
} subject;

typedef struct room{
	char name[MAX_NAME_LENGTH];
} room;

typedef struct teacher{
	char name[MAX_NAME_LENGTH];
} teacher;

int main(int argc, char const *argv[]){
	/* VARIABLES BEGIN */
	int randomnumber;
	/* VARIABLES END */

	srand(time(NULL));

    randomnumber = rand() % 10;
    printf("%d\n", randomnumber);
	return 0;
}