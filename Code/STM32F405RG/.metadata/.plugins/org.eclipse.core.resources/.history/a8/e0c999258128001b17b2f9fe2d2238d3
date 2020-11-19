/*
 * PID.h
 * By: Kyle Rodrigues
 *
 */
#ifndef INC_PID_H_
#define INC_PID_H_


#include <stdbool.h>

// Define PID (PI) state variables
typedef	struct PID_STATE_t {
	float error; // last error value
	float I;     // running integral of error

	float ref;  // last setpoint
	float fb;   // last feed back
	float u;    // last output
}PID_STATE;

// Define PID Configuration including state
typedef struct PID_t {
	float kp; // Proportional tuning constant
	float ki; // Integral tuning constant

	float dt;    // time interval for updates

	bool openLoop; // set true PID update is open loop pass through mode
	const char * tag; // tag label for debug messages

	PID_STATE state; // current state of the controller

}PID;

float PID_update (float target, float current, PID *pid);

extern PID pid_right;
extern PID pid_left;


#endif /* INC_PID_H_ */
