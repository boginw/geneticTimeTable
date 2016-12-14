

int crossover_points      =   10;
int mutation_size         =  100;
int crossover_probability =   90;
int mutation_probability  =   10;

individual crossover(individual *p1, individual *p2, int classCount){
	int i,p,c,l;
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

	first = randomNumber(0,1);
	for (c = 0; c < classCount; c++){
		/* determine crossover point (randomly) */

		for (l = 0; l < MAX_LECTURES; l++){
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


			if(first){
				if(p1->t[c].lectures[l].init != 1 && p2->t[c].lectures[l].init != 1){
					continue;
				}
				swapn(
					&p1->t[c].lectures[l],
					&p2->t[c].lectures[l],
					sizeof(lecture)
				);

				n.t[c].lectures[l] = p1->t[c].lectures[l];
			}else{
				n.t[c].lectures[l] = p2->t[c].lectures[l];
			}

			if( cp[ l ] ){
				/* change source chromosome */
				first = !first;
			}

		}
	}

	conflicts(&n,classCount);

	conflicts(p1,classCount);
	conflicts(p2,classCount);
	free(cp);
	return n;
}


void mutate(individual *i){
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
	if(amountOfMutations < 1){
		return;
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
	/*printf("Adding sugar");*/
}


void addSpice(individual *i){
	/* This layer mutates on the top level ie. the total school timetable */
	/*printf("Adding spice");*/
}


void addEverythingNice(individual *i){
	/* This layer mutates on the top level ie. the total school timetable */
	/*printf("Adding Everything Nice");*/
}
