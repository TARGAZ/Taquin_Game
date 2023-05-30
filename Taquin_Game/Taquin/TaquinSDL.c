#include "TaquinSDL.h"
#include "AStar.h"
#include <time.h>
#include "SDL_gfxprimitives.h"


// Fonction permettant de cr�er le taquin en SDL � partir de sa taille et du fichier BMP � utiliser
// Cette fonction cr�e aussi la fen�tre SDL et la stocke dans la structure TaquinSDL
int createTaquinSDL(TaquinSDL * pTaquinSDL,int hauteur, int largeur, char * pathBMPfile)
{
	// Test pour v�rifier que les donn�es pass�es ne sont pas corrompues
	if(!pTaquinSDL) return 0;

	// initialisation de la fen�tre SDL
	pTaquinSDL->pWindow = NULL;
	// initialisation de l'image � NULL
	pTaquinSDL->pFond = NULL;
	// On cr�e le taquin qui sera utilis� pour jouer
	pTaquinSDL->taquin.plateau = NULL;
	createTaquin(&(pTaquinSDL->taquin),hauteur,largeur);

	// On met � jour la taille du taquin
	largeur = pTaquinSDL->taquin.largeur;
	hauteur = pTaquinSDL->taquin.hauteur;

	// On initialise la SDL
	// On initialise non seulement le mode vid�o mais aussi la gestion du temps pour pouvoir utiliser SDL_GetTicks()
	if(SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER ))
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	// On charge le BMP qui servira de fond au taquin
	pTaquinSDL->pFond= SDL_LoadBMP(pathBMPfile);
	if(!pTaquinSDL->pFond) 
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}

	// Initialisation de la fen�tre

	// On r�cup�re la taille d'une case
	pTaquinSDL->resX = pTaquinSDL->pFond->w/largeur;
	pTaquinSDL->resY = pTaquinSDL->pFond->h/hauteur;

	// On cr�e la fen�tre en fonction de la r�solution de l'image d'entr�e
	pTaquinSDL->pWindow = SDL_SetVideoMode(pTaquinSDL->resX*largeur, pTaquinSDL->resY*hauteur, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
	if(!pTaquinSDL->pWindow)
	{
		freeTaquinSDL(pTaquinSDL);
		return 0;
	}


	SDL_WM_SetCaption("TAQUIN","TAQUIN" );

	SDL_WM_SetIcon(NULL,NULL);

	return 1;
	
}


int displayCaseTaquin(TaquinSDL * pTaquinSDL,unsigned char caseTaquin, SDL_Rect * pDest, int x, int y, int refresh)
{
	// TODO: displayCaseTaquin
	// on d�finit o� on veut la dessiner
	int positionx = (caseTaquin % pTaquinSDL->taquin.largeur);
	int positiony = (caseTaquin / pTaquinSDL->taquin.largeur);

	// Si la case n'est pas vide ...
	// on calcule o� est la case "caseTaquin" dans l'image initiale pour -par la suite - venir d�couper la zone qui correspond � la case
	SDL_Rect imgFrom = { positionx * pTaquinSDL->resX, positiony * pTaquinSDL->resY, pTaquinSDL->resX, pTaquinSDL->resY };

	if (caseTaquin == 0)
	{
		SDL_Surface* caseNoire = SDL_CreateRGBSurface(0, pTaquinSDL->resX * pTaquinSDL->taquin.largeur, pTaquinSDL->resY * pTaquinSDL->taquin.hauteur, 32, 0, 0, 0, 0);
		SDL_LowerBlit(caseNoire, &imgFrom, pTaquinSDL->pWindow, pDest);
		if (refresh) SDL_UpdateRect(pTaquinSDL->pWindow, pDest->x, pDest->y, pDest->w, pDest->h);
		SDL_FreeSurface(caseNoire);
		return 1;
	}

	// On dessine la case dans la fen�tre (en d�coupant dans l'image initiale avec la zone d�finie ci-dessus)
	SDL_LowerBlit(pTaquinSDL->pFond, &imgFrom, pTaquinSDL->pWindow, pDest);
	if(refresh) SDL_UpdateRect(pTaquinSDL->pWindow,pDest->x,pDest->y,pDest->w,pDest->h);

	return 1;
}

// fonction pour rendre le taquin dans son �tat actuel
int displayTaquinSDL(TaquinSDL * pTaquinSDL)
{
	// Test pour v�rifier que les donn�es pass�es ne sont pas corrompues
	if(!pTaquinSDL || !pTaquinSDL->taquin.plateau || !pTaquinSDL->pWindow ) return 0;

	// On dessine les cases une par une en allant d�couper l'image de fond avec un SDL_Rect
	{


		// On dessine le taquin termin� pour afficher quelque chose mais il faudra le changer
		//SDL_BlitSurface(pTaquinSDL->pFond,NULL,pTaquinSDL->pWindow,NULL);

		// TODO: displayTaquinSDL
		// ...
		if (endTaquin(&pTaquinSDL->taquin) == 1)
		{
			SDL_BlitSurface(pTaquinSDL->pFond, NULL, pTaquinSDL->pWindow, NULL);
			SDL_UpdateRect(pTaquinSDL->pWindow, 0, 0, 0, 0);
			return 1;
		}

		SDL_Rect temp;
		for (int i = 0; i < pTaquinSDL->taquin.hauteur; i++)
		{
			for (int j = 0; j < pTaquinSDL->taquin.largeur; j++)
			{
				temp.x = i * pTaquinSDL->resX;
				temp.y = j * pTaquinSDL->resY;
				temp.h = pTaquinSDL->resY;
				temp.w = pTaquinSDL->resX;

				displayCaseTaquin(pTaquinSDL, pTaquinSDL->taquin.plateau[j][i], &temp,i, j, 1);
				
			}
		}
		displayTaquin(&pTaquinSDL->taquin, 0);
		// On met � jour la fen�tre compl�te
		SDL_UpdateRect(pTaquinSDL->pWindow,0,0,0,0);

	}


	return 1;
}

