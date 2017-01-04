#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "Convoyeur.h"
#include "Affichage.h"
#include "Rapport.h"

//initialisation du convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur)
{
  char MessageAfficher[200];
  addConv=myConvoyeur;
	myConvoyeur->first = NULL;
	myConvoyeur->last = NULL;
  myConvoyeur->curseur = NULL;
  myConvoyeur->nbPiece =0;

  srand(time(NULL));
	if(pthread_mutex_init(&myConvoyeur->mtx, NULL) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Initialisation du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}

  if(pthread_cond_init(&myConvoyeur->condition, NULL) == -1)
  {
    sprintf(MessageAfficher,"[Erreur] : Initialisation mutex de condition d'acces au convoyeur'");
    affichageConsole(LigneErreur,MessageAfficher);
    exit(1);
  }
}


//ajout d'une piece sur le convoyeur
void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur, int tempsLimite)
{
  char MessageAfficher[200];

	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Verrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
  }

  if(pPiece.fini==1)
  {
    if(myConvoyeur->nbPiece==capaciteConvoyeur) // pas de defaillance si tempsLimite=0;
    {
      sprintf(MessageAfficher,"[Information] : Convoyeur surchargé : attente avant alimentation");
      affichageConsole(LigneInformation,MessageAfficher);
      sprintf(MessageAfficher,"[Information] : Reprise d'alimentation");
      affichageConsole(LigneInformation,MessageAfficher);
      pthread_cond_wait(&myConvoyeur->condition,&myConvoyeur->mtx);
    }
  }
  else {
    if(myConvoyeur->nbPiece==capaciteConvoyeur-1) // pas de defaillance si tempsLimite=0;
    {
      sprintf(MessageAfficher,"[Information] : Convoyeur surchargé : attente avant alimentation");
      affichageConsole(LigneInformation,MessageAfficher);
      pthread_cond_wait(&myConvoyeur->condition,&myConvoyeur->mtx);
    }
  }


  myConvoyeur->nbPiece++;

	struct maillon* m = malloc(sizeof(struct maillon));
	m->next = NULL;
	m->obj = pPiece;
	if(myConvoyeur->first == NULL)
	{
		myConvoyeur->first = m;
    myConvoyeur->curseur = m;

	} else {
		(myConvoyeur->last)->next = m;
	}
	myConvoyeur->last = m;

  if(tempsLimite!=0) // pas de defaillance si tempsLimite=0;
  {
    //Génere defaillance bras d'alimentation de la machine au hasard
    if(temps(tempsLimite)==0)
    {
      sprintf(MessageAfficher,"[Information] : Bras d'alimentation bloqué trop longtemps");
      affichageConsole(LigneInformation,MessageAfficher);
      sprintf(MessageAfficher,"\nBras d'alimentation bloqué trop longtemps : Défaillance\n");
      EcrireRapport(MessageAfficher);
      kill(getpid(),SIGUSR1);
    }
  }

	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
}

//temps d'utilisation des bras d'alimentation/retrait
int temps(int t)
{
  t = t * 1000000;
  int temps = rand()%(int)(t+(probaDefaillant*t)); //microsecondes
  if(temps > t)
  {
    usleep(t);
    return 0;

  }
  else {
    usleep(temps);
    return 1;
  }
}

void libereConvoyeur() {
 if(addConv->first !=NULL) {
   struct maillon * m = addConv->first;
   struct maillon * tmp;
   while(m!=addConv->last)
   {
     tmp = m->next;
     free(m);
     m = tmp;
   }
   free(m);
 }
}


//lecture et suppression d'un maillon en début de myConvoyeur
struct maillon* retire_convoyeur(struct convoyeur* myConvoyeur,int op, int tempsLimite)
{
  char MessageAfficher[200];
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Verrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
	struct maillon* m;
  struct maillon* tmp= NULL;
	m = myConvoyeur->first;
  myConvoyeur->nbPiece--;

  //Cherche la pièce en question
  if(tempsLimite!=0) //cherche en fonction de son operation
  {
  	while(m->obj.typePiece != op)
  	{
      tmp = m;
      m = m->next;
  	}
  }
  else{     //cherche une pièce fini
    while(m->obj.fini != 1)
  	{
      tmp = m;
      m = m->next;
  	}
  }

  //Reforme la chaine
  if(myConvoyeur->first==m) {
    myConvoyeur->first=m->next;
  }
  else
  {
    if(myConvoyeur->last==m)
    {
      myConvoyeur->last= tmp;
    }
    tmp->next = m->next;
    if(myConvoyeur->curseur==m)
      myConvoyeur->curseur=tmp;
  }

  if(tempsLimite!=0) //Génere defaillance bras de retrait de la machine au hasard
  {
    if(temps(tempsLimite)==0)
    {
      sprintf(MessageAfficher,"[Information] : Bras de retrait de la machine %d bloqué trop longtemps",op);
      affichageConsole(LigneInformation,MessageAfficher);
      sprintf(MessageAfficher,"\nBras de retrait de la machine %d bloqué trop longtemps\n",op);
      EcrireRapport(MessageAfficher);

      free(m);

      kill(getpid(),SIGUSR1);
      pthread_exit(0);
    }
  }

  pthread_cond_signal(&myConvoyeur->condition);

	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    char MessageAfficher[200];
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}

	return m;
}

//lecture d'un maillon en début de myConvoyeur
piece getPiece_convoyeur(struct convoyeur* myConvoyeur)
{
  char MessageAfficher[200];
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Verrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}

	piece res;
  if((myConvoyeur->curseur)->next!=NULL)
  {
    myConvoyeur->curseur = (myConvoyeur->curseur)->next;
  }
	res = (myConvoyeur->curseur)->obj;

	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
	return res;
}
