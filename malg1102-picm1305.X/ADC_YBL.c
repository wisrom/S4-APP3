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

#include "ADC_YBL.h"
#include "Timers_YBL.h"
#include "led.h"

extern volatile uint16_t g_adc_val;

volatile uint8_t Is_Recording = 0;
volatile uint32_t Index_Recording = 0;
volatile uint16_t audio_index = 0;
volatile uint16_t audio_buffer[AUDIO_BUF_SIZE] = {0};
volatile uint32_t Index_Playback = 0;
volatile uint8_t Is_Playing = 0;
extern volatile uint8_t Is_Testing;
//extern volatile uint8_t Is_Recording = 0;
extern const uint16_t sinus_400Hz[20] ;
extern volatile uint8_t Index_Sinus;
volatile uint16_t sample_count = 0;
extern volatile uint32_t Index_Testing ; 
void __ISR(_ADC_VECTOR, IPL2AUTO) Mic_ISR(void) 
{
    if (IFS0bits.AD1IF) {
        IFS0bits.AD1IF = 0;   // Clear flag

        g_adc_val = ADC1BUF0;  // Lire valeur ADC

        if (Is_Recording) {
            audio_buffer[audio_index++] = g_adc_val;
            if (audio_index >= AUDIO_BUF_SIZE)
                audio_index = 0;  // boucle circulaire
        }

        else if (Is_Testing) {
            if (sample_count < 32000) {
                // Jouer sinus
                audio_buffer[Index_Testing++] = sinus_400Hz[Index_Sinus++];
                if (Index_Sinus >= 20) Index_Sinus = 0;
            }
            else if ((sample_count > 32000) && sample_count <= AUDIO_BUF_SIZE) {
                audio_buffer[Index_Testing++] = 0;
                // Après les 1600 samples sinus, remplir buffer avec des zéros
                
            }
            else {
                // Fin du test : on peut déclencher la lecture
                Is_Testing = 0;
                Index_Recording = AUDIO_BUF_SIZE; // Simuler enregistrement plein
                Index_Testing = 0;
                Index_Sinus = 0;
                LED_SetValue(1, 0); // Éteint LED de test
            }
        }
    }
}


/* ************************************************************************** */
/***	ADC1_Config_YBL()
**
**	Parameters:
**		
**
**	Return Value:
**		
**
**	Description:
**		Cette fonction configure le module du convertisseur analogique-numérique
**      ADC1 pour être opéré par interruption via Timer3.
**
**      Pour effectuer cette configuration, on suit dans l'ordre les étapes de
**      la procédure décrite à la section 17.4 du document "PIC32 Family
**      Reference Manual".
*/

