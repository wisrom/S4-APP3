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

#ifndef _TIMERS_YBL_H    /* Guard against multiple inclusion */
#define _TIMERS_YBL_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif
extern volatile uint32_t playback_index;
void Timer3_Config(void);
void Timer3_Demarre(void);
void Timer2_Config(void);
void Timer2_Demarre(void);
void Timer1_Config(void);
void Timer1_Demarre(void);
//void Timer1_Config_YBL(void);
//void Timer1_Demarre_YBL(void);
#ifdef __cplusplus
}
#endif

#endif /* _TIMERS_YBL_H */

/* *****************************************************************************
 End of File
 */
