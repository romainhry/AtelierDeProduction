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
#include "RobotRetrait.h"
#include "Operateur.h"
#include "Rapport.h"

int pidOp;

/*Suppréssion de la file de message,du semaphore et libère les ressources allouées avant quitter l'app*/
void traitantSIGINT(int s)
{
  char MessageAfficher[200];
  libereConvoyeur();
  semctl(semid, 0, IPC_RMID, 0);
  msgctl(msgid,IPC_RMID,NULL);

  if(pidOp!=0)
  {
    kill(pidOp,SIGUSR1);
  }
  sprintf(MessageAfficher,"[Information] : Arrêt du système, libération des ressources");
  affichageConsole(LigneInformation,MessageAfficher);
  exit(0);
}

void traitantDefaillance(int s){
  char MessageAfficher[200];
  sprintf(MessageAfficher,"[Erreur] : Systeme dans un état de défaillance,");
  affichageConsole(LigneErreur,MessageAfficher);
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
  affichageConsole(2,MessageAfficher);
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

  nbPieceFini=0;
  pthread_t t_robotRetrait;
  //Création d'un moniteur pour le robot de retrait
  if(pthread_mutex_init(&mutex_RobotRetrait, NULL) == -1)
  {
    sprintf(MessageAfficher,"[Erreur] : Initialisation mutex de synchro de machine");
    affichageConsole(LigneErreur,MessageAfficher);
    exit(1);
  }
  if(pthread_cond_init(&attendre_RobotRetrait, NULL) == -1)
  {
    sprintf(MessageAfficher,"[Erreur] : Initialisation mutex d'attente de machine");
    affichageConsole(LigneErreur,MessageAfficher);
    exit(1);
  }

  if(pthread_create(&t_robotRetrait, NULL, &robotRetrait, &myConvoyeur) != 0)
  {
    sprintf(MessageAfficher,"[Erreur] : Création thread robotRetrait");
    affichageConsole(LigneErreur,MessageAfficher);
  }
  
  //Création d'un nouveau Rapport
  NouveauRapport();

  int arret = 0;

  piece pieceCourrente;


  while(!arret)
  {
    p(semid);

    pieceCourrente = getPiece_convoyeur(&myConvoyeur); //récupère une pièce du convoyeur

    if(pieceCourrente.fini==0)
    {
      int typePieceCourrente=pieceCourrente.typePiece;
      pthread_mutex_lock(&machines[typePieceCourrente].mutex);

      if(machines[typePieceCourrente].etatFonctionnement == 0)
      {
        sprintf(MessageAfficher,"[Information] : Pièce déstinée à machine en panne : defaillance");
        affichageConsole(LigneInformation,MessageAfficher);

        sprintf(MessageAfficher,"Pièce [%d] de type : %d Déstinée à machine en panne : Défaillance\n",pieceCourrente.identifiant,pieceCourrente.typePiece);
        EcrireRapport(MessageAfficher);

        kill(getpid(),SIGUSR1);
      }

      machines[typePieceCourrente].nbPiece++;

      pthread_cond_signal(&machines[typePieceCourrente].attendre); //Dit à la machine qu'il y au moins une pièce pour elle
      pthread_mutex_unlock(&machines[typePieceCourrente].mutex);
    }
    else {
      pthread_mutex_lock(&mutex_RobotRetrait);
      nbPieceFini++;
      pthread_cond_signal(&attendre_RobotRetrait); //Dit à la machine qu'il y au moins une pièce pour elle
      pthread_mutex_unlock(&mutex_RobotRetrait);
    }

  }
  return 0;
}
