atelier: superviseur.o operateur.o
	gcc -o atelier superviseur.o operateur.o

superviseur.o: superviseur.c
	gcc -o superviseur.o -c superviseur.c

operateur.o: operateur.c
	gcc -o operateur.o -c operateur.c

clean:
	rm -rf *.o
