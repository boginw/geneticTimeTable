int dubCount(void *a, size_t items, size_t size);

int fitness(induvidual ind){
	return 0;
}

void conflicts(induvidual *ind){
	int c,d,l;
	int conflicts = 0;
	room **dubRoom    = calloc(MAX_CLASSES*10,sizeof(room));
	teacher **dubTeacher = calloc(MAX_CLASSES*10,sizeof(teacher));

	for (d = 0; d < WEEK_LENGTH; d++){
		for (l = 0; l < MAX_LECTURES; l++){
			memset(dubRoom,'\0', MAX_CLASSES*sizeof(room));
			for (c = 0; c < MAX_CLASSES; c++){
				dubRoom[c] = ind->t[c].day[d].lectures[l].l_room;
				dubTeacher[c] = ind->t[c].day[d].lectures[l].l_teacher;
			}
			conflicts += dubCount(dubRoom,MAX_CLASSES,sizeof(room));
			conflicts += dubCount(dubTeacher,MAX_CLASSES,sizeof(teacher));
		}
	}

	ind->conflicts = conflicts;
	free(dubRoom);
}

int dubCount(void *a, size_t items, size_t size){
	int i,j;
	int conflicts = 0;
    char *x = (char *)a;

    for (i = 0; i < items - 1; i++){
    	for (j = i + 1; j < items; j++){
    		if(memcmp(&(x[i*size]), &(x[j*size]), size) == 0){
    			conflicts++;
    		}
    	}
    }

    return conflicts;
}

int conflictsQsort(const void * a, const void * b){
	const induvidual *oa = a;
	const induvidual *ob = b;

	return (oa->conflicts - ob->conflicts);
}
