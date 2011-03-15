/*  
    Copyright (C) Jonathan Druart
 
    This Program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; version 2
    of the License.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file licence.txt.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define log2(x)                 (log(x)*1.4426950408889634073599246810019)

typedef struct Arbre{
	char car;
	int cpt;
	struct Arbre *fd, *fa; //fils droit, fils ainé
}Arbre;

int id=0; // Variable globale pour la création du graphe


/********************************************************************
 * Retourne le séquence decaractère se situant à la position donnée *
 ********************************************************************/

char* CarPosition(FILE* fic, int position, int longueur){
	char* tabChar = (char*)malloc(longueur * sizeof(char));
	int i = 0;
	
	for (i = 0 ; i < longueur ; i++){
		fseek(fic, position + i, SEEK_SET);
		if (fscanf(fic, "%c", &tabChar[i]) == EOF)
			return NULL;
	}
	return tabChar;
}


/********************************************************************
 *	Ajoute une chaine une chaine de caractère à l'arbre	    *
 ********************************************************************/

Arbre* AjouterChaine(Arbre* arbre, char* tabChar, int taille){
	if (arbre == NULL){
		Arbre* racine = (Arbre*)malloc(sizeof(Arbre));
		racine->fd = NULL;
		racine->fa = NULL;
		racine->cpt = 1;
		racine->car = tabChar[0];

		taille --;
		arbre=racine;
		if (taille > 0)
			racine->fa = AjouterChaine(arbre->fa, tabChar + sizeof(char), taille);
		return racine;
	}else{
		Arbre* marqueur = arbre;

		while(1){
			if (marqueur->car == tabChar[0]){
				marqueur->cpt ++;

				taille --;
				if (taille > 0)
					marqueur->fa = AjouterChaine(marqueur->fa, tabChar + sizeof(char), taille);
				return arbre;
			}else{
				if (marqueur->fd == NULL)
					break;
				else
					marqueur = marqueur->fd;
			}
		}

		Arbre* noeud = (Arbre*)malloc(sizeof(Arbre));
		noeud->fd = NULL;
		noeud->fa = NULL;
		noeud->cpt = 1;
		noeud->car = tabChar[0];

		marqueur->fd = noeud;

		taille --;

		if (taille > 0)
			marqueur->fd->fa = AjouterChaine(marqueur->fd->fa, tabChar + sizeof(char), taille);
		return arbre;
	}
}


/********************************************************************
 *		Initialisation de l'arbre			    *
 ********************************************************************/

Arbre* InitArbre(char* nomFic, int tailleFenetre){
	FILE* fic;
	char* tabChar;
	Arbre* racine = NULL;
	int pos = 0;
	fic = fopen(nomFic, "r");
	if (fic != NULL){
		while((tabChar = CarPosition(fic, pos, tailleFenetre)) != NULL){
			racine = AjouterChaine(racine, tabChar, tailleFenetre);
			pos = pos + 1;
		}
		fclose(fic);
		return racine;
	}else{
		printf("Le fichier %s n'a pas été ouvert, pb de droit ou n'existe pas\n",nomFic);
		return NULL;
        }
}


/********************************************************************
 *	Retourne le caractère se situant à la position donnée	    *
 ********************************************************************/

char CarCorrespondant(Arbre* arbre, int pos){
	if (pos < arbre->cpt)
		return (arbre->car);
	else{
		pos = pos - arbre->cpt;
		return (CarCorrespondant(arbre->fd, pos));
	}
}


/********************************************************************
 *		Retourne le caractère suivant			    *
 ********************************************************************/

char CarSuivant(char* tabChar, int tailleFenetre, Arbre* arbre){
	Arbre* arbreRec = arbre;

	if (tailleFenetre == 0){
		int somme = 0;
		do{
			somme = somme + arbreRec->cpt;
			arbreRec = arbreRec->fd;
		}while(arbreRec != NULL);
		int rand = random() % somme;
		return CarCorrespondant(arbre, rand);
	}else{
		do{
			if (tabChar[0] == arbreRec->car)
				return CarSuivant(&tabChar[1], tailleFenetre - 1, arbreRec->fa);
			else arbreRec = arbreRec->fd;
			
		}while(arbreRec != NULL);
	}
	return '\0';
}


/********************************************************************
 *		Décale la fenetre d'une position		    *
 ********************************************************************/

void DecalerFenetre(char* tabChar, int tailleFenetre){
	memmove(tabChar, &(tabChar[1]), tailleFenetre - 1);
	tabChar[tailleFenetre - 1] = '\0';
}


/********************************************************************
 *		Génère le texte aléatoire			    *
 ********************************************************************/

char* CreationTxtAleatoire(Arbre* arbre, int tailleFenetre, int tailleTxtAGenerer){
	char* tabChar = (char*)malloc(sizeof(char)*(tailleFenetre+1));
	char* txt = (char*)malloc(sizeof(char)*(tailleTxtAGenerer));
	int i = 0;
	for (i = 0 ; i < tailleFenetre ; i++){
		tabChar[i] = CarSuivant(tabChar, i, arbre);
	}
	tabChar[i] = '\0';
	strcpy(txt, tabChar);
	for (; i < tailleTxtAGenerer ; i++){
		DecalerFenetre(tabChar, tailleFenetre);
		tabChar[tailleFenetre-1] = CarSuivant(tabChar, tailleFenetre - 1, arbre);
		if (tabChar[tailleFenetre - 1] == '\0'){
			printf("Chaine de caractères introuvable\n");
			break;
		}
		txt[i] = tabChar[tailleFenetre - 1];
	}
	txt[i] = '\0';
	free(tabChar);
	return (txt);
}


