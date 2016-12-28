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
#include "RobotAlimentation.h"
#include "Convoyeur.h"

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


// Le main est le thread Superviseur
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


  int nb_threads_occupes = 0;

  //Initialisation de la file d'attente des pièces à usiner
  struct convoyeur myConvoyeur;
	struct maillon* maillon_piece;
  init_convoyeur(&convoyeur);

  if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1) {
    erreur("Déclaration de la sémaphore de réception principale");
  }
  if (semctl(semid, 0, SETVAL, 0) == -1) {
    erreur("Initialisation de la sémaphore de réception principale");
  }
  pthread_t t_robotAlimentation;
  if(pthread_create(&t_robotAlimentation, NULL, &robotAlimentation, &convoyeur) != 0) {
		erreur("Création thread robotAlimentation");
	}


	printf("~~ M : création du thread robotAlimentation\n");

  int arret = 0;

  int typePieceCourrente;

  while(!arret) {
    printf("~~ M : en attente d'une pièce...\n");
    p(semid);
    /*
    if(pthread_mutex_lock(&mutex_occupes) == -1) {
      erreur("Verrouillage mutex pour variable nb threads occupes");
    }*/
    printf("~~ M : Pièce mise sur convoyeur ...\n");
    if(nb_threads_occupes < nbMachines) {
      typePieceCourrente = typePiece_convoyeur(&convoyeur);
        // section à protéger
        if(machines[typePieceCourrente].etatFonctionnement == 1) {
          if(machines[typePieceCourrente].dispo == 1) { //attente active ????????
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
