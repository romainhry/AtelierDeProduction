
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "GestionnaireMachines.h"

#define MARCHE		1
#define PANNE		0



void *fonctionnementMachine(void *machine_thread)
{
  char * marchePanne;
  Machine * machines=(Machine *) machine_thread;
  while(1) {
    pthread_mutex_lock(&machines->mutex_sync);
    printf("Reveil machine %d\n",machines->numeroMachine);
    //section à proteger
    machines->dispo=0;
    printf("[Machine numero : %d ] va travailler : %d secondes, effectuer la tache : %d\n",machines->numeroMachine,machines->tempsUsinage,machines->typeOperation);
    sleep(machines->tempsUsinage);
    printf("[Machine numero : %d ] à fini de travailler\n",machines->numeroMachine);
    //section à proteger
    machines->dispo=1;
  }

  pthread_exit(NULL);
}


void creationMachines(int nbMachines, pthread_t * threads, Machine * machines)
{

  pthread_attr_t thread_attr;
  int t;


  if (pthread_attr_init (&thread_attr) != 0)
  {
    perror("pthread_attr_init error\n");
    exit (1);
  }
  if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0)
  {
    perror("pthread_attr_setdetachstate error\n");
    exit (1);
  }

  for(t=0;t<nbMachines;t++)
  {
    machines[t].numeroMachine=t;
    machines[t].tempsUsinage=3*(t+1);
    machines[t].typeOperation=t;
    machines[t].etatFonctionnement=MARCHE;
    machines[t].dispo=1;

    if(pthread_mutex_init(&machines[t].mutex_sync, NULL) == -1) {
  		perror("Initialisation mutex de synchro de machine\n");
  		exit(1);
  	}

    if(pthread_mutex_lock(&machines[t].mutex_sync)==-1){
      perror("Lock mutex pour syncro de machine\n");
      exit(1);
    }
    if (pthread_create(&threads[t], &thread_attr, fonctionnementMachine, (void *)&machines[t]) == 1)
    {
      perror("Erreur Creation Threads\n");
      exit(-1);
    }

  }

}
