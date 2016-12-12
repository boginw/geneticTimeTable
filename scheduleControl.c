int dubCount(void *a, size_t items, size_t size);
void conflicts(induvidual *ind, int classCount);
int fitness(induvidual ind);
int dubCount(void *a, size_t items, size_t size);
int conflictsQsort(const void * a, const void * b);

int fitness(induvidual ind){
	return 0;
}

/**
 * Counts conflicts of rooms and teachers in a single induvidual
 * @param ind        induvidual
 * @param classCount amount of classes
 */
void conflicts(induvidual *ind, int classCount){
	int c,d,l;
	int conflicts = 0;

	/* Temporary holder of rooms and teachers */
	room    **dubRoom    = calloc(classCount * 10, sizeof(room));
	teacher **dubTeacher = calloc(classCount * 10, sizeof(teacher));

	for (d = 0; d < WEEK_LENGTH; d++){
		for (l = 0; l < MAX_LECTURES; l++){
			/* Clean array before usage */
			memset(dubRoom,'\0', classCount*sizeof(room));

			/* Copy room and teacher to their respective array */
			for (c = 0; c < classCount; c++){
				dubRoom[c] = ind->t[c].day[d].lectures[l].l_room;
				dubTeacher[c] = ind->t[c].day[d].lectures[l].l_teacher;
			}

			/* Check for dublicates in teachers and rooms */
			conflicts += dubCount(dubRoom,classCount,sizeof(room));
			conflicts += dubCount(dubTeacher,classCount,sizeof(teacher));
		}
	}

	ind->conflicts = conflicts;
	free(dubRoom);
	free(dubTeacher);
}

/**
 * Returns count of dublicates in an array
 * @param  a     array to check in
 * @param  items how many entries in array
 * @param  size  size of each entry
 * @return       returns amount of dublicates/conflicts
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
 * @param  a first induvidual to compare
 * @param  b second induvidual to compare
 * @return   whether a should come first or b
 */
int conflictsQsort(const void * a, const void * b){
	const induvidual *oa = a;
	const induvidual *ob = b;

	return (oa->conflicts - ob->conflicts);
}
