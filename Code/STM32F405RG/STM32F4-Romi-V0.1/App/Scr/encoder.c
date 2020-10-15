/*
 * encoder.c
 *
 *  Created on: Sep 16, 2020
 *      Author: Kyle R
 */

#include "encoder.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> //this is required to use bool statements
//OLED Includes
#include "fonts.h"
#include "ssd1306.h"

char position[10]; // used to write a int to char
uint8_t oddeven = 0; //used to flip left and right oled screen location
//static bool oddeven = 0;
//Update Encoder
void updateEncoder(ENC_STATUS *enc){

	int16_t pos16 = (int16_t) __HAL_TIM_GET_COUNTER(enc->htim); //treat timers as a signed 16bit
	int32_t pos32 = (int32_t)pos16; //sign extended to 32bit

	int16_t last = enc -> last; //get last raw timer value
	int32_t diff = pos32-last; // change in position (velocity)

	if (abs(last) > 20000) { // is timer likely to have over/underflowed (sign changes near 0 are ok)
		if(pos16 < 0 && last >= 0) { // overflow forward
			diff += (int32_t)0x10000;
		}
		else if(pos16 >= 0 && last < 0) { // underflow backwards
			diff -= (int32_t)0x10000;
		}
	}

	enc->vel = diff*ENCODER_VEL_SCALE;
	enc->pos += diff*ENCODER_VEL_SCALE;
	enc->last = pos16;


	if(oddeven < 1){
		SSD1306_GotoXY(75, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(75, 30);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		sprintf(position, "%ld",pos32); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven++;

	}else if(2 > oddeven >= 1) {
		SSD1306_GotoXY(10, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(10, 30);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		sprintf(position, "%ld",pos32); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven = 0;
	}else{
		oddeven = 0;
	}


}
