void printLecture(lecture l);
char *autoPadding(char *header, int width);
char *initials(char *name);

/**
 * Returns a padded header instead of left alligned
 * @param  header the text to padd to center
 * @param  width  width of the table
 * @return        returns padded string
 */
char *autoPadding(char *header, int width){
    int i = 0;
    int padding = 0;
    char *newHeader;

    int len = strlen(header);
    newHeader = malloc(width * sizeof(char));

    padding = width/2 -len / 2;

    for (i = 0; i < padding; i++){
        newHeader[i] = ' ';
    }

    strcpy(&(newHeader[padding]),header);

    return newHeader;
}

/**
 * Prints core elements of a lecture
 * @param l lecture to print
 */
void printLecture(lecture l){
	char *requirements = calloc((l.l_subject->roomRequireLength) * 7, sizeof(char));
	int i;
	for (i = 0; i < l.l_subject->roomRequireLength; i++){
		if(i!=0)
			strcat(requirements,", ");
		strcat(requirements,l.l_subject->roomRequire[i]->name);
	}

	printf("| %d | %3s | %-7s | %-17s | %-25s | %-14s |\n",
		l.l_datetime.dayOfWeek,
		l.l_class->name,
		l.l_room->name,
		l.l_subject->name,
		l.l_teacher->name,
		(l.l_subject->roomRequireLength != 0 ? requirements : "*")
	);

	free(requirements);
}

void printTimeTable(timetable t){
	char rows[MAX_LECTURES*3][1024];
	int i,j;
	char temp[20];

	for (j = 0; j < MAX_LECTURES; j++){
		for (i = 0; i < WEEK_LENGTH; i++){
			if(i == 0){
				sprintf(temp,"| %-6d |",
					j);
				strcat(rows[j*3+0],"|        |");
				strcat(rows[j*3+1],temp);
				strcat(rows[j*3+2],"|--------|");
			}

			if(j < t.day[i].lectureLength){
				sprintf(temp,"| %-16s |",
					t.day[i].lectures[j].l_subject->name);

				strcat(rows[j*3+0],temp);

				sprintf(temp,"| %-16s |",
					t.day[i].lectures[j].l_teacher->name);

				strcat(rows[j*3+1],temp);

				strcat(rows[j*3+2],"|------------------|");

				
			}else{
				strcat(rows[j*3+0],"|                  |");
				strcat(rows[j*3+1],"|                  |");
				strcat(rows[j*3+2],"|                  |");
			}

			if(i+1 == WEEK_LENGTH){
				strcat(rows[j*3+0],"\n");
				strcat(rows[j*3+1],"\n");
				strcat(rows[j*3+2],"\n");
			}
		}
	}

	printf("\n--------------------------------------------------------------------------------------------------------------\n"
		     "| Tid    ||      Mandag      ||      Tirsdag     ||      Onsdag      ||     Torsdag      ||      Fredag      |\n"
		     "--------------------------------------------------------------------------------------------------------------\n");
	for (i = 0; i < MAX_LECTURES*3; i++){
		printf("%s", rows[i]);
	}
}

char *initials(char *name){
	char *inital = malloc(3*sizeof(char));
	int i=0;
	int n=0;
    while(name[i]!='\0'){
       if(name[i]==' '){
            i++;
            inital[n++] = *(name+i);
       }
       i++;
   }

   return inital;
}