/**
  ******************************************************************************************************************
  * @file    ImagesBMP.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Declaration des sprites bitmap pour l'affichage sur ecran LCD.
  *
  * Format des tableaux : unsigned short (RGB565, 16 bits/pixel).
  * Taille = HEIGHT x WIDTH elements de 2 octets = HEIGHT x WIDTH x 2 octets.
  *******************************************************************************************************************/

#ifndef __IMAGES_BMP_H
#define __IMAGES_BMP_H

#include "GLCD_Config.h"

/* ======================================================================== */
/*                          SPRITE VAISSEAU (legacy)                       */
/* ======================================================================== */

#define HEIGHT_SPACE_SHIP  30
#define WIDTH_SPACE_SHIP   32
extern const unsigned short bmpSpaceShip[HEIGHT_SPACE_SHIP * WIDTH_SPACE_SHIP * 2];

/* ======================================================================== */
/*                        SPRITES PAC-MAN (JOUEUR)                        */
/* ======================================================================== */

#define HEIGHT_PACMAN  8   /**< Hauteur du sprite joueur (pixels). */
#define WIDTH_PACMAN   8   /**< Largeur  du sprite joueur (pixels). */

extern const unsigned short bmpPacManClose    [HEIGHT_PACMAN * WIDTH_PACMAN];
extern const unsigned short bmpPacManOpenRight[HEIGHT_PACMAN * WIDTH_PACMAN];
extern const unsigned short bmpPacManOpenLeft [HEIGHT_PACMAN * WIDTH_PACMAN];
extern const unsigned short bmpPacManOpenUp   [HEIGHT_PACMAN * WIDTH_PACMAN];
extern const unsigned short bmpPacManOpenDown [HEIGHT_PACMAN * WIDTH_PACMAN];

/* ======================================================================== */
/*                        SPRITES ENNEMIS (ROUGE)                          */
/* ======================================================================== */

#define HEIGHT_ENNEMI  8   /**< Hauteur du sprite ennemi (pixels). */
#define WIDTH_ENNEMI   8   /**< Largeur  du sprite ennemi (pixels). */

extern const unsigned short bmpEnnemiClose    [HEIGHT_ENNEMI * WIDTH_ENNEMI];
extern const unsigned short bmpEnnemiOpenRight[HEIGHT_ENNEMI * WIDTH_ENNEMI];
extern const unsigned short bmpEnnemiOpenLeft [HEIGHT_ENNEMI * WIDTH_ENNEMI];
extern const unsigned short bmpEnnemiOpenUp   [HEIGHT_ENNEMI * WIDTH_ENNEMI];
extern const unsigned short bmpEnnemiOpenDown [HEIGHT_ENNEMI * WIDTH_ENNEMI];

/* ======================================================================== */
/*                           SPRITE PAC-GOMME                              */
/* ======================================================================== */

#define HEIGHT_FOOD  6   /**< Hauteur de la pac-gomme (pixels). */
#define WIDTH_FOOD   6   /**< Largeur  de la pac-gomme (pixels). */

/** Sprite de la pac-gomme (raisin violet). Defini dans ImagesBMP.c. */
extern const unsigned short bmpFood[HEIGHT_FOOD * WIDTH_FOOD];

/* ======================================================================== */
/*                   SPRITES LEGACY (ennemis type 1/2/3/boss)              */
/* ======================================================================== */

#define HEIGHT_ENNEMI1  18
#define WIDTH_ENNEMI1   18

#define HEIGHT_ENNEMI2  18
#define WIDTH_ENNEMI2   24

#define HEIGHT_ENNEMI3  18
#define WIDTH_ENNEMI3   26

#define HEIGHT_BOSS     32
#define WIDTH_BOSS      34

extern const unsigned short bmpEnnemi1     [HEIGHT_ENNEMI1 * WIDTH_ENNEMI1 * 2];
extern const unsigned short bmpEnnemi2White[HEIGHT_ENNEMI2 * WIDTH_ENNEMI2 * 2];
extern const unsigned short bmpEnnemi2Red  [HEIGHT_ENNEMI2 * WIDTH_ENNEMI2 * 2];
extern const unsigned short bmpEnnemi3White[HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2];
extern const unsigned short bmpEnnemi3Green[HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2];
extern const unsigned short bmpEnnemi3Red  [HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2];
extern const unsigned short bmpBoss        [HEIGHT_BOSS     * WIDTH_BOSS    * 2];

/* ======================================================================== */
/*                          SPRITE MISSILE (legacy)                        */
/* ======================================================================== */

#define HEIGHT_ROCKET  10
#define WIDTH_ROCKET    2

#endif /* __IMAGES_BMP_H */
