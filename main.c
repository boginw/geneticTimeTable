#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>

#define MAX_NAME_LENGTH   255
#define MAX_LABEL_LENGTH   12
#define MAX_YEAR           10

#define MAX_SUBJECTS       20
#define MAX_ROOMS          10
#define MAX_CLASSES        10
#define MAX_TEACHERS       20
#define MAX_TIMETABLES     10
#define MAX_INDIVIDUALS    35

#define SCHOOL_DAYS_YEAR  200
#define WEEK_LENGTH         5
#define MINUTES_IN_HOUR    60
#define MAX_LECTURES       45
#define MUTATION_CHANCE     5
#define MAX_MUTATIONS      10
#define NUM_OF_GEN       8000 /* Max amount of generations to run for */
#define KILL_SHIT_GEN    4000


#define FITNESS_FOR_CONFLICTS                600
#define FITNESS_FOR_NULL_HOURS               110
#define FITNESS_FOR_PREPARATION_TIME         20
#define FITNESS_FOR_CLASS_MIN_HOURS          10
#define FITNESS_FOR_TEACHERHOURS             20

/**
 * ASSUMPTIONS:
 *  every room has enough space for an entire class
 *  there are 10 different classyears
 */

int debug = 0;
int conflictOnly = 0;

typedef struct room{
    char name[MAX_NAME_LENGTH];
} room;

typedef struct class{
    int    year; /* 1B = 1 */
    char   name[MAX_NAME_LENGTH];
    room*  classRoom;
    int    maxWorkHoursPerDay;
} class;

typedef struct subject{
	int   id;
    char  name[MAX_NAME_LENGTH];
    int   perYear[MAX_YEAR];
    room* roomRequire[MAX_ROOMS];
    int   roomRequireLength;
    int   totalHours;
} subject;

typedef struct teacher{
	int      id;
    char     name[MAX_NAME_LENGTH];
    int      isClassleader; 
    class*   leaderOfClass; 
    subject* canTeach[MAX_SUBJECTS]; 
    int      canTeachLength; 
    int      maxWorkHours;
} teacher;

typedef struct datetime{
    int dayOfWeek;
    int hour;
} datetime;

typedef struct lecture{
    int     init; 
    int     free; 
    int     conflictRoom;
    int     conflictTeacher;
    room    *l_room; 
    class   *l_class; 
    subject *l_subject; 
    teacher *l_teacher; 
    datetime l_datetime; 
} lecture;

typedef struct timetable{
    lecture lectures[MAX_LECTURES]; 
    int numOfConflicts;
    int lectureLength; 
    class *forClass;
} timetable;

typedef struct individual{
    timetable t[MAX_CLASSES]; 
    int fitness; 
    int conflicts;
    int mutations;
    int nullHours;
} individual;

typedef struct params{
    room       *rooms; 
    subject    *subjects; 
    class      *classes; 
    teacher    *teachers;
    individual *individuals;
    individual *childrens;
    individual *tempPopulation;

    int roomCount;
    int subjectCount;
    int classCount;
    int teacherCount;
    int individualsCount;
    int childrensCount;
    int tempPopulationCount;
    long int accFitnessPoints;
    long int accConflicts;
    long int nullHoursAcc;
    int biggestConflicts;
    int biggestNullHours;

    char intervalLabels[MAX_LECTURES][MAX_LABEL_LENGTH];
} params;

int randomNumber(int min, int max);
int isEmpty(int *array, size_t size);
void strip(char *s);
void prepend(char* s, const char* t);

#include "headers.c"
#include "fileParse.c"
#include "lectureControl.c"
#include "scheduleControl.c"
#include "print.c"
#include "genetics.c"
#include "tests.c"

