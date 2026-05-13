/**
  ******************************************************************************************************************
  * @file    TIMERS.c 
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   configuration des timers
  ******************************************************************************************************************/

#include "stm32f10x.h"
#include "interrupt.h"
#include "TIMERS.h"
#include "game.h"

/**
 * @brief Initialise TIM1 et TIM8.
 *
 * Appelle les fonctions de configuration individuelles dans l'ordre.
 * Les timers NE sont PAS demarres ici ; utiliser startTimerX() pour cela.
 */
void initTIMERS(void)
{
    configTimer1();
    configTimer8();
}

/* ========================================================================
 * Timer 1 : deplacement de Pac Man
 * Horloge APB2 = 72 MHz  =>  T = (PSC+1)*(ARR+1)/72e6 = 4*64801/72e6 ~= 3.6 ms
 * ======================================================================== */

/**
 * @brief Configure TIM1 : PSC=3, ARR=64800, active l'IRQ Update.
 *
 * L'IRQ NVIC correspondante est la numero 25 (TIM1_UP_TIM10).
 */
void configTimer1(void)
{
    RCC->APB2ENR |= (1 << 11);  /* TIM1EN : activation de l'horloge TIM1    */
    TIM1->PSC  = 3;             /* Prescaler : divise par 4                  */
    TIM1->ARR  = 64800;         /* Periode   : 64801 cycles * 4 / 72 MHz ~= 3.6 ms */
    TIM1->DIER |= UIE;          /* Autorisation de l'interruption Update     */
    stopTimer1();               /* S'assurer que le timer est arrete a l'init */
    SETENA0 |= (1 << 25);       /* Active l'IRQ TIM1_UP dans le NVIC         */
}

/** @brief Demarre TIM1 (CEN=1). */
void startTimer1(void)
{
    TIM1->CR1 |= CEN;
}

/** @brief Arrete TIM1 (CEN=0). */
void stopTimer1(void)
{
    TIM1->CR1 &= ~CEN;
}

/**
 * @brief ISR TIM1 Update : met a jour la position du vaisseau.
 *
 * Appelee toutes les ~3.6 ms. On teste UIF pour ignorer les
 * interruptions parasites et on le remet a 0 apres traitement.
 */
void TIM1_UP_TIM10_IRQHandler(void)
{
    if (TIM1->SR & UIF)
    {
        updateSpaceShip();
        TIM1->SR &= ~UIF;   /* Acquitter le drapeau Update */
    }
}

/* ========================================================================
 * Timer 8 : deplacement des ennemis (vitesse variable)
 * Horloge APB2 = 72 MHz, ARR = 36000
 * Periode = (PSC+1) * 36001 / 72e6
 *   NORMAL     : PSC=31 => ~16 ms
 *   SPEED      : PSC=25 => ~13 ms
 *   HARDCORE   : PSC=19 => ~10 ms
 *   IMPOSSIBLE : PSC=13 => ~7 ms
 * ======================================================================== */

/**
 * @brief Configure TIM8 : ARR=36000, vitesse initiale NORMAL, active l'IRQ.
 *
 * L'IRQ NVIC est le bit 12 de ISER1 (TIM8_UP = IRQ numero 44).
 */
void configTimer8(void)
{
    RCC->APB2ENR |= (1 << 13);  /* TIM8EN : activation de l'horloge TIM8   */
    setSpeedTimer8(NORMAL);     /* Vitesse initiale par defaut               */
    TIM8->ARR   = 36000;        /* Periode de base (modifiee via PSC)        */
    TIM8->DIER |= UIE;          /* Autorisation de l'interruption Update     */
    stopTimer8();
    SETENA1 |= (1 << 12);       /* Active l'IRQ TIM8_UP dans le NVIC         */
}

/**
 * @brief Regle le prescaler de TIM8 pour ajuster la vitesse des ennemis.
 * @param speed  Niveau de vitesse : NORMAL, SPEED, HARDCORE ou IMPOSSIBLE.
 */
void setSpeedTimer8(Speed speed)
{
    if      (speed == NORMAL)     TIM8->PSC = 31;  /* ~16 ms */
    else if (speed == SPEED)      TIM8->PSC = 25;  /* ~13 ms */
    else if (speed == HARDCORE)   TIM8->PSC = 19;  /* ~10 ms */
}

/** @brief Demarre TIM8 (CEN=1). */
void startTimer8(void)
{
    TIM8->CR1 |= CEN;
}

/** @brief Arrete TIM8 (CEN=0). */
void stopTimer8(void)
{
    TIM8->CR1 &= ~CEN;
}

/**
 * @brief ISR TIM8 Update : met a jour la position de tous les ennemis.
 *
 * Appelee a la cadence du Timer 8 (de ~7 a ~16 ms selon le niveau).
 * On acquitte UIF apres traitement.
 */
void TIM8_UP_TIM13_IRQHandler(void)
{
    if (TIM8->SR & UIF)
    {
        updateEnnemis();
        TIM8->SR &= ~UIF;   /* Acquitter le drapeau Update */
    }
}

/* ========================================================================
 * Timer 3 : compteur libre pour la graine aleatoire
 * PSC=0, ARR=0xFFFF => compte de 0 a 65535 en continu
 * ======================================================================== */

/**
 * @brief Demarre TIM3 en mode compteur libre (pas d'IRQ).
 *
 * Sa valeur (TIM3->CNT) evalue au moment du lancement de la partie
 * sert de graine a srand() dans random.c.
 */
void initTimer3(void)
{
    RCC->APB1ENR |= (1 << 1);  /* TIM3EN : activation de l'horloge TIM3 */
    TIM3->PSC = 0;              /* Pas de division : 72 MHz               */
    TIM3->ARR = 0xFFFF;         /* Modulo 65536                           */
    TIM3->CR1 |= CEN;           /* Demarrer le compteur                   */
}

/**
 * @brief Retourne la valeur courante du compteur TIM3.
 * @return Entier 16 bits entre 0 et 65535.
 */
int getTimer3Value(void)
{
    return TIM3->CNT;
}


