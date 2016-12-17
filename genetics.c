#define FLOAT_MULTIPLIER 10000

int crossover_points      =   10;
int mutation_size         =  100;
int crossover_probability =   90;
int mutation_probability  =   10;

void crossover(individual *child1, individual *child2, const individual *p1,const individual *p2, int classCount){
    int i,p,c,l;
    int first;
    int *cp = calloc(MAX_LECTURES, sizeof(int));
    *child1 = *p1;
    *child2 = *p2;

    /* check probability of crossover operation */
    /*if( randomNumber(0,100) > crossover_probability ){

        return *p1;
    }*/

    /* make new code by combining parent codes */
    first = randomNumber(0,1);
    for (c = 0; c < classCount; c++){
        /* determine crossover point (randomly) */

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
            memset(cp,0,MAX_LECTURES*sizeof(int));

            if(child1->t[c].lectures[l].init != 1 || child2->t[c].lectures[l].init != 1){
                continue;
            }

            if(first){
                /*swapn(
                    &child1->t[c].lectures[l],
                    &child2.t[c].lectures[l],
                    sizeof(lecture)
                );*/

                child1->t[c].lectures[l] = child2->t[c].lectures[l];
            }else{
                child2->t[c].lectures[l] = child1->t[c].lectures[l];
            }

            if( cp[ l ] ){
                /* change source chromosome */
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

    conflicts(child1,classCount);
    conflicts(child2,classCount);
    free(cp);

}


void mutate(individual *i, params *populationParams){
    /* TODO
     * vælg valg der skal muteres
     * vælge hvordan det skal muterer
     * muterer det
     * returner og afslut
     */
    int amountOfMutations = randomNumber(1, MAX_MUTATIONS);
    weapon_x(i, amountOfMutations, populationParams);
}

void weapon_x(individual *i, int amountOfMutations, params *populationParams){
    if(amountOfMutations < 1){
        return;
    }
    injectSerumX(i, populationParams);
    i->mutations++;
    weapon_x(i, (amountOfMutations-1), populationParams);
}

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

void addSugar(individual *i, params *populationParams){
    /* This layer mutates on the top level ie. the total school timetable */
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
    int rndDay, rndHour;
    getRandomDatetimeWithNoLecture(&i->t[rndClass], &rndDay, &rndHour);
    i->t[rndClass].lectures[rndLec].l_datetime.dayOfWeek=rndDay;
    i->t[rndClass].lectures[rndLec].l_datetime.hour=rndHour;
}


void addSpice(individual *i, params *populationParams){
    /* This layer mutates on the top level ie. the total school timetable */
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
    lecture *thelecture = &i->t[rndClass].lectures[rndLec];

    if(thelecture->init != 1){
        return;
    }

    thelecture->l_teacher = findRandomTeacherForSubject(thelecture, populationParams->teachers, populationParams->teacherCount);
}


void addEverythingNice(individual *i, params *populationParams){
    /* This layer mutates on the top level ie. the total school timetable */
    int j;
    int rndClass = randomNumber(0, populationParams->classCount-1);
    int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
    lecture *currentLecture = &i->t[rndClass].lectures[rndLec];
    lecture *selectedLecture;
    int prevOrNext;
    int nextLec = lectureOnDateTime(i->t[rndClass], currentLecture->l_datetime.dayOfWeek, currentLecture->l_datetime.hour + 1);
    int prevLec = lectureOnDateTime(i->t[rndClass], currentLecture->l_datetime.dayOfWeek, currentLecture->l_datetime.hour - 1);

    if(currentLecture->l_datetime.hour == MAX_LECTURES / WEEK_LENGTH){
        selectedLecture = &i->t[rndClass].lectures[prevLec];

    }else if(currentLecture->l_datetime.hour == 0){
        selectedLecture = &i->t[rndClass].lectures[nextLec];

    }else{
        if(prevLec != -1 && nextLec != -1){
            prevOrNext = randomNumber(0,1);

        }else if(prevLec == -1 && nextLec == -1){
            prevOrNext = prevLec;

        }else{
            prevOrNext = prevLec == -1 ? prevLec : nextLec;

        }

        selectedLecture = &i->t[rndClass].lectures[prevOrNext];
    }

    if(currentLecture->l_teacher != selectedLecture->l_teacher && currentLecture->l_subject != selectedLecture->l_subject){
        for (j = 0; j < MAX_LECTURES; j++){

            if(currentLecture != &i->t[rndClass].lectures[j]
               && currentLecture->l_teacher == i->t[rndClass].lectures[j].l_teacher 
               && currentLecture->l_subject == i->t[rndClass].lectures[j].l_subject){

                if(prevOrNext != -1){
                    selectedLecture->l_datetime = i->t[rndClass].lectures[j].l_datetime;
                    /*nextLecture->l_datetime.dayOfWeek = i->t[rndClass].lectures[j]->l_datetime.dayOfWeek;
                    nextLecture->l_datetime.hour      = i->t[rndClass].lectures[j]->l_datetime.hour;*/
                }
                
                i->t[rndClass].lectures[j].l_datetime.dayOfWeek = currentLecture->l_datetime.dayOfWeek;
                i->t[rndClass].lectures[j].l_datetime.hour      = currentLecture->l_datetime.hour+1;

                break;
            }
        }
    }
}

teacher *findRandomTeacherForSubject(lecture *l, teacher *t, int teacherCount){
    int rndTeacher = randomNumber(0, teacherCount-1);
    if(teacherCanTeach(t[rndTeacher].canTeach, t[rndTeacher].canTeachLength, l->l_subject->name)){
        return &t[rndTeacher];
    }
    return findRandomTeacherForSubject(l,t,teacherCount);
}

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


void setFitness(params *populationParams){
    int i, j, k, l, m, akk = 0, fitnessRatio, count1, count2, klasse, res_i;
    /*int test1, test2, test3, test4;*/
    float value1, value2, res_f;
    double mellem_resultat;
    int maxConflicts = populationParams->tempPopulation[populationParams->tempPopulationCount - 1].conflicts;
    for (i = 0; i < populationParams->tempPopulationCount; i++){
        akk += (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
    }
    for (i = 0; i < populationParams->tempPopulationCount; i++){
        fitnessRatio = (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
        fitnessRatio = fitnessRatio / (float) akk * 100;
        populationParams->tempPopulation[i].fitness = (float) fitnessRatio;
    }
    /* F2B - Udregn en fitness-værdi baseret på fag i blokke af 2 */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
        count2 = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            res_i = 0;
            count1 = 0;
            for (k = 1; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                if (populationParams->tempPopulation[i].t[j].lectures[k-1].init == 1) {
                    count1++;
                }
                if((populationParams->tempPopulation[i].t[j].lectures[k-1].init != 1) || (populationParams->tempPopulation[i].t[j].lectures[k].init != 1)) {
                    continue;
                }
                l = k;
                if((populationParams->tempPopulation[i].t[j].lectures[k-1].l_datetime.dayOfWeek == populationParams->tempPopulation[i].t[j].lectures[l].l_datetime.dayOfWeek) &&
                   (populationParams->tempPopulation[i].t[j].lectures[k-1].l_room == populationParams->tempPopulation[i].t[j].lectures[l].l_room) &&
                   (populationParams->tempPopulation[i].t[j].lectures[k-1].l_subject == populationParams->tempPopulation[i].t[j].lectures[l].l_subject) &&
                   (populationParams->tempPopulation[i].t[j].lectures[k-1].l_teacher == populationParams->tempPopulation[i].t[j].lectures[l].l_teacher)) {
                    res_i += 1;
                }
            }
            value1 = (float) (res_i * 11) + 11;
            value2 = (float) count1 * 22;
            count2 += (int) value1 / value2 * FLOAT_MULTIPLIER;
        }
        value1 = (float) count2;
        value2 = (float) populationParams->classCount;
        populationParams->tempPopulation[i].fitness += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
        populationParams->akkFitnessPoints += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
    }

    /* SL - Udregn en fitness-værdi baseret på samme lokale til samme fag for en klasse */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
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
    }

    /* SF - Udregn en fitness-værdi baseret på samme lærer i samme fag for en klasse */
    /* Modificér EM til at få det til at virke */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
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
    }

    /* EM - Udregn en fitness-værdi baseret på at en klasse opnår minimumskrav. */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
        mellem_resultat = 0.0;
        count1 = 0;
        count2 = 0;
        res_i  = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            klasse = populationParams->tempPopulation[i].t[j].forClass->year;
            for (l = 0; l < populationParams->subjectCount; l++) {
                if (populationParams->subjects[l].perYear[klasse] > 0) {
                    count2++; /* mellem-resultat maks */
                    for (k = 0; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                        if (populationParams->tempPopulation[i].t[j].lectures[k].init != 1) {
                            break;
                        }
                        /*printf("DEBUG: %d\n", populationParams->tempPopulation[i].t[j].lectures[k].l_datetime.dayOfWeek);*/
                        if (populationParams->tempPopulation[i].t[j].lectures[k].l_subject == &populationParams->subjects[l]) {
                            count1++;
                        }
                    }
                    if ((count1 * (SCHOOL_DAYS_YEAR / WEEK_LENGTH)) >= populationParams->subjects[l].perYear[klasse]) {
                        res_i++; /* mellem-resultat aktuelt */
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
    }

    /* EF - Udregn en fitness-værdi baseret på samme at en klasse kun undervises i de fag de skal have */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
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
    }
    /* LF - Udregn en fitness-værdi baseret på at alle lærere underviser i fag som de kan */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
        count1 = 0;
        count2 = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            for (k = 0; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                if (populationParams->tempPopulation[i].t[j].lectures[k].init != 1) {
                    break;
                }
                if (populationParams->tempPopulation[i].t[j].lectures[k].l_teacher->canTeachLength != 0) {
                    for (l = 0; l < populationParams->tempPopulation[i].t[j].lectures[k].l_teacher->canTeachLength; l++) {
                        if (strcmp(populationParams->tempPopulation[i].t[j].lectures[k].l_teacher->canTeach[l]->name, populationParams->tempPopulation[i].t[j].lectures[k].l_subject->name) == 0) {
                            count1++;
                            break;
                        }
                    }
                }
            }
            count2 += populationParams->tempPopulation[i].t[j].lectureLength;
        }
        value1 = (float)count1;
        value2 = (float)count2;
        populationParams->tempPopulation[i].fitness += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
        populationParams->akkFitnessPoints += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
    }

    /* FLo - Udregn en fitness-værdi baseret på at alle lokaler understøtter de fag der undervises */
    for (i = 0; i < populationParams->tempPopulationCount; i++) {
        count1 = 0;
        count2 = 0;
        for (j = 0; j < populationParams->classCount; j++) {
            for (k = 0; k < populationParams->tempPopulation[i].t[j].lectureLength; k++) {
                if (populationParams->tempPopulation[i].t[j].lectures[k].init != 1) {
                    break;
                }
                if (populationParams->tempPopulation[i].t[j].lectures[k].l_subject->roomRequireLength != 0) {
                    for (l = 0; l < populationParams->tempPopulation[i].t[j].lectures[k].l_subject->roomRequireLength; l++) {
                        if (&populationParams->tempPopulation[i].t[j].lectures[k].l_subject->roomRequire[l] == &populationParams->tempPopulation[i].t[j].lectures[k].l_room) {
                            count1++;
                        }
                    }
                } else {
                    count1++;
                }
            }
            count2 += populationParams->tempPopulation[i].t[j].lectureLength;
        }
        value1 = (float)count1;
        value2 = (float)count2;
        populationParams->tempPopulation[i].fitness += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
        populationParams->akkFitnessPoints += (int) ((value1 / value2) * FLOAT_MULTIPLIER);
    }

}


