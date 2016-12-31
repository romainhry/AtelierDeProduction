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

#include "Operateur.h"


//structure de données qui sera envoyée via une file de messages IPC
typedef struct
{
  long type;
  int typePiece;
  int nbrPiece;
} message;

int main(int argc, char* argv[]) 
{
	int choix;
	message msg;	//Message transmit à RobotAlimentation qui contient les types de pièce, nbr etc.. 
	msg.type=1;	
	messageMachine msgMachine; //Message transmet au Superviseur qui contient le nombre de Machines à initialiser
	msgMachine.type=1;
  	
	msgMachine.nbrMachine=atoi(argv[1]); //Nombre de machines à initialiser
	int typePieceDif=msgMachine.nbrMachine;  //On change de nom juste pour la compréhension

	int msqid = msgget(cle2, 0);	//Clé pour envoyé premier message
	if(msqid == -1) //file non existante
	{ 
		perror("File inexistante");
		exit(1);
	}

	if(msgsnd(msqid, &msgMachine, (sizeof(messageMachine)-sizeof(long)), 0) == -1) 
	{
  		perror("Envoi de message");
  		exit(1);
  	}

	msqid = msgget(cle, 0);		//Nouvelle Clé pour deuxième message
  while(1) 
  {
	do
	{
	printf("\nEntrer une pièce après l'autre : 1\nEntrer plusieur pièce d'un coup : 2\nAction : ");
	scanf("%d",&choix);
	printf("\n");
	}
	while (choix != 1 && choix != 2);

    if (choix == 1)
    {
    printf("Il y a %d différents types de pièces. Quel type de pièce voulez-vous ajouter ?\ntype de pièce n° : ",typePieceDif); 
	do
	{
	scanf("%d",&msg.typePiece);

		if (msg.typePiece < 1  || msg.typePiece > typePieceDif)
		{
		printf("\ntype de pièce invalide\ntype de pièce n° : ");
		}
	}
	while (msg.typePiece < 1  || msg.typePiece > typePieceDif);
    msg.nbrPiece=1;
    }
    else
    {
     printf("Il y a %d différents types de pièces. Quel type de pièces voulez-vous ajouter ?\ntype de pièce n° : ",typePieceDif); 

	do
	{
	scanf("%d",&msg.typePiece);

		if (msg.typePiece < 1  || msg.typePiece > typePieceDif)
		{
		printf("\ntype de pièce invalide\ntype de pièce n° : ");
		}
	}
	while (msg.typePiece < 1  || msg.typePiece > typePieceDif);
     
	printf("combien de pièces de type %d voulez vous ajouter ?\n",msg.typePiece);
	printf("nombre de pièces de type %d = ",msg.typePiece);
	scanf("%d",&msg.nbrPiece);
    }
  	//envoi du paramètre dans la file
	printf("envoi : %d \n",msg.nbrPiece);
  	if(msgsnd(msqid, &msg, (sizeof(message)-sizeof(long)), 1) == -1) 
	{
  		perror("Envoi de message");
  		exit(1);
  	}
  }

	return 0;
}
