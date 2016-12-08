#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_NAME_LENGTH 255
#define MUTATION_CHANCE 1
#define MAX_SUBJECTS_PER 10
#define WEEK_LENGTH 5
#define MAX_YEAR 10
#define MAX_REQUIRE_ROOM 5

#define MAX_SUBJECTS 20
#define MAX_ROOMS    10
#define MAX_CLASSES  10
#define MAX_TEACHERS 10

/**
 * ASSUMPTIONS:
 * 	every room has enough space for an entire class
 * 	there are 10 different classyears
 */

int debug = 1;

typedef struct room{
	char name[MAX_NAME_LENGTH];
} room;

typedef struct class{
	char   name[MAX_NAME_LENGTH];
	room* classRoom;
	int   maxWorkHoursPerDay;
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
	int day;
	int month;
	int year;
	int hour;
	int minute;
} datetime;

typedef struct lecture{
	room    *l_room;
	class   *l_class;
	subject *l_subject;
	teacher *l_teacher;
	datetime l_datetime;
} lecture;

#include "fileParse.c"


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
	/* VARIABLES END */

	srand(time(NULL));

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
	    	printf("%2d => %15s, roomRequire: %s\n", i+1, subjects[i].name, (subjects[i].roomRequireLength != 0 ? subjects[i].roomRequire[0]->name : "*"));
	    }

	    printf("\nTeachers:\n");
	    for (i = 0; i < teacherCount; i++){
	    	printf("%2d => %15s, isClassleader: %s\n", i+1, teachers[i].name, (teachers[i].isClassleader ? teachers[i].leaderOfClass->name : "no"));
	    }
    }

	/* mutation demo */
	/*for (i = 0; i < 100; ++i){
		printf("%d ",shouldMutate());
	}*/

	return 0;
}


int shouldMutate(){
	int randomnumber;
	randomnumber = rand() % 100;

    return randomnumber <= MUTATION_CHANCE;
}