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

#define cle 314

//structure de données qui sera envoyée via une file de messages IPC
typedef struct{
  long type;
  int typePiece;
} message;

int main(int argc, char* argv[]) {

	message msg;
  msg.type=1;

	int msqid = msgget(cle, 0);
	if(msqid == -1) { //file non existante
		perror("File inexistante");
		exit(1);
	}
  while(1) {
    printf("Entrez l'opération d'usinage pour une nouvelle pièce, numéro correspondant au numéro de la machine \n");
    scanf("%d",&msg.typePiece);

  	//envoi du paramètre dans la file

  	if(msgsnd(msqid, &msg, sizeof(int), 0) == -1) {
  		perror("Envoi de message");
  		exit(1);
  	}
  }

	return 0;
}
