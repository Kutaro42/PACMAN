/**
  ******************************************************************************************************************
  * @file    random.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Interface de génération de nombres pseudo-aléatoires.
  *
  * La graine est fournie par le Timer 3 (compteur libre), dont la valeur
  * au moment du lancement d'une partie dépend du temps passé dans le menu
  * — garantissant ainsi des parties différentes.
  *******************************************************************************************************************/

#ifndef __RANDOM_H
#define __RANDOM_H

/**
 * @brief Initialise le Timer 3 comme source d'entropie.
 *
 * À appeler une seule fois lors de l'initialisation du système.
 */
void initRandom(void);

/**
 * @brief Initialise la graine du PRNG avec la valeur courante du Timer 3.
 *
 * À appeler au début de chaque nouvelle partie pour garantir des
 * positions aléatoires différentes d'une partie à l'autre.
 */
void setRandom(void);

/**
 * @brief Retourne un entier pseudo-aléatoire dans l'intervalle [min, max[.
 * @param min  Borne inférieure (incluse).
 * @param max  Borne supérieure (exclue).
 * @return     Valeur telle que min <= résultat < max.
 */
int getRandom(int min, int max);

#endif /* __RANDOM_H */