void ADC1_Config_YBL(void)
{
    /* Cette configuration suit les étapes de la procédure décrite à la section
     * 17.4 du document "PIC32 Family Reference Manual"; les étapes plus bas
     * sont numérotées selon les sous-sections 17.4.1, 17.4.2, ... (on n'utilise
     * que le numéro de fin de la sous-section). L'ordre des étapes n'est pas
     * suivies rigoureusement dans l'ordre, notamment l'étape 2 qui ne suit
     * pas plus bas l'étape 1. */

    /* Étape 1: Configuring Analog Port Pins */
    /* On veut mesurer la tension du AIC qui est connecté au module ADC par la
     * broche AN2; il faut donc configurer la broche AN2 (RB2) comme une entrée
     * et il faut mettre le bit 2 du registre ANSELB à 1, v. sect. 12.2.5 du
     * manuel de réf. de PIC32, ou sect. 12.1.2 de la fiche tech. du PIC32. */
    TRISBbits.TRISB2 = 1;
    ANSELBbits.ANSB2 = 1;
     
    AD1CON1 = 0; /* AD1CON1 est définie dans p32mx370f512l.h qui est inclus via xc.h;
                  * Ceci met tout le registre AD1CON1 à zéro pour s'assurer que
                  * tout est bien à zéro avant de configurer le registre. */
    AD1CON2 = 0;

    /* Étape 3: Selecting the Format of the ADC Result */
    AD1CON1bits.FORM = 0b000; /* Integer 16-bit (DOUT = 0000 0000 0000 0000 0000 00dd dddd dddd) */
    
    /* Étape 4: Selecting the Sample Clock Source */
    AD1CON1bits.SSRC = 0b010; /* Timer3 period match ends sampling and starts conversion */
    
    
    
    /* Étapes 5 et 6 sont ensemble. */
    /* Étape 5: Synchronizing ADC Operations to Internal or External Events */
    /* Étape 6: Selecting Automatic or Manual Sampling */
    AD1CON1bits.ASAM = 1; /* Automatic sampling: Sampling begins immediately after
                           * last conversion completes; SAMP bit is automatically set in this approach. */
    
    /* Étape 7: Selecting the Voltage Reference Source */
    AD1CON2bits.VCFG = 0b000; /* VREFH = AVDD, VREFL = AVSS */
    
    /* Étape 8: Selecting the Scan Mode */
    AD1CON2bits.CSCNA = 0; /* Scan mode is disabled and positive input to MUX A controlled by CH0SA bits */

    /* Étape 11: Selecting the MUX to be Connected to the ADC (Alternating Sample Mode)*/
    AD1CON2bits.ALTS = 0; /* Always use Sample A input multiplexer settings */
    /* Étape 2: Selecting the Analog Inputs to the ADC Multiplexers */
    AD1CHSbits.CH0SA = 0b00010; /* Channel 0 positive input is AN2 for Sample A parce que l'AIC est branché sur AN2 */
    AD1CHSbits.CH0NA = 0; /* Channel 0 negative input is VREFL for Sample A */
    //AD1CHSbits.CH0SB = 0b00010;  /* Channel 0 positive input is AN2 for Sample B CETTE LIGNE N'EST PAS NÉCESSAIRE, CAR ALTS = 0. */
    //AD1CHSbits.CH0NB = 0; /* Channel 0 negative input is VREFL for Sample B CETTE LIGNE N'EST PAS NÉCESSAIRE, CAR ALTS = 0. */

    /* Étape 9: Setting the Number of Conversions per Interrupt */
    AD1CON2bits.SMPI = 0b0000; /* Interrupt at completion of conversion for each sample/convert sequence */
    
    /* Étape 10: Buffer Fill Mode */
    AD1CON2bits.BUFM = 0; /* Buffer configured as one 16-word buffer, CETTE LIGNE NE SERAIT PAS NÉCESSAIRE, CAR AD1CON2 A ÉTÉ MIS TOUT À ZÉRO AU DÉPART. */

    AD1CON3 = 0;
    
    /* Étape 12: Selecting the ADC Conversion Clock Source and Prescaler */
    AD1CON3bits.ADRC = 0; /* Clock derived from Peripheral Bus Clock (PBCLK) */
    //AD1CON3bits.SAMC = 0b00010;
    //AD1CON3bits.ADCS = 0b00000001;

    //AD1CSSL = 0;
    //AD1CSSLbits.CSSL2 = 1;
    
    /* Configurer les interruptions générées par le module ADC */
    IPC5bits.AD1IP = 2;
    IPC5bits.AD1IS = 0;
    IFS0bits.AD1IF = 0; /* S'assurer que le fanion d'interruption est à 0 - make sure interrupt flag is cleared */
    IEC0bits.AD1IE = 1; /* Activer les interruptions du module ADC - Enable ADC interrupts */

} /* fin ADC1_Config_YBL */


void ADC1_Demarre_YBL(void);
//{
    /* Étape 14: Turning ON the ) */
    //AD1CON1bits.ON = 1; /* On démarre le module ADC */
//} /* fin ADC1_Demarre_YBL */


