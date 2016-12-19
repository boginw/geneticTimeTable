/**
 * @brief      Initates all the data from the dat.sched file.
 *
 * @param      populationParams  The population parameters
 *
 * @return     1
 */
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
                populationParams->subjects[populationParams->subjectCount].id = populationParams->subjectCount;
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
                    &populationParams->teachers[populationParams->teacherCount].maxWorkHours
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

                populationParams->teachers[populationParams->teacherCount].id = populationParams->teacherCount;

                populationParams->teacherCount += 1;
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
 * @brief      Counts the number of lines.
 * @author    http://stackoverflow.com/q/12733105/1426327
 *
 * @param      fp  The file pointer
 *
 * @return     Number of lines.
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

/**
 * @brief      Parses the room to data.
 *
 * @param      line  The line
 *
 * @return     Returns the room.
 */
room parseRoom(char *line){
    room returnRoom;

    strip(line);

    strcpy(returnRoom.name, line);

    return returnRoom;
}

/**
 * @brief      parse the subject from data.
 *
 * @param      line       The line
 * @param      rooms      The rooms
 * @param[in]  roomCount  The room count
 *
 * @return     the subject
 */
subject parseSubject(char *line, room* rooms, int roomCount){
    char *subjectBuffer;
    char *hoursBuffer;
    char *curSub;
    subject returnSubject;
    int i;
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
        for (i = 0; i < MAX_YEAR-1; i++){
            returnSubject.perYear[i] = SCHOOL_DAYS_YEAR;
        }
    }else if(strcmp("ONCE_A_WEEK",hoursBuffer)==0){
        for (i = 0; i < MAX_YEAR-1; i++){
            returnSubject.perYear[i] = WEEK_LENGTH;
        }
    }else{
        sscanf(hoursBuffer,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
            &returnSubject.perYear[0],
            &returnSubject.perYear[1],
            &returnSubject.perYear[2],
            &returnSubject.perYear[3],
            &returnSubject.perYear[4],
            &returnSubject.perYear[5],
            &returnSubject.perYear[6],
            &returnSubject.perYear[7],
            &returnSubject.perYear[8],
            &returnSubject.perYear[9]
        );
    }
    returnSubject.roomRequireLength = findRoomsFromString(subjectBuffer,returnSubject.roomRequire, rooms, roomCount);
    returnSubject.totalHours =
        returnSubject.perYear[0]+
        returnSubject.perYear[1]+
        returnSubject.perYear[2]+
        returnSubject.perYear[3]+
        returnSubject.perYear[4]+
        returnSubject.perYear[5]+
        returnSubject.perYear[6]+
        returnSubject.perYear[7]+
        returnSubject.perYear[8]+
        returnSubject.perYear[9];

    free(subjectBuffer);
    free(hoursBuffer);
    free(curSub);
    return returnSubject;
}

/**
 * @brief      Finds the room.
 *
 * @param      roomName   The room name
 * @param      rooms      The rooms
 * @param[in]  roomCount  The room count
 *
 * @return     the room pointer, if not found NULL
 */
room *findRoom(char *roomName, room *rooms, int roomCount){
    int i;
    for (i = 0; i < roomCount; i++){
        if(strcmp(roomName, rooms[i].name) == 0){
            return &rooms[i];
        }
    }

    return NULL;
}

/**
 * @brief      Finds the subject
 *
 * @param      subjectName   The subject name
 * @param      subjects      The subjects
 * @param[in]  subjectCount  The subject count
 *
 * @return     the subject pointer, if not found NULL
 */
subject *findSubject(char *subjectName, subject subjects[MAX_SUBJECTS], int subjectCount){
    int i;
    for (i = 0; i < subjectCount; i++){
        if(strcmp(subjectName, subjects[i].name) == 0){
            return &subjects[i];
        }
    }

    return NULL;
}

/**
 * @brief      Finds the class
 *
 * @param      className   The class name
 * @param      classes     The classes
 * @param[in]  classCount  The class count
 *
 * @return     the class pointer, if not found NULL
 */
class *findClass(char *className, class *classes, int classCount){
    int i;
    for (i = 0; i < classCount; i++){

        if(strcmp(className, classes[i].name) == 0){
            return &classes[i];
        }
    }

    return NULL;
}

/**
 * @brief      Finds the teacher
 *
 * @param      teacherName   The teacher name
 * @param      teachers      The teachers
 * @param[in]  teacherCount  The teacher count
 *
 * @return     the teacher pointer, if not found NULL
 */
teacher *findTeacher(char *teacherName, teacher *teachers, int teacherCount){
    int i;
    for (i = 0; i < teacherCount; i++){

        if(strcmp(teacherName, teachers[i].name) == 0){
            return &teachers[i];
        }
    }

    return NULL;
}

/**
 * @brief      Finds rooms form a string
 *
 * @param      roomString  The room string
 * @param      roomsFound  The rooms found
 * @param      rooms       The rooms
 * @param[in]  roomCount   The room count
 *
 * @return     Number of rooms
 */
int findRoomsFromString(char *roomString, room* roomsFound[MAX_ROOMS], room* rooms, int roomCount){
    int len = strlen(roomString);
    int i;
    int curChar = 0;
    int curRoom = 0;
    char tmpName[MAX_NAME_LENGTH];


    for (i = 1; i < len; i++){
        if(roomString[i] != ';' && roomString[i] != ']'){
            tmpName[curChar++] = roomString[i];
        }else{
            tmpName[curChar] = '\0';
            curChar = 0;
            if(strlen(tmpName) > 0){
                roomsFound[curRoom] = findRoom(tmpName,rooms,roomCount);
                curRoom++;
            }
        }
    }
    return curRoom;
}

/**
 * @brief      Finds subjects form a string
 *
 * @param      subjectString  The subject string
 * @param      subjectsFound  The subjects found
 * @param      subjects       The subjects
 * @param[in]  subjectCount   The subject count
 *
 * @return     number of subjects
 */
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
