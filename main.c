#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#define MAX_NAME_LENGTH   255
#define MAX_LABEL_LENGTH   12
#define MAX_YEAR            9

#define MAX_SUBJECTS       20
#define MAX_ROOMS          10
#define MAX_CLASSES        10
#define MAX_TEACHERS       10
#define MAX_TIMETABLES     10
#define MAX_individuals    25

#define SCHOOL_DAYS_YEAR  190
#define WEEK_LENGTH         5
#define MAX_LECTURES       10
#define MUTATION_CHANCE     1
#define MAX_MUTATIONS       7
#define FREE_LECTURE_CH    30

/**
 * ASSUMPTIONS:
 *  every room has enough space for an entire class
 *  there are 10 different classyears
 */

int debug = 0;

/* Selve rummet */
typedef struct room{
    char name[MAX_NAME_LENGTH]; /* Rummets navn */
} room;

/* Selve klassen */
typedef struct class{
    int    year; /* 1b = 1 */
    char   name[MAX_NAME_LENGTH]; /* klassenavn ex. 1b */
    room*  classRoom; /* pointer til room struct */
    int    maxWorkHoursPerDay; /* Maks antal timer man må gå i skole pr dag ! */
} class;

/* Selve faget */
typedef struct subject{
    char  name[MAX_NAME_LENGTH]; /* navn på faget */
    int   perYear[MAX_YEAR]; /* timer krævet pr år */
    room* roomRequire[MAX_ROOMS]; /* array af pointers til kompatible rum, hvis tomt klasse lokale */
    int   roomRequireLength; /* Antal rum i arrayet oven over */
    int   totalHours; /* løbende akkumuleret timer */
} subject;

/* Selve læren */
typedef struct teacher{
    char     name[MAX_NAME_LENGTH]; /* Lærens navn */
    int      isClassleader; /* boolean klasselærer eller ej */
    class*   leaderOfClass; /* pointer til klassen de er klasselærer for */
    subject* canTeach[MAX_SUBJECTS]; /* array af pointers til kompatible fag */
    int      canTeachLength; /* Antal pointers i canTeach array'et */
    int      maxWorkHoursPerDay; /* Maks antal arbejdstimer pr dag */
} teacher;

/* Timestamp
 * Således at hour=0 => første lektions tidpunkt på dagens altså 08.00-08.45
 * osv....
 * Se dat.sched ## TIMEINTERVALS ## for at se lektions intervalerne
 */
typedef struct datetime{
    int dayOfWeek;
    int hour;
} datetime;

/* Selve lektoren */
typedef struct lecture{
    int     init; /* boolean værdi afgører om den er i et skema eller ej */
    int     free; /* hvorvidt om det er en fri time */
    room    *l_room; /* pointer til det rum selve lektionen foregår i */
    class   *l_class; /* pointer til den pågældende klasse som skal have lektionen */
    subject *l_subject; /* pointer til faget lektionen omhandler */
    teacher *l_teacher; /* pointer til den lærer der skal undervise */
    datetime l_datetime; /* hvilken dag og lektions nummer det skal foregå i */
} lecture;

/* En enekelt dag i skolen */
typedef struct schoolDay{
    int lectureLength; /* antal lektioner på den pågældende dag */
    lecture lectures[MAX_LECTURES]; /* array af lektioner til dagen */
} schoolDay;

/* Selve skemaet for en enkelt klasse */
typedef struct timetable{
    schoolDay day[WEEK_LENGTH]; /* et array af schoolDay structs */
    class *forClass; /* pointer til den pågældene klasse. */
} timetable;

/* Individet (STAVEFEJL ER GRATIS !) */
typedef struct induvidual{
    timetable t[MAX_CLASSES]; /* Et array af klasseskemaer for en hel skole */
    int fitness; /* fitness værdien for individet */
    int conflicts; /* antal konflikter på tværs af skolen i individet. */
} induvidual;


int randomNumber(int min, int max);
void swapn(void *a, void *b, size_t n);
int factorial(int n);
int generateAllCombinations(void *items, size_t size, int sizeOfVariable, void **finalItems);
int isEmpty(int *array, size_t size);
int shouldMutate();

#include "fileParse.c"
#include "lectureControl.c"
#include "print.c"
#include "scheduleControl.c"
#include "genetics.c"

