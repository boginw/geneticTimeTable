#define FLOAT_MULTIPLIER 10000

int crossover_points      =   10;
int mutation_size         =  100;
int crossover_probability =   90;
int mutation_probability  =   10;

/**
 * @brief      Do the Crossover on a individuel
 *
 * @param      child1            The child 1's point for presisting the data
 * @param      child2            The child 2's point for presisting the data
 * @param[in]  p1                The parrnt 1's data pointer
 * @param[in]  p2                The parent 2's data pointer
 * @param      populationParams  The population parameters
 */
void crossover(individual *child1, individual *child2, const individual *p1, const individual *p2, params *populationParams){
    int i,p,c,l;
    int first;
    int *cp = calloc(MAX_LECTURES, sizeof(int));
    *child1 = *p1;
    *child2 = *p2;
    first = randomNumber(0,1);

    for (c = 0; c < populationParams->classCount; c++){
        for(i = crossover_points; i > 0; i--){
            while( 1 ){
                p = randomNumber(0,MAX_LECTURES-1);
                if( !cp[ p ] ){
                    cp[ p ] = 1;
                    break;
                }
            }
        }

        for (l = 0; l < MAX_LECTURES; l++){
            memset(cp, 0, MAX_LECTURES * sizeof(int));
            if(child1->t[c].lectures[l].init != 1 || child2->t[c].lectures[l].init != 1){
                continue;
            }
            if(first){
               child1->t[c].lectures[l] = child2->t[c].lectures[l];
            }else{
                child2->t[c].lectures[l] = child1->t[c].lectures[l];
            }

            if( cp[ l ] ){
                first = !first;
            }

            if(child1->t[c].lectures[l].l_subject->roomRequireLength == 0){
                child1->t[c].lectures[l].l_room = child1->t[c].forClass->classRoom;
            }

            if(child2->t[c].lectures[l].l_subject->roomRequireLength == 0){
                child2->t[c].lectures[l].l_room = child2->t[c].forClass->classRoom;
            }
        }
    }

    conflictsAndPreperation(child1,populationParams);
    conflictsAndPreperation(child2,populationParams);
    free(cp);
}


/**
 * Counts conflicts of rooms and teachers in a single individuals
 * @param ind               individuals
 * @param classCount amount of classes
 */

/**
 * @brief      Counts conflicts and prepare data for fitness function on a single individual.
 *
 * @param      ind               The ind
 * @param      populationParams  The population parameters
 */
