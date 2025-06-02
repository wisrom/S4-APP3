#include <xc.h>
#include "config.h"
#include "ADC_YBL.h"
#include "Timers_YBL.h"
#include "MEF_1.h"
#include "led.h"
#include "Bouton.h"

// Variables globales externes partag�es entre modules
extern volatile uint8_t Is_Recording;
extern volatile uint32_t Index_Recording;
extern volatile uint32_t Index_Playback;
extern volatile uint8_t Is_Playing;

// Tableau contenant une p�riode d'un sinus de 400 Hz (20 �chantillons)
const uint16_t sinus_400Hz[20] = {
    310, 406, 492, 561, 605, 620, 605, 561, 492, 406,
    310, 214, 128, 59, 15, 0, 15, 59, 128, 214};

// Volume initial maximal
int Volume = VOLUME_MAX;

// Sauvegarde de l'�tat pr�c�dent du bouton
int bouton_precedent = 0;

// Niveaux de gain associ�s aux diff�rents niveaux de volume
const float Gain_Level[5] = {0.00, 0.25, 0.50, 0.75, 1.00};

// Index utilis� pour parcourir le tableau du sinus
volatile uint8_t Index_Sinus = 0;

// Indique si le test sinus est en cours
extern volatile uint8_t Is_Testing;

// �tat actuel de la machine � �tats
Etat etat_courant = ETAT_ATTENTE;

// Fonction principale de la machine � �tats
void MEF_1(void)
{
    // Lecture du bouton press� (valeur cod�e)
    int bouton = Lecture_BTN();

    // S�lection de l'action selon l'�tat courant
    switch (etat_courant)
    {
        case ETAT_ATTENTE:
            // Affichage du niveau de volume pendant l?attente
            Attente();

            // D�tection de front montant sur un bouton
            if (bouton != 0 && bouton_precedent == 0)
            {
                switch (bouton)
                {
                    case BTN_UP:
                        Volume_Up(); // Augmente volume
                        break;

                    case BTN_DOWN:
                        Volume_Down(); // Diminue volume v
                        break;

                    case BTN_LEFT:
                        etat_courant = ETAT_JOUER_ENREGISTREMENT;
                        Jouer_Enregistrement(); // D�clenche la lecture
                        break;

                    case BTN_RIGHT:
                        Start_Recording(); // Initialise l'enregistrement
                        LED_SetValue(0, 1); // Allume LED0
                        etat_courant = ETAT_ENREGISTREMENT;
                        break;

                    case BTN_CENTER:
                        etat_courant = ETAT_SINUS_TEST;
                        Index_Sinus = 0;   // R�initialise index du sinus
                        Is_Testing = 1;    // Active le mode test
                        LED_SetValue(0, 1); // Allume LED0
                        break;
                }
            }

            bouton_precedent = bouton; // M�morise l?�tat du bouton
            break;

        case ETAT_ENREGISTREMENT:
            // Si l'enregistrement s?est termin�, on peut lancer la lecture avec BTN_LEFT
            if (Is_Recording == 1) {
                // Tant que l?enregistrement est actif, ignorer tous les boutons
                break;
            }
            /*Ici je mets une safety pour m'assurer que je puisse passer � l'�tat d'enregistrement si l'�tat
             n'est pas chang� automatiquement*/
            if (Is_Recording == 0 && bouton == BTN_LEFT)
            {
                etat_courant = ETAT_JOUER_ENREGISTREMENT;
                Jouer_Enregistrement(); // D�bute lecture
            }
            else
            {
                etat_courant = ETAT_ATTENTE;
            }
            bouton_precedent = bouton;
            break;

        case ETAT_JOUER_ENREGISTREMENT:
            // Retour � l?attente si la lecture est termin�e
            if (!Is_Playing) {
                LED_SetValue(1, 0); // �teint LED1
                etat_courant = ETAT_ATTENTE;
            }

            // Gestion du volume pendant la lecture
            if (bouton != 0 && bouton_precedent == 0)
            {
                switch (bouton)
                {
                    case BTN_UP:
                        Volume_Up();
                        break;

                    case BTN_DOWN:
                        Volume_Down();
                        break;
                }
            }

            bouton_precedent = bouton;
            break;

        case ETAT_SINUS_TEST:
            // Fin du test sinus : retour � l?�tat d?attente
            if (!Is_Testing) {
                LED_SetValue(0, 0); // �teint LED0
                etat_courant = ETAT_ATTENTE;
            }

            // Gestion du volume pendant le test
            if (bouton != 0 && bouton_precedent == 0)
            {
                switch (bouton)
                {
                    case BTN_UP:
                        Volume_Up();
                        break;

                    case BTN_DOWN:
                        Volume_Down();
                        break;
                }
            }

            bouton_precedent = bouton;
            break;
    }
}

// Affiche le volume en allumant les LED de LD2 � LD6
void Attente(void)
{
    Affiche_Volume(Volume);
}

// Pr�pare la lecture du buffer audio
void Jouer_Enregistrement(void)
{
    if (Index_Recording == 0) return; // Rien � lire, on ne fait rien

    Is_Recording = 0;     // On s'assure que l'enregistrement est arr�t�
    Is_Playing = 1;       // Active la lecture
    Index_Playback = 0;   // Red�marre au d�but du buffer
    LED_SetValue(1, 1);   // Allume LED1 pour indiquer la lecture
}

// Allume les LED de LD2 � LD6 en fonction du niveau de volume
void Affiche_Volume(int niveau)
{
    int i;
    for (i = 2; i <= 6; i++) {
        LED_SetValue(i, (i - 2 <= niveau) ? 1 : 0);
    }
}

// Incr�mente le volume (si possible) et met � jour l'affichage
void Volume_Up(void)
{
    if (Volume < VOLUME_MAX) {
        Volume++;
        Affiche_Volume(Volume);
    }
}

// Diminue le volume (si possible) et met � jour l'affichage
void Volume_Down(void)
{
    if (Volume > VOLUME_MIN) {
        Volume--;
        Affiche_Volume(Volume);
    }
}

// Initialise un nouvel enregistrement
void Start_Recording(void)
{
    Is_Recording = 1;
    Index_Recording = 0; // Red�marre au d�but du buffer
}

// Arr�te explicitement l'enregistrement (non utilis� ici directement)
void Start_Sending(void)
{
    Is_Recording = 0;
}
