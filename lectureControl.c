int isRoomCorrect(lecture *l);
int isTeacherCorrect(lecture *l);
lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
lecture randomLectureForClass(room *rooms, int roomCount, subject *subjects, int subjectCount, teacher *teachers, int teacherCount, class *curClass);

int checkLecture(lecture l){
	if(!isRoomCorrect(&l)){
		return 0;
	}else if(!isTeacherCorrect(&l)){
		return 0;
	}
	return 1;
}

int isRoomCorrect(lecture *l){
	int i;

	/* Hvad fanden skal den her kode lave???☻ */
	/*if(l->l_subject->roomRequireLength > 0 && strcmp(l->l_room->name,l->l_class->classRoom->name) == 0){
		return 1;
	}*/

	/* If the lecture has a requirement for a room, make sure it is fulfilled */
	if(l->l_subject->roomRequireLength > 0){
		for(i = 0; i < l->l_subject->roomRequireLength; i++){
			if(strcmp(l->l_subject->roomRequire[i]->name, l->l_room->name) == 0){
				return 1;
			}
		}

		/* This lecture's room is not in the list of required rooms */
		return 0;
	}

	return 1;
}

int isTeacherCorrect(lecture *l){
	int i;
	for(i = 0; i < l->l_teacher->canTeachLength; i++){
		if(strcmp(l->l_teacher->canTeach[i]->name, l->l_subject->name) == 0){
			return 1;
		}
	}
	return 0;
}



lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount){
	lecture r_lecture;

	r_lecture = randomLectureForClass(rooms, roomCount, subjects, subjectCount, teachers, teacherCount, &classes[randomNumber(0,classCount-1)]);

	return r_lecture;
}

lecture randomLectureForClass(room *rooms, int roomCount, subject *subjects, int subjectCount, teacher *teachers, int teacherCount, class *curClass){
	int i;
	int shouldBreak = 0;
	lecture r_lecture;

	r_lecture.l_class = curClass;

	while((r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)]) && r_lecture.l_subject->perYear[r_lecture.l_class->year+1] == 0){
		r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)];
	}

	if(r_lecture.l_subject->roomRequireLength > 0){
		r_lecture.l_room = r_lecture.l_subject->roomRequire[randomNumber(0,r_lecture.l_subject->roomRequireLength-1)];
	}else{
		r_lecture.l_room = &rooms[randomNumber(0,roomCount-1)];
	}

	while((r_lecture.l_teacher = &teachers[randomNumber(1,teacherCount)-1])){
		/*printf("%-17s %s\n", r_lecture.l_subject->name, r_lecture.l_teacher->name);*/
		for(i = 0; i < r_lecture.l_teacher->canTeachLength; i++){
			if(strcmp(r_lecture.l_teacher->canTeach[i]->name, r_lecture.l_subject->name) == 0){
				shouldBreak = 1;
				break;
			}
		}

		/*sleep(1);*/
		if(shouldBreak){
			shouldBreak = 0;
			break;
		}
	}

	return r_lecture;
}

