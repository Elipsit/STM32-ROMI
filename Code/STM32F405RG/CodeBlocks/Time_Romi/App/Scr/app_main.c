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
**
  * @brief  Gets RTC current time.
  * @param  hrtc pointer to a RTC_HandleTypeDef structure that contains
  *                the configuration information for RTC.
  * @param  sTime Pointer to Time structure
  * @param  Format Specifies the format of the entered parameters.
  *          This parameter can be one of the following values:
  *            @arg RTC_FORMAT_BIN: Binary data format
  *            @arg RTC_FORMAT_BCD: BCD data format
  * @note  You can use SubSeconds and SecondFraction (sTime structure fields returned) to convert SubSeconds
  *        value in second fraction ratio with time unit following generic formula:
  *        Second fraction ratio * time_unit= [(SecondFraction-SubSeconds)/(SecondFraction+1)] * time_unit
  *        This conversion can be performed only if no shift operation is pending (ie. SHFP=0) when PREDIV_S >= SS
  * @note You must call HAL_RTC_GetDate() after HAL_RTC_GetTime() to unlock the values
  *        in the higher-order calendar shadow registers to ensure consistency between the time and date values.
  *        Reading RTC current time locks the values in calendar shadow registers until current date is read.
  * @retval HAL status
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
#include "rtc.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"
#include <stm32f4xx_hal_rtc.h>


//******Custom Files******

#include "app_main.h"

//OLED Includes
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"

//******timers******
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim12;
extern TIM_HandleTypeDef htim9; //Sonar 1mS Timer
extern RTC_TimeTypeDef sTime;
extern RTC_DateTypeDef sDate;


char updatescr[10]; //use to update screen
void updateDisplay (void);
char* getDayofweek(uint8_t number);

// Hardware Revision bits
uint8_t RevBit[3];


// main application loop
void appMain(void){


	/* Check Hardware Revision Bits*/
	RevBit[0] =	HAL_GPIO_ReadPin(REV_BIT0_GPIO_Port, REV_BIT0_Pin);
	RevBit[1] =	HAL_GPIO_ReadPin(REV_BIT1_GPIO_Port, REV_BIT1_Pin);
	RevBit[2] =	HAL_GPIO_ReadPin(REV_BIT2_GPIO_Port, REV_BIT2_Pin);

	printf("RTC Test\r\n");
	printf("Hardware Revision: %d%d%d\r\n",RevBit[2],RevBit[2],RevBit[2]);

	//Initialize OLED
	SSD1306_Init();
	SSD1306_Clear();
	SSD1306_DrawBitmap(0, 0, ologic, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(2000);
	SSD1306_Clear();
	SSD1306_GotoXY(30, 0);
	SSD1306_Puts("RTC Test", &Font_7x10, 1);
	SSD1306_UpdateScreen();


	//Main program to loop forever
	while(1){

		HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

		printf("Current Time is : %02d:%02d:%02d\r\n",sTime.Hours,\
				sTime.Minutes,sTime.Seconds);
		printf("Current Date is : %02d-%2d-%2d  <%s> \r\n",sDate.Month,sDate.Date,\
				sDate.Year,getDayofweek(sDate.WeekDay));
		//Update display with time
		updateDisplay();
		HAL_Delay(1000);

			} //end of while loop



} //end of main loop

void updateDisplay (void){

	//Time
	sprintf(updatescr, "%d",sTime.Hours); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(30, 20);
	SSD1306_Puts(updatescr, &Font_7x10, 1);

	SSD1306_GotoXY(40, 20);
	SSD1306_Puts(":", &Font_7x10, 1);

	sprintf(updatescr, "%d",sTime.Minutes); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(45, 20);
	SSD1306_Puts(updatescr, &Font_7x10, 1);

	SSD1306_GotoXY(60, 20);
	SSD1306_Puts(":", &Font_7x10, 1);

	sprintf(updatescr, "%d",sTime.Seconds); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(65, 20);
	SSD1306_Puts(updatescr, &Font_7x10, 1);
	SSD1306_UpdateScreen();

	//Date
	sprintf(updatescr, "%d",sDate.Month); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(30, 40);
	SSD1306_Puts(updatescr, &Font_7x10, 1);

	SSD1306_GotoXY(45, 40);
	SSD1306_Puts("/", &Font_7x10, 1);

	sprintf(updatescr, "%d",sDate.Date); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(50, 40);
	SSD1306_Puts(updatescr, &Font_7x10, 1);

	SSD1306_GotoXY(65, 40);
	SSD1306_Puts("/", &Font_7x10, 1);

	sprintf(updatescr, "%d",sDate.Year); //this is used to convert to the char array position[10]
	SSD1306_GotoXY(70, 40);
	SSD1306_Puts(updatescr, &Font_7x10, 1);
	SSD1306_UpdateScreen();
}


char* getDayofweek(uint8_t number)
{
	char *weekday[] = { "Monday", "TuesDay", "Wednesday","Thursday","Friday","Saturday","Sunday"};

	return weekday[number-1];
}
