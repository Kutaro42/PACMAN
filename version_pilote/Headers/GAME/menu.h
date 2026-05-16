/**
  ******************************************************************************************************************
  * @file    menu.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Déclarations des fonctions d'affichage du menu et des écrans de fin.
  *
  * Gère l'écran d'accueil (logo + navigation joystick), l'écran de défaite
  * et l'écran de victoire. Tous les écrans de fin se bloquent jusqu'à
  * l'appui sur le bouton utilisateur.
  *******************************************************************************************************************/

#ifndef __MENU_H
#define __MENU_H

/**
 * @brief Options sélectionnables dans le menu principal.
 */
typedef enum {
    JOUER,   /**< Lancer une nouvelle partie. */
    QUITTER  /**< Quitter (retour à l'accueil dans la boucle principale). */
} MenuSelected;

/**
 * @brief Affiche le menu principal avec la sélection courante en surbrillance.
 * @param menuSelected  Option actuellement mise en évidence (JOUER ou QUITTER).
 */
void displayMenu(MenuSelected menuSelected);

/**
 * @brief Rafraîchit la sélection du menu d'après les entrées du joystick.
 * @return 1 si le joueur a validé "Jouer" (joystick Select + JOUER), 0 sinon.
 */
int refreshMenu(void);

/**
 * @brief Affiche l'écran de défaite avec le score final.
 *
 * Bloquant : attend l'appui sur le bouton utilisateur avant de rendre la main.
 */
void displayGameLost(void);

/**
 * @brief Affiche l'écran de victoire.
 *
 * Bloquant : attend l'appui sur le bouton utilisateur avant de rendre la main.
 */
void displayGameWon(void);

/**
 * @brief Dessine le logo "SPACE-DEFENDER" et les sprites des trois types d'ennemis.
 *
 * Utilisée comme arrière-plan commun aux écrans de menu, victoire et défaite.
 */
void displayLogo(void);

#endif /* __MENU_H */
