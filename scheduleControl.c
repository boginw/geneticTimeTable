

int fitness(individual ind){
	return 0;
}

/**
 * Counts conflicts of rooms and teachers in a single individuals
 * @param ind				individuals
 * @param classCount amount of classes
 */
void conflicts(individual *ind, int classCount){
	int c1,l1,c2,l2,day,hour,
			conflicts = 0;

	for(c1=0; c1 < classCount; c1++){
			ind->t[c1].numOfConflicts = 0;
	    for(l1=0; l1 < ind->t[c1].lectureLength; l1++){
	    		if(!ind->t[c1].lectures[l1].init){
	    			continue;
	    		}
	        day = ind->t[c1].lectures[l1].l_datetime.dayOfWeek;
	        hour = ind->t[c1].lectures[l1].l_datetime.hour;
	        for(c2 = c1; c2 < classCount; c2++){
	            for(l2 = 0; l2 < ind->t[c2].lectureLength; l2++){
	            	if(ind->t[c2].lectures[l2].l_datetime.dayOfWeek == day && ind->t[c2].lectures[l2].l_datetime.hour==hour){
	            			ind->t[c2].lectures[l2].conflictFlag = 0;
	            			ind->t[c1].lectures[l1].conflictFlag = 0;
	            			if(ind->t[c2].lectures[l2].l_teacher == ind->t[c1].lectures[l1].l_teacher){
	            				ind->t[c2].lectures[l2].conflictFlag += TEACHER_CONFLICT;
	            				conflicts++;
	            				ind->t[c1].numOfConflicts++;
	            			}
	            			if(ind->t[c2].lectures[l2].l_class == ind->t[c1].lectures[l1].l_class){
	            				ind->t[c2].lectures[l2].conflictFlag += ROOM_CONFLICT;
	            				conflicts++;
	            				ind->t[c1].numOfConflicts++;
	            			}
	                	break;
	              }
	            }
	        }
	    }
  	}
	ind->conflicts = conflicts;
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

	return (ob->fitness - oa->fitness);
}


individual randomIndividual(params *populationParams){
	int c,day,hour,s;
	int subjectIndex = 0;
	individual r_individual;
	lecture r_lecture; /* variable til at midlertidig gemme random genereret lektion, indtil de bliver placerer i et klasseskema */
	int *hoursPerWeek;
	hoursPerWeek = calloc(populationParams->subjectCount, sizeof(int)); /* intierer arrayet således at der er plads til alle fag */
	if(hoursPerWeek == NULL){
		printf("Not enough ram, sorry...\n");
		exit(EXIT_FAILURE);
	}
	memset(&r_individual,'\0',sizeof(individual));

	/* For hvert individ op til maks antal individer */
	for (c = 0; c < populationParams->classCount; c++){
		r_individual.t[c].forClass = &populationParams->classes[c];
		/* Get all the required hours for class */
		for (s = 0; s < populationParams->subjectCount; s++){
		    hoursPerWeek[s] = ceil(
		    	populationParams->subjects[s].perYear[populationParams->classes[c].year] / ((float)SCHOOL_DAYS_YEAR / (float)WEEK_LENGTH)
		    );
		  /*  if(strcmp(classes[c].name, "1B") == 0 && hoursPerWeek[s] > 0){
		    	 printf("%s skal have %d antal timer i %s om ugen hvilket er %d om året\n", classes[c].name, hoursPerWeek[s], subjects[s].name, subjects[s].perYear[classes[c].year]);
			}*/
		}

		while(!isEmpty(hoursPerWeek,populationParams->subjectCount)){
    		day = randomNumber(0,WEEK_LENGTH-1);
    		hour = randomNumber(0,MAX_LECTURES/WEEK_LENGTH-1);
        	subjectIndex = randomNumber(0,populationParams->subjectCount-1);
        	if(hoursPerWeek[subjectIndex] > 0 && lectureOnDateTime(r_individual.t[c], day, hour) < 0){
	            r_lecture = randomLectureForClassAndSubject(
	            	populationParams,
	            	&populationParams->classes[c],
	            	&populationParams->subjects[subjectIndex]
	            );
	            r_lecture.l_datetime.dayOfWeek = day;
	            r_lecture.l_datetime.hour = hour;
	            r_lecture.init = 1;
	            /*r_individual.t[c].lectureLength += 1;*/
	            hoursPerWeek[subjectIndex] -= 1;
	            r_individual.t[c].lectures[r_individual.t[c].lectureLength++] = r_lecture;
	        }
		}
		qsort(&r_individual.t[c], MAX_LECTURES, sizeof(lecture), dayHourQsort);
	}

	conflicts(&r_individual,populationParams->classCount);

	free(hoursPerWeek);
	return r_individual;
}

int dayHourQsort(const void * a, const void * b){
	const lecture *oa = a;
	const lecture *ob = b;

	if(oa->l_datetime.dayOfWeek != ob->l_datetime.dayOfWeek){
		return oa->l_datetime.dayOfWeek - ob->l_datetime.dayOfWeek;
	}else{
		return oa->l_datetime.hour - ob->l_datetime.hour;
	}
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
