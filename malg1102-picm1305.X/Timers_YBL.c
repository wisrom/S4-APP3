/* ************************************************************************** */
/** Descriptive File Name
 
  @Company
    Company Name
 
  @File Name
    filename.c
 
  @Summary
    Brief description of the file.
 
  @Description
    Describe the purpose of this file.
*/
/* ************************************************************************** */
 
#include <xc.h>
#include <sys/attribs.h>
 
#include "Timers_YBL.h"
#include "ADC_YBL.h"
#include "MEF_1.h"
#include "led.h"
 
extern volatile uint16_t g_adc_val;
#define SAMPLE_RATE //8000
#define PWM_FREQ    //48900
#define PWM_MAX     //1023
//#define BUFFER_SIZE 64000
//volatile uint16_t audioBuffer[BUFFER_SIZE];
//volatile uint32_t sampleIndex = 0;
extern volatile uint16_t audio_buffer[AUDIO_BUF_SIZE];
volatile uint32_t playback_index = 0;          // Index de lecture dans le buffer
extern  int Volume;                          // Niveau de volume (0 à 4)
/* Routine d'interruption pour le Timer3 */
extern Etat etat_courant;
extern const uint16_t sinus_400Hz[] ;
extern volatile uint8_t Index_Sinus;
volatile uint8_t Is_Testing = 0;
volatile uint32_t Index_Testing = 0;          // Index de lecture dans le buffer
extern volatile uint16_t sample_count;




void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3_ISR(void) 
{
    IFS0bits.T3IF = 0; // Clear flag
 
     if (Is_Recording) {
        if (Index_Recording < AUDIO_BUF_SIZE) {
            audio_buffer[Index_Recording++] = g_adc_val;
        } else {
            // Buffer plein, fin d'enregistrement
            Is_Recording = 0;
            LED_SetValue(0, 0); // Éteindre LED0
        }
    } 
    
    
     else  if (Is_Testing) {
        if (Index_Testing < 32000) {
            // 1. Ajouter sinus
            audio_buffer[Index_Testing++] = sinus_400Hz[Index_Sinus++];
            if (Index_Sinus >= 20) Index_Sinus = 0;
        }
        else if (Index_Testing < 32000) {
            // 2. Ajouter silence
            audio_buffer[Index_Testing++] = 0;
        }
        else {
            // 3. Fin du test
            Is_Testing = 0;
            Index_Recording = AUDIO_BUF_SIZE; // Permet lecture complète
            Index_Testing = 0;
            Index_Sinus = 0;
            LED_SetValue(1, 0); // Éteint LED test
        }
    }

   
    else if (Is_Playing) {
        if (Index_Playback < Index_Recording) {
            float gain = Gain_Level[Volume];
            uint16_t sample = audio_buffer[Index_Playback++];
            uint16_t adjusted = (uint16_t)(sample * gain);
            OC1RS = adjusted;
        } else if(Is_Playing){
            // Fin de lecture
            Is_Playing = 0;
            Index_Playback = 0;
            LED_SetValue(1, 0); // Éteint LD1 (lecture)
        }
    }
    
    
} /* Fin Timer3_ISR_YBL */
void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2_ISR(void)
{
    float gain = Gain_Level[Volume];
    
    // Charger la valeur PWM correspondante à l?échantillon courant
    OC1RS = (uint16_t)(audio_buffer[playback_index] * gain);
    
    playback_index++;
    if (playback_index >= AUDIO_BUF_SIZE) {
        playback_index = 0; // Reboucle à zéro ou stopper la lecture selon l?usage
    }
    
    // Effacer le flag d'interruption
    IFS0bits.T2IF = 0;
}
 
