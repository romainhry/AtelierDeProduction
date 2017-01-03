# définition des cibles particulières
.PHONY: clean, mrproper

# désactivation des règles implicites
.SUFFIXES:


# création de l'exécutable 'Programme'
Test:
	make mrproper && make Superviseur && make Operateur && make clean

Superviseur:  Superviseur.o Affichage.o GestionnaireMachines.o Convoyeur.o RobotRetrait.o RobotAlimentation.o
	gcc Superviseur.o Affichage.o GestionnaireMachines.o Convoyeur.o RobotRetrait.o RobotAlimentation.o -o Superviseur -pthread

Affichage.o: Affichage.c
	gcc -c Affichage.c -o Affichage.o

Superviseur.o: Superviseur.c
	gcc -c Superviseur.c -o Superviseur.o

GestionnaireMachines.o: GestionnaireMachines.c
	gcc -c GestionnaireMachines.c -o GestionnaireMachines.o

Convoyeur.o: Convoyeur.c
	gcc -c Convoyeur.c -o Convoyeur.o

RobotRetrait.o: RobotRetrait.c
	gcc -c RobotRetrait.c -o RobotRetrait.o

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
