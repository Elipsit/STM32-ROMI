/*
 * motors.c
 *
 *  Created on: Sep 11, 2020
 *      Author: Kyle Rodrigues
 *
 *     | DIR | PWM | /SLEEP | MOTOR+ | MOTOR- |OPERATION
 *     |  0  | PWM |    1   |  PWM   |   L    | Forward/break at speed of %PWM
 *     |  1  | PWM |    1   |   L    |  PWM   | Reverse/break at speed of %PWM
 *     |  X  |  0  |    1   |   L    |   L    |brake low - output shorted to gnd
 *     |  X  |  X  |    0   |   Z    |   Z    |Coast (outputs floating
 *
 *TIM2 = PWMR
 *TIM4 = PWML
 *
 *PWM Frequency 250kHZ
 *
 *encoder count 1440
 *gear ratio 120:1
 *Max RPM 150, PRS =
 *
 */

//Includes
#include <stdio.h>
#include "motors.h"
#include "main.h"
#include "app_main.h"
#include "PID.h"
#include "encoder.h"
#include "sonar.h"
#include "edge_sensor.h"

#define WHEEL_BASE   0.087f
#define WHEEL_RADIUS (0.070f/2.0f)

const float M_PI_F = (3.141592653589793f);
const float M_2PI_F = (2.0f*3.141592653589793f);

void setMTRSpeed(float speed, const MOTOR_CONF *motor);

static void updatePose(float DT);

static float speed_l=0.0f; // desired left wheel speed (rad/sec)
static float speed_r=0.0f; // desired right wheel speed (rad/sec)

static float target_dist_2=0.0f;
static float target_heading=0.0f;

static float start_pose_x=0.0f;
static float start_pose_y=0.0f;
static float start_heading=0.0f;

float pose_x=0.0f;
float pose_y=0.0f;
float heading=0.0f;

static bool driving=false;
static bool turn_ccw=false;

const MOTOR_CONF mot_left = {"Left",TIM_CHANNEL_1, &htim4, ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin}; //add a status bit?
const MOTOR_CONF mot_right = {"Right",TIM_CHANNEL_3, &htim2, ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin};


void setMTRSpeed(float speed, const MOTOR_CONF *motor){
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, SET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, SET);

	uint32_t direction = speed > 0?0:1; //if assignment, ternary operator
	speed = abs(speed); //takes speed and returns absolute value
	HAL_GPIO_WritePin(motor->gpio_port, motor->gpio_pin, direction==1 ?SET:RESET);

	if(speed > MOTOR_PWM_PERIOD){
		speed = MOTOR_PWM_PERIOD;
	}
	__HAL_TIM_SET_COMPARE(motor->htim,motor->tim_ch,speed); //sets capture/compare register for the the duty; how fast the
}

void setMotorSpeed(float left, float right) {
	speed_l = left;
	speed_r = right;
}

void drive(float lin_vel, float ang_vel) {
	speed_l =  (lin_vel - ang_vel * WHEEL_BASE/2.0f)/WHEEL_RADIUS;
	speed_r =  (lin_vel + ang_vel * WHEEL_BASE/2.0f)/WHEEL_RADIUS;
}

void STOP(void){
	printf("Stop Detected\n\r");
	speed_l = 0.0;
	speed_r = 0.0;
	setMTRSpeed(0.0f,&mot_right);
	setMTRSpeed(0.0f,&mot_left);
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, RESET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, RESET);
	driving = false;
}

MotorEvent updateMotors(bool pid_update, float DT) {

	MotorEvent event = ME_NONE;

	if(pid_update) {
		float duty_l=0.0f; // left wheel output duty cycle  (-1.0 -- 1.0)
		float duty_r=0.0f; // right wheel output duty cycle (-1.0 -- 1.0)

		// get latest speed and position estimates from encoders
		updateEncoder(&enc_right);
		updateEncoder(&enc_left);

		// run PID for speed control
		duty_l = PID_update(speed_l,enc_left.vel,&pid_left);
		duty_r = PID_update(speed_r,enc_right.vel,&pid_right);

		// set output PWM duty for both motors
		setMTRSpeed(duty_r*MOTOR_PWM_PERIOD,&mot_right);
		setMTRSpeed(duty_l*MOTOR_PWM_PERIOD,&mot_left);

		updatePose(DT);

		float ref_heading = heading;

		if(driving && (target_heading != 0.0f)) {


			if (((ref_heading < 0.0f) && (start_heading >=0.0f)) || ((ref_heading >= 0.0f) && (start_heading < 0.0f))) {
				if(ref_heading < 0.0f ) {
					if(turn_ccw) {
					   ref_heading += M_2PI_F;
					}
				}
				else {
					if(!turn_ccw) {
					   ref_heading -= M_2PI_F;
					}
				}
			}

			if(fabsf(ref_heading-start_heading) >= target_heading) {
				STOP();
				event = ME_DONE_TURN;
			}
		}

		if(driving && (target_dist_2 != 0.0f)) {
			if ((pose_x-start_pose_x*pose_x-start_pose_x+pose_y-start_pose_y*pose_x-start_pose_y) >= target_dist_2) {
				STOP();
				event = ME_DONE_DRIVE;

			}
		}
		if(driving) {
			//printf("sh=%5.2f, th=%5.2f, h=%5.2f, rh=%5.2f\n",start_heading,target_heading,heading,ref_heading);
		}

	}

	bool leftClif = getEdgeSensorState(BUMP_BIT_LEFT)==ES_HIT;
	bool rightClif = getEdgeSensorState(BUMP_BIT_RIGHT)==ES_HIT;

	if(leftClif || rightClif){
		event = leftClif?ME_BUMP_LEFT:ME_BUMP_RIGHT;
	}

	return event;
}

void turnTo(float angle, float ang_vel) {

	start_heading = heading;
	target_heading = fabsf(angle);

	target_dist_2=0.0f;
	driving=true;

	if(angle<0.0f) {
		drive(0.0f,-fabsf(ang_vel));
	}
	else {
		drive(0.0f,fabsf(ang_vel));
	}
}


void driveTo(float dist, float lin_vel) {

	start_pose_x = pose_x;
	start_pose_y = pose_y;

	target_dist_2 = dist*dist; // use squared distance to save abs and sqrt
	target_heading= 0.0f;

	driving=true;

	if(dist < 0.0f) {
	   turn_ccw=true;
	   drive(-lin_vel,0.0f);

	}
	else {
	   turn_ccw=false;
	   drive(lin_vel,0.0f);
	}
}

void updatePose(float DT) {

	float dl = enc_left.state.vel*DT*WHEEL_RADIUS;
	float dr = enc_right.state.vel*DT*WHEEL_RADIUS;

	float d = (dl+dr)/2.0f;
	float dt = (dr-dl)/WHEEL_BASE;

    heading += dt;


	if(heading > M_PI_F) {
		heading -= M_2PI_F;
	}
	else if(heading <= -M_PI_F) {
		heading += M_2PI_F;
	}

    pose_x += d * cosf(heading);
	pose_y += d * sinf(heading);

}
