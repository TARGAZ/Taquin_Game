// Inclusion des bibliothèques utiles
#include "taquin.h"
#include "AStar.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>



// Fonction pour copier un plateau de taquin pSrc vers pDest
int copyTaquin(Taquin* pSrc, Taquin* pDest)
{
	pSrc->hauteur = pDest->hauteur;
	pSrc->largeur = pDest->largeur;
	if (pSrc->plateau == NULL)
	{
		pSrc->plateau = (unsigned char**)calloc(pSrc->hauteur, sizeof(unsigned char*));
		if (!pSrc->plateau)
			return 0;
		for (int i = 0; i < pSrc->hauteur; i++)
		{
			pSrc->plateau[i] = (unsigned char*)calloc(pSrc->largeur,sizeof(unsigned char));
			if (!pSrc->plateau[i])
				return 0;
		}
	}
	for (int i = 0; i < pSrc->hauteur; i++)
	{
		for (int j = 0; j < pSrc->largeur; j++)
		{
			pSrc->plateau[i][j] = pDest->plateau[i][j];
		}
	}
	pSrc->x = pDest->x;
	pSrc->y = pDest->y;

	return 1;
}

// fonction qui renvoie 1 si les 2 taquins sont identiques
// 0 sinon
// -1 si il y a une erreur au passage des paramètres
int equalTaquin(Taquin* pTaquin1, Taquin* pTaquin2)
{
	int i = 0, j = 0;
	if (pTaquin1->hauteur != pTaquin2->hauteur || pTaquin1->largeur != pTaquin2->largeur)
		return -1;

	for (int i = 0; i < pTaquin1->hauteur; i++)
	{
		for (int j = 0; j < pTaquin1->largeur; j++)
		{
			if (pTaquin1->plateau[i][j] != pTaquin2->plateau[i][j])
			{
				return 0;
			}
		}
	}
	return 1;
}

// Fonction qui crée un plateau de taquin 
// ATTENTION le plateau doit être NULL avant l'appel à cette fonction 
// Si le plateau n'est pas NULL la fonction essayera de libérer la zone mémoire occupée par le plateau et cela peut donc aboutir à un crash si le plateau n'a pas été initialisé
int createTaquin(Taquin * pTaquin, unsigned char hauteur, unsigned char largeur)
{
	// Test pour vérifier que les données passées ne sont pas corrompues
	if(!pTaquin) return 0;
	
	// Tests pour vérifier les paramètres de taille de taquin
	if(hauteur<SIZE_MINI) hauteur = SIZE_MINI;
	if(hauteur>SIZE_MAXI) hauteur = SIZE_MAXI;
	if(largeur<SIZE_MINI) largeur = SIZE_MINI;
	if(largeur>SIZE_MAXI) largeur = SIZE_MAXI;
	
	{
		int i;

		// On vérifie que le plateau n'existe pas
		// S'il existe on libère la mémoire avant de recréer le plateau
		if(pTaquin->plateau)
		{
			// On libère le plateau ligne par ligne
			for(i=0; i < pTaquin->hauteur; i++) if(pTaquin->plateau[i]) free(pTaquin->plateau[i]);
			// On libère le tableau qui stockait les lignes
			free(pTaquin->plateau);
			pTaquin->plateau = NULL;
		}

		pTaquin->hauteur = hauteur;
		pTaquin->largeur = largeur;

		// on alloue la zone mémoire pour stocker les adresses des lignes du tableau
		pTaquin->plateau = (unsigned char**) malloc(sizeof(unsigned char*)*hauteur);
		
		// si on a pas réussi à allouer la zone mémoire on retourne 0
		if(!pTaquin->plateau) return 0;

		for(i=0; i < hauteur; i++) pTaquin->plateau[i] = NULL;

		for(i=0; i < hauteur; i++)
		{
			// On alloue la zone mémoire pour contenir la ligne i
			pTaquin->plateau[i] = (unsigned char*) malloc(sizeof(unsigned char)*largeur);
			// S'il y a eu un souci à l'allocation on libère tout ce qui a déjàà été alloué et on retourne 0
			if(!pTaquin->plateau[i])
			{
				freeTaquin(pTaquin);
				return 0;
			}
		}
		// On initialise le taquin
		if(!initTaquin(pTaquin)) return 0;
	}

	return 1;
}


// Fonction qui initialise le taquin
int initTaquin(Taquin * pTaquin)
{
	unsigned char value = 0;
	pTaquin->x = 0;
	pTaquin->y = 0;

	for (int i = 0; i < pTaquin->hauteur; i++)
	{
		for (int j = 0; j < pTaquin->largeur; j++)
		{
			pTaquin->plateau[i][j] = value;
			value++;
		}
	}

	return 1;
}

// Fonction qui mélange le taquin en effectuant entre minRandom et maxRandom coups aléatoires
int mixTaquin(Taquin * pTaquin, int minRandom, int maxRandom)
{
	srand(((time(0) % minRandom) * 30) / 6);
	int nbMelange = 0;
	int	oualler, fait = 0;
	while (nbMelange <= minRandom)
	{
		nbMelange = rand() % maxRandom;
	}

	for (int i = 0; i < nbMelange; i++)
	{
		while (1)
		{
			oualler = (rand() % 4) + 1;
			fait = moveTaquin(pTaquin, (oualler == 1) ? GAUCHE : (oualler == 2) ? DROITE : (oualler == 3) ? HAUT : (oualler == 4) ? BAS : 0);
			if (fait == 1)
			{
				fait = 0;
				break;
			}
		}
	}

	return 1;
}

