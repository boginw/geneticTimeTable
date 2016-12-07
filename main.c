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

/**
 * ASSUMPTIONS:
 * 	every room has enough space for an entire class
 * 	there are 11 different classyears
 */

int debug = 1;

typedef struct room{
	char name[MAX_NAME_LENGTH];

} room;

typedef struct class{
	char name[MAX_NAME_LENGTH];
	int maxWorkHoursPerDay;
} class;


typedef struct subject{
	char name[MAX_NAME_LENGTH];
	int perYear[MAX_YEAR];
	room roomRequire[MAX_REQUIRE_ROOM+1];
	int roomRequireLength;
	int totalHours;
} subject;


typedef struct teacher{
	char name[MAX_NAME_LENGTH];
	int isClassleader;
	char classleaderOfClass[3];
	subject canTeach[MAX_SUBJECTS_PER+1];
	int canTeachLength;
	int maxWorkHoursPerDay;
} teacher;

int shouldMutate();
int init(room *rooms, int *roomCount, subject *subjects, int *subjectCount, class *classes, int *classCount, teacher *teachers, int *teacherCount);
int countLines(FILE *fp);
int subjectsFromString(char *subjectString, subject* subjects);
int roomsFromString(char *roomString, room* rooms);

int main(int argc, char const *argv[]){
	/* VARIABLES BEGIN */
	room rooms[10];
	int roomCount = 0;

	subject subjects[15];
	int subjectCount = 0;
	
	class classes[10];
	int classCount = 0;

	teacher teachers[10];
	int teacherCount = 0;
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

	/* mutation demo */
	/*for (i = 0; i < 100; ++i){
		printf("%d ",shouldMutate());
	}*/

	return 0;
}

int init(room *rooms, int *roomCount, subject *subjects, int *subjectCount, class *classes, int *classCount, teacher *teachers, int *teacherCount){
	FILE *dataFile;
	int i,lines,res;
	
	char buffer[256];
	char subjectBuffer[256];
	char lastType[15];
	
	if(!(dataFile = fopen("dat.sched", "r"))){
        printf("DATAFILE DOES NOT EXIST...\n");
        exit(0);
    }

    lines = countLines(dataFile);

    if(debug)
	    printf("Parsing file:\n");

    for(i = 0; i < lines; i++){
        fgets(buffer, 256, dataFile);

        if(sscanf(buffer,"### %s ###",lastType) != 0){
        	if(debug)
	        	printf("%s\n", lastType);
        }else if(buffer[0] == '#' || buffer[0] == '\n' || strlen(buffer) == 0){
        	/* Found a comment... just ignore */
        	continue;
        }else{
        	if(strcmp(lastType,"TEACHER") == 0){
	        	res = sscanf(buffer," %[^,],%d,%s,%[^,],%d ",
	        		teachers[*teacherCount].name,
	        		&teachers[*teacherCount].isClassleader,
	        		teachers[*teacherCount].classleaderOfClass,
	        		subjectBuffer,
	        		&teachers[*teacherCount].maxWorkHoursPerDay
	        	);

	        	teachers[*teacherCount].canTeachLength = subjectsFromString(subjectBuffer, teachers[*teacherCount].canTeach);
	        	*teacherCount+=1;
        	}else if(strcmp(lastType,"ROOM") == 0){
        		res = sscanf(buffer," %[^\n]",
	        		rooms[*roomCount].name
	        	);

	        	*roomCount +=1;
        	}else if(strcmp(lastType,"SUBJECT") == 0){
        		res = sscanf(buffer," %[^,],%[^,],%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d ",
	        		subjects[*subjectCount].name,
	        		subjectBuffer,
	        		&subjects[*subjectCount].perYear[0],
	        		&subjects[*subjectCount].perYear[1],
	        		&subjects[*subjectCount].perYear[2],
	        		&subjects[*subjectCount].perYear[3],
	        		&subjects[*subjectCount].perYear[4],
	        		&subjects[*subjectCount].perYear[5],
	        		&subjects[*subjectCount].perYear[6],
	        		&subjects[*subjectCount].perYear[7],
	        		&subjects[*subjectCount].perYear[8],
	        		&subjects[*subjectCount].perYear[9],
	        		&subjects[*subjectCount].perYear[10]
	        	);

        		subjects[*subjectCount].roomRequireLength = roomsFromString(subjectBuffer,subjects[*subjectCount].roomRequire);

	        	*subjectCount +=1;
        	}else if(strcmp(lastType,"CLASS") == 0){
        		res = sscanf(buffer," %[^,],%d ",
	        		classes[*classCount].name,
	        		&classes[*classCount].maxWorkHoursPerDay
	        	);

	        	*classCount+=1;
        	}else{
        		printf("Warning: Uknown type on line %i\n", i+1);
        	}
        }
    }

    res = res + 1;


    fclose(dataFile);

    return 1;
}

int shouldMutate(){
	int randomnumber;
	randomnumber = rand() % 100;

    return randomnumber <= MUTATION_CHANCE;
}

/**
 * Counts lines in a file
 * @author    http://stackoverflow.com/q/12733105/1426327
 * @param  fp file pointer
 * @return    returns number of lines in file
 */
int countLines(FILE *fp){
    int n=0;
    char ch;

    if (fp == NULL)
        return 0;

    while((ch = fgetc(fp)) != EOF){
        if(ch == '\n'){
            n++;
        }
    }
    rewind(fp);
    return n+1;
}

int subjectsFromString(char *subjectString, subject* subjects){
	int len = strlen(subjectString);
	int i;
	int curChar = 0;
	int curSubj = 0;
	char tmpName[20];
	subject tmpSub;

	tmpSub.totalHours = 0;

	for (i = 1; i < len; i++){
		if(subjectString[i] != ';' && subjectString[i] != ']'){
			tmpName[curChar++] = subjectString[i];
		}else{
			tmpName[curChar] = '\0';
			curChar = 0;
			strcpy(tmpSub.name, tmpName);
			subjects[curSubj++] = tmpSub;
		}
	}

	return curSubj;
}

int roomsFromString(char *roomString, room* rooms){
	int len = strlen(roomString);
	int i;
	int curChar = 0;
	int curRoom = 0;
	char tmpName[20];
	room tmpRoom;

	for (i = 1; i < len; i++){
		if(roomString[i] != ';' && roomString[i] != ']'){
			tmpName[curChar++] = roomString[i];
		}else{
			tmpName[curChar] = '\0';
			curChar = 0;
			strcpy(tmpRoom.name, tmpName);
			rooms[curRoom++] = tmpRoom;
		}
	}

	return curRoom;
}