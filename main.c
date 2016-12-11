#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_LENGTH   255
#define MAX_YEAR            9

#define MAX_SUBJECTS       20
#define MAX_ROOMS          10
#define MAX_CLASSES        10
#define MAX_TEACHERS       10
#define MAX_TIMETABLES     10
#define MAX_INDUVIDUALS    25

#define SCHOOL_DAYS_YEAR  190
#define WEEK_LENGTH         5
#define MAX_LECTURES       10
#define MUTATION_CHANCE     1

/**
 * ASSUMPTIONS:
 * 	every room has enough space for an entire class
 * 	there are 10 different classyears
 */

int debug = 0;

typedef struct room{
	char name[MAX_NAME_LENGTH];
} room;

typedef struct class{
	int    year;
	char   name[MAX_NAME_LENGTH];
	room*  classRoom;
	int    maxWorkHoursPerDay;
} class;


typedef struct subject{
	char  name[MAX_NAME_LENGTH];
	int   perYear[MAX_YEAR];
	room* roomRequire[MAX_ROOMS];
	int   roomRequireLength;
	int   totalHours;
} subject;


typedef struct teacher{
	char     name[MAX_NAME_LENGTH];
	int      isClassleader;
	class*   leaderOfClass;
	subject* canTeach[MAX_SUBJECTS];
	int      canTeachLength;
	int      maxWorkHoursPerDay;
} teacher;

typedef struct datetime{
	int dayOfWeek;
	int hour;
} datetime;

typedef struct lecture{
	room    *l_room;
	class   *l_class;
	subject *l_subject;
	teacher *l_teacher;
	datetime l_datetime;
} lecture;

typedef struct schoolDay{
	int lectureLength;
	lecture lectures[MAX_LECTURES];
} schoolDay;

typedef struct timetable{
	schoolDay day[WEEK_LENGTH];
} timetable;

typedef struct induvidual{
	timetable t[MAX_CLASSES];
} induvidual;


int randomNumber(int min, int max);
void swapn(void *a, void *b, size_t n);
int factorial(int n);
int generateAllCombinations(void *items, size_t size, int sizeOfVariable, void **finalItems);

#include "fileParse.c"
#include "lectureControl.c"
#include "print.c"

/* GENETIC FUNCTIONS*/
int shouldMutate();

int main(int argc, char const *argv[]){
	/* VARIABLES BEGIN */
	room rooms[MAX_ROOMS];
	/*room b_rooms[MAX_ROOMS];*/
	int roomCount = 0;

	subject subjects[MAX_SUBJECTS];
	/*subject b_subjects[MAX_SUBJECTS];*/
	int subjectCount = 0;

	class classes[MAX_CLASSES];
	/*class b_classes[MAX_CLASSES];*/
	int classCount = 0;

	teacher teachers[MAX_TEACHERS];
	/*teacher b_teachers[MAX_TEACHERS];*/
	int teacherCount = 0;

	/* needs a better structure... 510000 bytes atm.. */
	induvidual induviduals[MAX_INDUVIDUALS];

	int i;
	int c,d,l;
	clock_t start_t, end_t;


	lecture r_lecture;
	/* VARIABLES END */

	srand(time(NULL) * 100);

	if(argc > 1){
		if(strcmp(argv[1],"--debug") == 0){
			debug = 1;
		}
	}

	init(rooms,&roomCount,subjects,&subjectCount,classes,&classCount,teachers,&teacherCount);

    if(argc > 1){
		if(strcmp(argv[1],"--bench") == 0){
		    printf("\n                                  Random lectures:\n"
		    	   "---------------------------------------------------------------------------------\n");
			start_t = clock();

		    for (i = 0; i < 100; i++){
			    r_lecture = randomLecture(rooms,roomCount,subjects,subjectCount,classes,classCount,teachers,teacherCount);

			    if(checkLecture(r_lecture)){
			    	/*accept state*/
					printLecture(r_lecture);
				  }else{
				  	/*reject state*/
				  	printf("Lecture rejected !\n");
				  }
		    }

		    end_t = clock();


		    printf("---------------------------------------------------------------------------------\n");

		    printf("%f\n", (end_t-start_t)/1000.0);
		    printf("%f lectures per seconds\n", 100.0/((end_t-start_t/1000.0)));
		}
	}


    /* Create initial population */
    for (i = 0; i < MAX_INDUVIDUALS; i++){

    	for (c = 0; c < classCount; c++){

    		for (d = 0; d < WEEK_LENGTH; d++){

    			for (l = 0; l < MAX_LECTURES; l++){
					r_lecture = randomLectureForClass(rooms,roomCount,subjects,subjectCount,teachers,teacherCount, &classes[c]);
					/*printf("%-10d%s\n", induviduals[i].t[c].day[d].lectureLength, r_lecture.l_subject->name);*/
					r_lecture.l_datetime.dayOfWeek = d;
					r_lecture.l_datetime.hour = l;
					induviduals[i].t[c].day[d].lectures[induviduals[i].t[c].day[d].lectureLength++] = r_lecture;
    			}
    		}
	    }
    }



	return 0;
}

void killTimetable(timetable *t){

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

	char *allCombinations	= calloc(endSize * size, sizeOfVariable);
	char *tempItems 		= calloc(size, sizeOfVariable);
	*finalItems				= calloc(endSize * size, sizeOfVariable);

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