int main(int argc, char const *argv[]){
    /* VARIABLES BEGIN */
    char intervalLabels[MAX_LECTURES][MAX_LABEL_LENGTH]; /* Array af TIMEINTERVALS [se. dat.sched] */
    room rooms[MAX_ROOMS]; /* Array af rum, bliver deklaret men IKKE initieret */
    subject subjects[MAX_SUBJECTS]; /* Array af fag, bliver deklaret men IKKE initieret */
    class classes[MAX_CLASSES]; /* Array af klasser, bliver deklaret men IKKE initieret */
    teacher teachers[MAX_TEACHERS]; /* Array af lærer, bliver deklaret men IKKE initieret */
    induvidual individuals[MAX_individuals]; /* Array af individer (også kendt som populationen), bliver deklaret men IKKE initieret */
	int roomCount = 0, subjectCount = 0, classCount = 0, teacherCount = 0; /* variabler til at tælle antal værdier i de enkelte arrays */
    int i,j; /* iteration counters */
    int seed = time(NULL) * 100; /* Token til at genskabe samme resultater på andre maskiner */
 	/* VARIABLES END */
    srand(seed); /* Generationen af selve token til genbrug */

  	/*
   	 * Tjekker efter debug mode
  	 */
    if(argc > 1){
        if(strcmp(argv[1],"--debug") == 0){
            debug = 1;
        }
    }

    if(debug){
        printf("Seed: %d\n", seed);
    }

    /*
     * Initierer variablerner, ved at parse dat.sched igennem filParse.c funktionerne
     */
    init(rooms,&roomCount,subjects,&subjectCount,classes,&classCount,teachers,&teacherCount,intervalLabels);

    /* Create initial population */
    /* TODO: OMSKRIV DETTE TIL RECURSIVE FUNKTIONS KALD !!!!!! */
    for (i = 0; i < MAX_individuals; i++){
        /* For hvert individ op til maks antal individer */
        individuals[i] = randomIndividual(rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
    }

    qsort(individuals, MAX_individuals, sizeof(induvidual), conflictsQsort);
    printf("First conflicts: %3d\n", individuals[0].conflicts);

    /* Conflicts preview */
    for (j = 0; j < 10000; j++){

        for (i = 0; i < MAX_individuals-2; i+=2){
            crossover(&individuals[i], &individuals[i+1], classCount);
        }

        qsort(individuals, MAX_individuals, sizeof(induvidual), conflictsQsort);

        if(j%100==0){
            printf("Best conflicts: %3d", individuals[0].conflicts);
            for (i = 0; i < 19; i++){
                printf("\b");
            }
        }
    }

    /* Uncomment for demo of schedules */
    for (i = 0; i < classCount; i++){
        printf("\nClass %s, conflicts: %d\n", individuals[0].t[i].forClass->name, individuals[0].conflicts);
        printTimeTable(individuals[0].t[i], intervalLabels);
    }


    /* Conflicts preview */
    /*qsort(individuals, MAX_individuals, sizeof(induvidual), conflictsQsort);
    for (i = 0; i < MAX_individuals; i++){
        printf("Ind: %2d, conflicts: %d\n", i, individuals[i].conflicts);
    }*/
    /* Dump csv files in folder schedules */
    /*dumpCSV(&individuals[0],classCount,intervalLabels);*/

    return 0;
}

void killTimetable(timetable *t){

}

int isEmpty(int *array, size_t size){
    int i;
    int empty = 0;
    for (i = 0; i < size; i++){
        empty += array[i] <= 0;
    }
    return empty == size;
}


int shouldMutate(){
    int randomnumber;
    randomnumber = rand() % 100;

    return randomnumber <= MUTATION_CHANCE;
}

/**
 * Generates a random number
 * @param  min smallest possible number
 * @param  max largest possible number
 * @return     random number between min and max
 */
int randomNumber(int min, int max){
    return (rand() % (max + 1 - min)) + min;
}

/*int generateAllCombinations(void *items, size_t size, int sizeOfVariable, void **finalItems){
    int i;
    int j;
    int k = 0;
    int endSize = factorial(size);

    char *allCombinations   = calloc(endSize * size, sizeOfVariable);
    char *tempItems         = calloc(size, sizeOfVariable);
    *finalItems             = calloc(endSize * size, sizeOfVariable);

    memcpy(tempItems, items, sizeOfVariable * size);

    for (j = 0; j < endSize; j++){
        for (i = 0; i < size-1; i++){
            swapn(
                &(tempItems[i*sizeOfVariable]),
                &(tempItems[(i+1)*sizeOfVariable]),
                sizeOfVariable
            );

            memcpy(
                &(allCombinations[(k++)*size*sizeOfVariable]),
                tempItems,
                sizeOfVariable*size
            );
        }
    }
    for (j = 0; j < size; j++){
        for (i = 0; i < size-1; i++){

        }
    }

    memcpy(
        *finalItems,
        allCombinations,
        endSize*sizeOfVariable
    );

    free(tempItems);
    return endSize*size;
}*/


/**
 * Swaps the values of two variables
 * @param a first var
 * @param b seconds var
 * @param n size of variables
 */
void swapn(void *a, void *b, size_t n) {
    int i;
    char *x = (char *)a,
         *y = (char *)b;

    if (a == b) {
        return;
    }


    for (i = 0; i < n; i++) {
        *x ^= *y;
        *y ^= *x;
        *x ^= *y;
        x++;
        y++;
    }
}

/**
 * Iterative factorial function
 * @param  n what to calculate on
 * @return   returns factorial of n
 */
int factorial(int n){
    int result = 1;
    int i;
    for (i = 2; i <= n; i++)    {
        result *= i;
    }
    return result;
}
