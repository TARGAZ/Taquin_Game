#include "AStar.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "SDL.h"
#include "SDL_image.h"



// fonction pour créer (allouer) un noeud de liste et l'initialiser avec le taquin passé en paramètre
ListAStar* createNodeList(Taquin * pTaquin, int gValue, int fValue, deplacement d, ListAStar* pPrevPlay)
{
	if (pPrevPlay != NULL && cancelMove(pPrevPlay->d) == d)
		return NULL;

	ListAStar* newnode;
	int canplay;

	newnode = (ListAStar*)calloc(1, sizeof(ListAStar));
	if (!newnode)
		return NULL;
	newnode->actuel = (Taquin*)calloc(1, sizeof(Taquin));
	if (!newnode->actuel)
	{
		free(newnode);
		return NULL;
	}
	
	canplay = copyTaquin(newnode->actuel, pTaquin);
	if (canplay == 0)
	{
		free(newnode->actuel);
		free(newnode);
		return NULL;
	}

	canplay = moveTaquin(newnode->actuel, d);
	if (canplay == 0)
	{
		free(newnode->actuel);
		free(newnode);
		return NULL;
	}

	newnode->d = d;
	newnode->parent = pPrevPlay;
	newnode->g = gValue;
	newnode->h = h(newnode->actuel) + gValue;

	return newnode;
}

// Insertion dans la liste de façon triée ou non
// si on passe le paramètre tri à 0, on insère en tête de liste
int insertList(ListAStar** ppHead, ListAStar* pNode, int tri)
{
	if (!(*ppHead))
	{
		(*ppHead) = pNode;
		return 1;
	}

	if (tri == 1)
	{
		// On insere directement le node dans la liste en début !!!!!!!
		pNode->enfant = (*ppHead);
		(*ppHead) = pNode;
		return 1;
	}
	ListAStar* curseur = (*ppHead);
	// Si
	while (curseur->enfant && curseur->enfant->h <= pNode->h)
	{
		curseur = curseur->enfant;
	}
	pNode->enfant = curseur->enfant;
	curseur->enfant = pNode;
	return 1;
}

// Fonction pour prélever le noeud en tête de liste
// Retourne le noeud prélevé
ListAStar* popList(ListAStar** ppHead)
{
	if (!ppHead || !(*ppHead))
		return 0;
	ListAStar* pop = (*ppHead);
	(*ppHead) = (*ppHead)->enfant;
	pop->enfant = NULL;
	return pop;
}

// fonction qui retourne le noeud dans lequel on trouve le taquin passé en paramètre (pointeur sur le pointeur dans la liste)
ListAStar** isInList(ListAStar** ppHead, Taquin * pTaquin)
{
	ListAStar* isin = (*ppHead);
	while (isin && isin->enfant)
	{
		if (equalTaquin(isin->actuel, pTaquin) == 1)
			return NULL;
		isin = isin->enfant;
	}
	if (equalTaquin(isin->actuel, pTaquin) == 1)
		return NULL;

	return ppHead;


}

// fonction pour afficher une liste
// si on met displayFGH à 0 les valeur de F, G et H ne sont pas affichées
int displayList(ListAStar* pHead, int displayFGH)
{

	return 1;
}

// Fonction pour résoudre le taquin en utilisant l'algorithme A*
// La fonction prend comme taquin initial le taquin spécifié par pTaquin
// elle remplit 
//   - pTabDeplacement (un tableau contenant les déplacements à effectuer pour résoudre le taquin)
//   - pNbDeplacements
//   - pNbTaquinsGeneres
//   - pTimeElapsed
// Si stepByStep est différent de 0 on affiche dans la console toutes les étapes de la résolution
// pWindow
int solveTaquin(Taquin *pTaquin, ListAStar** res, SDL_Surface* pWindow)
{
	int hValue = 0;
	ListAStar* tempnode = NULL;
	deplacement prevmove = AUCUN;

	ListAStar* OpenList = NULL;
	ListAStar* CloseList = NULL;

	OpenList = NULL;
	CloseList = createNodeList(pTaquin, 0, 0, AUCUN, NULL);

	while (endTaquin(CloseList->actuel) != 1)
	{
		tempnode = createNodeList(CloseList->actuel, CloseList->g + 1, hValue, GAUCHE, CloseList);
		if (tempnode != NULL)
		{
			insertList(&OpenList, tempnode, 0);
			tempnode = NULL;
		}
			
		tempnode = createNodeList(CloseList->actuel, CloseList->g + 1, hValue, DROITE, CloseList);
		if (tempnode != NULL)
		{
			insertList(&OpenList, tempnode, 0);
			tempnode = NULL;
		}

		tempnode = createNodeList(CloseList->actuel, CloseList->g + 1, hValue, BAS, CloseList);
		if (tempnode != NULL)
		{
			insertList(&OpenList, tempnode, 0);
			tempnode = NULL;
		}

		tempnode = createNodeList(CloseList->actuel, CloseList->g + 1, hValue, HAUT, CloseList);
		if (tempnode != NULL)
		{
			insertList(&OpenList, tempnode, 0);
			tempnode = NULL;
		}

		if (OpenList == NULL)
			return 0;

		//ça marche
		tempnode = popList(&OpenList);
		while (isInList(&CloseList, tempnode->actuel) == NULL)
			tempnode = popList(&OpenList);

		insertList(&CloseList, tempnode, 1);

	}
		*res = CloseList;
		return 1;
	}
// fonction d'évaluation pour la résolution avec AStar
int h(Taquin * pTaquin)
{
	int hValue = 0;
	int count = 0;
	for (int i = 0; i < pTaquin->hauteur; i++)
	{
		for (int j = 0; j < pTaquin->largeur; j++)
		{
			hValue = (abs(count - pTaquin->plateau[i][j])) + hValue;
			count++;
		}
	}

	return hValue;
}