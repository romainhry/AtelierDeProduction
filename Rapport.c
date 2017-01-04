#include <stdlib.h>
#include <stdio.h>

#include "Rapport.h"

void NouveauRapport()
{
    FILE* fichierIndex = NULL;

    int caractereActuel = 0;
    int cpt=0;
    char chaine[10000];
    int unite;
    int dixaine;
    int add;

    fichierIndex = fopen("Rapport/Index_Rapport.txt", "a+");

    if (fichierIndex != NULL)
    {
        do
        {
          caractereActuel = fgetc(fichierIndex); // On lit le caractère
          chaine[cpt]=caractereActuel;
          cpt++;
        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

      if (cpt < 3)
      {
        add=1;
      }
      else
      {
        dixaine=(int)chaine[cpt-4];
        dixaine=dixaine-'0';
        if(dixaine > 9 || dixaine < 1)
        {dixaine=0;}

        unite=(int)chaine[cpt-3];
        unite=unite-'0';

        add=unite+(10*dixaine)+1;
      }

	  fprintf(fichierIndex, "Rapport %d\n", add);	//On rajoute le rapport dans l'index
    fclose(fichierIndex);


    sprintf(nomRapport, "Rapport/Rapport%d.txt", add); //On défini le nom du nouveau rapport
    fichierRapport = fopen(nomRapport, "a+");  //On créé le nouveau Rapport
    fprintf(fichierRapport, "Rapport %d\n\n", add);	//On ajoute le titre du rapport
    fclose(fichierRapport);
    }

}

void EcrireRapport(char * MessageRapport)
{
  fichierRapport = fopen(nomRapport, "a+");  //On créé le nouveau Rapport
  fprintf(fichierRapport,"%s", MessageRapport);	//On ajoute le titre du rapport
  fclose(fichierRapport);
}
