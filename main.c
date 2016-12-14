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
#define MAX_INDIVIDUALS    35

#define SCHOOL_DAYS_YEAR  200
#define WEEK_LENGTH         5
#define MAX_LECTURES       50
#define MUTATION_CHANCE     1
#define MAX_MUTATIONS       7
#define FREE_LECTURE_CH    30
#define NUM_OF_GEN      30000

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

/* Selve skemaet for en enkelt klasse */
typedef struct timetable{
    lecture lectures[MAX_LECTURES]; /* array af lektioner til dagen */
    int lectureLength; /* antal lektioner på den pågældende dag */
    class *forClass; /* pointer til den pågældene klasse. */
} timetable;

/* Individet (STAVEFEJL ER GRATIS !) */
typedef struct individual{
    timetable t[MAX_CLASSES]; /* Et array af klasseskemaer for en hel skole */
    int fitness; /* fitness værdien for individet */
    int conflicts; /* antal konflikter på tværs af skolen i individet. */
    int mutations; /* antal mutationer foretaget */
} individual;

typedef struct params{
    room       *rooms; /* Array af rum, bliver deklaret men IKKE initieret */
    subject    *subjects; /* Array af fag, bliver deklaret men IKKE initieret */
    class      *classes; /* Array af klasser, bliver deklaret men IKKE initieret */
    teacher    *teachers; /* Array af lærer, bliver deklaret men IKKE initieret */
    individual *individuals; /* Array af individer (også kendt som populationen), bliver deklaret men IKKE initieret */
    individual *childrens; /* Array af individer (også kendt som populationen), bliver deklaret men IKKE initieret */
    individual *tempPopulation; /* Array af individer (også kendt som populationen), bliver deklaret men IKKE initieret */

    int roomCount;
    int subjectCount;
    int classCount;
    int teacherCount;
    int individualsCount;
    int childrensCount;
    int tempPopulationCount;

    char intervalLabels[MAX_LECTURES][MAX_LABEL_LENGTH];
} params;

int randomNumber(int min, int max);
void swapn(void *a, void *b, size_t n);
int factorial(int n);
int generateAllCombinations(void *items, size_t size, int sizeOfVariable, void **finalItems);
int isEmpty(int *array, size_t size);
int shouldMutate();
void prepend(char* s, const char* t);

#include "headers.c"
#include "fileParse.c"
#include "lectureControl.c"
#include "scheduleControl.c"
#include "print.c"
#include "genetics.c"

