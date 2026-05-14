/**
  ******************************************************************************************************************
  * @file    game.c
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   Gestion du jeu
  ******************************************************************************************************************/

#include "TIMERS.h"
#include "GPIO.h"
#include "GLCD.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"

#include "game.h"
#include "ImagesBMP.h"
#include "ExtGlobales.h"
#include "random.h"

#include <stdio.h>  /* sprintf() */



/* ---- Variables statiques du module ---- */
static int xPacMan;    /*< Position X courante du Pac (coin gauche). */
static int yPacMan;
/**
 * @brief Affiche le nom du niveau courant en haut a gauche de l'ecran.
 *
 * Niveaux 1 a 6 : "Niveau N" en jaune/6x8.
 * Niveau 7       : "Boss".
 * Niveau 8       : "Boss - Le retour..".
 */
void displayLevelName(void)
{
    char chaine[20];
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
    GLCD_SetFont(&GLCD_Font_6x8);

    if (level == 1)
    {
        sprintf(chaine, "Niveau %d", level);
        GLCD_DrawString(0, 0, chaine);
    }
    else if (level == 2)
    {
        sprintf(chaine, "Niveau %d", level);
        GLCD_DrawString(0, 0, chaine);
    }
    else if (level == 3)
    {
        sprintf(chaine, "Niveau %d", level);
        GLCD_DrawString(0, 0, chaine);
    }
}

/**
 * @brief Affiche le score courant au centre du bord superieur de l'ecran.
 *
 * Police 16x24, couleur bleue sur fond noir.
 */
void displayScore(void)
{
    char chaine[5];
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
    GLCD_SetFont(&GLCD_Font_16x24);
    sprintf(chaine, "%d", score);
    GLCD_DrawString((GLCD_WIDTH / 2) - 30, 0, chaine);
}
/**
 * @brief Initialise les donnees du niveau avant son demarrage.
 *
 * Positionne le joueur aleatoirement dans la zone de jeu, remet a zero
 * les compteurs de scores (joueur et ennemis), place aléatoirement la pac gomme
 * et charge les ennemis et leur directions en mémoire selon le score du joueur.
 * Configure aussi la vitesse du Timer 8 selon le niveau.
 */
void setupLevel(void)
{

    /* Position XY aleatoire du joueur au debut du niveau */
    xPacMan   = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);
		yPacMan 	= getRandom(GLCD_HEIGHT - HEIGHT_PACMAN, 0);

		/* Position XY aleatoire de la pac gomme*/
		Gum itsGum;
		itsGum.itsPosition.x=getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);
	  itsGum.itsPosition.y=getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);
	
		//on initialise le premier ennemi par défaut
				Ennemi ennemi1;
				
        /* Vitesse de l'ennemi par défaut */
        setSpeedTimer8(NORMAL);


        /* Initialisation de la direction de l'ennemi */
				Position * direction =trackGum(ennemi1.itsPosition,itsGum.itsPosition);

        /* Position X aleatoire*/
        ennemi1.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemi1.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

            /* Dimensions et vies*/
        ennemi1.width   = WIDTH_ENNEMI;
        ennemi1.height  = HEIGHT_ENNEMI1;
   
			
    if (score >= 1)//si le joueur récupère la première pac gomme, ajouter l'ennemi 2
    {
				Ennemi ennemi2;//on initialise un second ennemi
				ennemi2.inLive=true;

        /* Position X aleatoire*/
        ennemi2.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemi2.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);
			
				/* Initialisation de la direction de l'ennemi */
				Position * direction =trackGum(ennemi2.itsPosition,itsGum.itsPosition);

            /* Dimensions */
        ennemi2.width   = WIDTH_ENNEMI;
        ennemi2.height  = HEIGHT_ENNEMI1;
    }

    if (score == 2) // Si le joueur récupère la seconde pac gomme, initialiser l'ennemi 3
    {
				Ennemi ennemi3;
				ennemi3.inLive=true;

        /* Initialisation de la direction de l'ennemi */
				Position * direction =trackGum(ennemi3.itsPosition,itsGum.itsPosition);
        /* Position X aleatoire*/
        ennemi3.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemi3.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

            /* Dimensions et vies selon le type */
        ennemi3.width   = WIDTH_ENNEMI;
        ennemi3.height  = HEIGHT_ENNEMI1;
    }


}

