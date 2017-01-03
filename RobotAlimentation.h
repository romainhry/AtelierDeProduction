#ifndef ROBOTALIMENTATION_H_
#define ROBOTALIMENTATION_H_

#include "Convoyeur.h"

#define cle 314
#define tempsLimiteRobotAlim 5


int msgid, semid ;

void p(int semid);
void v(int semid);


typedef struct
{
  long type;
  int operation;
  int nbrPiece;
}messageOperateur;

//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotAlimentation(void* arg);


#endif