void conflictsAndPreperation(individual *ind, params *populationParams){
    int class1,   class2,
        lecture1, lecture2,
        day,      hour,
        teacherIndex,
        tempMaxHours,
        totalLectures = 0,
        conflicts = 0;

    /* Put in dat.shed */
    int preperationTime = 20;
    int lectionTime     = 45;

    float penalty = 1.0;

    float tempTotalWorkHours;
    float tempWorkHoursPreperationNormalized;

    int subjectIndex;
    int curYear;
    int tempSubjectSum = 0;
    int requiredSubjectCounter = 0;
    int accumulativeCorrectSubject = 0;
    int nullHoursAcc = 0;

    int *teacherTotalHours  = calloc(populationParams->teacherCount, sizeof(int));
    int *teacherPreperation = calloc(populationParams->teacherCount, sizeof(int));

    /* Reset fitness points */
    ind->fitness = 0;

    /* Sort timetables by day and hour */
    for (class1 = 0; class1 < populationParams->classCount; class1++){
        qsort(&ind->t[class1], ind->t[class1].lectureLength, sizeof(lecture), dayHourQsort);
    }

    /* Reset conflict flags */
    for (class1 = 0; class1 < populationParams->classCount; class1++){
        for(lecture1 = 0; lecture1 < ind->t[class1].lectureLength; lecture1++){
            ind->t[class1].lectures[lecture1].conflictTeacher = 0;
            ind->t[class1].lectures[lecture1].conflictRoom    = 0;
        }
    }

    for (class1 = 0; class1 < populationParams->classCount; class1++){
        curYear = ind->t[class1].forClass->year;

        /* Sum total lectures for subject for class per day */
        for (subjectIndex = 0; subjectIndex < populationParams->subjectCount; subjectIndex++){
            tempSubjectSum += (int) ceil(populationParams->subjects[subjectIndex].perYear[curYear] / ((float)SCHOOL_DAYS_YEAR / (float)WEEK_LENGTH));
        }

        for(lecture1 = 0; lecture1 < ind->t[class1].lectureLength; lecture1++){

            day  = ind->t[class1].lectures[lecture1].l_datetime.dayOfWeek;
            hour = ind->t[class1].lectures[lecture1].l_datetime.hour;

            /* Don't check empty lectures */
            if(ind->t[class1].lectures[lecture1].init == 1){

                /* Count total inited lectures */
                totalLectures += 1;

                /* If class should have subject */
                if (ind->t[class1].lectures[lecture1].l_subject->perYear[curYear] != 0) {
                    accumulativeCorrectSubject += 1;
                }

                /* Check if subject is required */
                if(populationParams->subjects[ind->t[class1].lectures[lecture1].l_subject->id].perYear[curYear] > 0){

                    /* Count is required subject */
                    requiredSubjectCounter += 1;
                }else{
                    /* Penalty if subject isn't required */
                    penalty = penalty * 0.9;
                }

                /* Count teacher workhours */
                teacherTotalHours[ind->t[class1].lectures[lecture1].l_teacher->id] += 1;

                /* If previous lecture is the same */
                if(lecture1 > 0 && isSameLectureSequent(&ind->t[class1].lectures[lecture1], &ind->t[class1].lectures[lecture1 - 1])){

                    /* Last lecture is the same, so add half hour */
                    teacherPreperation[ind->t[class1].lectures[lecture1].l_teacher->id] +=  5;
                }else{
                    /* Last lecture is not the same, so add hour */
                    teacherPreperation[ind->t[class1].lectures[lecture1].l_teacher->id] += 10;
                }


                /* If next lecture is more than 1 hour away */
                if(lecture1 + 1 != ind->t[class1].lectureLength &&
                        ind->t[class1].lectures[lecture1].l_datetime.dayOfWeek == ind->t[class1].lectures[lecture1 + 1].l_datetime.dayOfWeek &&
                        ind->t[class1].lectures[lecture1].l_datetime.hour + 1  != ind->t[class1].lectures[lecture1 + 1].l_datetime.hour){

                    nullHoursAcc += 1;
                }


                /* Don't check last timetable for conflicts */
                if(class1 + 1 != populationParams->classCount){

                    for(class2 = class1 + 1; class2 < populationParams->classCount; class2++){

                        /* Foreach lecture in other classes where day is less or equal to the day */
                        for(
                            lecture2 = 0;
                            lecture2 < ind->t[class2].lectureLength &&
                                ind->t[class2].lectures[lecture2].l_datetime.dayOfWeek <= day;
                            lecture2++
                        ){

                            /* If same day and hour */
                            if(ind->t[class2].lectures[lecture2].l_datetime.dayOfWeek == day &&
                                    ind->t[class2].lectures[lecture2].l_datetime.hour == hour){

                                /* Same room */
                                if(ind->t[class2].lectures[lecture2].l_room == ind->t[class1].lectures[lecture1].l_room){

                                    ind->t[class1].lectures[lecture1].conflictRoom = 1;
                                    ind->t[class2].lectures[lecture2].conflictRoom = 1;
                                }

                                /* Same teacher */
                                if(ind->t[class2].lectures[lecture2].l_teacher == ind->t[class1].lectures[lecture1].l_teacher){

                                    ind->t[class1].lectures[lecture1].conflictTeacher = 1;
                                    ind->t[class2].lectures[lecture2].conflictTeacher = 1;
                                }
                            }
                        }
                    }
                }

                /* Count total conflicts */
                conflicts +=
                    ind->t[class1].lectures[lecture1].conflictRoom +
                    ind->t[class1].lectures[lecture1].conflictTeacher;
            }
        }
    }

    /* Calculate points for subject requirements */
    ind->fitness += FITNESS_FOR_CLASS_MIN_HOURS * (1 - (tempSubjectSum - requiredSubjectCounter) / tempSubjectSum);

    /* Calculate points from actual work hours */
    for (teacherIndex = 0; teacherIndex < populationParams->teacherCount; teacherIndex++){
        tempMaxHours = populationParams->teachers[teacherIndex].maxWorkHours;

        tempTotalWorkHours = (          teacherTotalHours[teacherIndex]        * lectionTime     ) +
                             ( ((float) teacherPreperation[teacherIndex] / 10) * preperationTime );

        tempWorkHoursPreperationNormalized = (tempMaxHours - (tempTotalWorkHours / MINUTES_IN_HOUR)) / tempMaxHours;

        ind->fitness += FITNESS_FOR_PREPARATION_TIME / ((float) teacherPreperation[teacherIndex] / 10);
        ind->fitness += FITNESS_FOR_TEACHERHOURS * (1 - tempWorkHoursPreperationNormalized);
    }

    /* Penalty for fitness */
    ind->fitness = ind->fitness * penalty;

    /* Save conflicts */
    ind->conflicts = conflicts;

    /* Save acc null hours */
    ind->nullHours = nullHoursAcc;

    free(teacherTotalHours);
    free(teacherPreperation);
}

