int init(room *rooms, int *roomCount, subject *subjects, int *subjectCount, class *classes, int *classCount, teacher *teachers, int *teacherCount, char (*labels)[MAX_LABEL_LENGTH]);
int countLines(FILE *fp);

/* PARSE FUNCTIONS */
room parseRoom(char *line);
subject parseSubject(char *line, room* rooms, int roomCount);

/* FIND FUNCTIONS */
int findRoomsFromString(char *roomString, room* roomsFound[MAX_ROOMS], room* rooms, int roomCount);
int findSubjectsFromString(char *subjectString, subject* subjectsFound[MAX_SUBJECTS], subject* subjects, int subjectCount);
room *findRoom(char *roomName, room rooms[MAX_ROOMS], int roomCount);
class *findClass(char *className, class classes[MAX_CLASSES], int classCount);


int isRoomCorrect(lecture *l);
int isTeacherCorrect(lecture *l);
lecture randomLecture(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
lecture randomLectureForClass(room *rooms, int roomCount, subject *subjects, int subjectCount, teacher *teachers, int teacherCount, class *curClass);
lecture randomLectureForClassAndSubject(room *rooms, int roomCount, teacher *teachers, int teacherCount, class *curClass, subject *curSubject);

void mutate(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void weapon_x(individual *i, int amountOfMutations, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void injectSerumX(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void addSugar(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void addSpice(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void addEverythingNice(individual *i, room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
void getRandomDatetimeWithNoLecture(timetable *t, int *day, int*hour);
teacher *findRandomTeacherForSubject(lecture *l, teacher *t, int teacherCount);
int teacherCanTeach(subject **canTeach, int canTeachLength, char *subjectName);

void printLecture(lecture l);
char *autoPadding(char *header, int width);
char *initials(char *name);
void printTimeTable(timetable t, char (*labels)[MAX_LABEL_LENGTH]);

int fitness(individual ind);
void conflicts(individual *ind, int classCount);
int dublicateCount(const void *items, const size_t numberOfItems, const size_t itemSize);
int conflictsQsort(const void * a, const void * b);
individual randomIndividual(room *rooms, int roomCount, subject *subjects, int subjectCount, class *classes, int classCount, teacher *teachers, int teacherCount);
int lectureOnDateTime(timetable t, int day, int hour);


int dayHourQsort(const void * a, const void * b);