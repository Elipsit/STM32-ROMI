/*
 *
 *
 *
 *
 */
#include <stdio.h>
#include "PID.h"

float PID_update (float target, float current, PID *PID_state){

	float error = target - current; //compute error here

	float I = PID_state->I+ error*PID_state->dt; //compute integral

	//reset intergral when stopped
	if(target==0.0 && current==0.0) {
	    	I=0;
	    }

	//compute output as Kp * error + Ki * dT*Integral(error)
	float duty = PID_state->kp*error + PID_state -> ki * I;

	if(duty>1.0){
		duty = 1.0;
	}

	if(duty<-1.0){
		duty = -1.0;
	}

	PID_state->error = error;
	PID_state->I = I;

	printf("PID %s \t target %5.2f\t current %5.2f\t Duty = %f\n\n\r", PID_state->tag, target, current, duty);
	return duty;

}

