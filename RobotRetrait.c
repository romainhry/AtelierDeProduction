#include <stdio.h>

#include "RobotRetrait.h"
#include "Affichage.h"

//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotRetrait(void* arg)
{
  char MessageAfficher[200];
  struct convoyeur* myConvoyeur = arg;
  while(1)
  {
    pthread_mutex_lock(&mutex_RobotRetrait);


    if(nbPieceFini>=1)
    {
      sprintf(MessageAfficher,"[Robot de Retrait] : Retire pièce du convoyeur : pièce en transit");
      affichageConsole(LigneRobotRetrait,MessageAfficher);

      struct maillon* maillon;
      maillon = retire_convoyeur(myConvoyeur,-1,0);

      sprintf(MessageAfficher,"[Robot de Retrait] : Création du rapport");
      affichageConsole(LigneRobotRetrait,MessageAfficher);

      // TODO : Creation rapport

      nbPieceFini--;
     }
     else
     {
          pthread_cond_wait(&attendre_RobotRetrait,&mutex_RobotRetrait);
     }

     pthread_mutex_unlock(&mutex_RobotRetrait);
  }
  pthread_exit(NULL);
}