void Mic_Config(void)
{
    TRISBbits.TRISB4 = 1; //initialisation du micro
    ANSELBbits.ANSB4 = 1;
    AD1CON1 = 0; /* AD1CON1 est définie dans p32mx370f512l.h qui est inclus via xc.h;
                  * Ceci met tout le registre AD1CON1 à zéro pour s'assurer que
                  * tout est bien à zéro avant de configurer le registre. */
    AD1CON2 = 0;
 
    /* Étape 3: Selecting the Format of the ADC Result */
    AD1CON1bits.FORM = 0b000; /* Integer 16-bit (DOUT = 0000 0000 0000 0000 0000 00dd dddd dddd) */
    /* Étape 4: Selecting the Sample Clock Source */
    AD1CON1bits.SSRC = 0b010; /* Timer3 period match ends sampling and starts conversion */

    /* Étapes 5 et 6 sont ensemble. */
    /* Étape 5: Synchronizing ADC Operations to Internal or External Events */
    /* Étape 6: Selecting Automatic or Manual Sampling */
    AD1CON1bits.ASAM = 1; /* Automatic sampling: Sampling begins immediately after
                           * last conversion completes; SAMP bit is automatically set in this approach. */
    /* Étape 7: Selecting the Voltage Reference Source */
    AD1CON2bits.VCFG = 0b000; /* VREFH = AVDD, VREFL = AVSS */
    /* Étape 8: Selecting the Scan Mode */
    AD1CON2bits.CSCNA = 0; /* Scan mode is disabled and positive input to MUX A controlled by CH0SA bits */
    /* Étape 11: Selecting the MUX to be Connected to the ADC (Alternating Sample Mode)*/
    AD1CON2bits.ALTS = 0; /* Always use Sample A input multiplexer settings */
    /* Étape 2: Selecting the Analog Inputs to the ADC Multiplexers */
    AD1CHSbits.CH0SA = 0b00100; /* Channel 0 positive input is AN2 for Sample A parce que l'AIC est branché sur AN2 */
    AD1CHSbits.CH0NA = 0; /* Channel 0 negative input is VREFL for Sample A */
    //AD1CHSbits.CH0SB = 0b00010;  /* Channel 0 positive input is AN2 for Sample B CETTE LIGNE N'EST PAS NÉCESSAIRE, CAR ALTS = 0. */
    //AD1CHSbits.CH0NB = 0; /* Channel 0 negative input is VREFL for Sample B CETTE LIGNE N'EST PAS NÉCESSAIRE, CAR ALTS = 0. */
 
    /* Étape 9: Setting the Number of Conversions per Interrupt */
    AD1CON2bits.SMPI = 0b0000; /* Interrupt at completion of conversion for each sample/convert sequence */
    /* Étape 10: Buffer Fill Mode */
    AD1CON2bits.BUFM = 0; /* Buffer configured as one 16-word buffer, CETTE LIGNE NE SERAIT PAS NÉCESSAIRE, CAR AD1CON2 A ÉTÉ MIS TOUT À ZÉRO AU DÉPART. */
 
    AD1CON3 = 0;
    /* Étape 12: Selecting the ADC Conversion Clock Source and Prescaler */
    AD1CON3bits.ADRC = 0; /* Clock derived from Peripheral Bus Clock (PBCLK) */
    //AD1CON3bits.SAMC = 0b00010;
    //AD1CON3bits.ADCS = 0b00000001;
 
    //AD1CSSL = 0;
    //AD1CSSLbits.CSSL2 = 1;
    /* Configurer les interruptions générées par le module ADC */
    IPC5bits.AD1IP = 2;
    IPC5bits.AD1IS = 0;
    IFS0bits.AD1IF = 0; /* S'assurer que le fanion d'interruption est à 0 - make sure interrupt flag is cleared */
    IEC0bits.AD1IE = 1; /* Activer les interruptions du module ADC - Enable ADC interrupts */
 
}
 
void Mic_Demarre(void)
{
    AD1CON1bits.ON = 1; /* On démarre le module ADC */
 
/* *****************************************************************************
End of File
*/
}

/*
void Speaker_Init(void)
{
    // Configure la broche RC14 (OC1) en sortie numérique
    TRISBbits.TRISB14 = 0;  // RC14 en sortie
    ANSELBbits.ANSB14 = 0;  // Valeur initiale
    RPB14R = 0x0C;          // Timer2 configuration (pour OC1)
    T2CON = 0;               // Clear timer settings
    T2CONbits.TCKPS = 0b000; // Prescaler 1:1
    PR2 = 1022;               // Ajusté pour obtenir une fréquence d'interruption environ 48 kHz
    TMR2 = 0;                // Clear timer
    T2CONbits.TON = 1;       // Démarre Timer2
    // OC1 configuration
    OC1CON = 0;              // Reset OC1
    OC1CONbits.OCM = 0b110;  // PWM mode without fault pin
    OC1CONbits.OCTSEL = 0;   // OC1 utilise Timer2
    OC1RS = 0;             // Rapport cyclique (50% par défaut)
    OC1R = 0;              // Valeur initiale
    OC1CONbits.ON = 1;       // Active OC1
}*/

void Speaker_Init(void)
{
    TRISBbits.TRISB14 = 0;        // RB14 en sortie
    ANSELBbits.ANSB14 = 0;        // Numérique seulement
    RPB14R = 0x0C;                // OC1 sur RB14

    Timer2_Config();              // Configure Timer2 (48kHz)
    Timer2_Demarre();            // Lance Timer2

    OC1CON = 0;
    OC1CONbits.OCM = 0b110;      // PWM mode sans fault pin
    OC1CONbits.OCTSEL = 0;       // Timer2 comme horloge
    OC1RS = 0;
    OC1R = 0;
    OC1CONbits.ON = 1;
}
 
void Speaker_Demarre(void);

