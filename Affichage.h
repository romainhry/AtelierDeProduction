#ifndef AFFICHAGE_H_
#define AFFICHAGE_H_

void affichageConsole (int depLigne,char * Message);
void init_affichage(int nbrMachine);
int nombreDeMachines;
pthread_mutex_t mutexAff;


#define LigneDebug 20
#define LigneErreur 18
#define LigneInformation 1
#define LigneRobotRetrait 16
#define LigneRobotAlim 3

#endif
