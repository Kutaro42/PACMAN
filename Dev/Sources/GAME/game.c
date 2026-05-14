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
 * Positionne le vaisseau aleatoirement dans la zone de jeu, remet a zero
 * les compteurs de missile et d'ennemis, place 30 ennemis en 3 rangees
 * (niveaux 1 a 6) ou cree le boss unique (niveaux 7 et 8).
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
	
    if (score == 1)
    {
				Ennemi ennemis1;
        /* Vitesse de l'ennemi selon le niveau */
        setSpeedTimer8(NORMAL);


        /* Initialisation de l'ennemi */
        ennemis1.direction = 1;    /* Depart vers la droite */

        /* Position X aleatoire*/
        ennemis1.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemis1.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

            /* Dimensions et vies selon le type */
        ennemis1.width   = WIDTH_ENNEMI;
        ennemis1.height  = HEIGHT_ENNEMI1;
    }

    else if (level == 2)
    {
				Ennemi ennemis2;
        /* Vitesse de l'ennemi selon le niveau */
        setSpeedTimer8(SPEED);


        /* Initialisation de l'ennemi */
        ennemis2.direction = 1;    /* Depart vers la droite */

        /* Position X aleatoire*/
        ennemis2.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemis2.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

            /* Dimensions */
        ennemis2.width   = WIDTH_ENNEMI;
        ennemis2.height  = HEIGHT_ENNEMI1;
    }

    else
    {
				Ennemi ennemis3;
        /* Vitesse de l'ennemi selon le niveau */
        setSpeedTimer8(HARDCORE);


        /* Initialisation de l'ennemi */
        ennemis3.direction = 1;    /* Depart vers la droite */

        /* Position X aleatoire*/
        ennemis3.itsPosition.x = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

        /* Position Y aleatoire*/
        ennemis3.itsPosition.y = getRandom(0, GLCD_WIDTH - WIDTH_PACMAN);

            /* Dimensions et vies selon le type */
        ennemis3.width   = WIDTH_ENNEMI;
        ennemis3.height  = HEIGHT_ENNEMI1;
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
 * @brief Met a jour la position du vaisseau selon l'etat du joystick.
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
