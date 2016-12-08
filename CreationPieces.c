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
#include "CreationPieces.h"



#define NBR_PIECES	200
#define TYPE_PIECES	5


void erreur(const char *msg)
{
  perror(msg);
  exit(1);
}



//initialisation de la file
void init_file(struct tete* file) 
{
	file->first = NULL;
	file->last = NULL;
	if(pthread_mutex_init(&file->mtx, NULL) == -1) 
	{
		erreur("Initialisation du mutex");
	}
}

//ajout d'un maillon en fin de file
void ajout_file(piece pPiece, struct tete* tete) 
{
	if(pthread_mutex_lock(&tete->mtx) == -1) 
	{
		erreur("Verrouillage du mutex");
	}
	struct maillon* m = malloc(sizeof(struct maillon));
	m->next = NULL;
	m->obj = pPiece;

	if(tete->first == NULL) 
	{
		tete->first = m;
	} 
	else 
	{
		(tete->last)->next = m;
	}
	tete->last = m;
	if(pthread_mutex_unlock(&tete->mtx) == -1) 
	{
		erreur("Déverrouillage du mutex");
	}
}

//lecture et suppression d'un maillon en début de file
struct maillon* lecture_file(struct tete* tete) 
{
	if(pthread_mutex_lock(&tete->mtx) == -1) 
	{
		erreur("Verrouillage du mutex");
	}
	struct maillon* m;
	m = tete->first;
	if(tete->first != NULL) 
	{
		tete->first = (tete->first)->next;
	}
	if(pthread_mutex_unlock(&tete->mtx) == -1) 
	{
		erreur("Déverrouillage du mutex");
	}
	return m;
}

void CreationPiece(int nbrPiece,int nbrTypePieceDifferentes)
{

  int t,u,cpt=0;
  piece pieces[nbrPiece];

	for(t=0;t<(nbrPiece/nbrTypePieceDifferentes);t++) 
	{
		for(u=1;u<=nbrTypePieceDifferentes;u++) 
		{
		pieces[cpt].typePiece=u;
		cpt++;		
		}
	}

  	init_file(&fileAttente_pieces);

	for(t=0;t<nbrPiece;t++) 
	{
	ajout_file(pieces[t], &fileAttente_pieces);
	}
}
void AffichageListeChaine(int nbrPiece)
{


  piece convoyeur;
  int nb_threads_occupes = 0;
  int t;
  piece pieceRecep[nbrPiece];


	maillon_recep_piece=*lecture_file(&fileAttente_pieces);

	
	for (t=0;t<nbrPiece;t++)
	{
			if (t==0)
			{
			pieceRecep[t]=maillon_recep_piece.obj;
			printf("File d'attente :\n first [%d]=%d -> ",t,pieceRecep[t].typePiece);
			}
			else	
			{
			maillon_recep_piece = *maillon_recep_piece.next;
			pieceRecep[t]=maillon_recep_piece.obj;
			printf("[%d]=%d -> ",t,pieceRecep[t].typePiece);
			}
	}
	printf("last\n");
}

int main(int argc,char* argv[])
{

	CreationPiece(NBR_PIECES,TYPE_PIECES);	
	AffichageListeChaine(NBR_PIECES);


  return 0;
}
