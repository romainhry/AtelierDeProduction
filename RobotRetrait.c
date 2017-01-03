#include <stdio.h>
#include <stdlib.h>

#include "RobotRetrait.h"
#include "Affichage.h"
#include "Rapport.h"

//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotRetrait(void* arg)
{
  char MessageAfficher[200];
  struct convoyeur* myConvoyeur = arg;

  sprintf(MessageAfficher,"[Robot de Retrait] : Prêt");
  affichageConsole(LigneRobotRetrait,MessageAfficher);

  while(1)
  {
    pthread_mutex_lock(&mutex_RobotRetrait);

    if(nbPieceFini>=1)
    {
      sprintf(MessageAfficher,"[Robot de Retrait] : Retire pièce du convoyeur : pièce en transit");
      affichageConsole(LigneRobotRetrait,MessageAfficher);

      struct maillon* maillon;
      maillon = retire_convoyeur(myConvoyeur,-1,0);

      sprintf(MessageAfficher,"[Robot de Retrait] : Création du rapport de la pièce [%d]", maillon->obj.identifiant);
      affichageConsole(LigneRobotRetrait,MessageAfficher);

      sprintf(MessageAfficher,"Pièce [%d] de type : %d Temps d'usinage : %d\n",maillon->obj.identifiant,maillon->obj.typePiece,maillon->obj.tempsUsinage);
      EcrireRapport(MessageAfficher);

      //libère ressource
      free(maillon);


      nbPieceFini--;

      sprintf(MessageAfficher,"[Robot de Retrait] : Prêt");
      affichageConsole(LigneRobotRetrait,MessageAfficher);
     }
     else
     {
          pthread_cond_wait(&attendre_RobotRetrait,&mutex_RobotRetrait);
     }

     pthread_mutex_unlock(&mutex_RobotRetrait);
  }
  pthread_exit(NULL);
}
