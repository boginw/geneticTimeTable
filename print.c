

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
    newHeader = calloc(width, sizeof(char));
    if(newHeader == NULL){
    	printf("Not enough ram, sorry...\n");
      exit(EXIT_FAILURE);
    }
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
	char *requirements;
	int i;
	requirements = calloc((l.l_subject->roomRequireLength) * 7, sizeof(char));
	if(requirements == NULL){
    	printf("Not enough ram, sorry...\n");
      exit(EXIT_FAILURE);
    }
	for (i = 0; i < l.l_subject->roomRequireLength; i++){
		if(i!=0)
			strcat(requirements,", ");
		strcat(requirements,l.l_subject->roomRequire[i]->name);
	}

	printf("| %2d - %2d | %3s | %-7s | %-17s | %-25s | %-14s |\n",
		l.l_datetime.dayOfWeek,
		l.l_datetime.hour,
		l.l_class->name,
		l.l_room->name,
		l.l_subject->name,
		l.l_teacher->name,
		(l.l_subject->roomRequireLength != 0 ? requirements : "*")
	);

	free(requirements);
}

void printTimeTable(timetable t, char (*labels)[MAX_LABEL_LENGTH]){
	char rows[MAX_LECTURES * 4 + 3][1024];
	lecture tempLecture;
	int i,j,index;

	char temp[22];
	char timeTemp0[6];
	char timeTemp1[6];

	memset(rows,'\0',(MAX_LECTURES * 4 + 3)*1024);
	
	for (j = 0; j < MAX_LECTURES/WEEK_LENGTH; j++){
		strcpy(rows[j*4+0],"");
		strcpy(rows[j*4+1],"");
		strcpy(rows[j*4+2],"");
		strcpy(rows[j*4+3],"");

		for (i = 0; i < WEEK_LENGTH; i++){
			if((index = lectureOnDateTime(t,i,j)) != -1){
				tempLecture = t.lectures[index];
			}
			
			if(i == 0){
				sscanf(labels[j]," %[^,],%s",timeTemp0,timeTemp1);

				strcat(rows[j*4+0],"|        |");
				sprintf(temp,"| %5s  |",
					timeTemp0);
				strcat(rows[j*4+1],temp);

				sprintf(temp,"| %5s  |",
					timeTemp1);
				strcat(rows[j*4+2],temp);

				strcat(rows[j*4+3],"|--------|");
			}
			if(index != -1 && !tempLecture.free && tempLecture.init == 1){
				sprintf(temp,"| %-17s |",
					tempLecture.l_subject->name);

				strcat(rows[j*4+0],temp);

				sprintf(temp,"| %-17s |",
					tempLecture.l_teacher->name);

				strcat(rows[j*4+1],temp);

				sprintf(temp,"| %17s |",
					tempLecture.l_room->name);

				strcat(rows[j*4+2],temp);

				strcat(rows[j*4+3],"|-------------------|");

			}else{
				strcat(rows[j*4+0],"|                   |");
				strcat(rows[j*4+1],"|                   |");
				strcat(rows[j*4+2],"|                   |");
				if(((index = lectureOnDateTime(t,i,j+1)) != -1) && !t.lectures[index].free && t.lectures[index].init){
					strcat(rows[j*4+3],"|-------------------|");
				}else{
					strcat(rows[j*4+3],"|                   |");
				}
			}

			if(i+1 == WEEK_LENGTH){
				strcat(rows[j*4+0],"\n");
				strcat(rows[j*4+1],"\n");
				strcat(rows[j*4+2],"\n");
				strcat(rows[j*4+3],"\n");
			}
		}
	}

	printf("\n-------------------------------------------------------------------------------------------------------------------\n"
		     "| Tid    ||       Mandag      ||      Tirsdag      ||      Onsdag       ||      Torsdag      ||      Fredag       |\n"
		     "-------------------------------------------------------------------------------------------------------------------\n");
	for (i = 0; i < MAX_LECTURES*4; i++){
		printf("%s", rows[i]);
	}
}

char *initials(char *name){
	char *inital;
	int i=0;
	int n=0;
	inital = calloc(3,sizeof(char));
	if(inital == NULL){
		printf("Not enough ram, sorry...\n");
    exit(EXIT_FAILURE);
	}
    while(name[i]!='\0'){
       if(name[i]==' '){
            i++;
            inital[n++] = *(name+i);
       }
       i++;
   }

   return inital;
}

void dumpCSV(individual *ind, int classCount, char (*labels)[MAX_LABEL_LENGTH]){
	/*int c,l,d;
	FILE *fp;
	char temp[20];
	char rows[MAX_LECTURES*3][1024];

	for (c = 0; c < classCount; c++){
		sprintf(temp,"./schedules/%s.csv",ind->t[c].forClass->name);
		printf("%s\n", temp);
		fp = fopen(temp, "w+");

		fprintf(fp, "from,to,mon,teacher,room,tue,teacher,room,wed,teacher,room,thu,teacher,room,fri,teacher,room\n");
		for (l = 0; l < MAX_LECTURES; l++){
			for (d = 0; d < WEEK_LENGTH; d++){
				if(d == 0){
					strcat(rows[l],labels[l]);
					strcat(rows[l],",");
				}

				if(l < ind->t[c].day[d].lectureLength){
					sprintf(temp,"%s,",
						ind->t[c].day[d].lectures[l].l_subject->name);

					strcat(rows[l],temp);

					sprintf(temp,"%s,",
						ind->t[c].day[d].lectures[l].l_teacher->name);

					strcat(rows[l],temp);

					sprintf(temp,"%s",
						ind->t[c].day[d].lectures[l].l_room->name);

					strcat(rows[l],temp);

				}else{
					strcat(rows[l],",");
				}

				if(d+1 == WEEK_LENGTH){
					strcat(rows[l],"\n");
				}else{
					strcat(rows[l],",");
				}
			}
			fprintf(fp, "%s",rows[l]);
		}

		fclose(fp);
	}*/
}
