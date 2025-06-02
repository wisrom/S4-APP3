#ifndef _BOUTON_H    /* Guard against multiple inclusion */
#define _BOUTON_H



#ifdef __cplusplus   /* Provide C++ Compatibility */
extern "C" {
#endif

#define BTN_UP 1
#define BTN_RIGHT 2
#define BTN_DOWN 3
#define BTN_LEFT 4
#define BTN_CENTER 5
    

void Initialisation_BTN(void);
int Lecture_BTN(void);


#ifdef __cplusplus /* Provide C++ Compatibility */
}
#endif

#endif /* _ADC_YBL_H */