int init(params *populationParams);
int countLines(FILE *fp);
int generateInitialPopulation(params *populationParams);
void crossoverPopulation(params *populationParams);
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

void mutate(individual *i, params *populationParams);
void weapon_x(individual *i, int amountOfMutations, params *populationParams);
void injectSerumX(individual *i, params *populationParams);
void addSugar(individual *i, params *populationParams);
void addSpice(individual *i, params *populationParams);
void addEverythingNice(individual *i, params *populationParams);
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
individual randomIndividual(const params *populationParams);
int lectureOnDateTime(timetable t, int day, int hour);


int dayHourQsort(const void * a, const void * b);
