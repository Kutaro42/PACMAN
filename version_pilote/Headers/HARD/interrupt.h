/**
  ******************************************************************************************************************
  * @file    interrupt.h 
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @modification	
  * @brief   définition des registres utilisés pour la configuration des interruptions 
  *
  ******************************************************************************************************************/

#ifndef __INTERRUPT_H
#define __INTERRUPT_H

/* ---- NVIC : registres d'activation des IRQ ---- */
/**
 * NVIC_ISER0 (0xE000E100) : active les interruptions 0 a 31.
 * Ecrire 1 sur un bit active l'IRQ correspondante ; ecrire 0 est sans effet.
 */
#define SETENA0   (*(volatile unsigned long *)0xE000E100)

/**
 * NVIC_ISER1 (0xE000E104) : active les interruptions 32 a 63.
 */
#define SETENA1   (*(volatile unsigned long *)0xE000E104)

/* ---- AFIO : selection de la source GPIO pour les lignes EXTI ---- */
/**
 * AFIO_EXTICR3 (0x40010010) : configure les lignes EXTI 8 a 11.
 * Bits [3:0] = EXTI8 => 0110 (0x6) selectionne GPIOG comme source.
 */
#define AFIO_EXTICR3  (*(volatile unsigned long *)0x40010010)

/* ---- Etats des entrees GPIO (logique active bas) ---- */
/** Valeur lue sur un GPIO quand le bouton/joystick est enfonce (niveau bas). */
#define APPUYE  0

/* ---- Masques de bits communs aux registres de controle des timers ---- */
#define UIE  (1 << 0)  /**< TIMx_DIER bit 0 : Update Interrupt Enable. */
#define UIF  (1 << 0)  /**< TIMx_SR   bit 0 : Update Interrupt Flag.   */
#define CEN  (1 << 0)  /**< TIMx_CR1  bit 0 : Counter Enable.          */

#endif /* __INTERRUPT_H */
