


int init(params *populationParams){
    FILE *dataFile;
    int i,j,lines,res;
    int labelCounter = 0;
    char *buffer;
    char *inlineBuffer;
    char *shortBuffer;
    char *lastType;
    char *requirements;

    requirements = calloc(5 * 20, sizeof(char));
    buffer       = calloc(200,    sizeof(char));
    inlineBuffer = calloc(200,    sizeof(char));
    shortBuffer  = calloc(20,     sizeof(char));
    lastType     = calloc(15,     sizeof(char));

    if(!(dataFile = fopen("dat.sched", "r"))){
        printf("DATAFILE DOES NOT EXIST...\n");
        exit(0);
    }

    if(requirements == NULL || buffer == NULL || inlineBuffer == NULL || shortBuffer == NULL || lastType == NULL){
        printf("Not enough ram, sorry...\n");
        exit(EXIT_FAILURE);
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
                populationParams->rooms[populationParams->roomCount] = parseRoom(buffer);
                populationParams->roomCount +=1;
            }else if(strcmp(lastType,"SUBJECT") == 0){
                populationParams->subjects[populationParams->subjectCount] = parseSubject(buffer, populationParams->rooms, populationParams->roomCount);
                populationParams->subjectCount +=1;
            }else if(strcmp(lastType,"CLASS") == 0){
                res = sscanf(buffer," %[^,],%[^,],%d ",
                    populationParams->classes[populationParams->classCount].name,
                    inlineBuffer,
                    &populationParams->classes[populationParams->classCount].maxWorkHoursPerDay
                );

                sscanf(
                    populationParams->classes[populationParams->classCount].name,
                    "%d%*c",
                    &populationParams->classes[populationParams->classCount].year
                );

                populationParams->classes[populationParams->classCount].classRoom =
                    findRoom(inlineBuffer, populationParams->rooms, populationParams->roomCount);

                populationParams->classCount+=1;
            }else if(strcmp(lastType,"TEACHER") == 0){
                res = sscanf(buffer," %[^,],%d,%[^,],%[^,],%d ",
                    populationParams->teachers[populationParams->teacherCount].name,
                    &populationParams->teachers[populationParams->teacherCount].isClassleader,
                    shortBuffer,
                    inlineBuffer,
                    &populationParams->teachers[populationParams->teacherCount].maxWorkHoursPerDay
                );

                populationParams->teachers[populationParams->teacherCount].leaderOfClass =
                    findClass(shortBuffer, populationParams->classes, populationParams->classCount);

                populationParams->teachers[populationParams->teacherCount].canTeachLength =
                    findSubjectsFromString(
                        inlineBuffer,
                        populationParams->teachers[populationParams->teacherCount].canTeach,
                        populationParams->subjects,
                        populationParams->subjectCount
                    );

                populationParams->teacherCount+=1;
            }else if(strcmp(lastType,"TIMEINTERVALS") == 0){
                res = sscanf(buffer," %s ",
                    populationParams->intervalLabels[labelCounter++]
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
               populationParams->roomCount,
               populationParams->subjectCount,
               populationParams->classCount,
               populationParams->teacherCount
        );

        /* dumping info for demo purposes */
        printf("\nDUMPING DATA....\n\n");

        printf("Rooms:\n");
        for (i = 0; i < populationParams->roomCount; i++){
            printf("%d => %s\n", i+1, populationParams->rooms[i].name);
        }

        printf("\nClasses:\n");
        for (i = 0; i < populationParams->classCount; i++){
            printf("%d => %s, Classroom: %s\n",
                i+1,
                populationParams->classes[i].name,
                populationParams->classes[i].classRoom->name
            );
        }

        printf("\nSubjects:\n");
        for (i = 0; i < populationParams->subjectCount; i++){
            printf("%2d => %17s, roomRequire: %s\n",
                i+1,
                populationParams->subjects[i].name,
                (populationParams->subjects[i].roomRequireLength != 0 ? populationParams->subjects[i].roomRequire[0]->name : "*"));
        }

        printf("\nTeachers:\n");
        for (i = 0; i < populationParams->teacherCount; i++){
            strcpy(requirements, "");
            for (j = 0; j < populationParams->teachers[i].canTeachLength; j++){
                if(j!=0)
                    strcat(requirements,", ");

                strcat(requirements,populationParams->teachers[i].canTeach[j]->name);
            }

            printf("%2d => %15s, isClassleader: %s, %s\n",
                i+1,
                populationParams->teachers[i].name,
                (populationParams->teachers[i].isClassleader ? populationParams->teachers[i].leaderOfClass->name : "no"),
                requirements
            );
        }
    }

    res = res + 1;

    free(requirements);
    free(buffer);
    free(inlineBuffer);
    free(shortBuffer);
    free(lastType);

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

    strip(line);

    strcpy(returnRoom.name, line);
    
    return returnRoom;
}

