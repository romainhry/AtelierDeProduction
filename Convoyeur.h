#ifndef CONVOYEUR_H_
#define CONVOYEUR_H_

//maillon du convoyeur contenant une piece
struct maillon{
	struct maillon* next;
  piece obj;
};

//convoyeur du convoyeur
struct convoyeur{
	struct maillon* first;
	struct maillon* last;
	pthread_mutex_t mtx;
};

void init_convoyeur(struct convoyeur* myConvoyeur);

void alimente_convoyeur(piece pPiece, struct convoyeur* myConvoyeur);
#endif
