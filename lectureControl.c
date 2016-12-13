int isRoomCorrect(lecture *l);
int isTeacherCorrect(lecture *l);
lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
lecture randomLectureForClass(room *rooms, int roomCount, subject *subjects, int subjectCount, teacher *teachers, int teacherCount, class *curClass);
lecture randomLectureForClassAndSubject(room *rooms, int roomCount, teacher *teachers, int teacherCount, class *curClass, subject *curSubject);

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

	while((r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)]) && r_lecture.l_subject->perYear[r_lecture.l_class->year+1] == 0){
		r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)];
	}

	r_lecture = randomLectureForClassAndSubject(rooms, roomCount, teachers, teacherCount, &classes[randomNumber(0,classCount-1)], r_lecture.l_subject);

	return r_lecture;
}

lecture randomLectureForClassAndSubject(room *rooms, int roomCount, teacher *teachers, int teacherCount, class *curClass, subject *curSubject){
	int i;
	int shouldBreak = 0;
	lecture r_lecture;
	memset(&r_lecture, '\0', sizeof(lecture));
	r_lecture.l_class = curClass;
	r_lecture.l_subject = curSubject;

	/*if(randomNumber(0,100) <= FREE_LECTURE_CH){
		r_lecture.free = 1;
		return r_lecture;
	}*/

	if(r_lecture.l_subject->roomRequireLength > 0){
		r_lecture.l_room = r_lecture.l_subject->roomRequire[randomNumber(0,r_lecture.l_subject->roomRequireLength-1)];
	}else{
		r_lecture.l_room = &rooms[randomNumber(0,roomCount-1)];
	}

	while((r_lecture.l_teacher = &teachers[randomNumber(0, teacherCount-1)])){
		for(i = 0; i < r_lecture.l_teacher->canTeachLength; i++){
			if(strcmp(r_lecture.l_teacher->canTeach[i]->name, r_lecture.l_subject->name) == 0){
				shouldBreak = 1;
				break;
			}
		}

		if(shouldBreak){
			shouldBreak = 0;
			break;
		}
	}

	r_lecture.free = 0;
	r_lecture.init = 0;


	return r_lecture;
}

lecture randomLectureForClass(room *rooms, int roomCount, subject *subjects, int subjectCount, teacher *teachers, int teacherCount, class *curClass){
	int i;
	int shouldBreak = 0;
	lecture r_lecture;

	r_lecture.l_class = curClass;

	if(randomNumber(0,100) <= FREE_LECTURE_CH){
		r_lecture.free = 1;
		return r_lecture;
	}

	while((r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)]) && r_lecture.l_subject->perYear[r_lecture.l_class->year+1] == 0){
		r_lecture.l_subject = &subjects[randomNumber(0,subjectCount-1)];
	}

	if(r_lecture.l_subject->roomRequireLength > 0){
		r_lecture.l_room = r_lecture.l_subject->roomRequire[randomNumber(0,r_lecture.l_subject->roomRequireLength-1)];
	}else{
		r_lecture.l_room = &rooms[randomNumber(0,roomCount-1)];
	}

	while((r_lecture.l_teacher = &teachers[randomNumber(1,teacherCount)-1])){
		for(i = 0; i < r_lecture.l_teacher->canTeachLength; i++){
			if(strcmp(r_lecture.l_teacher->canTeach[i]->name, r_lecture.l_subject->name) == 0){
				shouldBreak = 1;
				break;
			}
		}

		if(shouldBreak){
			shouldBreak = 0;
			break;
		}
	}

	r_lecture.free = 0;
	r_lecture.init = 0;

	return r_lecture;
}

