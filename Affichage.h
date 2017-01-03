#ifndef AFFICHAGE_H_
#define AFFICHAGE_H_

void affichageConsole (int depLigne,char * Message);
void init_affichage(int nbrMachine);
int nombreDeMachines;
pthread_mutex_t mutexAff;


#define LigneErreur 2
#define LigneInformation nombreDeMachines+9
#define LigneRobotRetrait 4
#define LigneRobotAlim nombreDeMachines+7
#define ligneMachine nombreDeMachines+5-machines->numeroMachine

#endif
