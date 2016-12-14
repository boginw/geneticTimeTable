

int crossover_points      =   10;
int mutation_size         =  100;
int crossover_probability =   90;
int mutation_probability  =   10;

individual crossover(individual *p1, individual *p2, int classCount){
	int i,p,c,l;
	individual n;
	individual oldp2 = *p2;
	int first;
	int *cp = calloc(MAX_LECTURES, sizeof(int));
	/* check probability of crossover operation */
	if( randomNumber(0,100) > crossover_probability ){
		/* no crossover, just copy first parent */
		return *p1;
	}

	/* new chromosome object, copy chromosome setup */
	n = *p1;

	/* TODO: is it safe to assume everything running? */
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
				if(p1->t[c].lectures[l].init != 1 && oldp2.t[c].lectures[l].init != 1){
					continue;
				}
				swapn(
					&p1->t[c].lectures[l],
					&oldp2.t[c].lectures[l],
					sizeof(lecture)
				);

				n.t[c].lectures[l] = p1->t[c].lectures[l];
			}else{
				n.t[c].lectures[l] = oldp2.t[c].lectures[l];
			}

			if( cp[ l ] ){
				/* change source chromosome */
				first = !first;
			}

		}
	}

	conflicts(&n,classCount);
	conflicts(p1,classCount);
	conflicts(p2,classCount);
	free(cp);
	return n;
}


void mutate(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	/* TODO
	 * vælg valg der skal muteres
	 * vælge hvordan det skal muterer
	 * muterer det
	 * returner og afslut
	 */
	int amountOfMutations = randomNumber(1, MAX_MUTATIONS);
	weapon_x(i, amountOfMutations, rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
}

void weapon_x(individual *i, int amountOfMutations, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	if(amountOfMutations < 1){
		return;
	}
	injectSerumX(i, rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
	weapon_x(i, (amountOfMutations-1), rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
}

void injectSerumX(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	int ingredient = randomNumber(1,3);
	switch(ingredient){
		case 1:
			addSugar(i, rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
		break;
		case 2:
			addSpice(i, rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
		break;
		case 3:
			addEverythingNice(i, rooms, roomCount, subjects, subjectCount, classes, classCount, teachers, teacherCount);
		break;
	}
}

void addSugar(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	/* This layer mutates on the top level ie. the total school timetable */
	int rndClass = randomNumber(0, classCount-1);
	int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
	int rndDay, rndHour;
	getRandomDatetimeWithNoLecture(&i->t[rndClass], &rndDay, &rndHour);
	i->t[rndClass].lectures[rndLec].l_datetime.dayOfWeek=rndDay;
	i->t[rndClass].lectures[rndLec].l_datetime.hour=rndHour;
}


void addSpice(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	/* This layer mutates on the top level ie. the total school timetable */
	int rndClass = randomNumber(0, classCount-1);
	int rndLec = randomNumber(0, i->t[rndClass].lectureLength-1);
	lecture *thelecture = &i->t[rndClass].lectures[rndLec];

	if(thelecture->init != 1){
		return;
	}

	thelecture->l_teacher = findRandomTeacherForSubject(thelecture, teachers, teacherCount);
}


void addEverythingNice(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	/* This layer mutates on the top level ie. the total school timetable */
	/*printf("Adding Everything Nice");*/
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