/*
void compareLectures(individual *ind, int *consecutiveLectures);
*/

/*
void calcFitnessOnPopulation(params *populationParams) {
      int i;
      int *consecutiveLectures;
      consecutiveLectures = calloc(MAX_INDIVIDUALS,sizeof(int));
      populationParams->akkConflicts = 0;
      for(i = 0; i < populationParams->tempPopulationCount-1; i++){
          conflicts(&populationParams->tempPopulation[i], populationParams->classCount);
        populationParams->akkConflicts += populationParams->tempPopulation[i].conflicts;
        if (populationParams->tempPopulation[i].conflicts > populationParams->biggestConflicts){
              populationParams->biggestConflicts = populationParams->tempPopulation[i].conflicts;
        }
        compareLectures(&populationParams->tempPopulation[i], consecutiveLectures[i]);
      }
      setFitness(populationParams);
}
*/

/*
void compareLectures(individual *ind, int *consecutiveLectures){
      int a,b,c; *//* iteration counters *//*
      int doubleLectures = 0;


    for (a = 0; a < MAX_TIMETABLES; a++){
        for (b = 0; b < MAX_LECTURES; b++){
            c = b + 1;
            if(ind->t[a].lectures[b].l_datetime.dayOfWeek == ind->t[a].lectures[c].l_datetime.dayOfWeek &&
            strcmp(ind->t[a].lectures[b].l_room->name,ind->t[a].lectures[c].l_room->name) == 0 &&
            ind->t[a].lectures[b].l_subject == ind->t[a].lectures[c].l_subject &&
            ind->t[a].lectures[b].l_teacher == ind->t[a].lectures[c].l_teacher){
                doubleLectures += 1;
            }
        }
    }
    *consecutiveLectures = doubleLectures;
}

*/