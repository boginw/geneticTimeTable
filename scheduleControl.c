int dubCount(void *a, size_t items, size_t size);
void conflicts(induvidual *ind, int classCount);
int fitness(induvidual ind);
int dubCount(void *a, size_t items, size_t size);
int conflictsQsort(const void * a, const void * b);
induvidual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);


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
				if(ind->t[c].day[d].lectures[l].free == 0){
					dubRoom[c] = ind->t[c].day[d].lectures[l].l_room;
					dubTeacher[c] = ind->t[c].day[d].lectures[l].l_teacher;
				}
			}
			if(classCount > 0){
				/* Check for dublicates in teachers and rooms */
				conflicts += dubCount(dubRoom,classCount,sizeof(room));
				conflicts += dubCount(dubTeacher,classCount,sizeof(teacher));
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
 * @return			 returns amount of dublicates/conflicts
 */
int dubCount(void *a, size_t items, size_t size){
	/* TODO: https://codereview.stackexchange.com/questions/149602/duplicate-counter-in-c */
	int i,j;
	int conflicts = 0;
		char *x = (char *)a;

		for (i = 0; i < items - 1; i++){
			for (j = i + 1; j < items; j++){
				/* Check if chunks are equal, if so count */
				if(memcmp(&(x[i*size]), &(x[j*size]), size) == 0){
					conflicts++;
				}
			}
		}

		return conflicts;
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


induvidual crossover(induvidual p1, induvidual p2, int classCount){
	int i,p,c,d,l;
	induvidual n;
	int first;
	int *cp = calloc(MAX_LECTURES, sizeof(int));
	/* check probability of crossover operation */
	if( randomNumber(0,100) > crossover_probability ){
		/* no crossover, just copy first parent */
		return p1;
	}

	/* new chromosome object, copy chromosome setup */
	n = p1;

	/* TODO - is it safe to assume everything running? */
	/* make new code by combining parent codes */

	for (c = 0; c < classCount; c++){
		/* determine crossover point (randomly) */

		for (d = 0; d < WEEK_LENGTH; d++){
			first = randomNumber(0,1);
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

			for (l = 0; l < MAX_LECTURES; l++){
				if(first){
					n.t[c].day[d].lectures[l] = p1.t[c].day[d].lectures[l];
				}else{
					n.t[c].day[d].lectures[l] = p2.t[c].day[d].lectures[l];
				}
				if( cp[ l ] ){
					/* change source chromosome */
					first = !first;
				}
			}

		}
	}

	conflicts(&n,classCount);
	free(cp);
	return n;
}

induvidual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	int c,d,l;
	induvidual ind;
	lecture r_lecture;
	ind.fitness = 0;
	for (c = 0; c < classCount; c++){
		ind.t[c].forClass = &classes[c];
		for (d = 0; d < WEEK_LENGTH; d++){
			for (l = 0; l < MAX_LECTURES; l++){
				r_lecture = randomLectureForClass(rooms,roomCount,subjects,subjectCount,teachers,teacherCount, &classes[c]);
				r_lecture.l_datetime.dayOfWeek = d;
				r_lecture.l_datetime.hour = l;
				ind.t[c].day[d].lectures[ ind.t[c].day[d].lectureLength++ ] = r_lecture;
			}
		}
    }
	conflicts(&ind,classCount);

	return ind;
}