// Fonction qui permet de bouger une pièce du taquin (en bougeant la case vide)
int moveTaquin(Taquin * pTaquin, deplacement d)
{
	if (d == AUCUN)
	{
		return 1;
	}

	if (d == GAUCHE)
	{
		if (pTaquin->x - 1 >= 0)
		{
			pTaquin->plateau[pTaquin->x][pTaquin->y] = pTaquin->plateau[pTaquin->x - 1][pTaquin->y];
			pTaquin->plateau[pTaquin->x - 1][pTaquin->y] = 0;
			pTaquin->x = pTaquin->x - 1;
			return 1;
		}
	}

	if (d == DROITE)
	{
		if (pTaquin->x + 1 <= pTaquin->largeur - 1)
		{
			pTaquin->plateau[pTaquin->x][pTaquin->y] = pTaquin->plateau[pTaquin->x + 1][pTaquin->y];
			pTaquin->plateau[pTaquin->x + 1][pTaquin->y] = 0;
			pTaquin->x = pTaquin->x + 1;
			return 1;
		}
	}

	if (d == BAS)
	{
		if (pTaquin->y + 1 <= pTaquin->hauteur -1)
		{
			pTaquin->plateau[pTaquin->x][pTaquin->y] = pTaquin->plateau[pTaquin->x][pTaquin->y + 1];
			pTaquin->plateau[pTaquin->x][pTaquin->y + 1] = 0;
			pTaquin->y = pTaquin->y + 1;
			return 1;
		}
	}

	if (d == HAUT)
	{
		if (pTaquin->y - 1 >= 0)
		{
			pTaquin->plateau[pTaquin->x][pTaquin->y] = pTaquin->plateau[pTaquin->x][pTaquin->y - 1];
			pTaquin->plateau[pTaquin->x][pTaquin->y - 1] = 0;
			pTaquin->y = pTaquin->y - 1;
			return 1;
		}
	}
	return 0;
}

// Fonction qui renvoie le déplacement à effectuer pour annuler le déplacement donné en paramètre
deplacement cancelMove(deplacement d)
{
	// TODO: cancelMove
	return (AUCUN == d) ? AUCUN: (GAUCHE == d) ? DROITE : (DROITE == d) ? GAUCHE : (BAS == d) ? HAUT : (HAUT == d) ? BAS : 6;
}

// Fonction qui renvoie 1 si le taquin es résolu, 0 sinon
int endTaquin(Taquin * pTaquin)
{
	unsigned char count = 0;
	for (int i = 0; i < pTaquin->hauteur; i++)
	{
		for (int j = 0; j < pTaquin->largeur; j++)
		{
			if (pTaquin->plateau[i][j] == count)
				count++;
			else
				return 0;
		}
	}
	return 1;
}

// fonction d'affichage du taquin
int displayTaquin(Taquin * pTaquin, int offset)
{
	for (int i = 0; i < pTaquin->hauteur; i++)
	{
		printf("\n");
		for (int j = 0; j < pTaquin->largeur; j++)
		{
			if (pTaquin->plateau[i][j] <= 9)
				printf(" |%d | ", pTaquin->plateau[i][j]);
			else
				printf(" |%d| ", pTaquin->plateau[i][j]);
		}
	}
	printf("\n");
	return 1;
}

// Fonction pour libérer les zones mémoires occupées par un taquin
int freeTaquin(Taquin * pTaquin)
{
	// On libère le plateau ligne par ligne
	for (int i = 0; i < pTaquin->hauteur; i++) if (pTaquin->plateau[i]) free(pTaquin->plateau[i]);
	// On libère le tableau qui stockait les lignes
	free(pTaquin->plateau);
	pTaquin->plateau = NULL;
	return 1;
}

// Boucle de jeu 
int gameLoop(int hauteur,int largeur,int minRandom, int maxRandom)
{
	Taquin pTaquin = {0};
	int endgame = 0;
	int modejeux = 0;

	createTaquin(&pTaquin, hauteur, largeur);
	initTaquin(&pTaquin);
	mixTaquin(&pTaquin, minRandom, maxRandom);
	displayTaquin(&pTaquin, 0);
	while (modejeux < 1 || modejeux > 3)
	{
		printf("\n!!MENU DU JEUX!!\n");
		printf("1/ Jouer en solo sans limite\n");
		printf("2/ Jouer avec un nombre de coup limiter\n");
		printf("3/ Voir une IA faire un score parfait\n");
		scanf_s("%d", &modejeux, 1);
	}


	if (modejeux == 1)
	{
		while (endgame != 1)
		{
			if (_kbhit())
			{
				char oujevais = _getch();
				moveTaquin(&pTaquin, (oujevais == 'H') ? GAUCHE : (oujevais == 'M') ? BAS : (oujevais == 'P') ? DROITE : (oujevais == 'K') ? HAUT : 0);
				system("cls");
				displayTaquin(&pTaquin, 0);
			}
			endgame = endTaquin(&pTaquin);
			if (endgame == 1)
			{
				freeTaquin(&pTaquin);
				return 1;
			}
		}
	}
	if (modejeux == 2)
		return 0;

	if (modejeux == 3)
	{
		ListAStar* res = NULL;
		SDL_Surface* vide = NULL;
		system("cls");
		displayTaquin(&pTaquin, 0);
		solveTaquin(&pTaquin, &res, vide);
		if (res == NULL)
			return 0;

		while (res != NULL)
		{
			displayTaquin(res->actuel,0);
			res = res->parent;
		}
	}

	return 1;
}
