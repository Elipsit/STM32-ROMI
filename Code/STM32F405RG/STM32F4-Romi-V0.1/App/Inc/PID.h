/*
 * PID.h
 * By: Kyle Rodrigues
 *
 */
#ifndef INC_PID_H
#define INC_PID_H_

#include "app_main.h"

// The strut is named PID_t, the typedef is PID
typedef struct PID_t{
	float kp; //proportional error
	float ki; //intergral error
	float error; //error that is  tracking
	float I; //running intergral of error
	char *tag;

}PID;

float PID_update (float target, float current, PID *PID_state);




#endif /*INC_PID_H_ */
