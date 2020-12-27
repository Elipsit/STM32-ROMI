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
 * esp link http://192.168.4.1 | 	192.168.50.85
 *
 * To Do:
 * Work on PID Loop
 * Confirm PWM channel timer is 16bit or 32 bit
 * confirm PID Period and Duty is correct
 *
// remove all uneccessary code to check speed and see if delays are errors
// *****************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h> //this is required to use bool statements
#include "math.h"
#include "main.h"
#include "can.h"
#include "dac.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"


//******Custom Files******

#include "app_main.h"
#include "sonar.h"



//OLED Includes
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"

//******timers******
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim9; //Sonar 1mS Timer

SONAR_STATUS SONARS[] ={{"Left",TRIGL_GPIO_Port,TRIGL_Pin,ECHOL_GPIO_Port,ECHOL_Pin,0,0,0.0},
						{"Right",TRIGR_GPIO_Port,TRIGR_Pin,ECHOR_GPIO_Port,ECHOR_Pin,0,0,0.0},
						{"Center",TRIG_CTR_GPIO_Port,TRIG_CTR_Pin,ECHO_CTR_GPIO_Port,ECHO_CTR_Pin,0,0,0.0}};


char updatescr[10]; //use to update screen

// Hardware Revision bits
uint8_t RevBit[3];


// main application loop
void appMain(void){


	/* Check Hardware Revision Bits*/
	RevBit[0] =	HAL_GPIO_ReadPin(REV_BIT0_GPIO_Port, REV_BIT0_Pin);
	RevBit[1] =	HAL_GPIO_ReadPin(REV_BIT1_GPIO_Port, REV_BIT1_Pin);
	RevBit[2] =	HAL_GPIO_ReadPin(REV_BIT2_GPIO_Port, REV_BIT2_Pin);


	printf("Sonar Test\r\n");
	printf("Hardware Revision: %d%d%d\r\n",RevBit[2],RevBit[2],RevBit[2]);

	//Initialize OLED
	SSD1306_Init();
	SSD1306_Clear();
	SSD1306_DrawBitmap(0, 0, ologic, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(2000);
	SSD1306_Clear();
	SSD1306_GotoXY(30, 0);
	SSD1306_Puts("Sonar Test", &Font_7x10, 1);
	SSD1306_UpdateScreen();


	//Main program to loop forever
	while(1){
		printf("Check Sonars\r\n");
		//Check the sonars
		checkSonar(&SONARS[SONAR1]);
		checkSonar(&SONARS[SONAR2]);

		HAL_Delay(1000);
			} //end of while loop



} //end of main loop