/********************************************************************
 *		Somme les compteurs de chaque noeuds		    *
 ********************************************************************/

int SommeCpt(Arbre* arbre){
	if(arbre != NULL){
		if ((arbre->fd != NULL) && (arbre->fa != NULL)){
			return (arbre->cpt + SommeCpt(arbre->fa) + SommeCpt(arbre->fd));
		}else{
			if(arbre->fa != NULL){
				return (arbre->cpt + SommeCpt(arbre->fa));
			}else if(arbre->fd != NULL){
				return (arbre->cpt + SommeCpt(arbre->fd));
			}else{
				return arbre->cpt;
			}
		}
	}
}


/********************************************************************
 *	Trouve le mot correspondant à une feuille donnée    	    *
 ********************************************************************/

int TrouverMot(Arbre* feuille){
	if (feuille->fa == NULL) return feuille->cpt;
	else return (feuille->cpt + TrouverMot(feuille->fa));
}

/********************************************************************
 *		Calcule l'entropie des mots			    *
 ********************************************************************/

int EntropieMots(Arbre* arbre){
	if(arbre != NULL){
		if ((arbre->fd != NULL) && (arbre->fa != NULL)){
			return (EntropieMots(arbre->fa) + EntropieMots(arbre->fd));
		}else{
			if(arbre->fa != NULL) return (EntropieMots(arbre->fa));
			else if(arbre->fd != NULL) return (EntropieMots(arbre->fd));
			else return CreerMots(arbre);
		}
	}
}


/********************************************************************
 *	Libération de l'espace mémoire alloué pour l'arbre    	    *
 ********************************************************************/

void FreeArbre(Arbre* arbre){
	if (arbre != NULL){
		FreeArbre(arbre->fd);
		FreeArbre(arbre->fa);
		free(arbre);
	}
}


/********************************************************************
 *		Crée le fichier du graphe			    *
 ********************************************************************/

void CreerGraphe(Arbre* a, FILE* graph){
	if(a){
		fprintf(graph,"\"%p\" [label = \"<f0>%c %d|<f1>fd|<f2>fa\" shape = \"record\"];\n", a ,a->car, a->cpt);
		if(a->fd){
			fprintf(graph,"\"%p\":f1 -> \"%p\":f0 [id = %d];\n", a, a->fd, id);
			id ++;
		}
		if(a->fa){
			fprintf(graph,"\"%p\":f2 -> \"%p\":f0 [id = %d];\n", a, a->fa, id);
			id ++;
		}
		CreerGraphe(a->fd, graph);
		CreerGraphe(a->fa, graph);
	}
}


/********************************************************************
 *			Fonction principale			    *
 ********************************************************************/

int main(int argc, char* argv[]){

	char* nomFic;
	int tailleFenetre = 0;
	int tailleTxtAGenerer = 0;
	FILE* graph;

	if (argc != 4){
		printf("Utilisation : \n\t./txtaleatoire nomFic tailleFenetre tailleTxtAGenerer\n");
		printf("Taille de la fenêtre ?");
		scanf("%d", &tailleFenetre);
		printf("Taille du texte à générer ?");
		scanf("%d", &tailleTxtAGenerer);
		printf("Nom du fichier pour la création de l'arbre ?");
		scanf("%s", nomFic);

	}else{
		nomFic = argv[1];
		tailleFenetre = atoi(argv[2]);
		tailleTxtAGenerer = atoi(argv[3]);
	}
	srand(time(NULL));
	printf("--- Initialisation de L'arbre ---\n");
	Arbre* arbre = InitArbre(nomFic, tailleFenetre);
	printf("--- Arbre initialisé ---\n");
	if (arbre != NULL){
		char* txt;
		printf("--- L'arbre a été construit avec succès ---\n");

		printf("--- Création du fichier graph.dot ---\n");
		graph = fopen("graph.dot","w"); 
		if (graph != NULL){
			fprintf(graph,"digraph g {\n"); 
			CreerGraphe(arbre, graph);
			printf("--- Le fichier graph.dot à été généré avec succès ---\n");
		}else printf("--- Le fichier graph.dot n'à pas été généré, (problème de droits ?) ---\n");

		printf("--- Création du texte aléatoire ---\n");
		txt = CreationTxtAleatoire(arbre, tailleFenetre, tailleTxtAGenerer);
		if (txt != NULL){
			printf("--- Le texte a été généré avec succès ---\n");
			printf("-------------------------------------------------\n");
			printf("-------------------TEXTE GÉNÉRÉ------------------\n");
			printf("%s\n",txt);
			printf("-------------------------------------------------\n");
			//printf("Moy = %i\n",Moyenne());
			int somme = SommeCpt(arbre);
			float rlog = log2(45);
			printf("total des poids = %d\n -> *log2(45)=%f\n",somme, rlog*somme);
			printf("Utiliser la commande $dot -Tpng graph.dot -o graph.png pour générer le png correspondant au fichier graph.dot\n");
		}else{
			printf("--- ERREUR ! Le texte n'a pas pu être généré ---\n");
		}
		fprintf(graph,"}\n"); 
		fclose(graph);
		
	}else{
		printf("L'arbre n'a pu être construit, ou est vide\n");
		exit(0);
	}
	return 0;
}