int main(int argc, char const *argv[]){
    /* VARIABLES BEGIN */
    params populationParams;
    int i,j; /* iteration counters */
    int seed = time(NULL) * 100;

    char progressLine[50] = ">";
    int curProg = 1;

    int *roulette;


    int lastBestGen = 0;
    individual bestIndividual;
    int highestFitnessScore = 0;

    /* Stats */
    int startConflict;
    int startFitness;
    int killingCount = 0;


    populationParams.rooms          = calloc(MAX_ROOMS,           sizeof(room));
    populationParams.subjects       = calloc(MAX_SUBJECTS,        sizeof(subject));
    populationParams.classes        = calloc(MAX_CLASSES,         sizeof(class));
    populationParams.teachers       = calloc(MAX_TEACHERS,        sizeof(teacher));
    populationParams.individuals    = calloc(MAX_INDIVIDUALS,     sizeof(individual));
    populationParams.childrens      = calloc(MAX_INDIVIDUALS,     sizeof(individual));
    populationParams.tempPopulation = calloc(MAX_INDIVIDUALS * 2, sizeof(individual));

    populationParams.roomCount              = 0;
    populationParams.subjectCount           = 0;
    populationParams.classCount             = 0;
    populationParams.teacherCount           = 0;
    populationParams.individualsCount       = MAX_INDIVIDUALS;
    populationParams.childrensCount         = 0;
    populationParams.tempPopulationCount    = 0;
    populationParams.accFitnessPoints       = 0;
    populationParams.accConflicts           = 0;
    populationParams.biggestConflicts       = 0;

    roulette = calloc(100, sizeof(int));
    if(populationParams.rooms == NULL || populationParams.subjects == NULL ||
            populationParams.classes == NULL || populationParams.teachers == NULL ||
            populationParams.individuals == NULL){
        printf("Not enough ram, sorry...\n");
        exit(EXIT_FAILURE);
    }

    /* VARIABLES END */
    srand(seed);


    /*
     * Checks for debug mode
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
     * Initializing variables by parsing dat.sched through fileParse.c functions
     */

    init(&populationParams);


    if(argc > 2 && strcmp(argv[2],"--tests") == 0){
    	printf("\nRunning tests\n------------------------------\n");
    	printf("conflicts:              %s\n", testConflicts(&populationParams) ? "Passed" : "Failed");
    	printf("randomNumber:           %s\n", testRandomNumber()               ? "Passed" : "Failed");
    	printf("isEmpty:                %s\n", testIsEmpty()                    ? "Passed" : "Failed");
    	exit(0);
    }


    generateInitialPopulation(&populationParams);
    qsort(populationParams.individuals, MAX_INDIVIDUALS, sizeof(individual), fitnessQsort);
    printf("First conflicts: %3d\n", populationParams.individuals[0].conflicts);
    /* Conflicts preview */
    startConflict = populationParams.individuals[0].conflicts;
    startFitness  = populationParams.individuals[0].fitness;

    for (j = 0; j < NUM_OF_GEN; j++){
        crossoverPopulation(&populationParams);
        mergePopulation(&populationParams);
        mutatePopulation(&populationParams);
        calcFitnessOnPopulation(&populationParams);
        selection(&populationParams);

        /* If no progress in x generations, generate new random individual */
        if(lastBestGen + KILL_SHIT_GEN < j){
			printf("#%d: Killing worst individuals\n",++killingCount);
            for (i = 0 ; i < MAX_INDIVIDUALS - 5; i++){
                populationParams.individuals[MAX_INDIVIDUALS - 1 - i] = randomIndividual(&populationParams);
            }

            lastBestGen = j;
        }

        if(populationParams.individuals[0].fitness > highestFitnessScore){
            highestFitnessScore = populationParams.individuals[0].fitness;
            bestIndividual      = populationParams.individuals[0];
            lastBestGen         = j;
        }

        if(j % 50 == 0){
            if(curProg*2 < (int) ((((float) j) / NUM_OF_GEN) * 100) ){
                prepend(progressLine, "=");
                curProg++;
            }

            printf("%3d%% [%-50s] confl: %3d | fit: %7d | gen: %6d/%-6d",
                (int) ((((float) j) / NUM_OF_GEN) * 100),
                progressLine,
                populationParams.individuals[0].conflicts,
                populationParams.individuals[0].fitness,
                j,
                NUM_OF_GEN
            );

            for (i = 0; i < 105; i++){
                printf("\b");
            }
        }
    }

    for (i = 0; i < populationParams.classCount; i++){
        printf("\nClass %s, conflicts: %d\n", bestIndividual.t[i].forClass->name, bestIndividual.t[i].numOfConflicts);
        printTimeTable(bestIndividual.t[i], populationParams.intervalLabels);
    }

    printf("\nResults:\n"
    	   "-------------------------\n"
    	   "Generations: %11d\n"
    	   "Start fitness: %9d\n"
    	   "Start conflicts: %7d\n"
    	   "Start null hours: %6d\n"
    	   "-------------------------\n"
    	   "Final fitness: %9d\n"
    	   "Final conflicts: %7d\n"
    	   "Final null hours: %6d\n",
    	   j,
    	   startFitness,
    	   startConflict,
    	   startConflict,
    	   bestIndividual.fitness,
    	   bestIndividual.conflicts,
    	   bestIndividual.nullHours
    );

    free(populationParams.rooms);
    free(populationParams.subjects);
    free(populationParams.classes);
    free(populationParams.teachers);
    free(populationParams.individuals);
    free(populationParams.childrens);
    free(populationParams.tempPopulation);
    free(roulette);
    return 0;
}

