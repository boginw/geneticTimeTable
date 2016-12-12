int init(room *rooms, int *roomCount, subject *subjects, int *subjectCount, class *classes, int *classCount, teacher *teachers, int *teacherCount, char (*labels)[MAX_LABEL_LENGTH]);
int countLines(FILE *fp);

/* PARSE FUNCTIONS */
room parseRoom(char *line);
subject parseSubject(char *line, room* rooms, int roomCount);

/* FIND FUNCTIONS */
int findRoomsFromString(char *roomString, room* roomsFound[MAX_ROOMS], room* rooms, int roomCount);
int findSubjectsFromString(char *subjectString, subject* subjectsFound[MAX_SUBJECTS], subject* subjects, int subjectCount);
room *findRoom(char *roomName, room rooms[MAX_ROOMS], int roomCount);
class *findClass(char *className, class classes[MAX_CLASSES], int classCount);


int init(room *rooms, int *roomCount, subject *subjects, int *subjectCount, class *classes, int *classCount, teacher *teachers, int *teacherCount, char (*labels)[MAX_LABEL_LENGTH]){
    FILE *dataFile;
    int i,j,lines,res;
    int labelCounter = 0;
    char buffer[200];
    char inlineBuffer[200];
    char shortBuffer[20];
    char lastType[15];

    char *requirements;

    requirements = calloc(5 * 20, sizeof(char));

    if(!(dataFile = fopen("dat.sched", "r"))){
        printf("DATAFILE DOES NOT EXIST...\n");
        exit(0);
    }

    lines = countLines(dataFile);

    if(debug)
        printf("Parsing file:\n");

    for(i = 0; i < lines; i++){
        fgets(buffer, 200, dataFile);

        if(sscanf(buffer,"### %s ###",lastType) != 0){
            if(debug)
                printf("%s\n", lastType);
        }else if(buffer[0] == '#' || buffer[0] == '\n' || strcmp(buffer,"\r\n")==0 || strlen(buffer) == 0){
            /* Found a comment... just ignore */
            continue;
        }else{
            if(strcmp(lastType,"ROOM") == 0){
                rooms[*roomCount] = parseRoom(buffer);

                *roomCount +=1;
            }else if(strcmp(lastType,"SUBJECT") == 0){
                subjects[*subjectCount] = parseSubject(buffer, rooms, *roomCount);

                *subjectCount +=1;
            }else if(strcmp(lastType,"CLASS") == 0){
                res = sscanf(buffer," %[^,],%[^,],%d ",
                    classes[*classCount].name,
                    inlineBuffer,
                    &classes[*classCount].maxWorkHoursPerDay
                );

                sscanf(classes[*classCount].name,"%d%*c",&classes[*classCount].year);

                classes[*classCount].classRoom = findRoom(inlineBuffer, rooms, *roomCount);

                *classCount+=1;
            }else if(strcmp(lastType,"TEACHER") == 0){
                res = sscanf(buffer," %[^,],%d,%[^,],%[^,],%d ",
                    teachers[*teacherCount].name,
                    &teachers[*teacherCount].isClassleader,
                    shortBuffer,
                    inlineBuffer,
                    &teachers[*teacherCount].maxWorkHoursPerDay
                );

                teachers[*teacherCount].leaderOfClass = findClass(shortBuffer, classes, *classCount);
                teachers[*teacherCount].canTeachLength = findSubjectsFromString(inlineBuffer, teachers[*teacherCount].canTeach, subjects, *subjectCount);

                *teacherCount+=1;
            }else if(strcmp(lastType,"TIMEINTERVALS") == 0){
                res = sscanf(buffer," %s ",
                    labels[labelCounter++]
                );
            }else{
                printf("Warning: Uknown type on line %i\n", i+1);
            }
        }
    }

    if(debug){
        printf("\nParsing complete\n"
               "%3d Rooms\n"
               "%3d Subjects\n"
               "%3d Classes\n"
               "%3d Teachers\n",
               *roomCount,
               *subjectCount,
               *classCount,
               *teacherCount
        );

        /* dumping info for demo purposes */
        printf("\nDUMPING DATA....\n\n");

        printf("Rooms:\n");
        for (i = 0; i < *roomCount; i++){
            printf("%d => %s\n", i+1, rooms[i].name);
        }

        printf("\nClasses:\n");
        for (i = 0; i < *classCount; i++){
            printf("%d => %s, Classroom: %s\n", i+1, classes[i].name, classes[i].classRoom->name);
        }

        printf("\nSubjects:\n");
        for (i = 0; i < *subjectCount; i++){
            printf("%2d => %17s, roomRequire: %s\n", i+1, subjects[i].name, (subjects[i].roomRequireLength != 0 ? subjects[i].roomRequire[0]->name : "*"));
        }

        printf("\nTeachers:\n");
        for (i = 0; i < *teacherCount; i++){
            strcpy(requirements, "");
            for (j = 0; j < teachers[i].canTeachLength; j++){
                if(j!=0)
                    strcat(requirements,", ");

                strcat(requirements,teachers[i].canTeach[j]->name);
            }

            printf("%2d => %15s, isClassleader: %s, %s\n", i+1, teachers[i].name, (teachers[i].isClassleader ? teachers[i].leaderOfClass->name : "no"), requirements);
        }
    }

    res = res + 1;

    free(requirements);
    fclose(dataFile);

    return 1;
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

room parseRoom(char *line){
    room returnRoom;
    sscanf(line," %[^\n]",
        returnRoom.name
    );

    return returnRoom;
}

subject parseSubject(char *line, room* rooms, int roomCount){
    char subjectBuffer[256];
    
    char curSub[MAX_NAME_LENGTH];
    int curSubI = 0;
    int year = 0;
    char hoursBuffer[100];
    
    subject returnSubject;
    int i;
    int len;

    sscanf(line," %[^,],%[^,],%s ",
        returnSubject.name,
        subjectBuffer,
        hoursBuffer
    );

    len = strlen(hoursBuffer);

    for (i = 0; i < len; i++){
        if(hoursBuffer[i] != ','){
            curSub[curSubI++] = hoursBuffer[i];
        
            if(i+2 == len){
                curSub[curSubI++] = hoursBuffer[i+1];
                hoursBuffer[i+1] = ',';
            }
        }else{
            curSub[curSubI] = '\0';
            curSubI = 0;
            returnSubject.perYear[year++] = atoi(curSub);
        }
    }

    returnSubject.roomRequireLength = findRoomsFromString(subjectBuffer,returnSubject.roomRequire, rooms, roomCount);

    return returnSubject;
}

room *findRoom(char *roomName, room rooms[MAX_ROOMS], int roomCount){
    int i;
    for (i = 0; i < roomCount; i++){
        if(strcmp(roomName, rooms[i].name) == 0){
            return &rooms[i];
        }
    }

    return NULL;
}

subject *findSubject(char *subjectName, subject subjects[MAX_SUBJECTS], int subjectCount){
    int i;
    for (i = 0; i < subjectCount; i++){
        if(strcmp(subjectName, subjects[i].name) == 0){
            return &subjects[i];
        }
    }

    return NULL;
}

class *findClass(char *className, class classes[MAX_CLASSES], int classCount){
    int i;
    for (i = 0; i < classCount; i++){

        if(strcmp(className, classes[i].name) == 0){
            return &classes[i];
        }
    }

    return NULL;
}

teacher *findTeacher(char *teacherName, teacher teachers[MAX_TEACHERS], int teacherCount){
    int i;
    for (i = 0; i < teacherCount; i++){

        if(strcmp(teacherName, teachers[i].name) == 0){
            return &teachers[i];
        }
    }

    return NULL;
}

int findRoomsFromString(char *roomString, room* roomsFound[MAX_ROOMS], room* rooms, int roomCount){
    int len = strlen(roomString);
    int i;
    int curChar = 0;
    int curRoom = 0;
    char tmpName[20];

    for (i = 1; i < len; i++){
        if(roomString[i] != ';' && roomString[i] != ']'){
            tmpName[curChar++] = roomString[i];
        }else{
            tmpName[curChar] = '\0';
            curChar = 0;
            if(strlen(tmpName) > 0)
                roomsFound[curRoom++] = findRoom(tmpName,rooms,roomCount);
        }
    }

    return curRoom;
}


int findSubjectsFromString(char *subjectString, subject* subjectsFound[MAX_SUBJECTS], subject* subjects, int subjectCount){
    int len = strlen(subjectString);
    int i;
    int curChar = 0;
    int curSubj = 0;
    char tmpName[20];

    for (i = 1; i < len; i++){
        if(subjectString[i] != ';' && subjectString[i] != ']'){
            tmpName[curChar++] = subjectString[i];
        }else{
            tmpName[curChar] = '\0';
            curChar = 0;
            subjectsFound[curSubj++] = findSubject(tmpName,subjects,subjectCount);
        }
    }

    return curSubj;
}
