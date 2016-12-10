void printLecture(lecture l);
char *autoPadding(char *header, int width);

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