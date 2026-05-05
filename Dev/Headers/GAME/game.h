/**
  ******************************************************************************************************************
  * @file    game.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Déclarations des types, structures et fonctions de gestion du jeu.
  *
  * Ce module gère toute la logique de jeu : initialisation des niveaux,
  * déplacement du vaisseau et des ennemis, gestion du missile et détection
  * des collisions. Les timers matériels (TIM1, TIM2, TIM8) appellent les
  * fonctions de mise à jour depuis leurs ISR respectives.
  *******************************************************************************************************************/

#ifndef __GAME_H
#define __GAME_H

#include "stdbool.h"

/**
 * @brief Types d'ennemis disponibles dans le jeu.
 *
 * Chaque type possède un sprite, des dimensions et un nombre de vies distincts.
 */
typedef enum {
    ENNEMI_1,   /**< Basique  : 1 vie,  18×18 px, blanc.                         */
    ENNEMI_2,   /**< Moyen    : 2 vies, 18×24 px, blanc → rouge.                 */
    ENNEMI_3,   /**< Avancé   : 3 vies, 18×26 px, blanc → vert → rouge.          */
    BOSS        /**< Boss     : 20 vies, 32×34 px, blanc avec yeux rouges.        */
} Type;

/**
 * @brief Structure décrivant un ennemi à l'écran.
 *
 * Contient toutes les informations nécessaires au déplacement, à l'affichage
 * et à la gestion des collisions pour un ennemi donné.
 */
typedef struct Ennemi Ennemi;
struct Ennemi {
    int  direction; /**< Sens de déplacement horizontal : +1 → droite, -1 → gauche. */
    bool dead;      /**< true si l'ennemi a été éliminé (ne sera plus mis à jour).   */
    int  x;         /**< Position horizontale (pixels) du coin supérieur gauche.     */
    int  y;         /**< Position verticale   (pixels) du coin supérieur gauche.     */
    int  width;     /**< Largeur du sprite en pixels.                                */
    int  height;    /**< Hauteur du sprite en pixels.                                */
    Type type;      /**< Type de l'ennemi (détermine sprite et comportement).        */
    int  nbLives;   /**< Nombre de vies restantes avant destruction complète.        */
};

/* ---- Fonctions d'affichage HUD ---- */

/**
 * @brief Affiche le nom du niveau courant en haut à gauche de l'écran.
 *
 * Police 6×8 pixels, couleur jaune. Niveaux 1–6 : "Niveau N" ;
 * niveau 7 : "Boss" ; niveau 8 : "Boss - Le retour..".
 */
void displayLevelName(void);

/**
 * @brief Affiche le score courant au centre du bord supérieur de l'écran.
 *
 * Police 16×24 pixels, couleur bleue.
 */
void displayScore(void);

/* ---- Cycle de vie d'un niveau ---- */

/**
 * @brief Initialise les données du niveau avant son démarrage.
 *
 * Positionne aléatoirement le vaisseau, réinitialise le missile,
 * place les ennemis en formation et règle la vitesse du Timer 8.
 */
void setupLevel(void);

/**
 * @brief Lance le niveau : efface l'écran, affiche le HUD et démarre les timers.
 *
 * Active Timer 1 (vaisseau), Timer 2 (missile), Timer 8 (ennemis)
 * et l'interruption EXTI du bouton utilisateur.
 */
void startLevel(void);

/**
 * @brief Arrête tous les timers et l'interruption de tir en fin de niveau.
 */
void stopLevel(void);

/* ---- Fonctions de mise à jour (appelées depuis les ISR) ---- */

/**
 * @brief Met à jour la position du vaisseau selon l'état du joystick.
 *
 * Appelée par l'ISR du Timer 1 (~3.6 ms). Déplace le vaisseau d'un pixel
 * vers la gauche ou la droite et le redessine.
 */
void updateSpaceShip(void);

/**
 * @brief Fait monter le missile d'un pixel et efface sa trace.
 *
 * Appelée par l'ISR du Timer 2 (~1.8 ms). Désactive le missile si
 * il sort par le haut de l'écran.
 */
void updateRocket(void);

/**
 * @brief Déclenche un tir si aucun missile n'est déjà en vol.
 *
 * Appelée depuis l'ISR EXTI8 (bouton BP_USER). Positionne le missile
 * au centre du vaisseau.
 */
void shoot(void);

/**
 * @brief Met à jour tous les ennemis vivants : déplacement, collision, score.
 *
 * Appelée par l'ISR du Timer 8 (vitesse variable selon le niveau).
 * Gère : déplacement horizontal, rebond et descente aux bords, collision
 * missile/ennemi, mise à jour du score et détection de fin de niveau.
 */
void updateEnnemis(void);

#endif /* __GAME_H */
