/**
  ******************************************************************************************************************
  * @file    game.c
  * @author  Alexandre AUBIN/Leo BOUCARD (IUT Informatique La Rochelle)
  * @version v1.5
  * @date    2026
  * @brief   Logique principale du jeu PUCK-MAN.
  *
  * Deux ISR pilotent le jeu :
  *   TIM1 (~3.6 ms)  → updatePlayer()  : deplace le joueur
  *   TIM8 (~16 ms)   → updateEnnemis()    : deplace les ennemis (plus lents)
  *
  * Regles :
  *  - Le joueur collecte une pac-gomme → score++, nouvel ennemi spawn.
  *  - Un ennemi collecte la pac-gomme  → enemiScore++, les ennemis accelerent.
  *  - Joueur touche un ennemi          → gameLost = 1.
  *  - score >= 3                       → gameWon  = 1.
  *  - enemiScore >= 3                  → gameLost = 1.
  *  - Apres chaque collecte, une nouvelle gomme apparait a une position aleatoire.
  *******************************************************************************************************************/

#include "TIMERS.h"
#include "GPIO.h"
#include "GLCD.h"
#include "GLCD_Config.h"
#include "Board_GLCD.h"

#include "game.h"
#include "ImagesBMP.h"
#include "ExtGlobales.h"
#include "random.h"

#include <stdio.h>   /* sprintf() */

/* ======================================================================== */
/*                          CONSTANTES LOCALES                             */
/* ======================================================================== */

/*
 * Convention adoptee d'apres le code existant :
 *   GLCD_SIZE_Y = borne horizontale (axe x, largeur  de l'ecran)
 *   GLCD_SIZE_X = borne verticale   (axe y, hauteur  de l'ecran)
 * Sur la MCBSTM32E en mode paysage : GLCD_SIZE_Y = 320, GLCD_SIZE_X = 240.
 */
#define SCREEN_W    GLCD_SIZE_Y   /**< Largeur  ecran en pixels (axe x). */
#define SCREEN_H    GLCD_SIZE_X   /**< Hauteur  ecran en pixels (axe y). */
#define HUD_HEIGHT  10            /**< Pixels reserves pour le HUD en haut. */

/* Identifiants de direction (stockes dans Ennemi.direction et dirPacMan) */
#define DIR_RIGHT  0
#define DIR_LEFT   1
#define DIR_UP     2
#define DIR_DOWN   3

#define MAX_ENNEMIS  3   /**< Nombre maximal d'ennemis simultanement actifs. */

/* Macro abs sans dependance bibliotheque */
#define IABS(x)  ((x) >= 0 ? (x) : -(x))

/* ======================================================================== */
/*                     VARIABLES STATIQUES DU MODULE                       */
/* ======================================================================== */

static int xPacMan;     /**< Position x du joueur (coin sup. gauche du sprite). */
static int yPacMan;     /**< Position y du joueur (coin sup. gauche du sprite). */
static int dirPacMan;   /**< Derniere direction du joueur (DIR_*).              */
static int animPacMan;  /**< Frame d'animation : 0 = ferme, 1 = ouvert.        */

static Gum currentGum;  /**< Pac-gomme active (une seule a la fois).            */

static Ennemi ennemis[MAX_ENNEMIS]; /**< Tableau des ennemis actifs.            */
static int    nbEnnemis;            /**< Nombre d'ennemis actuellement actifs.  */
static int    animEnnemi;           /**< Frame d'animation commune aux ennemis. */

/* ======================================================================== */
/*                        PROTOTYPES INTERNES                              */
/* ======================================================================== */

static int  checkCollision(int x1, int y1, int w1, int h1,
                           int x2, int y2, int w2, int h2);
static void drawPacMan(void);
static void drawEnnemi(int i);
static void drawGum(void);
static void initGumPosition(void);
static void initEnnemi(int idx);

/* ======================================================================== */
/*                           AFFICHAGE HUD                                 */
/* ======================================================================== */

/**
 * @brief Affiche les deux scores dans le HUD (haut de l'ecran).
 *
 * Police 6x8 pixels, couleur jaune sur fond noir.
 *
 * @param sc    Score du joueur.
 * @param enSc  Score des ennemis.
 */
void displayScores(int sc, int enSc)
{
    char chaine[32];
    GLCD_SetBackgroundColor(GLCD_COLOR_BLACK);
    GLCD_SetForegroundColor(GLCD_COLOR_YELLOW);
    GLCD_SetFont(&GLCD_Font_6x8);
    sprintf(chaine, "Joueur:%d   Ennemi:%d", sc, enSc);
    GLCD_DrawString(0, 0, chaine);
}

