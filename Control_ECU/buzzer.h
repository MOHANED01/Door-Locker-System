/*
 * buzzer.h
 *
 *  Created on: Oct 31, 2022
 *      Author: mohaned
 */

#ifndef BUZZER_H_
#define BUZZER_H_
/*******************************************************************************
 *                      Preprocessor Macros                                    *
 *******************************************************************************/
#define BUZZER_PORT 3
#define BUZZER_PIN 2
/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void Buzzer_init(void);
void Buzzer_on(void);
void Buzzer_off(void);
#endif /* BUZZER_H_ */
