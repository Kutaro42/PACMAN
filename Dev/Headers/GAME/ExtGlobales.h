/**
  ******************************************************************************************************************
  * @file    ExtGlobales.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Déclarations externes des variables globales partagées entre modules.
  *
  * Ce fichier doit être inclus par tout module accédant aux variables d'état
  * global du jeu. Les définitions se trouvent dans Sources/main.c.
  *******************************************************************************************************************/

#ifndef __EXT_GLOBALES_H
#define __EXT_GLOBALES_H

#include "Board_GLCD.h"

/* ---- État de la partie ---- */
/* volatile obligatoire : ces variables sont ecrites depuis les ISR (TIM8)       */
/* et lues en boucle dans main() — sans volatile le compilateur peut cacher     */
/* leur valeur en registre et ne jamais voir les mises a jour des ISR.          */
extern volatile int gameLost;     /**< 1 si le joueur a perdu (ennemi trop bas ou contact). */
extern volatile int gameWon;      /**< 1 si tous les niveaux ont ete completes.             */
extern volatile int levelPassed;  /**< 1 lorsque tous les ennemis du niveau sont elimines.  */

/* ---- Progression ---- */
extern int          level;        /**< Niveau actuel (1-6 : ennemis normaux ; 7-8 : Boss).  */
extern volatile int score;        /**< Score courant (mis a jour depuis les ISR).           */

/* ---- Polices d'affichage (définies dans le pilote GLCD) ---- */
extern GLCD_FONT GLCD_Font_6x8;    /**< Police 6×8 pixels – petits textes (niveau, HUD). */
extern GLCD_FONT GLCD_Font_16x24;  /**< Police 16×24 pixels – titres et score.           */

#endif /* __EXT_GLOBALES_H */
