#ifndef RAPPORT_H_
#define RAPPORT_H_

FILE* fichierRapport;
char  nomRapport[40];
void NouveauRapport(void);
void EcrireRapport(char * MessageRapport);

pthread_mutex_t mutexRapport;

#endif
