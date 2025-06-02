/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _ADC_YBL_H    /* Guard against multiple inclusion */
#define _ADC_YBL_H



#ifdef __cplusplus   /* Provide C++ Compatibility */
extern "C" {
#endif

//#define BUFFER_SIZE 64000
#define AUDIO_BUF_SIZE 64000

    
extern volatile uint8_t Is_Recording;
extern volatile uint32_t Index_Recording;
extern volatile uint16_t audio_buffer[AUDIO_BUF_SIZE];
extern volatile uint16_t audio_index;
extern volatile uint32_t Index_Playback;
extern volatile uint8_t Is_Playing;

void ADC1_Config_YBL(void);
void ADC1_Demarre_YBL(void);
void Mic_Config(void);
void Mic_Demarre(void);
void Speaker_Init(void);
void Speaker_Demarre(void);
#ifdef __cplusplus /* Provide C++ Compatibility */
}
#endif

#endif /* _ADC_YBL_H */

/* *****************************************************************************
 End of File
 */