// fonction permettant de faire le rendu SDL du taquin et de jouer (gestion des �v�nements � l'int�rieur de la fonction)
int gameLoopSDL(int hauteur,int largeur, char * pathBMPfile, int minRandom, int maxRandom)
{
	int end = 0;
	TaquinSDL t;

	// On cr�e le taquin et la fen�tre pour le dessiner
	if(!createTaquinSDL(&t,hauteur,largeur,pathBMPfile)) return 0;


	// On boucle sur le jeu tant qu'on a pas demand� de quitter
	while(end>=0)
	{
		int tick = SDL_GetTicks();

		// On m�lange le taquin
		mixTaquin(&(t.taquin), minRandom, maxRandom);

		// On affiche le taquin m�lang�
		displayTaquinSDL(&t);

		end = 0;

		// On boucle tant que la solution n'a pas �t� trouv�e
		while(!end )
		{
			// Gestion des �v�nements
			SDL_Event e;

			int dt = SDL_GetTicks()-tick;
			tick +=dt;

			while(!end && SDL_PollEvent(&e))
			{
				switch(e.type)
				{
					case SDL_KEYDOWN:
						{
							deplacement d = AUCUN;
							
							switch(e.key.keysym.sym)
							{
								// On d�finit le mouvement � effectuer
								case SDLK_q:		;
								case SDLK_LEFT:		d = HAUT;
									break;
								case SDLK_d:
								case SDLK_RIGHT:	d = BAS;
									break;
								case SDLK_z:
								case SDLK_UP:		d = GAUCHE;
									break;
								case SDLK_s:
								case SDLK_DOWN:		d = DROITE;
									break;
								case SDLK_ESCAPE:	
									// On quitte l'application
									end = -1;
									break;
								case SDLK_F1:
									// On d�clenche la r�solution du taquin
									// r�solution SDL � faire par la suite pour laisser la main � l'utilisateur :
									// - Arr�ter la r�solution (appui sur n'importe qu'elle touche
									// - Quitter l'application (ECHAP ou CROIX en haut � droite)
									{
										
										//on initialise les variables permettant de r�cup�rer les informations issues de la r�solution
										deplacement * tabDeplacements = NULL;
										unsigned long nbDeplacements = 0;
										unsigned long nbSommetsParcourus = 0;
										unsigned long timeElapsed = 0;

										// On demande la r�solution du taquin � l'ordinateur
										ListAStar* resolve = NULL;
										if(solveTaquin(&(t.taquin), &resolve, t.pWindow))
										{
											// Si on a trouv� une solution on affiche les informations issues de la r�solution
											unsigned long i;
											int res = 0;
											system("cls");
											t.taquin = *resolve->actuel;
											displayTaquinSDL(&t);
											while (resolve != NULL)
											{
												displayTaquin(resolve->actuel, 0);
												resolve = resolve->parent;
											}
											
											//return 1;
										}
										// Si la r�solution n'a pas fonctionn�, on affiche le taquin tel qu'il �tait avant r�solution (on efface l'icone de "progression" si elle avait �t� dessin�e)
										else displayTaquinSDL(&t);

										if(tabDeplacements) 
										{
											free(tabDeplacements);
											tabDeplacements = NULL;
										}
									}

									break;
							}
							
							if(d!=AUCUN && moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t);

						}
						break;

					case SDL_MOUSEBUTTONDOWN:
						{
							// On r�cup�re la position et l'�tat des boutons de la souris
							int x,y;
							Uint8 state;
							state = SDL_GetMouseState(&x,&y);

							if(state & SDL_BUTTON_LEFT)
							{

								deplacement d = AUCUN;

								// On r�cup�re la position de la case dans le taquin
								int posX = x/t.resX;
								int posY = y/t.resY;

								// TODO: On d�finit le d�placement � effectuer (diff�rence en X de +/- 1 ou en Y de +/-1 => DEPLACEMENT sinon AUCUN)
								
								// ...
									
								// On applique le d�placement
								if(moveTaquin(&(t.taquin),d)) displayTaquinSDL(&t);
							}
						}
						break;

					case SDL_QUIT:
						// On quitte
						end = -1;
						break;
				}
			}

			// On relache du temps processeur pour les autres processus
			SDL_Delay(1);

			if(!end) end = endTaquin(&(t.taquin));
		}

		// Si on n'a pas demand� � quitter c'est qu'on a termin� le taquin
		if(end>0)
		{
			// Affichage de l'image compl�te par dessus le tout

			SDL_Delay(1000);
		}

		// On r�initialise le taquin pour le prochain tour de jeu
		initTaquin(&(t.taquin));
	}

	// On lib�re le taquin et les surfaces SDL
	freeTaquinSDL(&t);

	/* Shut them both down */
	SDL_Quit();


	return 1;
}

// fonction pour lib�rer le taquins et les surfaces SDL utilis�es
int freeTaquinSDL(TaquinSDL * pTaquinSDL)
{
	// test pour savoir si les donn�es pass�es ne sont pas corrompues
	if(!pTaquinSDL) return 0;

	// On lib�re la fen�tre SDL
	if(pTaquinSDL->pWindow) SDL_FreeSurface(pTaquinSDL->pWindow);

	// On lib�re l'image de fond
	if(pTaquinSDL->pFond) SDL_FreeSurface(pTaquinSDL->pFond);

	// On lib�re le taquin (mode console)
	freeTaquin(&(pTaquinSDL->taquin));

	return 1;
}