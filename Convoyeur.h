#ifndef CONVOYEUR_H_
#define CONVOYEUR_H_

#define probaDefaillant 0.02
#define capaciteConvoyeur 20

#include <pthread.h>

void erreur(const char *msg);

struct convoyeur* addConv;

typedef struct
{
  int typePiece;
  int fini;
  int identifiant;
  int tempsUsinage;
} piece;


//maillon du convoyeur contenant une piece
struct maillon
{
  struct maillon* next;
  piece obj;
};

//liste chainée du convoyeur
struct convoyeur
{
	struct maillon* first;
	struct maillon* last;
  struct maillon* curseur;
  int nbPiece;
	pthread_mutex_t mtx;
  pthread_cond_t condition;
};

//Libère les ressources du convoyeur
void libereConvoyeur();

//temps d'utilisation des bras d'alimentation/retrait
int temps(int t);

//ajout d'une piece sur le convoyeur
void init_convoyeur(struct convoyeur* myConvoyeur);

//ajout d'une piece sur le convoyeur
void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur, int tempsLimite);

//lecture et suppression d'une piece en début de myConvoyeur
struct maillon* retire_convoyeur(struct convoyeur* myConvoyeur,int op, int tempsLimite);

//lecture d'une piece en début de myConvoyeur
piece getPiece_convoyeur(struct convoyeur* myConvoyeur);
#endif
