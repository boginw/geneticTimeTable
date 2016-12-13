int fitness(individual ind);
void conflicts(individual *ind, int classCount);
int dublicateCount(const void *items, const size_t numberOfItems, const size_t itemSize);
int conflictsQsort(const void * a, const void * b);
individual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
individual crossover(individual *p1, individual *p2, int classCount);

int crossover_points      =  MAX_LECTURES/2;
int mutation_size         =  100;
int crossover_probability =  90;
int mutation_probability  =  10;

int fitness(individual ind){
	return 0;
}

/**
 * Counts conflicts of rooms and teachers in a single individuals
 * @param ind				individuals
 * @param classCount amount of classes
 */
void conflicts(individual *ind, int classCount){
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
	int c,d,l,s;
	int subjectIndex = 0;
	individual r_individual;
	lecture r_lecture; /* variable til at midlertidig gemme random genereret lektion, indtil de bliver placerer i et klasseskema */
	int *tempPerYear;
	tempPerYear = malloc(subjectCount * sizeof(int)); /* intierer arrayet således at der er plads til alle fag */

	memset(&r_individual,'\0',sizeof(individual));

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

		            if(tempPerYear[subjectIndex] != 0 &&
							r_individual.t[c].day[d].lectures[r_individual.t[c].day[d].lectureLength].init != 1){

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

	conflicts(&r_individual,classCount);

	free(tempPerYear);

	return r_individual;
}
