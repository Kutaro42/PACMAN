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
 * @brief Structure décrivant un ennemi à l'écran.
 *
 * Contient toutes les informations nécessaires au déplacement, à l'affichage
 * et à la gestion des collisions pour un ennemi donné.
 */
 
 typedef struct Position Position;
struct Position {
	//code
	int x;
	int y;
};
 
typedef struct Ennemi Ennemi;
struct Ennemi {
    int  direction; /**< Sens de déplacement horizontal : +1 → droite, -1 → gauche. */
	  bool inLive;//ennemi en vie ? faux par défaut.
		Position itsPosition;
	//---------------------------------
    /*int  x;   */      /**< Position horizontale (pixels) du coin supérieur gauche.     */
    /*int  y; */         /**< Position verticale   (pixels) du coin supérieur gauche.     */
	//----------------------------------
    int  width;     /**< Largeur du sprite en pixels.                                */
    int  height;    /**< Hauteur du sprite en pixels.                                */
};

typedef struct Gum Gum;
struct Gum {

	Position itsPosition;
	bool claimed;
	int width;
	int height;
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
 * @brief Met à jour tous les ennemis vivants : déplacement, collision, score.
 *
 * Appelée par l'ISR du Timer 8 (vitesse variable selon le niveau).
 * Gère : déplacement horizontal, rebond et descente aux bords, collision
 * missile/ennemi, mise à jour du score et détection de fin de niveau.
 */
void updateEnnemis(void);

//pac gomme :

void spawnGum(){
	//code :
//	Gum itsGum;
	//itsGum.itsPosition.x=getRandom(5, 100);
	//itsGum.itsPosition.y=getRandom(5, 100);
}

/**
 * @brief Calcul les vecteurs trajectoire d'une position a à une position b
 * Appellée dans la boucle logique des ennemis
 * Gère : direction de déplacement des ennemis pour traquer la pac Gomme 
 * et mettre en difficulté le joueur.
 * "Ici on utilise des pointeurs pour économiser quelques ressources (passage par réference moins coûteux que passage par copie)
*/
Position * trackGum(Position begin, Position end){
	//code :
	//on initialise les variables d'usage :
	//int positionalVector[2] = {0,0};
	Position * positionalVector;
	//Pos_X
	
	//dans un premier temps, on calcule le vecteur directeur d'origine (x;y) et d'extremitée (Gum.x;Gum.y) :
	positionalVector->x=end.x - begin.x;//on calcule pour la coordonée x
	positionalVector->y=end.y - begin.y;//on calcule pour la coordonée y
	
	//Puis on retourne les valeur sous forme de vectorPosition :
	return positionalVector;
	
}

#endif /* __GAME_H */