/**
 * @brief      Determines if two sequent lectures are the same.
 *
 * @param      l1    The first lecture
 * @param      l2    The second lecture
 *
 * @return     True if same lecture sequent, False otherwise.
 */
int isSameLectureSequent(lecture *l1, lecture *l2){
    return (l2->init                 == 1) &&
           (l1->l_datetime.hour - 1  == l2->l_datetime.hour) &&
           (l1->l_datetime.dayOfWeek == l2->l_datetime.dayOfWeek) &&
           (l1->l_room               == l2->l_room) &&
           (l1->l_subject            == l2->l_subject) &&
           (l1->l_teacher            == l2->l_teacher);
}

/**
 * @brief      Starts the mutation process by calling a random amount of mutations.
 *
 * @param      i                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 */
void mutate(individual *i, params *populationParams){
    int amountOfMutations = randomNumber(1, MAX_MUTATIONS);
    weapon_x(i, amountOfMutations, populationParams);
}

/**
 * @brief      The recursive function who makes sure that n-number of mutations is taken place
 *
 * @param      i                  The individual who should be affected by the mutation
 * @param[in]  amountOfMutations  The amount of mutations
 * @param      populationParams   The population parameters
 */
void weapon_x(individual *i, int amountOfMutations, params *populationParams){
    if(amountOfMutations < 1){
        return;
    }
    injectSerumX(i, populationParams);
    i->mutations++;
    weapon_x(i, (amountOfMutations-1), populationParams);
}

/**
 * @brief      Determines which mutation should be executed on the individual.
 *
 * @param      i                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 */
void injectSerumX(individual *i, params *populationParams){
    int ingredient = randomNumber(1,3);
    switch(ingredient){
        case 1:
            addSugar(i, populationParams);
        break;
        case 2:
            addSpice(i, populationParams);
        break;
        case 3:
            addEverythingNice(i, populationParams);
        break;
    }
}

/**
 * @brief      Mutates the individual by moving a random lecture in a random timetable
 *             to a place within the timetable where there isn't a lecture already.
 *
 * @param      i                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 */
void addSugar(individual *i, params *populationParams){
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
    int rndDay, rndHour;
    getRandomDatetimeWithNoLecture(&i->t[rndClass], &rndDay, &rndHour);
    i->t[rndClass].lectures[rndLec].l_datetime.dayOfWeek=rndDay;
    i->t[rndClass].lectures[rndLec].l_datetime.hour=rndHour;
}

/**
 * @brief      Mutates the individual by change the teacher of a random lecture on a random timetable
 *             to a new teacher, it is required that the new teacher actually is able to teach the subject.
 *
 * @param      i                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 */
void addSpice(individual *i, params *populationParams){
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
    lecture *thelecture = &i->t[rndClass].lectures[rndLec];

    if(thelecture->init != 1){
        return;
    }

    thelecture->l_teacher = findRandomTeacherForSubject(thelecture, populationParams);
}

/**
 * @brief      Mutates the individual by place two random lectures of same subject sequently in a random timetable
 *             this is done by selecting a random lecture, searching for a lecture with the same subject and
 *             swap it with either the lecture before or after the selected one.
 *
 * @param      i                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 */
