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
#include "Affichage.h"


void *fonctionnementMachine(void *machine_thread)
{
  char MessageAfficher[200];
  char * marchePanne;
  Machine * machines=(Machine *) machine_thread;
  while(1)
  {
    pthread_mutex_lock(&machines->mutex);

		if(machines->nbPiece>=1)
		{
          sprintf(MessageAfficher,"[Machine %d] : Reveil machine et retire pièce du convoyeur",machines->numeroMachine);
          affichageConsole(machines->numeroMachine+5,MessageAfficher);

		      struct maillon* maillon;
		      maillon = retire_convoyeur(machines->myConvoyeur,machines->typeOperation);
		      machines->dispo=0;

          sprintf(MessageAfficher,"[Machine %d] : travaille %d secondes, effectue la tache : %d",machines->numeroMachine,machines->tempsUsinage,machines->typeOperation);
          affichageConsole(machines->numeroMachine+5,MessageAfficher);

          pthread_mutex_unlock(&machines->mutex);
          sleep(machines->tempsUsinage);
          pthread_mutex_lock(&machines->mutex);

          sprintf(MessageAfficher,"[Machine %d] : a fini de travailler",machines->numeroMachine);
          affichageConsole(machines->numeroMachine+5,MessageAfficher);

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
