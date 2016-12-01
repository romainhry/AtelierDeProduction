
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NBR_MACHINES	8
#define MARCHE		1
#define PANNE		0

typedef struct 
{

int numeroMachine;
int tempsUsinage;
int typeOperation;
int etatFonctionnement;
pthread_mutex_t* mutex_occupes;
} Machine;

void *fonctionnementMachine(void *machine_thread)
{
    char * marchePanne;
    Machine machines=*((Machine *) machine_thread); 
    if (machines.etatFonctionnement == 1)
    {
	marchePanne="Marche";
    }
    else 
    {
	marchePanne="Panne";
    }
	
    printf("[Machine numero : %d ] va travailler : %d secondes, effectuer la tache : %d, etat : %s\n",machines.numeroMachine,machines.tempsUsinage,machines.typeOperation,marchePanne);
    sleep(machines.tempsUsinage);
    printf("Reveil machine %d\n",machines.numeroMachine);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{

	pthread_attr_t thread_attr;
	pthread_t threads[NBR_MACHINES];
	int t;
	Machine machines[NBR_MACHINES];
	

	if (pthread_attr_init (&thread_attr) != 0) 
	{
	    fprintf (stderr, "pthread_attr_init error");
	    exit (1);
	}
	if (pthread_attr_setdetachstate (&thread_attr, PTHREAD_CREATE_DETACHED) != 0) 
	{
		printf ("pthread_attr_setdetachstate error");
		exit (1);
	}

	for(t=0;t<NBR_MACHINES;t++) 
	{
	machines[t].numeroMachine=t;
	machines[t].tempsUsinage=3*(t+1);
	machines[t].typeOperation=t;
	machines[t].etatFonctionnement=MARCHE;
	}


	for(t=0;t<NBR_MACHINES;t++) 
	{

		if (pthread_create(&threads[t], &thread_attr, fonctionnementMachine, (void *)&machines[t]) == 1)
		{
		    printf("Erreur Creation Threads\n");
		    exit(-1);
		}

		//if ( pthread_join(threads[t],NULL) != 0)
		//pthread_join(id de la thread a attendre, valeur renvoye par exit())
		//printf("Erreur pthread_join");

	}

	pthread_exit(NULL);
}