int main(int argc, char const *argv[]){
    /* VARIABLES BEGIN */
    params populationParams;
    int i,j,r; /* iteration counters */
    int seed = time(NULL) * 100; /* Token til at genskabe samme resultater på andre maskiner */
    int lowestConflict = -1, highestConflict = 0, startlowconflict, starthighconflict;
    char progressLine[50] = ">";
    int curProg = 1;

    int *roulette;
    int conflictsSum = 0;
    int currentRoulette = 0;
    int rouletteRatio = 0;
    int maxConflicts = 0;
    int akk = 0;

    int lastBestGen = 0;

    individual lowestIndividual;

    populationParams.rooms       = calloc(MAX_ROOMS,       sizeof(room));
    populationParams.subjects    = calloc(MAX_SUBJECTS,    sizeof(subject));
    populationParams.classes     = calloc(MAX_CLASSES,     sizeof(class));
    populationParams.teachers    = calloc(MAX_TEACHERS,    sizeof(teacher));
    populationParams.individuals = calloc(MAX_INDIVIDUALS, sizeof(individual));
    populationParams.childrens = calloc(MAX_INDIVIDUALS, sizeof(individual));
    populationParams.tempPopulation = calloc(MAX_INDIVIDUALS, sizeof(individual));

    populationParams.roomCount              = 0;
    populationParams.subjectCount           = 0;
    populationParams.classCount             = 0;
    populationParams.teacherCount           = 0;
    populationParams.individualsCount       = 0;
    populationParams.childrensCount         = 0;
    populationParams.tempPopulationCount    = 0;

    roulette = calloc(100, sizeof(int));

    if(populationParams.rooms == NULL || populationParams.subjects == NULL ||
            populationParams.classes == NULL || populationParams.teachers == NULL ||
            populationParams.individuals == NULL){
        printf("Not enough ram, sorry...\n");
        exit(EXIT_FAILURE);
    }
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

    init(&populationParams);
    conflictsSum = generateInitialPopulation(&populationParams);
    qsort(populationParams.individuals, MAX_INDIVIDUALS, sizeof(individual), conflictsQsort);
    printf("First conflicts: %3d\n", populationParams.individuals[0].conflicts);
    /* Conflicts preview */
    starthighconflict = populationParams.individuals[MAX_INDIVIDUALS-1].conflicts;
    startlowconflict  = populationParams.individuals[0].conflicts;

    for (j = 0; j < NUM_OF_GEN; j++){

        crossoverPopulation(&populationParams);
        
        /*
        tempPOP = mergePopulation(individuals, childrens);
        mutatePopulation(tempPOP);
        calcFitnessOnPopulation(tempPOP);
        individuals = selectionOnPopulation(tempPOP);*/

        if(lastBestGen + 2000 < j){
            printf("new \n");
            for (i = 0 ; i < MAX_INDIVIDUALS-5; i++){
                populationParams.individuals[MAX_INDIVIDUALS-1-i] = randomIndividual(&populationParams);
            }
            lastBestGen = j;
        }
        /* Replace shit populationParams.individuals */
        for (i = MAX_INDIVIDUALS - 1; i > MAX_INDIVIDUALS/1.5; i--){
            populationParams.individuals[i] = randomIndividual(&populationParams);
        }

        /*qsort(populationParams.individuals, MAX_INDIVIDUALS, sizeof(individual), conflictsQsort);
        for (i = 1; i < MAX_INDIVIDUALS - 1; i++){
            if((populationParams.individuals[0].conflicts - populationParams.individuals[i].conflicts) / (float) populationParams.individuals[0].conflicts * 100 > 40){
                populationParams.individuals[i] = randomIndividual(rooms, roomCount, subjects, subjectCount, classes, classCount, teacFrs, teacherCount);
            }
        }*/

        qsort(populationParams.individuals, MAX_INDIVIDUALS, sizeof(individual), conflictsQsort);



        /* Selection */
        maxConflicts = populationParams.individuals[MAX_INDIVIDUALS - 1].conflicts;
        for (i = 0; i < MAX_INDIVIDUALS; i++){
            akk += (((maxConflicts - populationParams.individuals[i].conflicts) / (float) maxConflicts)) * 100;
        }

        for (i = 0; i < MAX_INDIVIDUALS; i++){

            rouletteRatio = (((maxConflicts - populationParams.individuals[i].conflicts) / (float) maxConflicts)) * 100;

            rouletteRatio = rouletteRatio / (float) akk * 100;

            for (r = 0; r < rouletteRatio; r++){
                roulette[currentRoulette] = i;
                currentRoulette++;
            }
        }



        /* Sort for lowest conflicts */
        qsort(populationParams.individuals, MAX_INDIVIDUALS, sizeof(individual), conflictsQsort);


        if(populationParams.individuals[0].conflicts < lowestConflict || lowestConflict == -1){
            lowestConflict = populationParams.individuals[0].conflicts;
            lowestIndividual = populationParams.individuals[0];
            lastBestGen = j;
        }

        if(populationParams.individuals[MAX_INDIVIDUALS-1].conflicts > highestConflict){
            highestConflict = populationParams.individuals[MAX_INDIVIDUALS-1].conflicts;
        }

        if(j % 20 == 0){
            if(curProg*2 < (int) ((((float) j) / NUM_OF_GEN) * 100) ){
                prepend(progressLine, "=");
                curProg++;
            }

            /*printTimeTable(populationParams.individuals[0].t[0], intervalLabels);*/
            printf("%3d%% [%-50s] conflicts: %3d | lowest: %3d | generation: %6d/%-6d",
                (int) ((((float) j) / NUM_OF_GEN) * 100),
                progressLine,
                populationParams.individuals[0].conflicts,
                lowestConflict,
                j,
                NUM_OF_GEN
            );



            for (i = 0; i < 6000; i++){
                printf("\b");
            }

        }

        akk = 0;
        currentRoulette = 0;
    }

    /* Uncomment for demo of schedules */
    for (i = 0; i < populationParams.classCount; i++){
        printf("\nClass %s, conflicts: %d\n", lowestIndividual.t[i].forClass->name, lowestIndividual.conflicts);
        printTimeTable(lowestIndividual.t[i], populationParams.intervalLabels);
    }


    /* Conflicts preview */
    /*qsort(populationParams.individuals, MAX_INDIVIDUALSs, sizeof(populationParams.individuals), conflictsQsort);
    for (i = 0; i < MAX_INDIVIDUALSs; i++){
        printf("Ind: %2d, conflicts: %d\n", i, populationParams.individuals[i].conflicts);
    }*/
    /* Dump csv files in folder schedules */
    /*dumpCSV(&populationParams.individuals[0],classCount,intervalLabels);*/
    printf("Start High: %d\nStart Low: %d\nLowest: %d\nHighest: %d", starthighconflict, startlowconflict, lowestConflict, highestConflict);
    free(populationParams.rooms);
    free(populationParams.subjects);
    free(populationParams.classes);
    free(populationParams.teachers);
    free(populationParams.individuals);
    free(roulette);
    return 0;
}

void crossoverPopulation(params *populationParams){
   int i;
    for(i = 0; i < MAX_INDIVIDUALS-2; i+=2){
        crossover(
                    &populationParams->childrens[populationParams->childrensCount+1],
                    &populationParams->childrens[populationParams->childrensCount+2],
                    &populationParams->individuals[i],
                    &populationParams->individuals[i+1],
                    populationParams->classCount
                );
        populationParams->childrensCount += 2;
        
        printf("%d\n", populationParams->childrensCount);
    }
}

int generateInitialPopulation(params *populationParams){
    int i, conflictsSum=0;
    /* Create initial population */
    for (i = 0; i < MAX_INDIVIDUALS; i++){
        /* For hvert individ op til maks antal individer */
        populationParams->individuals[i] = randomIndividual(populationParams);
        conflictsSum += populationParams->individuals[i].conflicts;
    }
    return conflictsSum;
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

    return randomnumber < MUTATION_CHANCE;
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

void prepend(char* s, const char* t){
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
}
