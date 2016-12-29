# définition des cibles particulières
.PHONY: clean, mrproper
 
# désactivation des règles implicites
.SUFFIXES:
 
 
# création de l'exécutable 'Programme'
Superviseur: Superviseur.o GestionnaireMachines.o Convoyeur.o RobotAlimentation.o
	gcc Superviseur.o GestionnaireMachines.o Convoyeur.o RobotAlimentation.o -o Superviseur -pthread
 
Superviseur.o: Superviseur.c
	gcc -c Superviseur.c -o Superviseur.o

GestionnaireMachines.o: GestionnaireMachines.c
	gcc -c GestionnaireMachines.c -o GestionnaireMachines.o

Convoyeur.o: Convoyeur.c
	gcc -c Convoyeur.c -o Convoyeur.o

RobotAlimentation.o: RobotAlimentation.c
	gcc -c RobotAlimentation.c -o RobotAlimentation.o

Operateur: Operateur.o
	gcc Operateur.o -o Operateur

Operateur.o: Operateur.c
	gcc -c Operateur.c -o Operateur.o

# suppression des fichiers temporaires
clean:
	rm -rf *.bak rm -rf *.o
 
# suppression de tous les fichiers, sauf les sources,
# en vue d'une reconstruction complète
mrproper: clean
	rm -rf Programme

# Pour compiler proprement copier : make mrproper && make Superviseur && make Operateur && make clean
