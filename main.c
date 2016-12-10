#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define MAX_NAME_LENGTH   255
#define MAX_YEAR           10

#define MAX_SUBJECTS       20
#define MAX_ROOMS          10
#define MAX_CLASSES        10
#define MAX_TEACHERS       10

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

#include "fileParse.c"
#include "lectureControl.c"

int generateAllCombinations(void *items, size_t size, int sizeOfVariable, void **finalItems);
void swapn(void *a, void *b, size_t n);
int factorial(int n);
int randomNumber(int min, int max);
lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void printLecture(lecture l);

/* GENETIC FUNCTIONS*/
int shouldMutate();

int main(int argc, char const *argv[]){
	/* VARIABLES BEGIN */
	room rooms[MAX_ROOMS];
	int roomCount = 0;

	subject subjects[MAX_SUBJECTS];
	int subjectCount = 0;

	class classes[MAX_CLASSES];
	int classCount = 0;

	teacher teachers[MAX_TEACHERS];
	int teacherCount = 0;

	int i;
	int j;

	char *requirements;


	lecture r_lecture;
	/* VARIABLES END */
	requirements = calloc(5 * 20, sizeof(char));

	if(argc > 1){
		if(strcmp(argv[1],"--debug") == 0){
			debug = 1;
		}
	}

	srand(time(NULL) * 100);

	init(rooms,&roomCount,subjects,&subjectCount, classes,&classCount,teachers,&teacherCount);


	 if(debug){
	    printf("\nParsing complete\n"
	    	   "%3d Rooms\n"
	    	   "%3d Subjects\n"
	    	   "%3d Classes\n"
	    	   "%3d Teachers\n",
	    	   roomCount,
	    	   subjectCount,
	    	   classCount,
	    	   teacherCount
	   	);
    }

    if(debug){
	    /* dumping info for demo purposes */
	    printf("\nDUMPING DATA....\n\n");

	    printf("Rooms:\n");
	    for (i = 0; i < roomCount; i++){
	    	printf("%d => %s\n", i+1, rooms[i].name);
	    }

	    printf("\nClasses:\n");
	    for (i = 0; i < classCount; i++){
	    	printf("%d => %s, Classroom: %s\n", i+1, classes[i].name, classes[i].classRoom->name);
	    }

	    printf("\nSubjects:\n");
	    for (i = 0; i < subjectCount; i++){
	    	printf("%2d => %17s, roomRequire: %s\n", i+1, subjects[i].name, (subjects[i].roomRequireLength != 0 ? subjects[i].roomRequire[0]->name : "*"));
	    }

	    printf("\nTeachers:\n");
	    for (i = 0; i < teacherCount; i++){
			strcpy(requirements, "");
			for (j = 0; j < teachers[i].canTeachLength; j++){
				if(j!=0)
					strcat(requirements,", ");

				strcat(requirements,teachers[i].canTeach[j]->name);
			}

	    	printf("%2d => %15s, isClassleader: %s, %s\n", i+1, teachers[i].name, (teachers[i].isClassleader ? teachers[i].leaderOfClass->name : "no"), requirements);
	    }
    }

	/* mutation demo */
	/*for (i = 0; i < 100; ++i){
		printf("%d ",shouldMutate());
	}*/


    printf("\n                                  Random lectures:\n"
    	   "---------------------------------------------------------------------------------\n");
    for (i = 0; i < 200; i++){
	    r_lecture = randomLecture(rooms,roomCount,subjects,subjectCount, classes,classCount,teachers,teacherCount);

	    if(checkLecture(r_lecture)){
	    	/*accept state*/
	    	printLecture(r_lecture);
		  }else{
		  	/*reject state*/
		  	printf("Lecture rejected !\n");
		  }
    }

    printf("---------------------------------------------------------------------------------\n");


    free(requirements);
	return 0;
}


/**
 * Prints core elements of a lecture
 * @param l lecture to print
 */
void printLecture(lecture l){
	char *requirements = calloc((l.l_subject->roomRequireLength) * 7, sizeof(char));
	int i;
	for (i = 0; i < l.l_subject->roomRequireLength; i++){
		if(i!=0)
			strcat(requirements,", ");
		strcat(requirements,l.l_subject->roomRequire[i]->name);
	}

	printf("| %-7s | %-17s | %-25s | %3s | %-14s |\n",
		l.l_room->name,
		l.l_subject->name,
		l.l_teacher->name,
		l.l_class->name,
		(l.l_subject->roomRequireLength != 0 ? requirements : "*")
	);

	free(requirements);
}


lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	int i;
	int year;
	int shouldBreak = 0;
	lecture r_lecture;

	r_lecture.l_class 	= &classes[randomNumber(0,classCount-1)];
	sscanf(r_lecture.l_class->name,"%d%*c",&year);

	while((r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)]) && r_lecture.l_subject->perYear[year] == 0){
		r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)];
	}

	if(r_lecture.l_subject->roomRequireLength > 0){
		r_lecture.l_room = r_lecture.l_subject->roomRequire[randomNumber(0,r_lecture.l_subject->roomRequireLength-1)];
	}else{
		r_lecture.l_room = &rooms[randomNumber(0,roomCount-1)];
	}

	while((r_lecture.l_teacher = &teachers[randomNumber(1,teacherCount)-1])){
		/*printf("%-17s %s\n", r_lecture.l_subject->name, r_lecture.l_teacher->name);*/
		for(i = 0; i < r_lecture.l_teacher->canTeachLength; i++){
			if(strcmp(r_lecture.l_teacher->canTeach[i]->name, r_lecture.l_subject->name) == 0){
				shouldBreak = 1;
				break;
			}
		}

		/*sleep(1);*/
		if(shouldBreak){
			shouldBreak = 0;
			break;
		}
	}

	return r_lecture;
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
