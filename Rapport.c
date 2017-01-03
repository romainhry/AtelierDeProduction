#include <stdlib.h>
#include <stdio.h>

#include "Rapport.h"

void NouveauRapport()
{
    FILE* fichierIndex = NULL;

    int caractereActuel = 0;
    int caractereAvant=0;
    int tmp=0;

    fichierIndex = fopen("Rapport/Index_Rapport.txt", "a+");

    if (fichierIndex != NULL)
    {
        do
        {
      	caractereAvant=tmp;	//caractereAvant = caractère qui a le numéro du dernier Rapport
      	tmp = caractereActuel;
        caractereActuel = fgetc(fichierIndex); // On lit le caractère
        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

	  caractereAvant=caractereAvant-47;	//On passe de l'ASCII -> int  en ajoutant +1

	  fprintf(fichierIndex, "Rapport %d\n", caractereAvant);	//On rajoute le rapport dans l'index
    fclose(fichierIndex);
    }

    sprintf(nomRapport, "Rapport/Rapport%d.txt", caractereAvant); //On défini le nom du nouveau rapport
    fichierRapport = fopen(nomRapport, "a+");  //On créé le nouveau Rapport
    fprintf(fichierRapport, "Rapport %d\n\n", caractereAvant);	//On ajoute le titre du rapport
    fclose(fichierRapport);

}

void EcrireRapport(char * MessageRapport)
{
  fichierRapport = fopen(nomRapport, "a+");  //On créé le nouveau Rapport
  fprintf(fichierRapport,"%s", MessageRapport);	//On ajoute le titre du rapport
  fclose(fichierRapport);
}
