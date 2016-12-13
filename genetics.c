
individuals crossover(individuals *p1, individuals *p2, int classCount){
	int i,p,c,d,l;
	individuals n;
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
				swapn(&p1->t[c].day[d].lectures[l], &p2->t[c].day[d].lectures[l], sizeof(lecture));

				if(first){
					n.t[c].day[d].lectures[l] = p1->t[c].day[d].lectures[l];
				}else{
					n.t[c].day[d].lectures[l] = p2->t[c].day[d].lectures[l];
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


void mutate(induvidual *i){
	/* TODO
	 * vælg valg der skal muteres
	 * vælge hvordan det skal muterer
	 * muterer det
	 * returner og afslut
	 */
	int amountOfMutations = randomNumber(1, MAX_MUTATIONS);
	weapon_x(i, amountOfMutations);
}

void weapon_x(individual *i, int amountOfMutations){
	if(amountOfMutations > 1){
		return 1;
	}
	injectSerumX(i);
	weapon_x(i, (amountOfMutations-1));
}

void injectSerumX(individual *i){
	int ingredient = randomNumber(1,3);
	switch(ingredient){
		case 1:
			addSugar(i);
		break;
		case 2:
			addSpice(i);
		break;
		case 3:
			addEverythingNice(i);
		break;
	}
}

void addSugar(individual *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding sugar");
}


void addSpice(individual *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding spice");
}


void addEverythingNice(individual *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding Everything Nice");
}
