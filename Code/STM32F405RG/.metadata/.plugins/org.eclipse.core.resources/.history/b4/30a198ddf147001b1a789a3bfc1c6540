/*
 * interupt.c
 *
 *  Created on: Oct 9, 2020
 *      Author: Kyle Rodrigues
 */

#include "main.h"
#include "sonar.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  switch (GPIO_Pin) {
	case ECHOL_Pin:
		sonarISR(SONAR1);
		break;

	case ECHOR_Pin:
		sonarISR(SONAR2);
		break;

	case ECHO_CTR_Pin:
		sonarISR(SONAR3);
		break;

	default:
		break;
}
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_GPIO_EXTI_Callback could be implemented in the user file
   */
}
