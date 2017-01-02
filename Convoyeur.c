#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "Convoyeur.h"
#include "Affichage.h"

//initialisation du convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur)
{
  char MessageAfficher[200];
	myConvoyeur->first = NULL;
	myConvoyeur->last = NULL;
  myConvoyeur->curseur = NULL;
	if(pthread_mutex_init(&myConvoyeur->mtx, NULL) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Initialisation du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
}


//ajout d'une piece sur le convoyeur
void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur)
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
	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
}


//lecture et suppression d'un maillon en début de myConvoyeur
struct maillon* retire_convoyeur(struct convoyeur* myConvoyeur,int op)
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
	while(m->obj.typePiece != op)
	{
    tmp = m;
    m = m->next;
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


	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    char MessageAfficher[200];
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}

	return m;
}

//lecture d'un maillon en début de myConvoyeur
int typePiece_convoyeur(struct convoyeur* myConvoyeur)
{
  char MessageAfficher[200];
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Verrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
	int res;
  if((myConvoyeur->curseur)->next!=NULL)
  {
    myConvoyeur->curseur = (myConvoyeur->curseur)->next;
  }
	res = (myConvoyeur->curseur)->obj.typePiece;
	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Déverrouillage du mutex");
    affichageConsole(LigneErreur,MessageAfficher);
	}
	return res;
}
