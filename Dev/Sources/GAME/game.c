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
static int xSpaceShip;    /*< Position X courante du vaisseau (coin gauche). */
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

    if (level <= 6)
    {
        sprintf(chaine, "Niveau %d", level);
        GLCD_DrawString(0, 0, chaine);
    }
    else if (level == 7)
    {
        GLCD_DrawString(0, 0, "Boss");
    }
    else if (level == 8)
    {
        GLCD_DrawString(0, 0, "Boss - Le retour..");
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
    GLCD_SetForegroundColor(GLCD_COLOR_BLUE);
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
    int i;

    /* Position X aleatoire du vaisseau au debut du niveau */
    xSpaceShip   = getRandom(0, GLCD_WIDTH - WIDTH_SPACE_SHIP);
    rocketActive = 0;
    levelPassed  = 0;
    nbDeadEnnemis = 0;

    if (level <= 6)
    {
        /* --- Niveaux classiques : 30 ennemis en grille 10x3 --- */
        nbEnnemis = MAX_ENEMIES;

        /* Vitesse des ennemis selon le niveau */
        switch (level)
        {
            case 1: case 2: case 3: setSpeedTimer8(NORMAL);     break;
            case 4:                 setSpeedTimer8(SPEED);      break;
            case 5:                 setSpeedTimer8(HARDCORE);   break;
            case 6:                 setSpeedTimer8(IMPOSSIBLE); break;
        }

        /* Initialisation de chaque ennemi */
        for (i = 0; i < nbEnnemis; i++)
        {
            ennemis[i].direction = 1;    /* Depart vers la droite */
            ennemis[i].dead      = 0;

            /* Position X : 10 colonnes, espacement ENEMY_COL_SPACING pixels */
            ennemis[i].x = (i % 10) * ENEMY_COL_SPACING + 1;

            /* Position Y : 3 rangees distinctes */
            if      (i < 10) ennemis[i].y = ENEMY_ROW1_Y;
            else if (i < 20) ennemis[i].y = ENEMY_ROW2_Y;
            else             ennemis[i].y = ENEMY_ROW3_Y;

            /* Type d'ennemi selon le niveau :
             *  Niveaux 1/2/3 : un seul type  (ennemis homogenes)
             *  Niveaux 4/5/6 : mix des 3 types */
            switch (level)
            {
                case 1: ennemis[i].type = ENNEMI_1; break;
                case 2: ennemis[i].type = ENNEMI_2; break;
                case 3: ennemis[i].type = ENNEMI_3; break;
                default:
                    if      (i < 10) ennemis[i].type = ENNEMI_1;
                    else if (i < 20) ennemis[i].type = ENNEMI_2;
                    else             ennemis[i].type = ENNEMI_3;
                    break;
            }

            /* Dimensions et vies selon le type */
            switch (ennemis[i].type)
            {
                case ENNEMI_1:
                    ennemis[i].width   = WIDTH_ENNEMI1;
                    ennemis[i].height  = HEIGHT_ENNEMI1;
                    ennemis[i].nbLives = 1;
                    break;
                case ENNEMI_2:
                    ennemis[i].width   = WIDTH_ENNEMI2;
                    ennemis[i].height  = HEIGHT_ENNEMI2;
                    ennemis[i].nbLives = 2;
                    break;
                case ENNEMI_3:
                    ennemis[i].width   = WIDTH_ENNEMI3;
                    ennemis[i].height  = HEIGHT_ENNEMI3;
                    ennemis[i].nbLives = 3;
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        /* --- Niveaux Boss : un seul ennemi avec beaucoup de vies --- */
        nbEnnemis = 1;

        switch (level)
        {
            case 7: setSpeedTimer8(NORMAL);     break;
            case 8: setSpeedTimer8(IMPOSSIBLE); break;
        }

        ennemis[0].direction = 1;
        ennemis[0].dead      = 0;
        ennemis[0].x        = BOSS_INITIAL_X;
        ennemis[0].y        = BOSS_INITIAL_Y;
        ennemis[0].type     = BOSS;
        ennemis[0].nbLives  = BOSS_LIVES;
        ennemis[0].height   = HEIGHT_BOSS;
        ennemis[0].width    = WIDTH_BOSS;
    }
}

/**
 * @brief Lance le niveau : efface l'ecran, affiche HUD et deMarre les timers.
 *
 * Active Timer 1 (vaisseau), Timer 2 (missile), Timer 8 (ennemis)
 * et l'interruption EXTI8 (bouton de tir).
 */
void startLevel(void)
{
    clearScreenGLCD();
    displayLevelName();
    displayScore();
    startTimer1();            /* Debut du deplacement du vaisseau        */
    startTimer2();            /* Debut du suivi du missile               */
    startTimer8();            /* Debut du deplacement des ennemis        */
    startInterruptBP_USER();  /* Activation du tir via le bouton USER    */
}

/**
 * @brief Arrete tous les timers et l'interruption de tir en fin de niveau.
 */
void stopLevel(void)
{
    stopTimer1();
    stopTimer2();
    stopTimer8();
    stopInterruptBP_USER();
}

/**
 * @brief Met a jour la position du Joueur selon l'etat du joystick.
 *
 * Appelee par l'ISR du Timer 1 (~3.6 ms).
 * - Joystick gauche : efface le pixel droit du joueur, decremente xSpaceShip.
 * - Joystick droite : efface le pixel gauche du vaisseau, incremente xSpaceShip.
 * Le vaisseau s'arrete aux bords (x=0 et x=GLCD_SIZE_Y-WIDTH_SPACE_SHIP).
 */
void updateSpaceShip(void)
{
    if (joystickLeftPressed() && xSpaceShip > 0)
    {
        /* Effacer la colonne droite avant de deplacer vers la gauche */
        clearZone(xSpaceShip + WIDTH_SPACE_SHIP - 1,
                  GLCD_HEIGHT - HEIGHT_SPACE_SHIP, 1, HEIGHT_SPACE_SHIP);
        xSpaceShip--;
    }
    else if (joystickRightPressed() && xSpaceShip + WIDTH_SPACE_SHIP < GLCD_SIZE_Y)
    {
        /* Effacer la colonne gauche avant de deplacer vers la droite */
        clearZone(xSpaceShip,
                  GLCD_HEIGHT - HEIGHT_SPACE_SHIP, 1, HEIGHT_SPACE_SHIP);
        xSpaceShip++;
    }
    /* Redessiner le vaisseau a la nouvelle position */
   /* GLCD_DrawBitmap(xSpaceShip, GLCD_HEIGHT - HEIGHT_SPACE_SHIP,
                    WIDTH_SPACE_SHIP, HEIGHT_SPACE_SHIP,
                    (unsigned char *)bmpSpaceShip);*/
		
		    /* Redessiner le vaisseau a la nouvelle position */
    GLCD_DrawBitmap(xSpaceShip, GLCD_HEIGHT - HEIGHT_PACMAN,
                    WIDTH_PACMAN, HEIGHT_PACMAN,
                    (unsigned char *)bmpPacManClose);
}

/**
 * @brief Fait apparaitre la Pac gomme sur l'écran à une position (x,y) aléatoire.
 *
 * Appelee par l'ISR du Timer 2 (~1.8 ms).
 * 
 * 
 */
void Spawn_Pac_Gum(void)
{
   
}

/**
 * @brief Met fin au niveau en fonction de l'entitée qui touche la pac-gomme en premier
 * 
 * Si la pac gomme est touchée par une entité : est-ce le joueur ? si oui : fin de niveau, sinon vie-=1;
 */
bool Gum_touched(){
	//code
	

}

/**
 * @brief Declenche un tir si aucun missile n'est deja en vol.
 *
 * Appelee depuis l'ISR EXTI9_5 (bouton BP_USER).
 * Le missile part du centre haut du vaisseau, a la position Y definie
 * par ROCKET_START_Y (= GLCD_HEIGHT - HEIGHT_SPACE_SHIP - HEIGHT_ROCKET).
 */
void shoot(void)
{
    if (!rocketActive)  /* Un seul missile autorise a la fois */
    {
        rocketActive = 1;
        xRocket      = xSpaceShip + WIDTH_SPACE_SHIP / 2;
        yRocket      = ROCKET_START_Y;
    }
}

/**
 * @brief Met a jour la position de tous les ennemis et detecte les collisions.
 *
 * Appelee par l'ISR du Timer 8 (vitesse variable selon le niveau).
 *
 * Algorithme pour chaque ennemi vivant :
 *  1. Si l'ennemi atteint un bord horizontal → descend de 20 px et inverse sa direction.
 *  2. Sinon efface uniquement le pixel de trace dans l'ancienne direction.
 *  3. Deplace d'un pixel dans la direction courante.
 *  4. Teste la collision avec le missile :
 *     - Decremente les vies de l'ennemi.
 *     - Si vies = 0 → ennemi marque comme dead, attribution de points, compteur incremente.
 *     - Met a jour le score, supprime le missile.
 *  5. Si pas de collision → redessine le sprite (couleur = etat de vies pour ENNEMI_2/3).
 *  6. Si l'ennemi atteint la hauteur du vaisseau → gameLost = true.
 *  7. Si tous les ennemis sont morts → levelPassed = true.
 */
void updateEnnemis(void)
{
    
        }
    }
}
