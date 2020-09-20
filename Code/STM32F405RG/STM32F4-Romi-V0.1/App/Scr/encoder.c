/*
 * encoder.c
 *
 *  Created on: Sep 16, 2020
 *      Author: Kyle R
 */

#include "encoder.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h> //this is required to use bool statements
//OLED Includes
#include "fonts.h"
#include "ssd1306.h"

char position[10]; // used to write a int to char
//uint8_t oddeven = 0; //used to flip left and right oled screen location
static bool oddeven = 0;
//Update Encoder
void updateEncoder(ENC_STATUS *enc){

	enc-> pos = __HAL_TIM_GET_COUNTER(enc->htim);

	//calculate the velocity
	enc-> vel = enc -> pos - enc -> last;

	//printf("encoder  %s: pos = %ld, vel = %ld, last = %ld\r\n\n",enc->tag, enc->pos, enc->vel, enc->last);

	//Save enc pause into inc last
	enc -> last  =  enc -> pos;

	if(oddeven < 1){
		SSD1306_GotoXY(50, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(50, 40);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven++;

	}else if(2 > oddeven >= 1) {
		SSD1306_GotoXY(0, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(0, 40);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven = 0;
	}else{
		oddeven = 0;
	}


}
