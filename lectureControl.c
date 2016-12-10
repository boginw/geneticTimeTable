int isRoomCorrect(lecture *l);
int isTeacherCorrect(lecture *l);

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
