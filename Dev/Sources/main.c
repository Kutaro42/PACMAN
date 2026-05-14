/**
 * @file    main.c
 * @author  IUT Informatique La Rochelle
 * @version v1.5
 * @date    2026
 * @brief   Point d'entree du jeu SPACE-DEFENDER.
 *
 * Architecture a trois boucles imbriquees :
 *  - Boucle externe (infinie) : menu → partie → ecran de fin → retour au menu.
 *  - Boucle de partie         : deroulement des niveaux 1 a 8.
 *  - Boucle de niveau         : attend que le joueur batte ou perde le niveau.
 */

#include "GLCD.h"
#include "GPIO.h"
#include "TIMERS.h"
#include "random.h"

#include "menu.h"
#include "game.h"


/* ---- Variables globales partagees entre les modules ---- */
/* Les variables modifiees depuis des ISR (Timer 8, Timer 2, Timer 1) doivent    */
/* etre volatile pour empecher le compilateur de les cacher en registre et de    */
/* rater les mises a jour effectuees hors du flot d'execution principal.        */
volatile int gameLost;     /**< 1 si le joueur a touché un bord bas ou un ennemi.  */
volatile int gameWon;      /**< 1 si les 8 niveaux ont été passés avec succès.       */
volatile int score;        /**< Score total du joueur (incrémente à chaque collisions avec la pac-gomme).    */
volatile int enemiScore;	/**< Score total de l'ennemi */
/**
 * @brief Point d'entree : initialise le materiel puis lance la boucle de jeu.
 *
 * Sequence de demarrage :
 *  1. Initialisation LCD, GPIO, timers et PRNG.
 *  2. Affichage du menu et attente de la selection JOUER.
 *  3. Boucle de partie : enchainent les niveaux jusqu'a la victoire ou la defaite.
 *  4. Affichage de l'ecran de resultat puis retour au menu.
 *
 * @return 0 (jamais atteint en pratique sur MCU bare-metal).
 */
int main(void)
{
    /* Initialisation du materiel */
    initGLCD();
    initGPIO();
    initTIMERS();
    initRandom();

    while (1)
    {
        /* --- Ecran de menu --- */
        displayMenu(JOUER);
        while (!refreshMenu());  /* Attente : joystick select sur JOUER */

        /* --- Initialisation de la partie --- */
        setRandom();
        gameLost = gameWon = 0;
        score = enemiScore = 0;
				//Gum * itsGum = newGum();//création de la gomme

        /* --- Boucle de partie : enchainement des niveaux --- */
        while (!gameLost && !gameWon)
        {
            setupLevel();           /* Prepare les donnees du niveau    itsGum            */
            startLevel();           /* Lance les timers, affiche le terrain         */

            while (!gameLost); /* Attente fin de niveau (ISR)       */

            stopLevel();            /* Arrete les timers   */

						}/* Passer au niveau suivant                */
            if (score >= 3)   gameWon = 1; /* Les 3 Gommes obtenues → victoire  */
						
						//conditions de défaites :
						if ((enemiScore>=3))	gameLost = 1;
						
        }

        /* --- Ecran de resultat --- */
        if (gameLost) {displayGameLost();}
        else if (gameWon) {displayGameWon();}
    
}

