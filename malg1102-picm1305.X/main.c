/* 
 * Fichier:   main.c
 * Auteur: bery2005
 *
 * Cr?? le 9 janvier 2025, 10:42
 * 
 * Description: Ce programme lit la tension contr?l?e par le potentiom?tre
 *              nomm? ANALOG INPUT CONTROL (AIC) avec un ADC ? tous les
 *              0.1 seconde et si cette tension d?passe la moiti? de la
 *              plage [0, 3.3V], la LED2 est allum?e, sinon elle est ?teinte.
 *              L'AIC est connect? sur la broche AN2 du module de l'ADC (voir
 *              manuel de r?f. de la carte BasysMX3 de Digilent).
 */

/* Microchip includes */
#include <xc.h>

/* Basys MX3 includes */
#include "config.h"
#include "config_bits.h"

/* Project-specific includes */
#include "ADC_YBL.h"
#include "Timers_YBL.h"
#include "MEF_1.h"
#include "Bouton.h"
#include "led.h"
/* Variables globales; d?butent par g_ */
int g_adc_val = 0;


int main(int argc, char** argv) {
    g_adc_val = 0;

    TRISACLR = 0x00FF; // Configure les LED A0-A7 en sortie
    LATACLR = 0x00FF;  // Éteint toutes les LED
    LED_SetValue(7, 1); // Allume LED7 pour indiquer que le système démarre

    Initialisation_BTN();
    Mic_Config();
    Mic_Demarre();
    Speaker_Init();

    Timer1_Config();
    Timer3_Config();

    Timer1_Demarre();
    Timer3_Demarre();

    macro_enable_interrupts();

    while (1) {
        MEF_1(); // Exécution de la machine d'états principale
    }
    return 0;
} /* fin main */
