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

#include "Affichage.h"
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

void traitantDefaillance(int s){
  printf("\nSysteme dans un état de défaillance, Veuillez redémarrer le système, les pièces non usinées sont perdues \n");
  if(pidOp!=0)
  {
    kill(pidOp,SIGUSR1);
    kill(getpid(),SIGINT);
  }
}


// Le main est le thread Superviseur
int main(int argc,char* argv[])
{
  char MessageAfficher[200];
  printf("\n");
  pidOp =0;
  messageMachine msgMachine;

  signal(SIGINT,traitantSIGINT);

  //defaillance
  signal(SIGUSR1,traitantDefaillance);

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
      sprintf(MessageAfficher,"[Erreur] : Suppression file");
      affichageConsole(LigneErreur,MessageAfficher);
		}
	}
	msgid = msgget(cle, IPC_CREAT | 0600); //création de la file
	if(msgid == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Création file");
    affichageConsole(LigneErreur,MessageAfficher);
	}
  nombreDeMachines=0;
  sprintf(MessageAfficher,"\n[Information] : Attente d'informations de l'opérateur");
  affichageConsole(10,MessageAfficher);
	//Recepetion du nombre de machines à initialiser
	if((msgrcv(msgid, &msgMachine, (sizeof(msgMachine)-sizeof(long)), 1, 0)) == -1)
	{
    sprintf(MessageAfficher,"[Erreur] : Reception de message");
    affichageConsole(LigneErreur,MessageAfficher);
	}
  pidOp = msgMachine.pid;


  // initialisation des machines
  nombreDeMachines=msgMachine.nbrMachine;
  Machine machines[nombreDeMachines];
  pthread_t thread_Machines[nombreDeMachines];
  creationMachines(nombreDeMachines, (pthread_t *)&thread_Machines, (Machine *)&machines, &myConvoyeur);

  init_affichage(nombreDeMachines);

  sprintf(MessageAfficher,"[Information] : L'opérateur souhaite %d machines différentes\n",msgMachine.nbrMachine);
  affichageConsole(LigneInformation,MessageAfficher);

  int nb_threads_occupes = 0;



  if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600)) == -1)
  {
    sprintf(MessageAfficher,"[Erreur] : Déclaration de la sémaphore de réception principale");
    affichageConsole(LigneErreur,MessageAfficher);
  }
  if (semctl(semid, 0, SETVAL, 0) == -1)
  {
    sprintf(MessageAfficher,"[Erreur] : Initialisation de la sémaphore de réception principale");
    affichageConsole(LigneErreur,MessageAfficher);
  }
  pthread_t t_robotAlimentation;
  if(pthread_create(&t_robotAlimentation, NULL, &robotAlimentation, &myConvoyeur) != 0)
  {
    sprintf(MessageAfficher,"[Erreur] : Création thread robotAlimentation");
    affichageConsole(LigneErreur,MessageAfficher);
  }

  sprintf(MessageAfficher,"[Robot Alimentation] : création du thread");
  affichageConsole(LigneRobotAlim,MessageAfficher);

  int arret = 0;

  int typePieceCourrente;

  while(!arret)
  {
    p(semid);

    typePieceCourrente = typePiece_convoyeur(&myConvoyeur); //récupère une pièce du convoyeur

    pthread_mutex_lock(&machines[typePieceCourrente].mutex);

    machines[typePieceCourrente].nbPiece++;
    pthread_cond_signal(&machines[typePieceCourrente].attendre);
    pthread_mutex_unlock(&machines[typePieceCourrente].mutex);

  }
  return 0;
}
