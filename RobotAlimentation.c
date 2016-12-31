#include "RobotAlimentation.h"
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define cle 314



void p(int semid) //prologue
{ 
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = -1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1) 
	{
		erreur("Opération prologue sur sémaphore");
	}
}

void v(int semid) //epilogue
{ 
	struct sembuf op[1];
	op[0].sem_num = 0;
	op[0].sem_op = 1;
	op[0].sem_flg = 0;
	if(semop(semid, op, 1) == -1) 
	{
		erreur("Opération épilogue sur sémaphore");
	}
}


//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotAlimentation(void* arg) 
{
  int i=0;
  messageOperateur msg;

  struct convoyeur* myConvoyeur = arg;



  piece nouvellePiece;
	while(1) 
	{
		if((msgrcv(msgid, &msg, (sizeof(msg)-sizeof(long)), 1, 1)) == -1) 
		{
			erreur("Reception de message");
		}
		if (msg.nbrPiece == 1)
		{
    		nouvellePiece.typePiece=msg.operation;
		//on ajoute la piece reçue à la file d'attente
		alimente_convoyeur(nouvellePiece, myConvoyeur);
		v(semid);
		printf("~~~ S : nouvelle pièce mise sur le convoyeur, opération : %d\n", nouvellePiece.typePiece);
		}
		else 
		{
			for (i=0; i<msg.nbrPiece; i++)
			{
			nouvellePiece.typePiece=msg.operation;
			alimente_convoyeur(nouvellePiece, myConvoyeur);
			v(semid);
			}
			
			printf("~~~ S : %d pièces mises sur le convoyeur, opération : %d\n",msg.nbrPiece, nouvellePiece.typePiece);
		}
		
	}

	printf("~~~ S : terminaison du thread robotAlimentation\n");
	pthread_exit(NULL);
}
