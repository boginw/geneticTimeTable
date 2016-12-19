/**
 * @brief      Generate a random lecture for a specific class and subject.
 *
 * @param      populationParams  The population parameters
 * @param      curClass          The current class
 * @param      curSubject        The current subject
 *
 * @return     The generated lecture.
 */
lecture randomLectureForClassAndSubject(params *populationParams, class *curClass, subject *curSubject){
	int i;
	int shouldBreak = 0;
	lecture r_lecture;
	memset(&r_lecture, '\0', sizeof(lecture));
	r_lecture.l_class = curClass;
	r_lecture.l_subject = curSubject;
	if(r_lecture.l_subject->roomRequireLength > 0){
		r_lecture.l_room = r_lecture.l_subject->roomRequire[randomNumber(0,r_lecture.l_subject->roomRequireLength-1)];
	}else{
		r_lecture.l_room = curClass->classRoom;
	}
	while((r_lecture.l_teacher = &populationParams->teachers[randomNumber(0,populationParams->teacherCount-1)])){
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
