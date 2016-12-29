#include "RobotAlimentation.h"
#include <sys/sem.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define cle 314



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


//thread communiquant avec le superviseur : attend des nouvelles pièces
void* robotAlimentation(void* arg) {
  messageOperateur msg;

  struct convoyeur* myConvoyeur = arg;

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
		alimente_convoyeur(nouvellePiece, myConvoyeur);
		v(semid);
		printf("~~~ S : nouvelle pièce arrivée, opération : %d\n", nouvellePiece.typePiece);
	}

	printf("~~~ S : terminaison du thread robotAlimentation\n");
	pthread_exit(NULL);
}
