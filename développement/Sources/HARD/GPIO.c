/**
  ******************************************************************************************************************
  * @file    GPIO.c 
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   configuration des ports GPIO
  ******************************************************************************************************************/

#include "stm32f10x.h"
#include "interrupt.h"
#include "GPIO.h"
#include "game.h"

/**
 * @brief Configure en entree tous les GPIO du joystick et du bouton USER.
 *
 * Chaque broche est mise en mode entree flottante (CNF=01, MODE=00 => 0x4).
 * Les horloges GPIOD (APB2 bit 5), GPIOG (bit 8) et AFIO (bits 0 et 1)
 * sont activees via RCC_APB2ENR.
 */
void initGPIO(void)
{
    unsigned long temp; /* Variable temporaire pour modifier les bits CRx sans toucher les autres */

    /* Joystick Gauche  : GPIOG pin 14  (IDR bit 14, CRH bits 27:24) */
    temp = GPIOG->CRH & ~0x0F000000;
    GPIOG->CRH = temp | 0x04000000;

    /* Joystick Droite  : GPIOG pin 13  (IDR bit 13, CRH bits 23:20) */
    temp = GPIOG->CRH & ~0x00F00000;
    GPIOG->CRH = temp | 0x00400000;

    /* Joystick Haut    : GPIOG pin 15  (IDR bit 15, CRH bits 31:28) */
    temp = GPIOG->CRH & ~0xF0000000;
    GPIOG->CRH = temp | 0x40000000;

    /* Joystick Bas     : GPIOD pin  3  (IDR bit  3, CRL bits 15:12) */
    temp = GPIOD->CRL & ~0x0000F000;
    GPIOD->CRL = temp | 0x00004000;

    /* Bouton USER (K1) : GPIOG pin  8  (IDR bit  8, CRH bits  3:0) */
    temp = GPIOG->CRH & 0xFFFFFFF0;
    GPIOG->CRH = temp | 0x00000004;

    /* Joystick Select  : GPIOG pin  7  (IDR bit  7, CRL bits 31:28) */
    temp = GPIOG->CRL & ~0xF0000000;
    GPIOG->CRL = temp | 0x40000000;

    /* --- Activation des horloges peripheriques --- */
    RCC->APB2ENR |= (1 << 5);  /* IOPDEN : horloge GPIOD                           */
    RCC->APB2ENR |= (1 << 8);  /* IOPGEN : horloge GPIOG                           */
    RCC->APB2ENR |= (1 << 0);  /* AFIOEN : horloge AFIO (requis avant config EXTI) */
    RCC->APB2ENR |= (1 << 1);  /* AFIOEN : remapping et lignes EXTI                */
}


/* ---- Lecture d'etat (actif bas : retourne 1 si appuye, 0 sinon) ---- */

/** @brief Joystick haut  (GPIOG IDR bit 15). */
int joystickUpPressed(void)
{
    return (GPIOG->IDR & 0x8000) == APPUYE;
}

/** @brief Joystick bas   (GPIOD IDR bit  3). */
int joystickDownPressed(void)
{
    return (GPIOD->IDR & 0x0008) == APPUYE;
}

/** @brief Joystick central / select (GPIOG IDR bit 7). */
int joystickSelectPressed(void)
{
    return (GPIOG->IDR & 0x0080) == APPUYE;
}

/** @brief Joystick gauche (GPIOG IDR bit 14). */
int joystickLeftPressed(void)
{
    return (GPIOG->IDR & 0x4000) == APPUYE;
}

/** @brief Joystick droite (GPIOG IDR bit 13). */
int joystickRightPressed(void)
{
    return (GPIOG->IDR & 0x2000) == APPUYE;
}

/** @brief Bouton utilisateur K1 (GPIOG IDR bit 8). */
int userButtonPressed(void)
{
    return (GPIOG->IDR & 0x0100) == APPUYE;
}

/**
 * @brief Active l'interruption externe EXTI8 sur front montant.
 *
 * Sequence de configuration :
 *  1. NVIC ISER0 bit 23 : active l'IRQ EXTI9_5 dans le NVIC.
 *  2. AFIO_EXTICR3 bits [3:0] = 0x6 : selectionne GPIOG comme source EXTI8.
 *  3. EXTI_IMR  bit 8 : demasque la ligne EXTI8.
 *  4. EXTI_RTSR bit 8 : declenchement sur front montant.
 */
void startInterruptBP_USER(void)
{
    unsigned long temp;

    SETENA0 |= (1 << 23);           /* Active l'IRQ EXTI9_5 dans le NVIC           */
    temp = AFIO_EXTICR3 & 0xFFF0;
    AFIO_EXTICR3 = temp | 0x0006;   /* EXTI8 source = GPIOG (code 0x6)             */
    EXTI->IMR  |= (1 << 8);         /* Demasque la ligne EXTI8                     */
    EXTI->RTSR |= (1 << 8);         /* Declenchement sur front montant             */
}

/**
 * @brief Desactive l'interruption externe EXTI8.
 *
 * A appeler en fin de niveau et lors de l'affichage des menus pour
 * eviter des tirs intempestifs hors du jeu.
 */
void stopInterruptBP_USER(void)
{
    EXTI->IMR &= ~(1 << 8);  /* Masque EXTI8 : plus d'interruption de tir */
}

/**
 * @brief ISR partagee pour les lignes EXTI 5 a 9.
 *
 * On identifie EXTI8 via le bit 8 du registre Pending (EXTI->PR).
 * Apres avoir appele shoot(), le drapeau est acquitte en ecrivant
 * 1 sur ce bit (ecrire 1 = remise a 0, comportement specifique STM32).
 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1 << 8))   /* Interruption provient-elle d'EXTI8 ? */
    {
        shoot();               /* Tenter de tirer un missile            */
        EXTI->PR |= (1 << 8);  /* Acquitter le drapeau                  */
    }
}
