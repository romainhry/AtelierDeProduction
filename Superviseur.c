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
#include <signal.h>


#include "Convoyeur.h"
#include "GestionnaireMachines.h"
#include "RobotAlimentation.h"
#include "Operateur.h"


int pidOp;

/*Suppréssion de la file de message et du semaphore avant quitter l'app*/
void traitantSIGINT(int s)
{
  semctl(semid, 0, IPC_RMID, 0);
  msgctl(msgid,IPC_RMID,NULL);
  msgctl(msgid_op,IPC_RMID,NULL);
  if(pidOp!=0)
  {
    kill(pidOp,SIGUSR1);
  }
  exit(0);
}


// Le main est le thread Superviseur
int main(int argc,char* argv[])
{
  printf("\n");
  pidOp =0;
  messageMachine msgMachine;

  signal(SIGINT,traitantSIGINT);

  //Initialisation du convoyeur des pièces à usiner
  struct convoyeur myConvoyeur;
  struct maillon* maillon_piece;
  init_convoyeur(&myConvoyeur);

  //Initialisation de file de messages

  //File Message Piece
	msgid = msgget(cle, 0);
	if(msgid != -1) //file existante donc suppression
	{
		if(msgctl(msgid, IPC_RMID, NULL) == -1)
		{
			erreur("Suppression file");
		}
	}
	msgid = msgget(cle, IPC_CREAT | 0600); //création de la file
	if(msgid == -1)
	{
		erreur("Création file");
	}

	printf("Attente d'informations de l'opérateur\n");

	//Recepetion du nombre de machines à initialiser
	if((msgrcv(msgid, &msgMachine, (sizeof(msgMachine)-sizeof(long)), 1, 0)) == -1)
	{
		erreur("Reception de message");
	}
  pidOp = msgMachine.pid;
	printf("L'opérateur souhaite %d machines différentes\n",msgMachine.nbrMachine);

  // initialisation des machines
  int nbMachines=msgMachine.nbrMachine;
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
    printf("~~ M : en attente de pièces...\n");
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
