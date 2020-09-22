/*
 * sonar.h
 *
 *  Created on: Sep 19, 2020
 *      Author: Kyle Rodrigues
 *
 *      ECHOL = GPIO_EXTI1 - EXTI1_IRQHandler(void)
 *      ECHOR = GPIO_EXTI9 - EXTI9_5_IRQHandler(void)
 *      ECHO_CTR = GPIO_EXTI7 -EXTI9_5_IRQHandler(void)
 */

#ifndef INC_SONAR_H_
#define INC_SONAR_H_

#include <stdbool.h> //this is required to use bool statements
#include <stdint.h> //standard interager
#include <stdio.h>
#include "tim.h"
#include "main.h"

#define uSTIM TIM9

// event flag to pass to updateSonars method
typedef enum SONAR_UPADTE_EVENT_t {
  SONAR_EVENT_TIMER,
  SONAR_EVENT_ERROR,
  SONAR_EVENT_ECHO,
  SONAR_EVENT_CHECK
} SONAR_UPDATE_EVENT;

typedef struct SONAR_t {
	char *sonar_ch;
	uint32_t trig_port;
	uint32_t trig_pin;
	uint32_t echo_port;
	uint32_t echo_pin;
	uint32_t tick;
	float distance;


}SONAR_STATUS;


void checkSonar(SONAR_STATUS *sonar);
void updateSonar(SONAR_STATUS *sonar);


#endif /* INC_SONAR_H_ */