void __ISR(_TIMER_1_VECTOR, IPL3AUTO) Timer1_ISR_YBL(void) 
{  
     if (etat_courant == ETAT_JOUER_ENREGISTREMENT && Is_Playing) {
        LATAINV = 0x0002;  // Toggle LED1
    } else {
        LATACLR = 0x0002;  // LED1 éteinte sinon
    }
 
    IFS0bits.T1IF = 0;
} /* Fin Timer1_ISR_YBL */
/* ************************************************************************** */
/***	Timer3_Config_YBL()
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		Cette fonction configure le module du convertisseur analogique-numérique (ADC)
**      pour être opéré par interruption via Timer3.
**
**      Pour effectuer cette configuration, on suit dans l'ordre les étapes de
**      la procédure décrite à la section 17.4 du document "PIC32 Family
**      Reference Manual".
*/
void Timer3_Config(void)
{
    /* Comme on utilise le Timer3 pour déclencher les conversions, on configure
     * le Timer3 pour déclencher une interrruption à toutes le 0.1 s tel que
     *  requis */
    T3CON = 0;
    T3CONbits.TCS = 0; /* Synchronous Clock Counter Mode */
    T3CONbits.TCKPS = 0b111; /* 1:256 prescale value */
    TMR3 = 0x0000; /* Compteur initialisé à zéro */
    PR3 = 22; /* Avec ces valeurs de TCKPS et de PR3 et avec PBCLK = 48 MHz, ça donne une fréquence d'interruption de 8kHz. */
    IPC3bits.T3IP = 1; /* Priorité de l'interruption */
    IPC3bits.T3IS = 0; /* Sous-priorité (subpriority) */
    IFS0bits.T3IF = 0; /* Mettre fanion d'interruption à 0 (clear interrupt flag) */
    IEC0bits.T3IE = 1; /* Activer les interruptions (enable interrupt) */
} /* fin Timer3_Config_YBL */
 
void Timer3_Demarre(void)
{
    T3CONbits.ON = 1; /* On démarre le timer */
} /* Fin Timer3_Demarre_YBL */
 
void Timer2_Config(void)
{
    T2CON = 0;
    T2CONbits.TCS = 0; /* Synchronous Clock Counter Mode */
    T2CONbits.TCKPS = 0b000; /* 1:1 prescale value */
    TMR2 = 0x0000; /* Compteur initialisé à zéro */
    PR2 = 1022; /* Avec ces valeurs de TCKPS et de PR3 et avec PBCLK = 48 MHz, ça donne une fréquence d'interruption de 10 Hz. */
    IPC2bits.T2IP = 1; /* Priorité de l'interruption */
    IPC2bits.T2IS = 0; /* Sous-priorité (subpriority) */
    IFS0bits.T2IF = 0; /* Mettre fanion d'interruption à 0 (clear interrupt flag) */
    IEC0bits.T2IE = 0; /* Activer les interruptions (enable interrupt) */
}
void Timer2_Demarre(void)
{
    T2CONbits.ON = 1;
}
 
void Timer1_Config(void)
{
     /* On utilise le Timer1 pour d?clencher les conversions, on configure
     * le Timer3 pour d?clencher une interrruption ? toutes le 0.1 s tel que
     *  requis */
    T1CON = 0;
    T1CONbits.TCS = 0; /* Synchronous Clock Counter Mode */
    T1CONbits.TCKPS = 0b11; /* 1:256 prescale value */
    TMR1 = 0x0000; /* Compteur initialis? ? z?ro */
    PR1 = 18749; /* Avec ces valeurs de TCKPS et de PR1 et avec PBCLK = 48 MHz, ?a donne une fr?quence d'interruption de 10 Hz. */
    IPC1bits.T1IP = 1; /* Priorit? de l'interruption */
    IPC1bits.T1IS = 0; /* Sous-priorit? (subpriority) */
    IFS0bits.T1IF = 0; /* Mettre fanion d'interruption ? 0 (clear interrupt flag) */
    IEC0bits.T1IE = 1; /* Activer les interruptions (enable interrupt) */
}
void Timer1_Demarre(void)
{
    T1CONbits.ON = 1; /* On d?marre le timer */
}
/* *****************************************************************************
End of File
*/