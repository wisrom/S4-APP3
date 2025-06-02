#ifndef _MEF_H    /* Guard against multiple inclusion */
#define _MEF_H



#ifdef __cplusplus   /* Provide C++ Compatibility */
extern "C" {
#endif

#define BTN_UP 1
#define BTN_RIGHT 2
#define BTN_DOWN 3
#define BTN_LEFT 4
#define BTN_CENTER 5
#define VOLUME_MAX 4
#define VOLUME_MIN 0

extern const float Gain_Level[5];
extern int Volume;

typedef enum {
    ETAT_ATTENTE,
    ETAT_ENREGISTREMENT,
    ETAT_JOUER_ENREGISTREMENT,
    ETAT_SINUS_TEST,
    
} Etat;

void MEF_1(void);
void Attente(void);
void Enregistrer(void);
void Jouer_Enregistrement(void);
void Sinus_Test(void);
void Volume_Up(void);
void Volume_Down(void);
void Affiche_Volume(int niveau);
void Start_Sending (void);
void Start_Recording (void);


#ifdef __cplusplus /* Provide C++ Compatibility */
}
#endif

#endif /* _ADC_YBL_H */