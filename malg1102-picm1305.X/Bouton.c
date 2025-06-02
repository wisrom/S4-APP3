#include <xc.h>

/* Basys MX3 includes */
#include "config.h"


/* Project-specific includes */
//#include "ADC_YBL.h"
#include "Timers_YBL.h"
#include "MEF_1.h"
#include "led.h"


extern int g_adc_val;
int Valeur_BTNR = 0;
int Valeur_BTNU = 0;
int Valeur_BTND = 0;
int Valeur_BTNL = 0;

void Initialisation_BTN(void)
{
    // BTNU = RB1
    TRISBbits.TRISB1 = 1;
    ANSELBbits.ANSB1 = 0;

    // BTNL = RB0
    TRISBbits.TRISB0 = 1;
    ANSELBbits.ANSB0 = 0;

    // BTNC = RF4
    TRISFbits.TRISF4 = 1;

    // BTNR = RB8
    TRISBbits.TRISB8 = 1;
    ANSELBbits.ANSB8 = 0;

    // BTND = RA15
    TRISAbits.TRISA15 = 1;
}

int Lecture_BTN(void)
{
   if (PORTBbits.RB1 == 1)
        return BTN_UP;
    else if (PORTAbits.RA15 == 1)
         return BTN_DOWN;
    else if (PORTBbits.RB0 == 1)
        return BTN_LEFT;
    else if (PORTBbits.RB8 == 1)
        return BTN_RIGHT;
    else if (PORTFbits.RF0== 1)
        return BTN_CENTER;
    
    else
        return 0;
}

