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

		for (l = 0; l < MAX_LECTURES; l++){
			memset(cp,0,MAX_LECTURES*sizeof(int));

			for(i = crossover_points; i > 0; i--){
				while( 1 ){
					p = randomNumber(0,MAX_LECTURES-1);
					if( !cp[ p ] ){
						cp[ p ] = 1;
						break;
					}
				}
			}

			if(first){
				if(child1->t[c].lectures[l].init != 1 && child2->t[c].lectures[l].init != 1){
					continue;
				}
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
  int i, akk = 0, fitnessRatio;
  int maxConflicts = populationParams->tempPopulation[populationParams->tempPopulationCount - 1].conflicts;
  for (i = 0; i < populationParams->tempPopulationCount; i++){
      akk += (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
  }
  for (i = 0; i < populationParams->tempPopulationCount; i++){
      fitnessRatio = (((maxConflicts - populationParams->tempPopulation[i].conflicts) / (float) maxConflicts)) * 100;
      fitnessRatio = fitnessRatio / (float) akk * 100;
      populationParams->tempPopulation[i].fitness = fitnessRatio;
  }
}
