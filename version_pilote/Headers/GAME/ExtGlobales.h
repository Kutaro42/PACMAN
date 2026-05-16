/**
  ******************************************************************************************************************
  * @file    ExtGlobales.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Declarations externes des variables globales partagees entre modules.
  *
  * Ce fichier doit etre inclus par tout module accedant aux variables d'etat
  * global du jeu. Les DEFINITIONS se trouvent dans Sources/main.c.
  *******************************************************************************************************************/

#ifndef __EXT_GLOBALES_H
#define __EXT_GLOBALES_H

#include "Board_GLCD.h"

/* ======================================================================== */
/*                          ETAT DE LA PARTIE                              */
/* ======================================================================== */

/*
 * volatile obligatoire : ces variables sont ECRITES depuis les ISR (TIM1,
 * TIM8) et LUES en boucle dans main(). Sans volatile, le compilateur
 * peut cacher leur valeur en registre et ne jamais voir les mises a jour
 * effectuees par les ISR.
 */

/** 1 si le joueur a perdu (contact avec un ennemi, ou enemiScore >= 3). */
extern volatile int gameLost;

/** 1 si le joueur a gagne (score >= 3). */
extern volatile int gameWon;

/** Score courant du joueur (gommes collectees, mis a jour depuis les ISR). */
extern volatile int score;

/**
 * Score courant des ennemis (gommes collectees par les ennemis,
 * mis a jour depuis les ISR). Quand enemiScore atteint 3, gameLost est leve.
 */
extern volatile int enemiScore;

/* ======================================================================== */
/*                        POLICES D'AFFICHAGE                              */
/* ======================================================================== */

/** Police 6x8 pixels – petits textes (HUD, scores). */
extern GLCD_FONT GLCD_Font_6x8;

/** Police 16x24 pixels – titres et grands affichages. */
extern GLCD_FONT GLCD_Font_16x24;

#endif /* __EXT_GLOBALES_H */
