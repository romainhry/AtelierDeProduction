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


#include "Convoyeur.h"
#include "GestionnaireMachines.h"
#include "RobotAlimentation.h"


#define IFLAGS (SEMPERM | IPC_CREAT)
#define SKEY   (key_t) IPC_PRIVATE
#define SEMPERM 0600				  /* Permission */


/*Suppréssion de la file de message et du semaphore avant quitter l'app*/
void traitantSIGINT(int s)
{
  semctl(semid, 0, IPC_RMID, 0);
  msgctl(msgid,IPC_RMID,NULL);
  exit(0);
}


/* affichage pour suivi du trajet */
void message(int i, char* s) 
{
   #define COLONNE 20
   int j, NbBlanc;
   NbBlanc=i*COLONNE;
   for (j=0; j<NbBlanc; j++) putchar(' ');
   printf("%s\n",s);
   fflush(stdout);
}


// Le main est le thread Superviseur
int main(int argc,char* argv[])
{
  int testInterblocage=0;  //A supprimer à la fin des tests
  if(argc-1 != 1) 	
  {
      erreur("Veuillez passer un nombre de machines en argument");
  }
  signal(SIGINT,traitantSIGINT);

  //Initialisation du convoyeur des pièces à usiner
  struct convoyeur myConvoyeur;
  struct maillon* maillon_piece;
  init_convoyeur(&myConvoyeur);


  // initialisation des machines
  int nbMachines=atoi(argv[1]);
  Machine machines[nbMachines];
  pthread_t thread_Machines[nbMachines];
  creationMachines(nbMachines, (pthread_t *)&thread_Machines, (Machine *)&machines, &myConvoyeur);


  int nb_threads_occupes = 0;



  if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1) 
  {
    erreur("Déclaration de la sémaphore de réception principale");
  }
  if (semctl(semid, 0, SETVAL, 0) == -1) 
  {
    erreur("Initialisation de la sémaphore de réception principale");
  }
  pthread_t t_robotAlimentation;
  if(pthread_create(&t_robotAlimentation, NULL, &robotAlimentation, &myConvoyeur) != 0) 
  {
    erreur("Création thread robotAlimentation");
  }

  printf("~~ M : création du thread robotAlimentation\n");

  int arret = 0;

  int typePieceCourrente;

  while(!arret) 
  {
    printf("~~ M : en attente d'une pièce...\n");
    p(semid);
    printf("~~ M : Pièce mise sur convoyeur ...\n");

    
    typePieceCourrente = typePiece_convoyeur(&myConvoyeur); //récupère une pièce du convoyeur
    
    pthread_mutex_lock(&machines[typePieceCourrente].mutex);

    machines[typePieceCourrente].nbPiece++;
    pthread_cond_signal(&machines[typePieceCourrente].attendre);
    pthread_mutex_unlock(&machines[typePieceCourrente].mutex);

  }
  return 0;
}
