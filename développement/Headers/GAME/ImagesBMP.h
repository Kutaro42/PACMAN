/**
  ******************************************************************************************************************
  * @file    ImagesBMP.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Déclaration des sprites bitmap pour l'affichage sur écran LCD.
  *
  * Format des tableaux : unsigned short (RGB565, 16 bits/pixel).
  * La taille est HEIGHT × WIDTH × 2 — le facteur ×2 est imposé par le pilote
  * GLCD_DrawBitmap de la MCBSTM32E qui traite les pixels par paires.
  *
  * Certains ennemis disposent de plusieurs variantes colorées reflétant
  * le nombre de vies restantes :
  *  - Ennemi 2 : blanc (2 vies) → rouge (1 vie)
  *  - Ennemi 3 : blanc (3 vies) → vert (2 vies) → rouge (1 vie)
  *******************************************************************************************************************/

#ifndef __IMAGES_BMP_H
#define __IMAGES_BMP_H

#include "GLCD_Config.h"

/* ---- Vaisseau du joueur ---- */
#define HEIGHT_SPACE_SHIP  30   /**< Hauteur du sprite vaisseau (pixels). */
#define WIDTH_SPACE_SHIP   32   /**< Largeur du sprite vaisseau (pixels). */
extern const unsigned short bmpSpaceShip[HEIGHT_SPACE_SHIP * WIDTH_SPACE_SHIP * 2];

/* ---- Dimensions des ennemis ---- */
#define HEIGHT_ENNEMI1  18   /**< Hauteur sprite Ennemi type 1 (pixels). */
#define WIDTH_ENNEMI1   18   /**< Largeur sprite Ennemi type 1 (pixels). */

#define HEIGHT_ENNEMI2  18   /**< Hauteur sprite Ennemi type 2 (pixels). */
#define WIDTH_ENNEMI2   24   /**< Largeur sprite Ennemi type 2 (pixels). */

#define HEIGHT_ENNEMI3  18   /**< Hauteur sprite Ennemi type 3 (pixels). */
#define WIDTH_ENNEMI3   26   /**< Largeur sprite Ennemi type 3 (pixels). */

#define HEIGHT_BOSS     32   /**< Hauteur sprite Boss (pixels). */
#define WIDTH_BOSS      34   /**< Largeur sprite Boss (pixels). */

/* Ennemi 1 – une seule variante (blanc, 1 vie) */
extern const unsigned short bmpEnnemi1     [HEIGHT_ENNEMI1 * WIDTH_ENNEMI1 * 2];

/* Ennemi 2 – deux variantes de couleur selon les vies restantes */
extern const unsigned short bmpEnnemi2White[HEIGHT_ENNEMI2 * WIDTH_ENNEMI2 * 2]; /**< 2 vies. */
extern const unsigned short bmpEnnemi2Red  [HEIGHT_ENNEMI2 * WIDTH_ENNEMI2 * 2]; /**< 1 vie.  */

/* Ennemi 3 – trois variantes de couleur selon les vies restantes */
extern const unsigned short bmpEnnemi3White[HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2]; /**< 3 vies. */
extern const unsigned short bmpEnnemi3Green[HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2]; /**< 2 vies. */
extern const unsigned short bmpEnnemi3Red  [HEIGHT_ENNEMI3 * WIDTH_ENNEMI3 * 2]; /**< 1 vie.  */

/* Boss */
extern const unsigned short bmpBoss        [HEIGHT_BOSS    * WIDTH_BOSS    * 2];

/* ---- Missile du joueur ---- */
#define HEIGHT_ROCKET  10   /**< Hauteur du missile (pixels). */
#define WIDTH_ROCKET    2   /**< Largeur du missile (pixels). */

#endif /* __IMAGES_BMP_H */
