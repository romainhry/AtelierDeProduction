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
#include "RobotAlimentation.h"
#include "Rapport.h"


void *fonctionnementMachine(void *machine_thread)
{
  char MessageAfficher[200];
  char * marchePanne;
  piece myPiece;
  Machine * machines=(Machine *) machine_thread;
  srand(time(NULL));
  while(1)
  {
    pthread_mutex_lock(&machines->mutex);

		if(machines->nbPiece>=1)
		{
      if(machines->etatFonctionnement==PANNE)
      {
        sprintf(MessageAfficher,"[Information] : Pièce déstinée à machine en panne : defaillance");
        affichageConsole(LigneInformation,MessageAfficher);

        sprintf(MessageAfficher,"Pièce déstinée à [Machine %d] en panne : defaillance\n",machines->numeroMachine);
        EcrireRapport(MessageAfficher);

        kill(getpid(),SIGUSR1);
        pthread_exit(0);
      }

      struct maillon* maillon;

      sprintf(MessageAfficher,"[Machine %d] : Retire pièce du convoyeur : pièce en transit",machines->numeroMachine);
      affichageConsole(ligneMachine,MessageAfficher);

      pthread_mutex_unlock(&machines->mutex);
      maillon = retire_convoyeur(machines->myConvoyeur,machines->typeOperation,tempsLimiteRetrait);
      pthread_mutex_lock(&machines->mutex);

      machines->dispo=0;

      sprintf(MessageAfficher,"[Machine %d] : Travaille sur la pièce [%d]",machines->numeroMachine, maillon->obj.identifiant);
      affichageConsole(ligneMachine,MessageAfficher);

      //Génere panne machine au hasard
      int t = machines->numeroMachine * 1000000;
      int temps = rand()%((int)(tempsLimiteTravail*1000000+(probaMachineDefaillant*tempsLimiteTravail*1000000))-t)+t;

      //Verif panne machine
      if(temps > tempsLimiteTravail*1000000)
      {
        pthread_mutex_unlock(&machines->mutex);
        sleep(tempsLimiteTravail);
        pthread_mutex_lock(&machines->mutex);

        sprintf(MessageAfficher,"[Machine %d] : Arrette de travailler, temps de travail trop élevé",machines->numeroMachine);
        affichageConsole(ligneMachine,MessageAfficher);

        sprintf(MessageAfficher,"[Machine %d] : Arrette de travailler, temps de travail trop élevé\n",machines->numeroMachine);
        EcrireRapport(MessageAfficher);

        sprintf(MessageAfficher,"[Machine %d] : En panne",machines->numeroMachine);
        affichageConsole(ligneMachine,MessageAfficher);

        sprintf(MessageAfficher,"[Machine %d] : En panne\n",machines->numeroMachine);
        EcrireRapport(MessageAfficher);

        machines->etatFonctionnement=PANNE;
        maillon->obj.fini=-1;
        maillon->obj.tempsUsinage=tempsLimiteTravail;


      }
      else
      {
        pthread_mutex_unlock(&machines->mutex);
        usleep(temps);
        pthread_mutex_lock(&machines->mutex);
        myPiece = maillon->obj;
        myPiece.fini=1;
        sprintf(MessageAfficher,"[Machine %d] : A travaillé %d secondes",machines->numeroMachine,temps/1000000);
        affichageConsole(ligneMachine,MessageAfficher);

        sprintf(MessageAfficher,"[Machine %d] : Pièce [%d] en transit vers convoyeur...",machines->numeroMachine,myPiece.identifiant);
  	    affichageConsole(ligneMachine,MessageAfficher);

        //pthread_mutex_unlock(&machines->mutex);
  			alimente_convoyeur(myPiece, machines->myConvoyeur, 0);
        //pthread_mutex_lock(&machines->mutex);

  			sprintf(MessageAfficher,"[Machine %d] : Pièce [%d] déposée...",machines->numeroMachine,myPiece.identifiant);
  	    affichageConsole(ligneMachine,MessageAfficher);

        v(semid); // signal au superviseur que la piece est posée

        sprintf(MessageAfficher,"[Machine %d] : Prête",machines->numeroMachine);
        affichageConsole(ligneMachine,MessageAfficher);

      }

      machines->nbPiece--;
      machines->dispo=1;

      //libère ressource
      free(maillon);
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


    //Création d'un moniteur par machine
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