/**
 * @brief Rafraichit le HUD avec les variables globales score / enemiScore.
 */
void displayEndGame(void)
{
    displayScores(score, enemiScore);
}

/* ======================================================================== */
/*                         DETECTION DE COLLISION                          */
/* ======================================================================== */

/**
 * @brief Teste le chevauchement de deux rectangles (AABB).
 *
 * @return 1 s'ils se chevauchent, 0 sinon.
 */
static int checkCollision(int x1, int y1, int w1, int h1,
                          int x2, int y2, int w2, int h2)
{
    return (x1 < x2 + w2) && (x1 + w1 > x2) &&
           (y1 < y2 + h2) && (y1 + h1 > y2);
}

/* ======================================================================== */
/*                              DESSIN                                     */
/* ======================================================================== */

/**
 * @brief Dessine le sprite du joueur a sa position courante.
 *
 * Choisit le sprite selon la direction et la frame d'animation.
 */
static void drawPacMan(void)
{
    const unsigned short *spr;

    if (animPacMan == 0)
    {
        spr = bmpPacManClose;
    }
    else
    {
        switch (dirPacMan)
        {
            case DIR_RIGHT: spr = bmpPacManOpenRight; break;
            case DIR_LEFT:  spr = bmpPacManOpenLeft;  break;
            case DIR_UP:    spr = bmpPacManOpenUp;    break;
            default:        spr = bmpPacManOpenDown;  break;
        }
    }

    GLCD_DrawBitmap(xPacMan, yPacMan,
                    WIDTH_PACMAN, HEIGHT_PACMAN, (unsigned char *)spr);
}

/**
 * @brief Dessine le sprite de l'ennemi d'indice @p i.
 *
 * Ne fait rien si l'ennemi n'est pas actif.
 *
 * @param i  Indice dans le tableau ennemis[] (0 a MAX_ENNEMIS-1).
 */
static void drawEnnemi(int i)
{
    const unsigned short *spr;

    if (!ennemis[i].inLive) return;

    if (animEnnemi == 0)
    {
        spr = bmpEnnemiClose;
    }
    else
    {
        switch (ennemis[i].direction)
        {
            case DIR_RIGHT: spr = bmpEnnemiOpenRight; break;
            case DIR_LEFT:  spr = bmpEnnemiOpenLeft;  break;
            case DIR_UP:    spr = bmpEnnemiOpenUp;    break;
            default:        spr = bmpEnnemiOpenDown;  break;
        }
    }

    GLCD_DrawBitmap(ennemis[i].itsPosition.x, ennemis[i].itsPosition.y,
                    ennemis[i].width, ennemis[i].height, (unsigned char *)spr);
}

/**
 * @brief Dessine la pac-gomme si elle n'a pas encore été collectee.
 */
static void drawGum(void)
{
    if (!currentGum.claimed)
    {
        GLCD_DrawBitmap(currentGum.itsPosition.x, currentGum.itsPosition.y,
                        WIDTH_FOOD, HEIGHT_FOOD, (unsigned char *)bmpFood);
    }
}

/* ======================================================================== */
/*                            PAC-GOMME                                    */
/* ======================================================================== */

/**
 * @brief Choisit une nouvelle position aleatoire pour la pac-gomme.
 *
 * Ne dessine pas encore la gomme (c'est drawGum qui le fait).
 */
static void initGumPosition(void)
{
    currentGum.itsPosition.x = getRandom(0,          SCREEN_W - WIDTH_FOOD);
    currentGum.itsPosition.y = getRandom(HUD_HEIGHT, SCREEN_H - HEIGHT_FOOD);
    currentGum.width         = WIDTH_FOOD;
    currentGum.height        = HEIGHT_FOOD;
    currentGum.claimed       = false;
}

/**
 * @brief Fait apparaitre une nouvelle pac-gomme (appelée en mid-game).
 *
 * Efface la gomme précédente à l'écran, tire de nouvelles coordonnées
 * aleatoires, puis dessine la nouvelle gomme. A appeler après chaque
 * collecte (joueur ou ennemi).
 */
void spawnGum(void)
{
    /* Effacer l'ancienne gomme si elle etait encore a l'ecran */
    clearZone(currentGum.itsPosition.x, currentGum.itsPosition.y,
              currentGum.width, currentGum.height);

    initGumPosition();
    drawGum();
}

