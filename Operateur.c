#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "Operateur.h"

void traitantSIGUSR(int s)
{
  printf("\n\nLe superviseur c'est arretté\nFin de l'opérateur\n");
  exit(0);
}

int main(int argc, char* argv[])
{
  int choix;
  message msg ;	//Message transmit à RobotAlimentation qui contient les types de pièce, nbr, etc..
  messageMachine msgMachine; //Message transmet au Superviseur qui contient le nombre de Machines à initialiser

  signal(SIGUSR1,traitantSIGUSR);
  if(argc-1 != 1)
  {
      perror("\nVeuillez passer un nombre de machines en argument\n");
  }



  int msqid = msgget(cle, 0);
  if(msqid == -1) //file non existante
  {
    perror("\nSuperviseur inexistant, file de message inexistante\n");
    exit(1);
  }


  //Informe sur le nombre de machine voulu et du pid
  msgMachine.type=1;
  msgMachine.nbrMachine=atoi(argv[1]); //Nombre de machines à initialiser
  msgMachine.pid=getpid();
  int typePieceDif=msgMachine.nbrMachine;  //On change de nom juste pour la compréhension
  if(msgsnd(msqid, &msgMachine, (sizeof(messageMachine)-sizeof(long)), 0) == -1)
  {
    perror("Envoi de message");
    exit(1);
  }

  //Informe des nouvelles pièces
  msg.type=1;
  while(1)
  {
    do
    {

      printf("\nEntrer une pièce après l'autre : 1\nEntrer plusieur pièce d'un coup : 2\nAction : ");
      scanf("%d",&choix);
      /* vidange du flux */
      while(getchar() != '\n');
      printf("\n");
    }
    while (choix != 1 && choix != 2);

    if (choix == 1)
    {
      printf("Il y a %d différents types de pièces. Quel type de pièce voulez-vous ajouter ?\ntype de pièce n° : ",typePieceDif);
      do
      {
        scanf("%d",&msg.typePiece);
        /* vidange du flux */
        while(getchar() != '\n');

        if (msg.typePiece < 0  || msg.typePiece >= typePieceDif)
        {
          printf("\ntype de pièce invalide\ntype de pièce n° : ");
        }
      }
      while (msg.typePiece < 0  || msg.typePiece >= typePieceDif);
      msg.nbrPiece=1;
    }
    else
    {
      printf("Il y a %d différents types de pièces. Quel type de pièces voulez-vous ajouter ?\ntype de pièce n° : ",typePieceDif);

      do
      {
        scanf("%d",&msg.typePiece);
        /* vidange du flux */
        while(getchar() != '\n');

        if (msg.typePiece < 0  || msg.typePiece >= typePieceDif)
        {
          printf("\ntype de pièce invalide\ntype de pièce n° : ");
        }
      }
      while (msg.typePiece < 0  || msg.typePiece >= typePieceDif);

      printf("combien de pièces de type %d voulez vous ajouter ?\n",msg.typePiece);
      printf("nombre de pièces de type %d = ",msg.typePiece);
      scanf("%d",&msg.nbrPiece);
      /* vidange du flux */
      while(getchar() != '\n');
    }
    //envoi du paramètre dans la file
    if(msgsnd(msqid, &msg, (sizeof(message)-sizeof(long)), 1) == -1)
    {
      perror("Envoi de message");
      exit(1);
    }
  }

  return 0;
}
