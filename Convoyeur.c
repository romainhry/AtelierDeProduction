#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "Convoyeur.h"
#include "Affichage.h"

//initialisation du convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur)
{
  char MessageAfficher[200];
	myConvoyeur->first = NULL;
	myConvoyeur->last = NULL;
  myConvoyeur->curseur = NULL;
  srand(time(NULL));
	if(pthread_mutex_init(&myConvoyeur->mtx, NULL) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Initialisation du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
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
      kill(getpid(),SIGUSR1);
      pthread_exit(0);
    }
  }


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
