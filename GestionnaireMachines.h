#ifndef GESTIONNAIREMACHINES_H_
#define GESTIONNAIREMACHINES_H_

#include "Convoyeur.h"

typedef struct
{
  struct convoyeur * myConvoyeur;
  int nbPiece;
  int numeroMachine;
  int tempsUsinage;
  int typeOperation;
  int etatFonctionnement; //0= panne 1=marche
  int dispo; 		  //0= occupé 1=dispo
  pthread_mutex_t mutex;
  pthread_cond_t attendre;

} Machine;


void *fonctionnementMachine(void *machine_thread);

void creationMachines(int nbMachines, pthread_t * threads, Machine * machines, struct convoyeur * conv);


#endif