/* ======================================================================== */
/*                        INITIALISATION DES ENNEMIS                      */
/* ======================================================================== */

/**
 * @brief Initialise l'ennemi d'indice @p idx a une position aleatoire.
 *
 * @param idx  Indice dans ennemis[] (0 a MAX_ENNEMIS-1).
 */
static void initEnnemi(int idx)
{
    ennemis[idx].itsPosition.x = getRandom(0,          SCREEN_W - WIDTH_ENNEMI);
    ennemis[idx].itsPosition.y = getRandom(HUD_HEIGHT, SCREEN_H - HEIGHT_ENNEMI);
    ennemis[idx].width         = WIDTH_ENNEMI;
    ennemis[idx].height        = HEIGHT_ENNEMI;
    ennemis[idx].inLive        = true;
    ennemis[idx].direction     = DIR_RIGHT;
}

/* ======================================================================== */
/*                         CYCLE DE VIE DU JEU                            */
/* ======================================================================== */

/**
 * @brief Initialise les donnees de la partie (sans aucun dessin).
 *
 * Positionne le joueur au centre, cree le premier ennemi et choisit
 * une position aleatoire pour la premiere pac-gomme.
 * L'affichage est fait dans startLevel().
 */
void setupLevel(void)
{
    /* -- Joueur au centre de l'ecran -- */
    xPacMan   = (SCREEN_W - WIDTH_PACMAN)  / 2;
    yPacMan   = (SCREEN_H - HEIGHT_PACMAN) / 2;
    dirPacMan = DIR_RIGHT;
    animPacMan = 0;
    animEnnemi = 0;

    /* -- Premier ennemi -- */
    nbEnnemis = 1;
    initEnnemi(0);

    /* -- Premiere gomme (position uniquement, pas de dessin) -- */
    initGumPosition();
}

/**
 * @brief Lance la partie : efface l'écran, dessine la scène initiale, démarre les timers.
 */
void startLevel(void)
{
    clearScreenGLCD();
    displayScores(score, enemiScore);
    drawGum();
    drawPacMan();
    drawEnnemi(0);

    startTimer1();   /* Joueur : ~3.6 ms (modifiable)*/
    startTimer8();   /* Ennemis : ~16 ms (NORMAL) (modifiable aussi)*/
}

/**
 * @brief Arrete TIM1 et TIM8 en fin de partie.
 */
void stopLevel(void)
{
    stopTimer1();
    stopTimer8();
}

/* ======================================================================== */
/*                       CALCUL DE DIRECTION (trackGum)                   */
/* ======================================================================== */

/**
 * @brief Calcule le vecteur de deplacement unitaire de @p begin vers @p end.
 *
 * Choisit l'axe présentant la plus grande distance (evite les
 * deplacements diagonaux qui ne sont pas gérés par le moteur de rendu).
 * Le pointeur retourné pointe sur une variable statique interne pour éviter les fuites mémoires.
 *
 * @param begin  Position de depart (ennemi).
 * @param end    Position cible (pac-gomme).
 * @return       Pointeur sur {dx, dy} avec dx, dy dans {-1, 0, +1}.
 */
Position *trackGum(Position begin, Position end)
{

    static Position dir;
    int dx = end.x - begin.x;
    int dy = end.y - begin.y;

    /*
     * Ancienne logique (BUGGUEE) :
     * On compare les distances et on choisit l'axe dominant a chaque appel.
     * Si |dx| ≈ |dy|, l'axe dominant change chaque tick → zigzag visuel.
     *
     * Nouvelle logique :
     * On epuise entierement l'axe X avant de toucher a Y.
     * L'ennemi trace un chemin en L : d'abord horizontal, puis vertical.
     * Un seul pixel par tick, un seul axe a la fois : zero zigzag.
     */

    if (dx != 0)          /* Priorite absolue a l'axe X */
    {
        dir.x = (dx > 0) ? 1 : -1;
        dir.y = 0;
    }
    else if (dy != 0)     /* On passe a Y uniquement quand X est aligne */
    {
        dir.x = 0;
        dir.y = (dy > 0) ? 1 : -1;
    }
    else                  /* Deja sur la cible */
    {
        dir.x = 0;
        dir.y = 0;
    }

    return &dir;
}


/* ======================================================================== */
/*                  MISE A JOUR DU JOUEUR  (ISR TIM1, ~3.6 ms)            */
/* ======================================================================== */

