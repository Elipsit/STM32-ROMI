/*
 * encoder.h
 *
 *  Created on: Sep 16, 2020
 *      Author: Kyle Rodrigues
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include <stdint.h> //standard intriguer
#include "tim.h"

#define ENCODER_DIST_SCALE  (1.0/5456.902) // counts/m
#define ENCODER_VEL_SCALE 0.2617993878     // convert encoder velocity value to rad/sec

// encoder state variables
typedef struct ENCODER_STATE_t {
	float pos;     // cumulative position (m), signed relative to 0 when robot starts (wheel going forward increments, backwards decrements position)
	float vel;       // current wheel velocity rad/s.
} ENCODER_STATE;

//******Encoder Setup******
typedef struct ENC_STATUS_t {
	float pos;
	float vel;
	int16_t last;
	const char *tag;
	int32_t dir;
	TIM_HandleTypeDef *htim;
	ENCODER_STATE state;

} ENC_STATUS;


void updateEncoder(ENC_STATUS *enc); //pointer to this encoder status, use the type name = ENC_STATUS then point to *enc

extern ENC_STATUS enc_left;
extern ENC_STATUS enc_right;

#endif /* INC_ENCODER_H_ */