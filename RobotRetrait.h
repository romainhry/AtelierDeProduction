#ifndef ROBOTRETRAIT_H_
#define ROBOTRETRAIT_H_

#include "Convoyeur.h"

int nbPieceFini;

pthread_mutex_t mutex_RobotRetrait;
pthread_cond_t attendre_RobotRetrait;

//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotRetrait(void* arg);





#endif
