/*
 * sonar.c
 *
 *  Created on: Sep 19, 2020
 *      Author: livin
 */
#include "sonar.h"
#include "main.h"
#include "tim.h"

const float SpeedOfSound = 0.0343/2; //divided by 2 since its the speed to reach the object and come back

void checkSonar(SONAR_STATUS *sonar){
	uint32_t tock = 0;
	sonar->tick = ___HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET); //Set the Trigger pin low
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,SET);//keep high for 10uS
	while(tock-sonar->tick <= 10){
		tock = ___HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register
	}
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET); //Set the Trigger pin low
	sonar->tick = ___HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register
}

void updateSonar(SONAR_STATUS *sonar){
	//4. Estimate distance. 0.0f type casts as a float, multiply by actual delay 2.8uS
				sonar->distance = (sonar->tick + 0.0f)*2.8*SpeedOfSound;
				printf("%C Sonar Distance (cm): %f",sonar->sonar_ch,sonar->distance);

}

/*
void uSec_Delay(uint32_t uSec)
{
	if(uSec < 2)uSec = 2;
	uSTIM->ARR = uSec - 1; 	//Sets the value in the auto reload register
	uSTIM -> EGR = 1;		//Re-initialize the Timer
	uSTIM -> SR &= ~1;  	//Resets the flag
	uSTIM ->CR1 |= 1;		//Enables the counter
	while((uSTIM -> SR&0x0001) != 1);
	uSTIM -> SR &= ~(0x0001);

}*/


