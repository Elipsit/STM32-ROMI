/*
 * encoder.h
 *
 *  Created on: Sep 16, 2020
 *      Author: Kyle.R
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include <stdint.h> //standard interager
#include "tim.h"

#define ENCODER_DIST_SCALE  (1.0/5456.740906) // counts/m
#define ENCODER_VEL_SCALE 0.2617993878     // convert encoder velocity value to rad/sec

//******Encoder Setup******
typedef struct ENC_STATUS_t {
	float pos;
	float vel;
	int16_t last;
	const char *tag;
	int32_t dir;
	TIM_HandleTypeDef *htim;
} ENC_STATUS;

void updateEncoder(ENC_STATUS *enc); //pointer to this encoder status, use the type name = ENC_STATUS then point to *enc

#endif /* INC_ENCODER_H_ */
