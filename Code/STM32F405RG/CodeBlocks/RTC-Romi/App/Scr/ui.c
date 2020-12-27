/*
 * ui.c
 *
 *  Created on: Nov 14, 2020
 *      Author: Kyle R
 *
 *      This file sets the commands to control the robot over serial
 */

#include <stdio.h>
#include  <stdbool.h>

//******Custom Files******
#include "app_main.h"
#include "PID.h"
#include "encoder.h"
#include "sonar.h"
#include "edge_sensor.h"
#include "motors.h"

typedef struct telemetry_t {
	PID_STATE pid_left;
	PID_STATE pid_right;

  ENCODER_STATE enc_left;
  ENCODER_STATE enc_right;

  float ir_range_short;
  float ir_range_long;

  uint32_t clifs;

} TELEMETRY;

static TELEMETRY telemetry;


static float randf(void);

//#define SPEED_CHANGE 0.1f // input speed step for manual wheel speed commands


// send the current telemetry info out the UART (data encoded in a slip packet)
//void sendTelemetry(void) {
//	slipEncode((uint8_t*)&telemetry,sizeof(telemetry));
//}


// Update the current telemetry encoder state
void setEncoderState(ENCODER_STATE * enc_left, ENCODER_STATE * enc_right) {
	telemetry.enc_left = *enc_left;
	telemetry.enc_right = *enc_right;
}

// Update the current telemetry PID state
void setPIDState(PID_STATE * pid_left, PID_STATE * pid_right) {
	telemetry.pid_left = *pid_left;
	telemetry.pid_right = *pid_right;
}

// Update the current telemetry motor state info
void setMotorState(void) {
   // TBD
}

// Update the current telemetry controller state info
void setControlerState(void) {
   // TBD
}