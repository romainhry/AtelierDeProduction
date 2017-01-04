#ifndef AFFICHAGE_H_
#define AFFICHAGE_H_

void affichageConsole (int depLigne,char * Message);
void init_affichage(int nbrMachine);
int nombreDeMachines;
pthread_mutex_t mutexAff;


#define LigneErreur 1
#define LigneInformation nombreDeMachines+8
#define LigneRobotRetrait 3
#define LigneRobotAlim nombreDeMachines+6
#define ligneMachine nombreDeMachines+4-machines->numeroMachine

#endif
