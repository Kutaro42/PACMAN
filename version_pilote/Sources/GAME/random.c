/**
  ******************************************************************************************************************
  * @file    random.c
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   Gestion de l'al�atoire
  ******************************************************************************************************************/
	
#include "random.h"
#include "TIMERS.h"
#include <stdlib.h>   /* srand(), rand() */

/**
 * @brief Initialise le Timer 3 comme source d'entropie pour l'aleatoire.
 *
 * A appeler une seule fois lors de l'initialisation du systeme.
 * Le Timer 3 tourne ensuite librement jusqu'au debut de la premiere partie.
 */
void initRandom(void)
{
    initTimer3();
}

/**
 * @brief Initialise la graine du PRNG avec la valeur courante du Timer 3.
 *
 * La valeur de TIM3->CNT depend du temps ecoule depuis le boot (navigation
 * dans le menu), ce qui garantit des sequences aleatoires differentes
 * d'une partie a l'autre sans necessiter de generateur materiel dedie.
 * A appeler au debut de chaque nouvelle partie (juste avant setupLevel).
 */
void setRandom(void)
{
    srand(getTimer3Value());
}

/**
 * @brief Retourne un entier pseudo-aleatoire dans [min, max[.
 *
 * Utilise rand() (seede via setRandom()) et calcule le reste modulo
 * l'intervalle demande. Pour de petits intervalles (< RAND_MAX), la
 * distribution est quasi-uniforme.
 *
 * @param min  Borne inferieure (incluse).
 * @param max  Borne superieure (exclue).
 * @return     Valeur telle que min <= resultat < max.
 */
int getRandom(int min, int max)
{
    return rand() % (max - min) + min;
}

