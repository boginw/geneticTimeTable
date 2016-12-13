void weapon_x(individuals *i, int amountOfMutations);
void injectSerumX(individuals *i);
void addSugar(individuals *i);
void addSpice(individuals *i);
void addEverythingNice(individuals *i);

individual crossover(individual *p1, individual *p2, int classCount){
	int i,p,c,d,l;
	individual n;
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


void mutate(individuals *i){
	/* TODO
	 * vælg valg der skal muteres
	 * vælge hvordan det skal muterer
	 * muterer det
	 * returner og afslut
	 */
	int amountOfMutations = randomNumber(1, MAX_MUTATIONS);
	weapon_x(i, amountOfMutations);
}

void weapon_x(individuals *i, int amountOfMutations){
	if(amountOfMutations < 1){
		return;
	}
	injectSerumX(i);
	weapon_x(i, (amountOfMutations-1));
}

void injectSerumX(individuals *i){
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

void addSugar(individuals *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding sugar");
}


void addSpice(individuals *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding spice");
}


void addEverythingNice(individuals *i){
	/* This layer mutates on the top level ie. the total school timetable */
	printf("Adding Everything Nice");
}