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


#include "Affichage.h"


void init_affichage(int nbrMachine)
{

  if(pthread_mutex_init(&mutexAff, NULL) == -1)
  {
		perror("Initialisation mutex de synchro de machine\n");
		exit(1);
	}

  int i=0;
  printf("[Information] : \n\n");
  printf("[Robot Alimentation] : \n\n");
  for (i=0; i<nbrMachine; i++)
  {
    printf("[Machine %d] : Prête\n",i );
  }
  printf("\n[Robot de Retrait] : \n\n");
  printf("[Erreur] : Pas d'erreur\n\n");
  fflush(stdout);
}

void affichageConsole (int depLigne,char * Message)
{
  pthread_mutex_lock(&mutexAff);
  int depLigneReel=10+nombreDeMachines-depLigne;
  printf("\033[%dA",depLigneReel); //Deplacement curseur en haut
  fflush(stdout);
  printf("\033[200D");      //Retour curseur à gauche
  fflush(stdout);
  printf("\033[2K");        //Efface ligne
  fflush(stdout);
  printf("%s",Message);
  fflush(stdout);
  printf("\033[%dB",depLigneReel); //Deplacement curseur en  bas
  fflush(stdout);
  printf("\033[200D");      //Retour curseur à gauche
  fflush(stdout);
  sleep(1);
  pthread_mutex_unlock(&mutexAff);
}
