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
#include <signal.h>

#include "GestionnaireMachines.h"
#include "Affichage.h"


#define tempsLimiteTravail 40
#define tempsLimiteRetrait 10
#define probaDefaillant 0.02
#define ligneMachine machines->numeroMachine+5

void *fonctionnementMachine(void *machine_thread)
{
  char MessageAfficher[200];
  char * marchePanne;
  Machine * machines=(Machine *) machine_thread;
  srand(time(NULL));
  while(1)
  {
    pthread_mutex_lock(&machines->mutex);
		if(machines->nbPiece>=1)
		{
          sprintf(MessageAfficher,"[Machine %d] : Retire pièce du convoyeur : pièce en transit",machines->numeroMachine);
          affichageConsole(ligneMachine,MessageAfficher);

		      struct maillon* maillon;
		      maillon = retire_convoyeur(machines->myConvoyeur,machines->typeOperation,tempsLimiteRetrait);
		      machines->dispo=0;

          sprintf(MessageAfficher,"[Machine %d] : Travaille",machines->numeroMachine);
          affichageConsole(ligneMachine,MessageAfficher);

          int t = machines->numeroMachine * 1000000;
          int temps = rand()%((int)(tempsLimiteTravail*1000000+(probaDefaillant*tempsLimiteTravail*1000000))-t)+t; //microsecondes
          if(temps > tempsLimiteTravail*1000000)
          {
            usleep(tempsLimiteTravail);
            sprintf(MessageAfficher,"[Machine %d] : Arrette de travailler, temps de travail trop élevé",machines->numeroMachine);
            affichageConsole(ligneMachine,MessageAfficher);
            kill(getpid(),SIGUSR1);
            exit(0);

          }
          else {
            usleep(temps);
          }
          sprintf(MessageAfficher,"[Machine %d] : A travaillé %d secondes",machines->numeroMachine,temps/1000000);
          affichageConsole(ligneMachine,MessageAfficher);

		      machines->nbPiece--;
		      machines->dispo=1;
     }
		 else
		 {
		      pthread_cond_wait(&machines->attendre,&machines->mutex);
		 }
     sprintf(MessageAfficher,"[Machine %d] : Prête",machines->numeroMachine);
     affichageConsole(machines->numeroMachine+5,MessageAfficher);
     pthread_mutex_unlock(&machines->mutex);
  }

  pthread_exit(NULL);
}


void creationMachines(int nbMachines, pthread_t * threads, Machine * machines , struct convoyeur * conv)
{
  char MessageAfficher[200];
  pthread_attr_t thread_attr;
  int t;


  if (pthread_attr_init (&thread_attr) != 0)
  {
    sprintf(MessageAfficher,"[Erreur] : pthread_attr_init error");
    affichageConsole(LigneErreur,MessageAfficher);
    exit (1);
  }
  if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0)
  {
    sprintf(MessageAfficher,"[Erreur] : pthread_attr_setdetachstate error");
    affichageConsole(LigneErreur,MessageAfficher);
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
      sprintf(MessageAfficher,"[Erreur] : Initialisation mutex de synchro de machine");
      affichageConsole(LigneErreur,MessageAfficher);
  		exit(1);
  	}

    if(pthread_cond_init(&machines[t].attendre, NULL) == -1)
	  {
      sprintf(MessageAfficher,"[Erreur] : Initialisation mutex d'attente de machine");
      affichageConsole(LigneErreur,MessageAfficher);
  		exit(1);
  	}

    if (pthread_create(&threads[t], &thread_attr, fonctionnementMachine, (void *)&machines[t]) == 1)
    {
      sprintf(MessageAfficher,"[Erreur] : Erreur Creation Threads");
      affichageConsole(LigneErreur,MessageAfficher);
      exit(1);
    }

  }

}