/**
 * @brief Lance le niveau : efface l'ecran, affiche HUD et deMarre les timers.
 *
 * Active Timer 1 (vaisseau), Timer 8 (ennemis)
 * et l'interruption EXTI8 (bouton de tir).
 */
void startLevel(void)
{
    clearScreenGLCD();
    displayLevelName();
    displayScore();
    startTimer1();            /* Debut du deplacement du vaisseau        */
    startTimer8();            /* Debut du deplacement des ennemis        */
}

/**
 * @brief Arrete tous les timers et l'interruption de tir en fin de niveau.
 */
void stopLevel(void)
{
    stopTimer1();
    stopTimer8();
}

/**
 * @brief Met a jour la position du joueur selon l'etat du joystick.
 *
 * Appelee par l'ISR du Timer 1 (~3.6 ms).
 * - Joystick gauche : efface le pixel droit du vaisseau, decremente xSpaceShip.
 * - Joystick droite : efface le pixel gauche du vaisseau, incremente xSpaceShip.
 * Le vaisseau s'arrete aux bords (x=0 et x=GLCD_SIZE_Y-WIDTH_SPACE_SHIP).
 */
void updateSpaceShip(void)
{
    if (joystickLeftPressed() && xPacMan > 0)
    {
        /* Effacer la colonne droite avant de deplacer vers la gauche */
        clearZone(xPacMan + WIDTH_PACMAN - 1,
                  yPacMan, 1, HEIGHT_PACMAN);
        xPacMan--;
    }
    else if (joystickRightPressed() && xPacMan + WIDTH_PACMAN < GLCD_SIZE_Y)
    {
        /* Effacer la colonne gauche avant de deplacer vers la droite */
        clearZone(xPacMan,
                  yPacMan, 1, HEIGHT_PACMAN);
        xPacMan++;
    }
    else if (joystickUpPressed() /*&& yPacMan + HEIGHT_PACMAN < GLCD_SIZE_X*/)
    {
        /* Effacer la colonne gauche avant de deplacer vers la droite */
        clearZone(xPacMan,
                  yPacMan + HEIGHT_PACMAN-1, WIDTH_PACMAN, 1);
        yPacMan--;
    }
		else if (joystickDownPressed() /*&& yPacMan + HEIGHT_PACMAN < GLCD_SIZE_X*/)
    {
        /* Effacer la colonne gauche avant de deplacer vers la droite */
        clearZone(xPacMan,
                  yPacMan, WIDTH_PACMAN, 1);
        yPacMan++;
    }
		



    /* Redessiner le vaisseau a la nouvelle position */
   /* GLCD_DrawBitmap(xSpaceShip, GLCD_HEIGHT - HEIGHT_SPACE_SHIP,
                    WIDTH_SPACE_SHIP, HEIGHT_SPACE_SHIP,
                    (unsigned char *)bmpSpaceShip);*/
		
		    /* Redessiner le vaisseau a la nouvelle position */
    GLCD_DrawBitmap(xPacMan, yPacMan,
                    WIDTH_PACMAN, HEIGHT_PACMAN,
                    (unsigned char *)bmpPacManClose);
}


/**
 * @brief Met a jour la position de tous les ennemis et detecte les collisions.
 *
 * Appelee par l'ISR du Timer 8 (vitesse variable selon le niveau).
 *
 * Algorithme pour chaque ennemi vivant :

 */
void updateEnnemis(void)
{

}

