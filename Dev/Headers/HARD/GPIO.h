/**
  ******************************************************************************************************************
  * @file    GPIO.h
  * @author  IUT Informatique La Rochelle
  * @version v1.5
  * @date    2026
  * @brief   Configuration et lecture des entrées GPIO (joystick + bouton utilisateur).
  *
  * Mapping matériel (carte MCBSTM32E) :
  *  - Joystick Haut    : GPIOG pin 15  (IDR bit 15)
  *  - Joystick Gauche  : GPIOG pin 14  (IDR bit 14)
  *  - Joystick Droite  : GPIOG pin 13  (IDR bit 13)
  *  - Joystick Bas     : GPIOD pin  3  (IDR bit  3)
  *  - Joystick Select  : GPIOG pin  7  (IDR bit  7)
  *  - Bouton USER (K1) : GPIOG pin  8  (IDR bit  8, aussi connecté à EXTI8)
  *
  * Toutes les entrées sont actives à l'état bas (APPUYE = 0).
  *******************************************************************************************************************/

#ifndef __GPIO_H
#define __GPIO_H

/**
 * @brief Configure en entrée tous les GPIO utilisés (joystick + bouton USER).
 *
 * Active les horloges GPIOD, GPIOG et AFIO. À appeler une seule fois à l'init.
 */
void initGPIO(void);

/* ---- Lecture de l'état du joystick (retourne 1 = appuyé, 0 = relâché) ---- */

/** @return 1 si le joystick est poussé vers le haut    (GPIOG pin 15), 0 sinon. */
int joystickUpPressed(void);

/** @return 1 si le joystick est poussé vers le bas     (GPIOD pin  3), 0 sinon. */
int joystickDownPressed(void);

/** @return 1 si le joystick est enfoncé (central)      (GPIOG pin  7), 0 sinon. */
int joystickSelectPressed(void);

/** @return 1 si le joystick est poussé vers la gauche  (GPIOG pin 14), 0 sinon. */
int joystickLeftPressed(void);

/** @return 1 si le joystick est poussé vers la droite  (GPIOG pin 13), 0 sinon. */
int joystickRightPressed(void);

/**
 * @brief Lit l'état du bouton utilisateur K1 (GPIOG pin 8).
 * @return 1 si le bouton est appuyé, 0 sinon.
 */
int userButtonPressed(void);

/* ---- Gestion de l'interruption EXTI8 (bouton USER → tir missile) ---- */

/**
 * @brief Active l'interruption externe EXTI8 sur front montant.
 *
 * Chaque front montant sur GPIOG pin 8 déclenchera l'ISR EXTI9_5,
 * qui appelle shoot() pour tirer un missile.
 */
void startInterruptBP_USER(void);

/**
 * @brief Désactive l'interruption externe EXTI8.
 *
 * À appeler en fin de niveau ou lors de l'affichage des menus
 * pour éviter des tirs intempestifs.
 */
void stopInterruptBP_USER(void);

#endif /* __GPIO_H */
