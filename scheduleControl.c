/**
 * @brief      Function for qSort to sort for highest fitness
 *
 * @param[in]  a     First individuals to compare
 * @param[in]  b     Second individuals to compare
 *
 * @return     Whether a should come first or b
 */
int fitnessQsort(const void * a, const void * b){
	const individual *oa = a;
	const individual *ob = b;

	return (ob->fitness - oa->fitness);
}

/**
 * @brief      Function for qSort to sort for day and time
 *             it also sort so that all init = 0 comes last
 *
 * @param[in]  a     First individuals to compare
 * @param[in]  b     Second individuals to compare
 *
 * @return     Whether a should come first or b
 */
int dayHourQsort(const void * a, const void * b){
	const lecture *oa = (lecture*) a;
	const lecture *ob = (lecture*) b;

	if(oa->init && ob->init){
		if(oa->l_datetime.dayOfWeek != ob->l_datetime.dayOfWeek){
			return oa->l_datetime.dayOfWeek - ob->l_datetime.dayOfWeek;
		}else{
			return oa->l_datetime.hour - ob->l_datetime.hour;
		}
	}else{
		if(oa->init){
			return -1;
		}
		return 1;
	}
}

/**
 * @brief      Generates a random individual
 *
 * @param      populationParams  The population parameters
 *
 * @return     The random individual
 */
individual randomIndividual(params *populationParams){
	int c,day,hour,s;
	int subjectIndex = 0;
	individual r_individual;
	lecture r_lecture;
	int *hoursPerWeek;
	hoursPerWeek = calloc(populationParams->subjectCount, sizeof(int));

	if(hoursPerWeek == NULL){
		printf("Not enough ram, sorry...\n");
		exit(EXIT_FAILURE);
	}
	memset(&r_individual,'\0',sizeof(individual));
	for (c = 0; c < populationParams->classCount; c++){
		r_individual.t[c].forClass = &populationParams->classes[c];
		for (s = 0; s < populationParams->subjectCount; s++){
			if(populationParams->subjects[s].perYear[populationParams->classes[c].year] > 0){
			    hoursPerWeek[s] = ceil(
			    	populationParams->subjects[s].perYear[populationParams->classes[c].year] / ((float)SCHOOL_DAYS_YEAR / (float)WEEK_LENGTH)
			    );
			}else{
				hoursPerWeek[s] = 0;
			}
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
	            hoursPerWeek[subjectIndex] -= 1;
	            r_individual.t[c].lectures[r_individual.t[c].lectureLength++] = r_lecture;
	        }
		}
		qsort(&r_individual.t[c], MAX_LECTURES, sizeof(lecture), dayHourQsort);
	}

	fitnessConflictsData(&r_individual, populationParams);

	free(hoursPerWeek);
	return r_individual;
}

/**
 * @brief      Checks if a lacture exists on the given day and time
 *
 * @param[in]  t     Timetable
 * @param[in]  day   The day
 * @param[in]  hour  The hour
 *
 * @return     index-key of lecture if found or -1 if no lecture where found
 */
int lectureOnDateTime(timetable t, int day, int hour){
	int l;
	for (l = 0; l < t.lectureLength; l++){
		if(t.lectures[l].init == 1 && t.lectures[l].l_datetime.dayOfWeek == day && t.lectures[l].l_datetime.hour == hour){
			return l;
		}
	}

	return -1;
}
