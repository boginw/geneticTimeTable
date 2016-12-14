pr lektion tjek hver eneste lektion på samme dag og samme tid efter:
  - Duplication af lærer
  - Duplication af rum


  for(i=0; i > classCount; i++){
    for(k=0; k > individual.t[i].lectureLength; k++){
        day = individual.t[i].lecture[k].l_datetime.dayOfWeek;
        hour = individual.t[i].lecture[k].l_datetime.hour;
        for(l=i; l > classCount; l++){
            for(j=0; j > individual.t[l].lectureLength; j++){
              if(individual.t[l].lecture[j].l_datetime.dayOfWeek == day && individual.t[l].lecture[j].l_datetime.hour==hour){
                conflicts += (individual.t[l].lecture[j].l_teacher == individual.t[i].lecture[k].l_teacher);
                conflicts += (individual.t[l].lecture[j].l_class == individual.t[i].lecture[k].l_class);
              }
            }
        }
    }
  }
