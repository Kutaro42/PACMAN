/**
  ******************************************************************************************************************
  * @file    game.h
  * @author  Alexandre AUBIN/Leo BOUCARD (IUT Informatique La Rochelle)
  * @version v1.5
  * @date    2026
  * @brief   Declarations des types, structures et fonctions de gestion du jeu.
  *
  * Ce module gère toute la logique de jeu : initialisation,
  * deplacement du joueur et des ennemis, gestion de la pac-gomme et détection
  * des collisions. Les timers TIM1 et TIM8 appellent les fonctions de mise
  * a jour depuis leurs ISR respectives.
  *******************************************************************************************************************/

#ifndef __GAME_H
#define __GAME_H

#include "stdbool.h"

/* ======================================================================== */
/*                              STRUCTURES                                  */
/* ======================================================================== */

/**
 * @brief Coordonnees (x, y) en pixels d'un point de l'écran.
 * Permet une implémentation des position plus facile à manipuler.
 */
typedef struct Position Position;
struct Position {
    int x;  /**< Coordonnee horizontale (0 = bord gauche). */
    int y;  /**< Coordonnee verticale   (0 = bord haut).   */
};

/**
 * @brief Décrit un ennemi (Pac-Man rouge) à l'ecran.
 */
typedef struct Ennemi Ennemi;
struct Ennemi {
    int      direction;    /**< Direction actuelle : DIR_RIGHT/LEFT/UP/DOWN. */
    bool     inLive;       /**< true si l'ennemi est actif.                  */
    Position itsPosition;  /**< Coin superieur gauche du sprite (pixels).    */
    int      width;        /**< Largeur du sprite ( en px).                  */
    int      height;       /**< Hauteur du sprite ( en px).                  */
};

/**
 * @brief Decrit la pac-gomme (cible commune joueur/ennemis).
 */
typedef struct Gum Gum;
struct Gum {
    Position itsPosition;  /**< Coin superieur gauche du sprite (pixels). */
    bool     claimed;      /**< true si la gomme a dejà été collectée.    */
    int      width;        /**< Largeur du sprite (pixels).               */
    int      height;       /**< Hauteur du sprite (pixels).               */
};

/* ======================================================================== */
/*                           FONCTIONS D'AFFICHAGE                         */
/* ======================================================================== */

/**
 * @brief Affiche les scores du joueur et de l'ennemi dans le HUD.
 *
 * Police 6x8, couleur jaune, en haut à gauche de l'écran.
 *
 * @param score      Score courant du joueur.
 * @param enemiScore Score courant des ennemis.
 */
void displayScores(int score, int enemiScore);

/**
 * @brief Rafraichit l'affichage du score (appelle displayScores avec les
 *        variables globales score et enemiScore).
 */
void displayEndGame(void);

/* ======================================================================== */
/*                          CYCLE DE VIE DU JEU                             */
/* ======================================================================== */

/**
 * @brief Initialise les donnees de la partie avant son demarrage.
 *
 * Positionne le joueur au centre, cree le premier ennemi, place la
 * premiere pac-gomme à une position aleatoire.
 * Ne dessine pas (c'est startLevel qui le fait).
 */
void setupLevel(void);

/**
 * @brief Lance la partie : efface l'ecran, affiche la scene, demarre les timers.
 *
 * Active TIM1 (joueur, ~3.6 ms) et TIM8 (ennemis, ~16 ms).
 * (on peut toujours modifier si le jeu devient injouable)
 */
void startLevel(void);

/**
 * @brief Arrete TIM1 et TIM8 en fin de partie.
 */
void stopLevel(void);

/* ======================================================================== */
/*                     FONCTIONS DE MISE A JOUR (ISR)                      */
/* ======================================================================== */

/**
 * @brief Met à jour la position du joueur selon le joystick.
 *
 * Appelee par l'ISR de TIM1 (~3.6 ms).
 * Gere le deplacement, l'animation, la collision avec les ennemis
 * (→ gameLost) et la collision avec la gomme (→ score, nouvel ennemi).
 */
void updatePlayer(void);

/**
 * @brief Met a jour tous les ennemis : deplacement, collisions, score ennemi.
 *
 * Appelee par l'ISR de TIM8 (vitesse variable : NORMAL → SPEED → HARDCORE).
 * Gere : mouvement vers la gomme, collision ennemi/gomme (→ enemiScore,
 * acceleration), collision ennemi/joueur (→ gameLost).
 */
void updateEnnemis(void);

/* ======================================================================== */
/*                             GOMME (PAC-GOMME)                           */
/* ======================================================================== */

/**
 * @brief Fait apparaitre une nouvelle pac-gomme a une position aleatoire.
 *
 * Efface l'ancienne gomme a l'ecran, tire de nouvelles coordonnees
 * aleatoires dans la zone de jeu, puis dessine la nouvelle gomme.
 * Appelee quand la gomme courante est collectee (par le joueur ou un ennemi).
 */
void spawnGum(void);

/* ======================================================================== */
/*                           CALCUL DE DIRECTION                           */
/* ======================================================================== */

/**
 * @brief Calcule le vecteur de deplacement unitaire de @p begin vers @p end.
 *
 * Retourne un vecteur avec une seule composante non nulle (+1 ou -1)
 * sur l'axe presentant la plus grande distance absolue.
 * Utilise par les ennemis pour se diriger vers la pac-gomme.
 *
 * @note  Passage par valeur intentionnel (structures de 8 octets chacune).
 *        Le pointeur retourne pointe sur une variable statique interne :
 *        utiliser le resultat avant le prochain appel.
 *
 * @param begin  Position de depart (ennemi).
 * @param end    Position cible (pac-gomme).
 * @return       Pointeur sur le vecteur direction {-1,0,+1} x {-1,0,+1}.
 */
Position *trackGum(Position begin, Position end);

#endif /* __GAME_H */
