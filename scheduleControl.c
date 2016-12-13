int fitness(induvidual ind);
void conflicts(induvidual *ind, int classCount);
int dublicateCount(const void *items, const size_t numberOfItems, const size_t itemSize);
int conflictsQsort(const void * a, const void * b);
induvidual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
induvidual crossover(induvidual *p1, induvidual *p2, int classCount);

int crossover_points      =  MAX_LECTURES/2;
int mutation_size         = 100;
int crossover_probability =  90;
int mutation_probability  =  10;

int fitness(induvidual ind){
	return 0;
}

/**
 * Counts conflicts of rooms and teachers in a single induvidual
 * @param ind				induvidual
 * @param classCount amount of classes
 */
void conflicts(induvidual *ind, int classCount){
	int c,d,l;
	int conflicts = 0;

	/* Temporary holder of rooms and teachers */
	room    **dubRoom	 = calloc(classCount * 10, sizeof(room));
	teacher **dubTeacher = calloc(classCount * 10, sizeof(teacher));

	for (d = 0; d < WEEK_LENGTH; d++){
		for (l = 0; l < MAX_LECTURES; l++){
			/* Clean array before usage */
			memset(dubRoom,'\0', classCount*sizeof(room));

			/* Copy room and teacher to their respective array */
			for (c = 0; c < classCount; c++){
				if(ind->t[c].day[d].lectures[l].init != 1){
					break;
				}

				if(ind->t[c].day[d].lectures[l].free == 0){
					dubRoom[c] = ind->t[c].day[d].lectures[l].l_room;
					dubTeacher[c] = ind->t[c].day[d].lectures[l].l_teacher;
				}
			}
			if(classCount > 0){
				/* Check for dublicates in teachers and rooms */
				conflicts += dublicateCount(dubRoom,classCount,sizeof(room));
				conflicts += dublicateCount(dubTeacher,classCount,sizeof(teacher));
			}
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
 * @param	a first induvidual to compare
 * @param	b second induvidual to compare
 * @return	 whether a should come first or b
 */
int conflictsQsort(const void * a, const void * b){
	const induvidual *oa = a;
	const induvidual *ob = b;

	return (ob->conflicts - oa->conflicts);
}


induvidual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	int c,d,l,s;
	int subjectIndex = 0;
	induvidual r_individual;
	int tempPerYear = malloc(subjectCount * sizeof(int)); /* intierer arrayet således at der er plads til alle fag */

	/* For hvert individ op til maks antal individer */
	for (c = 0; c < classCount; c++){
		r_individual.t[c].forClass = &classes[c];
		/* Get all the required hours for class */
		for (s = 0; s < subjectCount; s++){
		    tempPerYear[s] = ceil(subjects[s].perYear[classes[c].year] / ((float)SCHOOL_DAYS_YEAR / (float)WEEK_LENGTH));
		}
		while(!isEmpty(tempPerYear,subjectCount)){
		    for (d = 0; d < WEEK_LENGTH; d++){
		        for (l = 0; l < MAX_LECTURES; l++){
		            subjectIndex = randomNumber(0,subjectCount-1);
		            if(tempPerYear[subjectIndex] != 0 && r_individual.t[c].day[d].lectures[r_individual.t[c].day[d].lectureLength].init != 1){
		                r_lecture = randomLectureForClassAndSubject(rooms,roomCount,teachers,teacherCount, &classes[c], &subjects[subjectIndex]);
		                r_lecture.l_datetime.dayOfWeek = d;
		                r_lecture.l_datetime.hour = l;
		                r_lecture.init = 1;
		                tempPerYear[subjectIndex]--;
		                r_individual.t[c].day[d].lectures[r_individual.t[c].day[d].lectureLength++] = r_lecture;
		            }
		        }
		    }
		}
	}

	free(tempPerYear);

	return r_individual;
}
