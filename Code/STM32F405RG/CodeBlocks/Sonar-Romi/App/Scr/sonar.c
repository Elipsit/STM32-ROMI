/*
 * sonar.c
 *
 *  Created on: December 26th, 2020
 *      Author: Kyle Rodrigues
 *
 *      To Do: fix timer scale factor
 */
#include "sonar.h"
#include "main.h"
#include "tim.h"

#define SR04_MAX_READING 0.05f  // 50ms
#define PING_MAX_READING 0.018f  // 18ms

uint8_t icFlag = 0;
uint8_t captureIdx=0;
uint32_t edge1Time=0, edge2Time=0;

const float speedOfSound = 0.0171821; //cm/uSec divided by 2 since its the speed to reach the object and come back


void uSec_Delay(uint32_t uSec);

void checkSonar(SONAR_STATUS *sonar){
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET); //Set the Trigger pin low
	uSec_Delay(3);
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,SET);//keep high for 10uS
	uSec_Delay(10); /* This is a 10uS delay*/
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET);//Set to low again to start reading

	//2. ECHO signal pulse width
	//Start IC timer (switch case to pick correct timer)
		switch (sonar->trig_pin) {
			case TRIGL_Pin:
				HAL_TIM_IC_Start_IT(&htim12, TIM_CHANNEL_2);
				break;

			case TRIGR_Pin:
				HAL_TIM_IC_Start_IT(&htim1, TIM_CHANNEL_2);
				break;

			default:
				break;
		}

		//HAL_TIM_IC_Start_IT(&htim12, TIM_CHANNEL_2);

		//Wait for IC flag
		uint32_t startTick = HAL_GetTick();
		do{
			if(icFlag) break;
		}while((HAL_GetTick() - startTick) < 500);  // timeout of 500ms
		icFlag = 0;

		switch (sonar->trig_pin) {
			case TRIGL_Pin:
				HAL_TIM_IC_Stop_IT(&htim12, TIM_CHANNEL_2);
				break;

			case TRIGR_Pin:
				HAL_TIM_IC_Stop_IT(&htim1, TIM_CHANNEL_2);
				break;

			default:
				break;
		}

		//HAL_TIM_IC_Stop_IT(&htim12, TIM_CHANNEL_2);

		//Calculate distance in cm
		if(edge2Time > edge1Time){
			sonar->distance = ((edge2Time - edge1Time) + 0.0f)*speedOfSound;
		}else{
			sonar->distance = 0.0f;
		}


	//Print to UART terminal for debugging
	printf("%s Sonar Distance (cm): %f\n\n\r",sonar->sonar_ch,sonar->distance);
	printf("Edge Time 1: %ld \t Edge Time 2: %ld\n\n\r",edge1Time,edge2Time);

}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{

	HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin);

		if(captureIdx == 0) //First edge
		{
			edge1Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2); //__HAL_TIM_GetCounter(&htim3);//

			captureIdx = 1;
		}
		else if(captureIdx == 1) //Second edge
		{
			edge2Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
			captureIdx = 0;
			icFlag = 1;
		}
}


void uSec_Delay(uint32_t uSec)
{
	if(uSec < 2)uSec = 2;
	uSTIM->ARR = uSec - 1; 	//Sets the value in the auto reload register
	uSTIM -> EGR = 1;		//Re-initialize the Timer
	uSTIM -> SR &= ~1;  	//Resets the flag
	uSTIM ->CR1 |= 1;		//Enables the counter
	while((uSTIM -> SR&0x0001) != 1);
	uSTIM -> SR &= ~(0x0001);

}