/**
 * @brief Deplace le joueur, gere l'animation et detecte les collisions.
 *
 * Appelee par TIM1_UP_TIM10_IRQHandler() toutes les ~3.6 ms.
 *
 * Priorites de traitement :
 *  1. Mouvement (joystick) + animation.
 *  2. Collision joueur ↔ ennemi → gameLost = 1.
 *  3. Collision joueur ↔ gomme → score++, spawn ennemi, nouvelle gomme.
 *     Si score >= 3 → gameWon = 1.
 */
void updatePlayer(void)

{
    int i;

    /* ------------------------------------------------------------------ */
    /* 1. Mise a jour de la direction si le joueur appuie sur le joystick  */
    /*    Le deplacement lui-meme est toujours automatique (voir etape 2)  */
    /* ------------------------------------------------------------------ */

    if      (joystickLeftPressed())  dirPacMan = DIR_LEFT;
    else if (joystickRightPressed()) dirPacMan = DIR_RIGHT;
    else if (joystickUpPressed())    dirPacMan = DIR_UP;
    else if (joystickDownPressed())  dirPacMan = DIR_DOWN;
    /* Si aucun joystick : dirPacMan conserve sa derniere valeur           */
    /* → Pac-Man continue dans la meme direction                           */

    /* ------------------------------------------------------------------ */
    /* 2. Deplacement automatique dans la direction courante               */
    /*    Bloque aux bords de l'ecran (Pac-Man s'arrete au mur)           */
    /* ------------------------------------------------------------------ */

    switch (dirPacMan)
    {
        case DIR_LEFT:
            if (xPacMan > 0)
            {
                clearZone(xPacMan + WIDTH_PACMAN - 1, yPacMan, 1, HEIGHT_PACMAN);
                xPacMan--;
            }
            break;

        case DIR_RIGHT:
            if (xPacMan + WIDTH_PACMAN < SCREEN_W)
            {
                clearZone(xPacMan, yPacMan, 1, HEIGHT_PACMAN);
                xPacMan++;
            }
            break;

        case DIR_UP:
            if (yPacMan > HUD_HEIGHT)
            {
                clearZone(xPacMan, yPacMan + HEIGHT_PACMAN - 1, WIDTH_PACMAN, 1);
                yPacMan--;
            }
            break;

        case DIR_DOWN:
            if (yPacMan + HEIGHT_PACMAN < SCREEN_H)
            {
                clearZone(xPacMan, yPacMan, WIDTH_PACMAN, 1);
                yPacMan++;
            }
            break;
    }

    /* Animation : alterne ouvert/ferme a chaque tick */
    animPacMan ^= 1;
    drawPacMan();

    /* ------------------------------------------------------------------ */
    /* 3. Collision joueur <-> ennemi                                      */
    /* ------------------------------------------------------------------ */

    for (i = 0; i < nbEnnemis; i++)
    {
        if (ennemis[i].inLive &&
            checkCollision(xPacMan, yPacMan, WIDTH_PACMAN, HEIGHT_PACMAN,
                           ennemis[i].itsPosition.x, ennemis[i].itsPosition.y,
                           ennemis[i].width, ennemis[i].height))
        {
            gameLost = 1;
            return;
        }
    }

    /* ------------------------------------------------------------------ */
    /* 4. Collision joueur <-> gomme                                       */
    /* ------------------------------------------------------------------ */

    if (!currentGum.claimed &&
        checkCollision(xPacMan, yPacMan, WIDTH_PACMAN, HEIGHT_PACMAN,
                       currentGum.itsPosition.x, currentGum.itsPosition.y,
                       currentGum.width, currentGum.height))
    {
        currentGum.claimed = true;
        score++;
        displayScores(score, enemiScore);

        if (score >= 3)
        {
            gameWon = 1;
            return;
        }

        if (nbEnnemis < MAX_ENNEMIS)
        {
            initEnnemi(nbEnnemis);
            drawEnnemi(nbEnnemis);
            nbEnnemis++;
        }

        spawnGum();
    }
}

/* ======================================================================== */
/*                  MISE A JOUR DES ENNEMIS  (ISR TIM8, ~16 ms)           */
/* ======================================================================== */

