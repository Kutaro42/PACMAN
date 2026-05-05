/**
  ******************************************************************************************************************
  * @file    GLCD.c
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   Gestion de l'�cran LCD
  ******************************************************************************************************************/
	
#include "stm32f10x.h"
#include "Board_GLCD.h"
#include "GLCD_Config.h"
#include "GLCD.h"

/**
 * @brief Initialise le controleur GLCD et efface l'ecran.
 *
 * A appeler une seule fois lors de l'initialisation du systeme.
 */
void initGLCD(void)
{
    GLCD_Initialize();   /* Initialisation materielle du controleur LCD */
    clearScreenGLCD();
}

/**
 * @brief Remplit tout l'ecran avec la couleur de fond (noir).
 *
 * A utiliser entre deux scenes pour repartir d'un ecran vide.
 */
void clearScreenGLCD(void)
{
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_ClearScreen();
}

/**
 * @brief Efface une zone rectangulaire en la remplissant de noir.
 *
 * Technique de suppression des traces lors du deplacement pixel a pixel
 * des sprites : seul le pixel qui "sort" du sprite est efface, evitant
 * de redessiner toute la scene a chaque frame.
 *
 * @param x       Colonne du coin superieur gauche de la zone.
 * @param y       Ligne   du coin superieur gauche de la zone.
 * @param width   Largeur de la zone a effacer (pixels).
 * @param height  Hauteur de la zone a effacer (pixels).
 */
void clearZone(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
    /* progress=1024 (100%) => rectangle plein noir = effacement */
    GLCD_DrawBargraph(x, y, width, height, 1024);
}
