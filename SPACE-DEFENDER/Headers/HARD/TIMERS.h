/**
  ******************************************************************************************************************
  * @file    TIMERS.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   configuration des timers
  ******************************************************************************************************************/
	
#ifndef __TIMERS_H
#define __TIMERS_H

/**
 * @brief Initialise TIM1, TIM2 et TIM8 (configuration seule, sans les démarrer).
 *
 * A appeler une seule fois lors de l'initialisation du système.
 */
void initTIMERS(void);

/* ---- Timer 1 : déplacement du vaisseau ---- */
/* PSC=3, ARR=64800  =>  T ~= 3.6 ms  @  72 MHz (APB2) */
void configTimer1(void);              /**< Configure TIM1.                             */
void startTimer1(void);               /**< Active TIM1 (démarre la mise a jour vaisseau). */
void stopTimer1(void);                /**< Arrête TIM1.                                */
void TIM1_UP_TIM10_IRQHandler(void);  /**< ISR TIM1 : appelle updateSpaceShip().       */

/* ---- Timer 2 : déplacement du missile ---- */
/* PSC=3, ARR=32400  =>  T ~= 1.8 ms  @  36 MHz (APB1) */
void configTimer2(void);              /**< Configure TIM2.                             */
void startTimer2(void);               /**< Active TIM2 (démarre le suivi du missile). */
void stopTimer2(void);                /**< Arrête TIM2.                                */
void TIM2_IRQHandler(void);           /**< ISR TIM2 : appelle updateRocket().          */

/* ---- Timer 8 : déplacement des ennemis (vitesse variable selon le niveau) ---- */

/**
 * @brief Niveaux de vitesse pour les ennemis.
 *
 * Valeur      | PSC | Periode (ARR=36000, @72 MHz APB2)
 * ------------|-----|-----------------------------------
 * NORMAL      |  31 | ~= 16 ms
 * SPEED       |  25 | ~= 13 ms
 * HARDCORE    |  19 | ~= 10 ms
 * IMPOSSIBLE  |  13 | ~=  7 ms
 */
typedef enum { NORMAL, SPEED, HARDCORE, IMPOSSIBLE } Speed;

void configTimer8(void);              /**< Configure TIM8 et active son IRQ NVIC.      */
void setSpeedTimer8(Speed speed);     /**< Regle le prescaler de TIM8.                 */
void startTimer8(void);               /**< Active TIM8 (démarre les ennemis).          */
void stopTimer8(void);                /**< Arrête TIM8.                                */
void TIM8_UP_TIM13_IRQHandler(void);  /**< ISR TIM8 : appelle updateEnnemis().         */

/* ---- Timer 3 : source d'entropie pour l'aleatoire ---- */
/* PSC=0, ARR=0xFFFF, CEN=1 => compteur libre 0..65535 */
void initTimer3(void);    /**< Demarre TIM3 en comptage libre (pas d'IRQ).              */
int  getTimer3Value(void);/**< Retourne la valeur courante de TIM3->CNT.                */

#endif /* __TIMERS_H */
