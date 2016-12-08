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

#include "GestionnaireMachines.h"

#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define cle 314
#define SEMPERM 0600				  /* Permission */

int msgid, semid ;

typedef struct{
  int typePiece;
} piece;

typedef struct {
  long type;
  int operation;
}messageOperateur;

//Enregistrement des pièces dans une liste chainée faisant office de file d'attente
//maillon de file d'attente
struct maillon{
	struct maillon* next;
  piece obj;
};

//tete de file d'attente
struct tete{
	struct maillon* first;
	struct maillon* last;
	pthread_mutex_t mtx;
};

void erreur(const char *msg)
{
  perror(msg);
  exit(1);
}
/*Suppréssion de la file de message et du semaphore avant quitter l'app*/
void traitantSIGINT(int s)
{
  semctl(semid, 0, IPC_RMID, 0);
  msgctl(msgid,IPC_RMID,NULL);
  exit(0);
}


/* affichage pour suivi du trajet */
void message(int i, char* s) {
   #define COLONNE 20
   int j, NbBlanc;
   NbBlanc=i*COLONNE;
   for (j=0; j<NbBlanc; j++) putchar(' ');
   printf("%s\n",s);
   fflush(stdout);
}

//initialisation de la file
void init_file(struct tete* file) {
	file->first = NULL;
	file->last = NULL;
	if(pthread_mutex_init(&file->mtx, NULL) == -1) {
		erreur("Initialisation du mutex");
	}
}

//ajout d'un maillon en fin de file
void ajout_file(piece pPiece, struct tete* tete) {
	if(pthread_mutex_lock(&tete->mtx) == -1) {
		erreur("Verrouillage du mutex");
	}
	struct maillon* m = malloc(sizeof(struct maillon));
	m->next = NULL;
	m->obj = pPiece;

	if(tete->first == NULL) {
		tete->first = m;
	} else {
		(tete->last)->next = m;
	}
	tete->last = m;
	if(pthread_mutex_unlock(&tete->mtx) == -1) {
		erreur("Déverrouillage du mutex");
	}
}

//lecture et suppression d'un maillon en début de file
struct maillon* lecture_file(struct tete* tete) {
	if(pthread_mutex_lock(&tete->mtx) == -1) {
		erreur("Verrouillage du mutex");
	}
	struct maillon* m;
	m = tete->first;
	if(tete->first != NULL) {
		tete->first = (tete->first)->next;
	}
	if(pthread_mutex_unlock(&tete->mtx) == -1) {
		erreur("Déverrouillage du mutex");
	}
	return m;
}

void p(int semid) { //prologue
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1) {
		erreur("Opération prologue sur sémaphore");
	}
}

void v(int semid) { //epilogue
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1) {
		erreur("Opération épilogue sur sémaphore");
	}
}

//thread communiquant avec l'opérateur : attend des nouvelles pièces
void* surveillant(void* arg) {
  messageOperateur msg;

  struct tete* fileAttente_pieces = arg;

	msgid = msgget(cle, 0);
	if(msgid != -1) { //file existante donc suppression
		if(msgctl(msgid, IPC_RMID, NULL) == -1) {
			erreur("Suppression file");
		}
	}
	msgid = msgget(cle, IPC_CREAT | 0600); //création de la file
	if(msgid == -1) {
		erreur("Création file");
	}

  piece nouvellePiece;
	while(1) {
		if((msgrcv(msgid, &msg, sizeof(int), 1, 0)) == -1) {
			erreur("Reception de message");
		}
    nouvellePiece.typePiece=msg.operation;
		//on ajoute la piece reçue à la file d'attente
		ajout_file(nouvellePiece, fileAttente_pieces);
		v(semid);
		printf("~~~ S : nouvelle pièce arrivée, opération : %d\n", nouvellePiece.typePiece);
	}

	printf("~~~ S : terminaison du thread surveillant\n");
	pthread_exit(NULL);
}



int main(int argc,char* argv[])
{

  if(argc-1 != 1) {
      erreur("Veuillez passer un nombre de machines en argument");
  }
  signal(SIGINT,traitantSIGINT);

  // initialisation des machines
  int nbMachines=atoi(argv[1]);
  Machine machines[nbMachines];
  pthread_t thread_Machines[nbMachines];
  creationMachines(nbMachines, (pthread_t *)&thread_Machines, (Machine *)&machines);




  piece convoyeur;
  int nb_threads_occupes = 0;

  //Initialisation de la file d'attente des pièces à usiner
  struct tete fileAttente_pieces;
	struct maillon* maillon_piece;
  init_file(&fileAttente_pieces);

  if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1) {
    erreur("Déclaration de la sémaphore de réception principale");
  }
  if (semctl(semid, 0, SETVAL, 0) == -1) {
    erreur("Initialisation de la sémaphore de réception principale");
  }
  pthread_t t_surveillant;
  if(pthread_create(&t_surveillant, NULL, &surveillant, &fileAttente_pieces) != 0) {
		erreur("Création thread surveillant");
	}


	printf("~~ M : création du thread surveillant\n");

  int arret = 0;

  struct maillon* maillon;
  piece pieceCourrente;

  while(!arret) {
    printf("~~ M : en attente d'une pièce...\n");
    p(semid);
    /*
    if(pthread_mutex_lock(&mutex_occupes) == -1) {
      erreur("Verrouillage mutex pour variable nb threads occupes");
    }*/
    printf("~~ M : Pièce en liste d'attente dispo ...\n");
    if(nb_threads_occupes < nbMachines) {
      maillon = lecture_file(&fileAttente_pieces);
  		if(maillon != NULL)
  		{
        pieceCourrente =maillon->obj;
        // section à protéger
        if(machines[pieceCourrente.typePiece].etatFonctionnement == 1) {
          if(machines[pieceCourrente.typePiece].dispo == 1) { //attente active ????????
            convoyeur =  pieceCourrente;
            pthread_mutex_unlock(&machines[pieceCourrente.typePiece].mutex_sync);
          }
          {
            v(semid);
          }
        }
        else{
          //machine en panne
        }
    }


/*
      if(pthread_mutex_unlock(&mutex_occupes) == -1) {
        erreur("Déverrouillage mutex pour variable nb threads occupes");
      }*/
      //printf("~~ M : thread disponible, déverrouillage mutex\n");
    }
  }

  return 0;
}
