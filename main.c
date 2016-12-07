#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_NAME_LENGTH 255
#define MUTATION_CHANCE 1
#define MAX_SUBJECTS_PER 10
#define WEEK_LENGTH 5


typedef struct room{
	char name[MAX_NAME_LENGTH];
} room;

typedef struct class{
	char name[MAX_NAME_LENGTH];
	int maxWorkHoursPerDay;
} class;


typedef struct subject{
	char name[MAX_NAME_LENGTH];
	int totalHours;
} subject;


typedef struct teacher{
	char name[MAX_NAME_LENGTH];
	int isClassleader;
	int classleaderOfClass;
	subject canTeach[MAX_SUBJECTS_PER];
	int canTeachLength;
	int maxWorkHoursPerDay;
} teacher;

int shouldMutate();
int init();
int countLines(FILE *fp);
int subjectsFromString(char *subjectString, subject* subjects);

int main(int argc, char const *argv[]){
	/* VARIABLES BEGIN */
	int i;
	/* VARIABLES END */

	srand(time(NULL));

	init();

	/* mutation demo */
	/*for (i = 0; i < 100; ++i){
		printf("%d ",shouldMutate());
	}*/

	return 0;
}

int init(){
	FILE *dataFile;
	int i,lines,res;
	
	char buffer[256];
	char subjectBuffer[256];
	char lastType[15];
	
	teacher teachers[10];
	int teacherCount = 0;
	
	if(!(dataFile = fopen("dat.csv", "r"))){
        printf("DATAFILE DOES NOT EXIST...\n");
        exit(0);
    }

    lines = countLines(dataFile);

    for(i = 0; i < lines; i++){
        fgets(buffer, 256, dataFile);

        if(sscanf(buffer," ### %s ### ",lastType) != 0){
        	printf("%s\n", lastType);
        }else if(buffer[0] == '#'){
        	printf("comment\n");
        	/* Found a comment... just ignore */
        }else{
        	if(strcmp(lastType,"TEACHER") == 0){
	        	res = sscanf(buffer," %[^,],%d,%d,%[^,],%d ",
	        		teachers[teacherCount].name,
	        		&teachers[teacherCount].isClassleader,
	        		&teachers[teacherCount].classleaderOfClass,
	        		subjectBuffer,
	        		&teachers[teacherCount].maxWorkHoursPerDay
	        	);

	        	teachers[teacherCount].canTeachLength = subjectsFromString(subjectBuffer, teachers[teacherCount].canTeach);
	        	teacherCount++;
        	}else{
        		printf("Warning: Uknown type on line %i\n", i+1);
        	}
        }

    }


    fclose(dataFile);
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



	return 1;
}