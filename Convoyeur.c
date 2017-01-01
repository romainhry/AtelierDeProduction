#include "Convoyeur.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


void erreur(const char *msg)
{
  perror(msg);
  exit(1);
}

//initialisation du convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur)
{
	myConvoyeur->first = NULL;
	myConvoyeur->last = NULL;
  myConvoyeur->curseur = NULL;
	if(pthread_mutex_init(&myConvoyeur->mtx, NULL) == -1)
	{
		erreur("Initialisation du mutex");
	}
}

//ajout d'une piece sur le convoyeur
void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur)
{
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
		erreur("Verrouillage du mutex");
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
		erreur("Déverrouillage du mutex");
	}
}

//lecture et suppression d'un maillon en début de myConvoyeur
struct maillon* retire_convoyeur(struct convoyeur* myConvoyeur,int op)
{
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
		erreur("Verrouillage du mutex");
	}
	struct maillon* m;
  struct maillon* tmp= NULL;
	m = myConvoyeur->first;
	while(m->obj.typePiece != op)
	{
    tmp = m;
    m = m->next;
	}
  if(tmp != NULL) {
    tmp->next = m->next;
    if(myConvoyeur->curseur==m)
      myConvoyeur->curseur=tmp;
  }
  else {
    myConvoyeur->first=NULL;
  }
	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
		erreur("Déverrouillage du mutex");
	}
	return m;
}

//lecture d'un maillon en début de myConvoyeur
int typePiece_convoyeur(struct convoyeur* myConvoyeur)
{
	if(pthread_mutex_lock(&myConvoyeur->mtx) == -1)
	{
		erreur("Verrouillage du mutex");
	}
	int res;
  if((myConvoyeur->curseur)->next!=NULL)
  {
    myConvoyeur->curseur = (myConvoyeur->curseur)->next;
  }
	res = (myConvoyeur->curseur)->obj.typePiece;
	if(pthread_mutex_unlock(&myConvoyeur->mtx) == -1)
	{
		erreur("Déverrouillage du mutex");
	}
	return res;
}