/**
 * @brief Deplace tous les ennemis actifs et detecte les collisions.
 *
 * Appelee par TIM8_UP_TIM13_IRQHandler() a cadence variable :
 *   NORMAL (~16 ms), SPEED (~13 ms), HARDCORE (~10 ms).
 *
 * Pour chaque ennemi :
 *  1. Calcul de la direction vers la gomme (trackGum).
 *  2. Effacement ancienne position.
 *  3. Deplacement d'un pixel dans la direction calculee.
 *  4. Maintien dans les bornes de l'ecran.
 *  5. Dessin a la nouvelle position.
 *  6. Collision ennemi ↔ joueur → gameLost = 1.
 *  7. Collision ennemi ↔ gomme → enemiScore++, acceleration, nouvelle gomme.
 *     Si enemiScore >= 3 → gameLost = 1.
 *
 * La gomme est redessinee en fin de fonction pour restaurer les pixels
 * qui auraient pu etre effaces lors du passage d'un ennemi.
 */
void updateEnnemis(void)
{
    int i;
    Position *dir;

    animEnnemi ^= 1; /* Alterner la frame d'animation a chaque passage */

    for (i = 0; i < nbEnnemis; i++)
    {
        if (!ennemis[i].inLive) continue;

        /* -------------------------------------------------------------- */
        /* 1. Direction vers la pac-gomme                                  */
        /* -------------------------------------------------------------- */
        dir = trackGum(ennemis[i].itsPosition, currentGum.itsPosition);

        /* -------------------------------------------------------------- */
        /* 2. Effacer l'ancienne position                                  */
        /* -------------------------------------------------------------- */
        clearZone(ennemis[i].itsPosition.x, ennemis[i].itsPosition.y,
                  ennemis[i].width, ennemis[i].height);

        /* -------------------------------------------------------------- */
        /* 3. Deplacement d'un pixel                                       */
        /* -------------------------------------------------------------- */
        if (dir->x != 0)
        {
            ennemis[i].direction      = (dir->x > 0) ? DIR_RIGHT : DIR_LEFT;
            ennemis[i].itsPosition.x += dir->x;
        }
        else if (dir->y != 0)
        {
            ennemis[i].direction      = (dir->y > 0) ? DIR_DOWN : DIR_UP;
            ennemis[i].itsPosition.y += dir->y;
        }

        /* -------------------------------------------------------------- */
        /* 4. Bornes de l'ecran                                            */
        /* -------------------------------------------------------------- */
        if (ennemis[i].itsPosition.x < 0)
            ennemis[i].itsPosition.x = 0;
        if (ennemis[i].itsPosition.x + ennemis[i].width > SCREEN_W)
            ennemis[i].itsPosition.x = SCREEN_W - ennemis[i].width;
        if (ennemis[i].itsPosition.y < HUD_HEIGHT)
            ennemis[i].itsPosition.y = HUD_HEIGHT;
        if (ennemis[i].itsPosition.y + ennemis[i].height > SCREEN_H)
            ennemis[i].itsPosition.y = SCREEN_H - ennemis[i].height;

        /* -------------------------------------------------------------- */
        /* 5. Dessin a la nouvelle position                                */
        /* -------------------------------------------------------------- */
        drawEnnemi(i);

        /* -------------------------------------------------------------- */
        /* 6. Collision ennemi ↔ joueur → DEFAITE                         */
        /* -------------------------------------------------------------- */
        if (checkCollision(ennemis[i].itsPosition.x, ennemis[i].itsPosition.y,
                           ennemis[i].width, ennemis[i].height,
                           xPacMan, yPacMan, WIDTH_PACMAN, HEIGHT_PACMAN))
        {
            gameLost = 1;
            return;
        }

        /* -------------------------------------------------------------- */
        /* 7. Collision ennemi ↔ gomme → POINT ENNEMI                     */
        /* -------------------------------------------------------------- */
        if (!currentGum.claimed &&
            checkCollision(ennemis[i].itsPosition.x, ennemis[i].itsPosition.y,
                           ennemis[i].width, ennemis[i].height,
                           currentGum.itsPosition.x, currentGum.itsPosition.y,
                           currentGum.width, currentGum.height))
        {
            currentGum.claimed = true; /* Marquer avant spawnGum */
            enemiScore++;
            displayScores(score, enemiScore);

            /* Defaite si les ennemis atteignent 3 points */
            if (enemiScore >= 3)
            {
                gameLost = 1;
                return;
            }

            /* Augmenter la vitesse des ennemis a chaque gomme collectee */
            if      (enemiScore == 1) setSpeedTimer8(SPEED);
            else if (enemiScore >= 2) setSpeedTimer8(HARDCORE);

            spawnGum();
        }
    }

    /*
     * Redessiner la gomme en dernier : elle peut avoir ete partiellement
     * effacee lors du clearZone d'un ennemi qui la croisait.
     */
    drawGum();
}