void selection(params *populationParams){
    int i, *roulette, rouletteCount=0, p, rouletteSelector;
    int prop;
    roulette = calloc(100, sizeof(int));

    for(i = 0; i < populationParams->tempPopulationCount; i++){
        if(populationParams->accFitnessPoints < 1){
            populationParams->accFitnessPoints = 1;
        }

        prop = (((float)populationParams->tempPopulation[i].fitness) / (populationParams->accFitnessPoints)) * 100;

        if(prop > 0){
            for(p = rouletteCount; p < rouletteCount + prop && p < 100; p++){
                roulette[p] = i;
            }

        	rouletteCount += prop;
        }
        
    }

    for(i = 0; i < MAX_INDIVIDUALS; i++){
        rouletteSelector = roulette[randomNumber(0,rouletteCount - 1)];
        populationParams->individuals[i] = populationParams->tempPopulation[rouletteSelector];
    }
    memset(populationParams->tempPopulation, '\0', (populationParams->tempPopulationCount * sizeof(individual)));
    populationParams->tempPopulationCount = 0;
    populationParams->accFitnessPoints    = 0;
    qsort(populationParams->individuals, populationParams->individualsCount, sizeof(individual), fitnessQsort);
    free(roulette);
}



void calcFitnessOnPopulation(params *populationParams){
    int i;
    populationParams->accConflicts     = 0;
    populationParams->accFitnessPoints = 0;
    populationParams->biggestConflicts = 0;
    populationParams->nullHoursAcc     = 0;

    for(i = 0; i < populationParams->tempPopulationCount; i++){

		fitnessConflictsData(&populationParams->tempPopulation[i], populationParams);
        populationParams->accConflicts     += populationParams->tempPopulation[i].conflicts;
    	populationParams->accFitnessPoints += populationParams->tempPopulation[i].fitness;
    	populationParams->nullHoursAcc     += populationParams->tempPopulation[i].nullHours;


        if(populationParams->tempPopulation[i].conflicts > populationParams->biggestConflicts){
            populationParams->biggestConflicts = populationParams->tempPopulation[i].conflicts;
        }

        if(populationParams->tempPopulation[i].nullHours > populationParams->biggestNullHours){
            populationParams->biggestNullHours = populationParams->tempPopulation[i].nullHours;
        }
    }
    setFitness(populationParams);
}

void mutatePopulation(params *populationParams){
    int i;
    for(i = 0; i < populationParams->tempPopulationCount; i++){
        if(shouldMutate()){
            mutate(&populationParams->tempPopulation[i], populationParams);
        }
    }
}

void mergePopulation(params *populationParams){
    int i,j=0;
    for(i=0; i < populationParams->individualsCount; i++){
        populationParams->tempPopulation[j] = populationParams->individuals[i];
        j++;
        populationParams->tempPopulation[j] = populationParams->childrens[i];
        j++;
    }
    populationParams->tempPopulationCount = j;
    memset(populationParams->childrens, '\0', (populationParams->childrensCount * sizeof(individual)));
    populationParams->childrensCount = 0;
}

void crossoverPopulation(params *populationParams){
   int i, countChildren = 0;
    for(i = 0; i < MAX_INDIVIDUALS - 1; i++){
        crossover(
            &populationParams->childrens[i],
            &populationParams->childrens[i + 1],
            &populationParams->individuals[i],
            &populationParams->individuals[i + 1],
            populationParams
        );
        countChildren++;
    }
    populationParams->childrensCount = countChildren;
}

int generateInitialPopulation(params *populationParams){
    int i, conflictsSum=0;
    /* Create initial population */
    for (i = 0; i < MAX_INDIVIDUALS; i++){
        populationParams->individuals[i] = randomIndividual(populationParams);
        conflictsSum += populationParams->individuals[i].conflicts;
    }
    return conflictsSum;
}







int isEmpty(int *array, size_t size){
    int i;
    int empty = 0;
    for (i = 0; i < size; i++){
        empty += array[i] <= 0;
    }
    return empty == size;
}

/**
 * Generates a random number
 * @param  min smallest possible number
 * @param  max largest possible number
 * @return     random number between min and max
 */
int randomNumber(int min, int max){
    int i =(rand() % (max + 1 - min)) + min;
    return i;
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

void strip(char *s) {
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' && *s != '\n' && *s != '\r') {
            *p2++ = *s++;
        } else {
            ++s;
        }
    }
    *p2 = '\0';
}
