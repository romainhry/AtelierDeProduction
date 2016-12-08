#ifndef GESTIONNAIREMACHINES_H_
#define GESTIONNAIREMACHINES_H_

typedef struct
{

  int numeroMachine;
  int tempsUsinage;
  int typeOperation;
  int etatFonctionnement; //0= panne 1=marche
  int dispo; //0= occupé 1=dispo
  pthread_mutex_t mutex_sync;

} Machine;


void *fonctionnementMachine(void *machine_thread);

void creationMachines(int nbMachines, pthread_t * threads, Machine * machines);

#endif
