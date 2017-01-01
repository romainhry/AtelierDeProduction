#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "GestionnaireMachines.h"

#define MARCHE		1
#define PANNE		0


void *fonctionnementMachine(void *machine_thread)
{
  int testInterblocage=0;  //A supprimer à la fin des tests
  char * marchePanne;
  Machine * machines=(Machine *) machine_thread;
  while(1)
  {
    pthread_mutex_lock(&machines->mutex);

		if(machines->nbPiece>=1)
		{
		      printf("Reveil machine %d et retire pièce du convoyeur\n",machines->numeroMachine);
		      struct maillon* maillon;
		      maillon = retire_convoyeur(machines->myConvoyeur,machines->typeOperation);
		      machines->dispo=0;
		      printf("[Machine numero : %d ] va travailler : %d secondes, effectuer la tache : %d\n",machines->numeroMachine,machines->tempsUsinage,machines->typeOperation);
          pthread_mutex_unlock(&machines->mutex);
          sleep(machines->tempsUsinage);
          pthread_mutex_lock(&machines->mutex);
		      printf("[Machine numero : %d ] à fini de travailler\n",machines->numeroMachine);
		      machines->nbPiece--;
		      machines->dispo=1;

     }
		 else
		 {
		      pthread_cond_wait(&machines->attendre,&machines->mutex);
		 }
     pthread_mutex_unlock(&machines->mutex);
  }

  pthread_exit(NULL);
}


void creationMachines(int nbMachines, pthread_t * threads, Machine * machines , struct convoyeur * conv)
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
    machines[t].myConvoyeur=conv;
    machines[t].numeroMachine=t;
    machines[t].tempsUsinage=3*(t+1);
    machines[t].typeOperation=t;
    machines[t].etatFonctionnement=MARCHE;
    machines[t].dispo=1;
    machines[t].nbPiece=0;

    if(pthread_mutex_init(&machines[t].mutex, NULL) == -1)
	{
  		perror("Initialisation mutex de synchro de machine\n");
  		exit(1);
  	}

    if(pthread_cond_init(&machines[t].attendre, NULL) == -1)
	{
  		perror("Initialisation mutex d'attente de machine\n");
  		exit(1);
  	}

    if (pthread_create(&threads[t], &thread_attr, fonctionnementMachine, (void *)&machines[t]) == 1)
    {
      perror("Erreur Creation Threads\n");
      exit(-1);
    }

  }

}