void addEverythingNice(individual *i, params *populationParams){
    int j;
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec   = randomNumber(0, i->t[rndClass].lectureLength-1);
    lecture *currentLecture = &i->t[rndClass].lectures[rndLec];
    lecture *selectedLecture;
    int prevOrNext;

    int nextLec = lectureOnDateTime(
            i->t[rndClass],
            currentLecture->l_datetime.dayOfWeek,
            currentLecture->l_datetime.hour + 1);

    int prevLec = lectureOnDateTime(
            i->t[rndClass],
            currentLecture->l_datetime.dayOfWeek,
            currentLecture->l_datetime.hour - 1);

    if(currentLecture->l_datetime.hour == MAX_LECTURES / WEEK_LENGTH){
        selectedLecture = &i->t[rndClass].lectures[prevLec];

    }else if(currentLecture->l_datetime.hour == 0){
        selectedLecture = &i->t[rndClass].lectures[nextLec];

    }else{
        if(prevLec != -1 && nextLec != -1){
            prevOrNext = randomNumber(0,1) ? prevLec : nextLec;

        }else if(prevLec == -1 && nextLec == -1){
            prevOrNext = prevLec;

        }else{
            prevOrNext = prevLec == -1 ? prevLec : nextLec;

        }

        selectedLecture = &i->t[rndClass].lectures[prevOrNext];
    }

    if(currentLecture->l_teacher == selectedLecture->l_teacher && currentLecture->l_subject == selectedLecture->l_subject){
        return;
    }


    for (j = 0; j < i->t[rndClass].lectureLength; j++){

        if(currentLecture != &i->t[rndClass].lectures[j] && i->t[rndClass].lectures[j].init == 1
           && currentLecture->l_teacher == i->t[rndClass].lectures[j].l_teacher
           && currentLecture->l_subject == i->t[rndClass].lectures[j].l_subject){

            if(prevOrNext != -1 && currentLecture->init == 1){
                swapn(
                    &i->t[rndClass].lectures[prevOrNext].l_datetime,
                    &i->t[rndClass].lectures[j].l_datetime,
                    sizeof(datetime)
                );
            }else{
                i->t[rndClass].lectures[j].l_datetime = i->t[rndClass].lectures[prevOrNext].l_datetime;
            }

            break;
        }
    }
}

/**
 * @brief      Finds a random teacher for a given subject, it only returns a teacher who is actually able to teach the subject
 *
 * @param      l                 The individual who should be affected by the mutation
 * @param      populationParams  The population parameters
 *
 * @return     Pointer to the teacher.
 */
teacher *findRandomTeacherForSubject(lecture *l, params *populationParams){
    int rndTeacher = randomNumber(0, populationParams->teacherCount-1);
    if(teacherCanTeach(populationParams->teachers[rndTeacher].canTeach, populationParams->teachers[rndTeacher].canTeachLength, l->l_subject->name)){
        return &populationParams->teachers[rndTeacher];
    }
    return findRandomTeacherForSubject(l, populationParams);
}

/**
 * @brief      Determines wether or not a given teacher can teach the given subject
 *
 * @param      canTeach        Indicates if teach
 * @param[in]  canTeachLength  Indicates if teach length
 * @param      subjectName     The subject name
 *
 * @return     1 if trie, 0 otherwise
 */
int teacherCanTeach(subject **canTeach, int canTeachLength, char *subjectName){
    int i;

    if(subjectName == NULL){
        return 0;
    }

    for(i = 0; i < canTeachLength; i++){
        if(strcmp(canTeach[i]->name, subjectName) == 0){
            return 1;
        }
    }
    return 0;
}

/**
 * @brief      Gets the a random datetime with no lecture.
 *
 * @param      t     The individual who should be affected by the mutation
 * @param      day   The day
 * @param      hour  The hour
 */
