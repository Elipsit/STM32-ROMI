/*
 *
 *
 *
 *
 */

#include "PID.h"
/*
float PID_update (float target, float current, PID *PID_state){
	float error = target - current;
	float I = PID_state -> I + error;
	float duty = pid_state -> kp * error + pid_state -> ki * I;

	if(duty>1.0){
		duty = 1.0;
	}

	if(duty<-1.0){
		duty = -1.0;
	}

	pid_state -> I = I;
	pid_state -> error = error;
	return duty;

}
*/