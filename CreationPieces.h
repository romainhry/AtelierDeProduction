
struct tete fileAttente_pieces;
struct maillon* maillon_piece, maillon_recep_piece,maillon_curseur;

typedef struct
{
  int typePiece;
} piece;


typedef struct
{
  int typePiece;
  int nbrPiece;   
} pieceAjouter;	    //utile quand on rentre plusieurs pièce du même type d'un coup (fonction CreationPieces)



struct maillon
{
	struct maillon* next;
  	piece obj;
};

//tete de file d'attente
struct tete
{
	struct maillon* first;
	struct maillon* last;
	pthread_mutex_t mtx;
};

void erreur(const char *msg);
void init_file(struct tete* file); 
void ajout_file(piece pPiece, struct tete* tete); 
struct maillon* lecture_file(struct tete* tete);
void AffichageListeChaine(int);
void CreationPleinPiece(int nbrPiece,int nbrTypePieceDifferentes);
int CreationPiece();
