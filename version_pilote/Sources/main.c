/**
 * @file    main.c
 * @author  Alexandre AUBIN/leo BOUCARD (IUT Informatique La Rochelle)
 * @version v1.5
 * @date    2026
 * @brief   Point d'entree du jeu PUCK-MAN.
 *
 * Architecture :
 *  - Boucle externe (infinie) : menu → partie → ecran de fin → retour menu.
 *  - La partie est un seul niveau continu :
 *      setupLevel() + startLevel() une seule fois,
 *      puis les ISR (TIM1, TIM8) font avancer le jeu jusqu'a ce que
 *      gameLost ou gameWon soit positionne a 1.
 */

#include "GLCD.h"
#include "GPIO.h"
#include "TIMERS.h"
#include "random.h"

#include "menu.h"
#include "game.h"

/* ---- Variables globales partagees entre modules (definies ici) --------- */
/*
 * volatile obligatoire : ces variables sont ecrites depuis les ISR (TIM1,
 * TIM8) et lues en boucle dans main(). Sans volatile, le compilateur
 * pourrait cacher leur valeur en registre et ne jamais voir les mises a
 * jour effectuees par les ISR.
 */
volatile int gameLost;    /**< 1 si le joueur a touche un ennemi ou si enemiScore >= 3. */
volatile int gameWon;     /**< 1 si le joueur a atteint un score de 3.                  */
volatile int score;       /**< Score du joueur (gommes collectees).                     */
volatile int enemiScore;  /**< Score des ennemis (gommes collectees par les ennemis).   */

/**
 * @brief Point d'entree : initialise le materiel puis lance la boucle de jeu.
 *
 * Sequence de demarrage :
 *  1. Initialisation LCD, GPIO, timers et PRNG.
 *  2. Affichage du menu, attente de la selection JOUER.
 *  3. Initialisation des variables de partie.
 *  4. setupLevel()  : positionne joueur, ennemis et premiere gomme.
 *  5. startLevel()  : efface l'ecran, dessine la scene, lance les timers.
 *  6. Attente passive (while) : les ISR font tout le travail.
 *  7. stopLevel()   : arrete les timers.
 *  8. Affichage de l'ecran de resultat, puis retour au menu.
 *
 * @return 0 (jamais atteint sur MCU bare-metal).
 */
int main(void)
{
    /* ---- Initialisation du materiel ---- */
    initGLCD();
    initGPIO();
    initTIMERS();
    initRandom();

    while (1)
    {
        /* ---- Ecran de menu ---- */
        displayMenu(JOUER);
        while (!refreshMenu());   /* Attend que le joueur choisisse "Jouer" */

        /* ---- Initialisation de la partie ---- */
        setRandom();
        gameLost   = 0;
        gameWon    = 0;
        score      = 0;
        enemiScore = 0;

        /*
         * Une seule session de jeu : pas de boucle de niveaux.
         * Le jeu est continu ; les ennemis s'ajoutent et accélèrent
         * dynamiquement selon les scores. Les ISR (TIM1 / TIM8)
         * mettent à jour gameLost et gameWon.
         */
        setupLevel();   /* Positionne joueur, ennemis, premiere gomme           */
        startLevel();   /* Efface ecran, dessine la scene, demarre les timers   */

        /* ---- Attente de fin de partie (ISR ecrivent gameLost / gameWon) ---- */
        while (!gameLost && !gameWon);

        /* ---- Arrêt des timers ---- */
        stopLevel();

        /* ---- Écran de resultat ---- */
        if (gameLost)     displayGameLost();
        else if (gameWon) displayGameWon();

    } /* Retour au menu */

    return 0; /* Normalement jamais atteint */
}
