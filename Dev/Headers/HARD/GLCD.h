/**
  ******************************************************************************************************************
  * @file    GLCD.h 
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   Gestion de l'�cran LCD
  ******************************************************************************************************************/
	
#ifndef __GLCD_H
#define __GLCD_H

#include "stm32f10x.h"

#define BACK_COLOR  GLCD_COLOR_BLACK  /**< Couleur de fond par défaut (noir).  */
#define TEXT_COLOR  GLCD_COLOR_WHITE  /**< Couleur de texte par défaut (blanc). */

/**
 * @brief Initialise le contrôleur GLCD et efface l'écran.
 *
 * A appeler une seule fois lors de l'initialisation du système.
 */
void initGLCD(void);

/**
 * @brief Remplit tout l'écran avec BACK_COLOR (noir).
 */
void clearScreenGLCD(void);

/**
 * @brief Efface une zone rectangulaire en la remplissant de noir.
 *
 * Utilisée pour supprimer les traces laissées par les sprites lors
 * de leur déplacement pixel par pixel.
 *
 * @param x       Colonne du coin supérieur gauche (pixels).
 * @param y       Ligne   du coin supérieur gauche (pixels).
 * @param width   Largeur de la zone a effacer (pixels).
 * @param height  Hauteur de la zone a effacer (pixels).
 */
void clearZone(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

#endif /* __GLCD_H */
