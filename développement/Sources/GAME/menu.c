/**
  ******************************************************************************************************************
  * @file    menu.c
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   Gestion du menu
  ******************************************************************************************************************/

#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "GPIO.h"
#include "GLCD.h"

#include "menu.h"
#include "ImagesBMP.h"
#include "ExtGlobales.h"

#include <stdio.h>  /* sprintf() */

/**
 * @brief Affiche l'ecran du menu principal avec l'option selectionnee en rouge.
 *
 * @param menuSelected  Item actuellement en surbrillance (JOUER ou QUITTER).
 */
void displayMenu(MenuSelected menuSelected)
{
    clearScreenGLCD();
    displayLogo();

    if (menuSelected == JOUER)
    {
        GLCD_SetForegroundColor(GLCD_COLOR_RED);
        GLCD_DrawString(100, 100, "Jouer");
        GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
        GLCD_DrawString(100, 140, "Quitter");
    }
    else if (menuSelected == QUITTER)
    {
        GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
        GLCD_DrawString(100, 100, "Jouer");
        GLCD_SetForegroundColor(GLCD_COLOR_RED);
        GLCD_DrawString(100, 140, "Quitter");
    }
}

/**
 * @brief Gere la navigation dans le menu et retourne 1 si le joueur a choisi de jouer.
 *
 * A appeler periodiquement depuis la boucle principale.
 * - Joystick haut   : remonte la selection sur JOUER.
 * - Joystick bas    : descend la selection sur QUITTER.
 * - Joystick select : confirme JOUER (retourne 1) ou QUITTER (retourne 0).
 *
 * @return 1 si l'utilisateur a selectionne JOUER et confirme, 0 sinon.
 */
int refreshMenu(void)
{
    static MenuSelected menuSelected = JOUER;

    if (joystickUpPressed() && menuSelected == QUITTER)
    {
        menuSelected = JOUER;
        displayMenu(menuSelected);
    }
    if (joystickDownPressed() && menuSelected == JOUER)
    {
        menuSelected = QUITTER;
        displayMenu(menuSelected);
    }

    return joystickSelectPressed() && menuSelected == JOUER;
}
	
/**
 * @brief Affiche l'ecran de fin de partie (defaite) avec le score final.
 *
 * Efface l'ecran, affiche le logo, le texte "Perdu !!!" en rouge,
 * le score du joueur, puis attend une pression sur le bouton USER pour continuer.
 */
void displayGameLost(void)
{
    char chaine[20];
    clearScreenGLCD();
    displayLogo();

    GLCD_SetForegroundColor(GLCD_COLOR_RED);
    GLCD_DrawString(100, 100, "Perdu !!!");
    GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
    sprintf(chaine, "Votre score : %d", score);
    GLCD_DrawString(20, 140, chaine);

    while (!userButtonPressed()); /* Attente confirmation joueur */
}

/**
 * @brief Affiche l'ecran de victoire.
 *
 * Affiche le logo et le texte "GAGNE !!!" en rouge,
 * puis attend une pression sur le bouton USER pour continuer.
 */
void displayGameWon(void)
{
    displayLogo();

    GLCD_SetForegroundColor(GLCD_COLOR_RED);
    GLCD_DrawString(100, 100, "GAGNE !!!");

    while (!userButtonPressed()); /* Attente confirmation joueur */
}

/**
 * @brief Dessine le logo "SPACE-DEFENDER" et les trois sprites d'ennemis en en-tete.
 *
 * Utilise la police 16x24, fond noir, texte blanc.
 * Affiche ensuite un ennemi de chaque type pour illustrer la grille de jeu.
 */
void displayLogo(void)
{
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_SetForegroundColor(GLCD_COLOR_WHITE);
    GLCD_SetFont(&GLCD_Font_16x24);
    GLCD_DrawString(3, 0, "   SPACE-DEFENDER");

    GLCD_DrawBitmap(100, 40, WIDTH_ENNEMI1,  HEIGHT_ENNEMI1,  (unsigned char *)bmpEnnemi1);
    GLCD_DrawBitmap(145, 40, WIDTH_ENNEMI2,  HEIGHT_ENNEMI2,  (unsigned char *)bmpEnnemi2Red);
    GLCD_DrawBitmap(190, 40, WIDTH_ENNEMI3,  HEIGHT_ENNEMI3,  (unsigned char *)bmpEnnemi3Green);
}