subject parseSubject(char *line, room* rooms, int roomCount){
    char *subjectBuffer;
    char *hoursBuffer;
    char *curSub;
    int curSubI = 0;
    int year = 0;
    subject returnSubject;
    int i;
    int len;
    subjectBuffer = calloc(256, sizeof(char));
    hoursBuffer   = calloc(100, sizeof(char));
    curSub        = calloc(MAX_NAME_LENGTH, sizeof(char));

    if(subjectBuffer == NULL || hoursBuffer == NULL || curSub == NULL){
        printf("Not enough ram, sorry...\n");
        exit(EXIT_FAILURE);
    }

    sscanf(line," %[^,],%[^,],%s",
        returnSubject.name,
        subjectBuffer,
        hoursBuffer
    );

    if(strcmp("ONCE_A_DAY",hoursBuffer)==0){
        for (i = 0; i < MAX_YEAR; i++){
            returnSubject.perYear[i] = SCHOOL_DAYS_YEAR;
        }
    }else if(strcmp("ONCE_A_WEEK",hoursBuffer)==0){
        for (i = 0; i < MAX_YEAR; i++){
            returnSubject.perYear[i] = WEEK_LENGTH;
        }
    }else{
        len = strlen(hoursBuffer);
        for (i = 0; i < len; i++){
            if(hoursBuffer[i] != ','){
                curSub[curSubI] = hoursBuffer[i];
                curSubI++;
                if(i+2 == len){
                    curSub[curSubI] = hoursBuffer[i+1];
                    curSubI++;
                    hoursBuffer[i+1] = ',';
                }
            }else{
                curSub[curSubI] = '\0';
                curSubI = 0;
                returnSubject.perYear[year++] = atoi(curSub);
            }
        }


        returnSubject.roomRequireLength = findRoomsFromString(subjectBuffer,returnSubject.roomRequire, rooms, roomCount);
    }
    free(subjectBuffer);
    free(hoursBuffer);
    free(curSub);
    return returnSubject;
}

room *findRoom(char *roomName, room *rooms, int roomCount){
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

class *findClass(char *className, class *classes, int classCount){
    int i;
    for (i = 0; i < classCount; i++){

        if(strcmp(className, classes[i].name) == 0){
            return &classes[i];
        }
    }

    return NULL;
}

teacher *findTeacher(char *teacherName, teacher *teachers, int teacherCount){
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
    char *tmpName;
    tmpName = calloc(20, sizeof(char));
    if(tmpName == NULL){
        printf("Not enough ram, sorry...\n");
        exit(EXIT_FAILURE);
    }
    for (i = 1; i < len; i++){
        if(subjectString[i] != ';' && subjectString[i] != ']'){
            tmpName[curChar++] = subjectString[i];
        }else{
            tmpName[curChar] = '\0';
            curChar = 0;
            subjectsFound[curSubj++] = findSubject(tmpName,subjects,subjectCount);
        }
    }

    free(tmpName);

    return curSubj;
}
