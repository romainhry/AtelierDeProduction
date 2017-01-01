#ifndef CONVOYEUR_H_
#define CONVOYEUR_H_


#include <pthread.h>

void erreur(const char *msg);

typedef struct
{
  int typePiece;
} piece;


//maillon du convoyeur contenant une piece
struct maillon
{
  struct maillon* next;
  piece obj;
};

//convoyeur du convoyeur
struct convoyeur
{
	struct maillon* first;
	struct maillon* last;
  struct maillon* curseur;
	pthread_mutex_t mtx;
};


//ajout d'une piece sur le convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur);

//ajout d'une piece sur le convoyeur
void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur);

//lecture et suppression d'une piece en début de myConvoyeur
struct maillon* retire_convoyeur(struct convoyeur* myConvoyeur,int op);

//lecture d'une piece en début de myConvoyeur
int typePiece_convoyeur(struct convoyeur* myConvoyeur);
#endif
