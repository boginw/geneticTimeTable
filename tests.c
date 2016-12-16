

int testConflicts(params *populationParams){

    individual conflictTest2[2];

    conflictTest2[0].t[0].lectures[0].init = 1;
    conflictTest2[0].t[0].lectures[0].free = 0;
    conflictTest2[0].t[0].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[0].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[0].lectures[0].l_datetime.dayOfWeek = 1;
    conflictTest2[0].t[0].lectures[0].l_datetime.hour = 0;
    conflictTest2[0].t[0].lectureLength = 1;

    conflictTest2[0].t[1].lectures[0].init = 1;
    conflictTest2[0].t[1].lectures[0].free = 0;
    conflictTest2[0].t[1].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[1].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[1].lectures[0].l_datetime.dayOfWeek = 0;
    conflictTest2[0].t[1].lectures[0].l_datetime.hour = 0;
    conflictTest2[0].t[1].lectureLength = 1;

    conflictTest2[0].t[2].lectures[0].init = 1;
    conflictTest2[0].t[2].lectures[0].free = 0;
    conflictTest2[0].t[2].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[2].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[2].lectures[0].l_datetime.dayOfWeek = 1;
    conflictTest2[0].t[2].lectures[0].l_datetime.hour = 0;
    conflictTest2[0].t[2].lectureLength = 1;

    conflictTest2[0].t[3].lectures[0].init = 1;
    conflictTest2[0].t[3].lectures[0].free = 0;
    conflictTest2[0].t[3].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[3].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[3].lectures[0].l_datetime.dayOfWeek = 0;
    conflictTest2[0].t[3].lectures[0].l_datetime.hour = 0;
    conflictTest2[0].t[3].lectureLength = 1;

    conflictTest2[0].t[4].lectures[0].init = 1;
    conflictTest2[0].t[4].lectures[0].free = 0;
    conflictTest2[0].t[4].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[4].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[4].lectures[0].l_datetime.dayOfWeek = 4;
    conflictTest2[0].t[4].lectures[0].l_datetime.hour = 1;
    conflictTest2[0].t[4].lectureLength = 1;

    conflictTest2[0].t[5].lectures[0].init = 1;
    conflictTest2[0].t[5].lectures[0].free = 0;
    conflictTest2[0].t[5].lectures[0].l_class   =  &populationParams->classes[0];
    conflictTest2[0].t[5].lectures[0].l_subject = &populationParams->subjects[0];
    conflictTest2[0].t[5].lectures[0].l_datetime.dayOfWeek = 4;
    conflictTest2[0].t[5].lectures[0].l_datetime.hour = 1;
    conflictTest2[0].t[5].lectureLength = 1;



    conflictTest2[0].t[0].lectures[0].l_room    =    &populationParams->rooms[0];
    conflictTest2[0].t[1].lectures[0].l_room    =    &populationParams->rooms[1];
    conflictTest2[0].t[2].lectures[0].l_room    =    &populationParams->rooms[2];
    conflictTest2[0].t[3].lectures[0].l_room    =    &populationParams->rooms[3];
    conflictTest2[0].t[4].lectures[0].l_room    =    &populationParams->rooms[4];
    conflictTest2[0].t[5].lectures[0].l_room    =    &populationParams->rooms[5];

    conflictTest2[0].t[0].lectures[0].l_teacher = &populationParams->teachers[5];
    conflictTest2[0].t[1].lectures[0].l_teacher = &populationParams->teachers[1];
    conflictTest2[0].t[2].lectures[0].l_teacher = &populationParams->teachers[1]; 
    conflictTest2[0].t[3].lectures[0].l_teacher = &populationParams->teachers[5];
    conflictTest2[0].t[4].lectures[0].l_teacher = &populationParams->teachers[5];
    conflictTest2[0].t[5].lectures[0].l_teacher = &populationParams->teachers[5];


    conflicts(&conflictTest2[0], 8);

    /*printTimeTable(conflictTest2[0].t[0], populationParams->intervalLabels);
    printTimeTable(conflictTest2[0].t[1], populationParams->intervalLabels);
    printTimeTable(conflictTest2[0].t[2], populationParams->intervalLabels);
    printTimeTable(conflictTest2[0].t[3], populationParams->intervalLabels);
    printTimeTable(conflictTest2[0].t[4], populationParams->intervalLabels);
    printTimeTable(conflictTest2[0].t[5], populationParams->intervalLabels);
    printf("Expected:  8, Conflicts: %2d\n", conflictTest2[0].conflicts);*/


    return conflictTest2[0].conflicts == 2;
}


int testRandomNumber(){
	int i;
	int rnd;
	for (i = 0; i < 100; i++){
		rnd = randomNumber(i - 50, i + 50);
		if(rnd < i - 50 && rnd > i + 50){
			return 0;
		}
	}

	return 1;
}

int testIsEmpty(){
	int arr1[] = {1,2,3,4,5};
	int arr2[] = {0,0,0,0,0};
	return !isEmpty(arr1, 5) && isEmpty(arr2, 5);
}