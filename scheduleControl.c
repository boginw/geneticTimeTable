

int fitness(individual ind){
	return 0;
}

/**
 * Counts conflicts of rooms and teachers in a single individuals
 * @param ind				individuals
 * @param classCount amount of classes
 */
void conflicts(individual *ind, int classCount){
	int c,l;
	int conflicts = 0;

	/* Temporary holder of rooms and teachers */
	room    **dubRoom	 = calloc(classCount * 10, sizeof(room));
	teacher **dubTeacher = calloc(classCount * 10, sizeof(teacher));

	for (l = 0; l < MAX_LECTURES; l++){
		/* Clean array before usage */
		memset(dubRoom,'\0', classCount*sizeof(room));

		/* Copy room and teacher to their respective array */
		for (c = 0; c < classCount; c++){
			if(ind->t[c].lectures[l].init != 1){
				break;
			}

			if(ind->t[c].lectures[l].free == 0){
				dubRoom[c] = ind->t[c].lectures[l].l_room;
				dubTeacher[c] = ind->t[c].lectures[l].l_teacher;
			}
		}
		if(classCount > 0){
			/* Check for dublicates in teachers and rooms */
			conflicts += dublicateCount(dubRoom,classCount,sizeof(room));
			conflicts += dublicateCount(dubTeacher,classCount,sizeof(teacher));
		}
	}


	ind->conflicts = conflicts;
	free(dubRoom);
	free(dubTeacher);
}

/**
 * Returns count of dublicates in an array
 * @param	a		 array to check in
 * @param	items how many entries in array
 * @param	size	size of each entry
 * @return			 returns amount of dublicates
 */
int dublicateCount(const void *items, const size_t numberOfItems, const size_t itemSize){
	/* TODO: https://codereview.stackexchange.com/questions/149602/duplicate-counter-in-c */
	int i,j;
	int dublicates = 0;
		char *x = (char *)items;

		for (i = 0; i < numberOfItems - 1; i++){
			for (j = i + 1; j < numberOfItems; j++){
				/* Check if chunks are equal, if so count */
				if(memcmp(&(x[i*itemSize]), &(x[j*itemSize]), itemSize) == 0){
					dublicates++;
				}
			}
		}

		return dublicates;
}

/**
 * Function for qSort to sort for lowest conflicts
 * @param	a first individuals to compare
 * @param	b second individuals to compare
 * @return	 whether a should come first or b
 */
int conflictsQsort(const void * a, const void * b){
	const individual *oa = a;
	const individual *ob = b;

	return (ob->conflicts - oa->conflicts);
}


individual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	int c,day,hour,s;
	int subjectIndex = 0;
	individual r_individual;
	lecture r_lecture; /* variable til at midlertidig gemme random genereret lektion, indtil de bliver placerer i et klasseskema */
	int *hoursPerWeek;
	hoursPerWeek = calloc(subjectCount, sizeof(int)); /* intierer arrayet således at der er plads til alle fag */
	if(hoursPerWeek == NULL){
		printf("Not enough ram, sorry...\n");
		exit(EXIT_FAILURE);
	}
	memset(&r_individual,'\0',sizeof(individual));

	/* For hvert individ op til maks antal individer */
	for (c = 0; c < classCount; c++){
		r_individual.t[c].forClass = &classes[c];
		/* Get all the required hours for class */
		for (s = 0; s < subjectCount; s++){
		    hoursPerWeek[s] = ceil(subjects[s].perYear[classes[c].year] / ((float)SCHOOL_DAYS_YEAR / (float)WEEK_LENGTH));
		    if(strcmp(classes[c].name, "1B") == 0 && hoursPerWeek[s] > 0){
		     /*printf("%s skal have %d antal timer i %s om ugen hvilket er %d om året\n", classes[c].name, hoursPerWeek[s], subjects[s].name, subjects[s].perYear[classes[c].year]);*/
				}
		}

		while(!isEmpty(hoursPerWeek,subjectCount)){ /* Makes sure to reject timetalbes that dosent meet the requried min hours per subject*/
    		day = randomNumber(0,WEEK_LENGTH-1);
    		hour = randomNumber(0,MAX_LECTURES/WEEK_LENGTH-1);
        	subjectIndex = randomNumber(0,subjectCount-1);
        	if(hoursPerWeek[subjectIndex] > 0 && lectureOnDateTime(r_individual.t[c], day, hour) == -1){
	            r_lecture = randomLectureForClassAndSubject(rooms,roomCount,teachers,teacherCount, &classes[c], &subjects[subjectIndex]);
	            r_lecture.l_datetime.dayOfWeek = day;
	            r_lecture.l_datetime.hour = hour;
	            r_lecture.init = 1;
	            /*r_individual.t[c].lectureLength += 1;*/
	            hoursPerWeek[subjectIndex] -= 1;

	            r_individual.t[c].lectures[r_individual.t[c].lectureLength++] = r_lecture;
	        }
		}
	}

	conflicts(&r_individual,classCount);

	free(hoursPerWeek);

	return r_individual;
}

int lectureOnDateTime(timetable t, int day, int hour){
	int l;
	for (l = 0; l < MAX_LECTURES; l++){
		if(t.lectures[l].init == 1 && t.lectures[l].l_datetime.dayOfWeek == day && t.lectures[l].l_datetime.hour == hour){
			return l;
		}
	}

	return -1;
}