void getRandomDatetimeWithNoLecture(timetable *t, int *day, int*hour){
    int rndDay = randomNumber(0, WEEK_LENGTH-1);
    int rndHour = randomNumber(0,(MAX_LECTURES/WEEK_LENGTH)-1);
    if(lectureOnDateTime(*t, rndDay, rndHour) == -1){
        *day = rndDay;
        *hour = rndHour;
        return;
    }
    getRandomDatetimeWithNoLecture(t, day, hour);
}

/**
 * @brief      Sets the fitness scorer.
 *
 * @param      populationParams  The population parameters
 */
void setFitness(params *populationParams){
    /*int i, j, k, l, m, fitnessRatio, count1, count2, res_i, klasse;
    float value1, value2, res_f;
    double mellem_resultat;*/

    int i, fitnessRatio, nullHoursRatio;
    int accFitness = 0;
    int accNullHours = 0;
    int maxConflicts = populationParams->biggestConflicts;
    int maxNullHours = populationParams->biggestNullHours;


    for (i = 0; i < populationParams->tempPopulationCount; i++){
        accFitness   += (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
        accNullHours += (((maxNullHours - populationParams->tempPopulation[i].nullHours) / (float) maxNullHours)) * 100;
    }

    for (i = 0; i < populationParams->tempPopulationCount; i++){
        fitnessRatio = (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
        fitnessRatio = fitnessRatio / (float) accFitness * FITNESS_FOR_CONFLICTS;


        nullHoursRatio = (((maxNullHours - populationParams->tempPopulation[i].nullHours) / (float) maxNullHours)) * 100;
        nullHoursRatio = nullHoursRatio / (float) accNullHours * FITNESS_FOR_NULL_HOURS;

        populationParams->tempPopulation[i].fitness += (float) fitnessRatio;
        populationParams->tempPopulation[i].fitness += (float) nullHoursRatio;
        populationParams->akkFitnessPoints += (float) fitnessRatio;
        populationParams->akkFitnessPoints += (float) nullHoursRatio;

        if(fitnessRatio < 0){
            printf("An error should be fixed... To dangorous to continue.\n");
            exit(0);
        }
    }
    /*int i, biggest;
    if(populationParams->biggestConflicts > 0){
        biggest = populationParams->biggestConflicts;
    }else{
        biggest = 1;
    }

    for (i = 0; i < populationParams->tempPopulationCount; i++){
        populationParams->tempPopulation[i].fitness = (((biggest - populationParams->tempPopulation[i].conflicts) / (float) biggest)) * 100000;
        populationParams->tempPopulation[i].fitness += 100;
        populationParams->akkFitnessPoints += populationParams->tempPopulation[i].fitness;
    }*/

    /* SL - Udregn en fitness-værdi baseret på samme lokale til samme fag for en klasse */
    /*for (i = 0; i < populationParams->tempPopulationCount; i++) {
        mellem_resultat = 0.0;
        for (j = 0; j < populationParams->classCount; j++) {
            count1 = 0;
            for (k = 0; k < populationParams->subjectCount; k++) {
                for (l = 0; l < populationParams->roomCount; l++) {
                    count2 = 0;
                    res_i  = 0;
                    for (m = 0; m < populationParams->tempPopulation[i].t[j].lectureLength; m++) {
                        if (populationParams->tempPopulation[i].t[j].lectures[m].init == 1) {
                            if (populationParams->tempPopulation[i].t[j].lectures[m].l_subject == &populationParams->subjects[k]) {
                                count2++;
                                if (populationParams->tempPopulation[i].t[j].lectures[m].l_room == &populationParams->rooms[l]) {
                                    res_i++;
                                }
                            }
                        } else {
                            continue;
                        }
                    }
                    if (res_i > count1) {
                        count1 = res_i;
                    }
                }
                if (count2 != 1) {
                    value1 = (float) count1;
                    value2 = (float) count2;
                    mellem_resultat += value1 / value2 * FLOAT_MULTIPLIER;
                }
            }
            value1 = mellem_resultat;
            value2 = (float) populationParams->subjectCount;
            mellem_resultat = value1 / value2;
        }
        value1 = mellem_resultat;
        value2 = (float) populationParams->classCount;
        populationParams->tempPopulation[i].fitness += (int) (value1 / value2);
        populationParams->akkFitnessPoints += (int) (value1 / value2);
    }*/

    /* SF - Udregn en fitness-værdi baseret på samme lærer i samme fag for en klasse */
    /* Modificér EM til at få det til at virke */
    /*for (i = 0; i < populationParams->tempPopulationCount; i++) {
        mellem_resultat = 0.0;
        for (j = 0; j < populationParams->classCount; j++) {
            count1 = 0;
            for (k = 0; k < populationParams->subjectCount; k++) {
                for (l = 0; l < populationParams->teacherCount; l++) {
                    count2 = 0;
                    res_i  = 0;
                    for (m = 0; m < populationParams->tempPopulation[i].t[j].lectureLength; m++) {
                        if (populationParams->tempPopulation[i].t[j].lectures[m].init == 1) {
                            if (populationParams->tempPopulation[i].t[j].lectures[m].l_subject == &populationParams->subjects[k]) {
                                count2++;
                                if (populationParams->tempPopulation[i].t[j].lectures[m].l_teacher == &populationParams->teachers[l]) {
                                    res_i++;
                                }
                            }
                        } else {
                            continue;
                        }
                    }
                    if (res_i > count1) {
                        count1 = res_i;
                    }
                }
                if (count2 != 0) {
                    value1 = (float) count1;
                    value2 = (float) count2;
                    mellem_resultat += value1 / value2 * FLOAT_MULTIPLIER;
                }
            }
            value1 = mellem_resultat;
            value2 = (float) populationParams->subjectCount;
            mellem_resultat = value1 / value2;
        }
        value1 = mellem_resultat;
        value2 = (float) populationParams->classCount;
        populationParams->tempPopulation[i].fitness += (int) (value1 / value2);
        populationParams->akkFitnessPoints += (int) (value1 / value2);
    }*/

    /* EM - Udregn en fitness-værdi baseret på at en klasse opnår minimumskrav. */
    /*for (i = 0; i < populationParams->tempPopulationCount; i++) {
        mellem_resultat = 0.0;
        count1 = 0;
        count2 = 0;
        res_i  = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            klasse = populationParams->tempPopulation[i].t[j].forClass->year;
            for (l = 0; l < populationParams->subjectCount; l++) {
                if (populationParams->subjects[l].perYear[klasse] > 0) {
                    count2++; *//* mellem-resultat maks *//*
                    for (k = 0; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                        if (populationParams->tempPopulation[i].t[j].lectures[k].init != 1) {
                            break;
                        }
                        *//*printf("DEBUG: %d\n", populationParams->tempPopulation[i].t[j].lectures[k].l_datetime.dayOfWeek);*//*
                        if (populationParams->tempPopulation[i].t[j].lectures[k].l_subject == &populationParams->subjects[l]) {
                            count1++;
                        }
                    }
                    if ((count1 * (SCHOOL_DAYS_YEAR / WEEK_LENGTH)) >= populationParams->subjects[l].perYear[klasse]) {
                        res_i++; *//* mellem-resultat aktuelt *//*
                    }
                }
            }
            res_f  = (float) res_i;
            value2 = (float) count2;
            mellem_resultat  += res_f / value2 * FLOAT_MULTIPLIER;
        }
        value1 = mellem_resultat;
        value2 = (float) populationParams->classCount;
        populationParams->tempPopulation[i].fitness += (int) (value1 / value2);
        populationParams->akkFitnessPoints += (int) (value1 / value2);
    }*/

    /* EF - Udregn en fitness-værdi baseret på samme at en klasse kun undervises i de fag de skal have */
    /*for (i = 0; i < populationParams->tempPopulationCount; i++) {
        count1 = 0;
        count2 = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            klasse = populationParams->tempPopulation[i].t[j].forClass->year;
            for (k = 0; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                if (populationParams->tempPopulation[i].t[j].lectures[k].init != 1) {
                    break;
                }
                if (populationParams->tempPopulation[i].t[j].lectures[k].l_subject->perYear[klasse] != 0) {
                    count1++;
                }
            }
            count2 += populationParams->tempPopulation[i].t[j].lectureLength;
        }
        value1 = (float)count1;
        value2 = (float)count2;
        populationParams->tempPopulation[i].fitness += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
        populationParams->akkFitnessPoints += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
    }*/

}
