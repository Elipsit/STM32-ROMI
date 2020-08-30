/** ****************************************************************************
//
//  @file  app_main.cpp
//
//  Project:  STM32-Romi.
//
//  @brief   handle main application loop
//
//  Created: Kyle Rodrigues
//
//Change Notes:
 * 8/29/2020
 * added Sonar left and right
 * added header and main files for oled screen
//
// *****************************************************************************
*/
#include "app_main.h"
#include "main.h"
#include "stm32f4xx_hal_conf.h"
#include "stm32f4xx_it.h"

#include <string.h>
#include <stdio.h>


//******Define Sonar******
char *user_data = "*****STM32 HC-SR04\r\n*****";
#define uSTIM TIM3
char uartBuff[100];
//Sonar ticks set to zero
uint32_t numTicks = 0;

//Speed of sound in air cm/uSec
const float SpeedOfSound = 0.0343/2; //divided by 2 since its the speed to reach the object and come back
float distanceL, distanceR;


// local functions
void uSec_Delay(uint32_t uSec);
void checksonar(uint32_t numTicks, float distanceL, float distanceR);
void updateUART ();


// main application loop
void appMain(void){



	//Main program to loop forever
	while(1){
		checksonar(numTicks);
		updateUART();

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

void checksonar(uint32_t numTicks, float distanceL, float distanceR){
//Left Sonar
	//Set the trigger pin low for a few uSec
		  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_RESET);
		  uSec_Delay(3);

		  ///Start the ultrasonic measurement routine
		  //1. output 10us pulse on trigger pin
		  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_SET);
		  uSec_Delay(10);
		  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_RESET);

		  //2. Wait for echo pin rising edge
		  while(HAL_GPIO_ReadPin(ECHOL_GPIO_Port, ECHOL_Pin) == GPIO_PIN_RESET);

		  //3. Start measuring echo pause width until the falling edge
		  numTicks = 0;
		  while(HAL_GPIO_ReadPin(ECHOL_GPIO_Port, ECHOL_Pin) == GPIO_PIN_SET)
		  {
			  numTicks++;
			  uSec_Delay(2);  //actually around 2.8 uSec, measures every 2.8us
		  }
		//4. Estimate distance. 0.0f type casts as a float, multiply by actuall delay 2.8uS
		distanceL = (numTicks + 0.0f)*2.8*SpeedOfSound;

//Right Sonar
			//Set the trigger pin low for a few uSec
				  HAL_GPIO_WritePin(TRIGR_GPIO_Port, TRIGR_Pin, GPIO_PIN_RESET);
				  uSec_Delay(3);

				  ///Start the ultrasonic measurement routine
				  //1. output 10us pulse on trigger pin
				  HAL_GPIO_WritePin(TRIGR_GPIO_Port, TRIGR_Pin, GPIO_PIN_SET);
				  uSec_Delay(10);
				  HAL_GPIO_WritePin(TRIGR_GPIO_Port, TRIGR_Pin, GPIO_PIN_RESET);

				  //2. Wait for echo pin rising edge
				  while(HAL_GPIO_ReadPin(ECHOR_GPIO_Port, ECHOR_Pin) == GPIO_PIN_RESET);

				  //3. Start measuring echo pause width until the falling edge
				  numTicks = 0;
				  while(HAL_GPIO_ReadPin(ECHOR_GPIO_Port, ECHOR_Pin) == GPIO_PIN_SET)
				  {
					  numTicks++;
					  uSec_Delay(2);  //actually around 2.8 uSec, measures every 2.8us
				  }
				//4. Estimate distance. 0.0f type casts as a float, multiply by actuall delay 2.8uS
				distanceR = (numTicks + 0.0f)*2.8*SpeedOfSound;

return distanceL, distanceR;
}

void updateUART (){
	//This will be to update the uart
	//5. Print to UART Terminal, convert to string
	sprintf(uartBuff,"Left Distance (cm) = %.1f\r\n",distanceL);
	HAL_UART_Transmit(&huart2, (uint8_t *)uartBuff, strlen(uartBuff), 100);
	HAL_Delay(500);

	sprintf(uartBuff,"Right Distance (cm) = %.1f\r\n",distanceR);
	HAL_UART_Transmit(&huart2, (uint8_t *)uartBuff, strlen(uartBuff), 100);
	HAL_Delay(500);

	return;
}