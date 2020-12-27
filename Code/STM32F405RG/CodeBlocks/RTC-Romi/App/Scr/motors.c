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

//// define robot geometry to calculate kinematics
#define WHEEL_BASE   0.087f // robot wheel base (distance between the wheels) m
#define WHEEL_RADIUS (0.070f/2.0f) // radius of the wheels (m)

// define PI and 2*PI as floats
const float M_PI_F = (3.141592653589793f);
const float M_2PI_F = (2.0f*3.141592653589793f);

// local prototypes
static void setMTRSpeed(float speed, const MOTOR_CONF *motor);
static void updatePose(float DT);

static float speed_l=0.0f; // desired left wheel speed (rad/sec)
static float speed_r=0.0f; // desired right wheel speed (rad/sec)

static float target_dist_2=0.0f;  // how far to drive (squared) when running a driveTo command - in meters
static float target_heading=0.0f; // how far to turn when running a turnTo command - in rad

// reference starting pose of robot when beginning a turnTo or driveTo command
static float start_pose_x=0.0f;
static float start_pose_y=0.0f;
static float start_heading=0.0f;

// current robot pose
float pose_x=0.0f;
float pose_y=0.0f;
float heading=0.0f;

// flags to control the driveTo and turnTo commands
static bool driving=false; // true if currently performing a driveTo or turnTo command
static bool turn_ccw=false; // set direction of turn in a turnTo, true = counter clock wise (to the left)

const MOTOR_CONF mot_left = {"Left",TIM_CHANNEL_1, &htim4, ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin}; //add a status bit?
const MOTOR_CONF mot_right = {"Right",TIM_CHANNEL_3, &htim2, ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin};

// Set PWM output for a motor for desired power
void setMTRSpeed(float speed, const MOTOR_CONF *motor){
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, SET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, SET);

	uint32_t direction = speed > 0?0:1; //if assignment, ternary operator
	speed = fabsf(speed); //takes speed and returns absolute value
	HAL_GPIO_WritePin(motor->gpio_port, motor->gpio_pin, direction==1 ?SET:RESET);

	if(speed > MOTOR_PWM_PERIOD){
		speed = MOTOR_PWM_PERIOD;
	}
	__HAL_TIM_SET_COMPARE(motor->htim,motor->tim_ch,(uint32_t)speed); //sets capture/compare register for the the duty; how fast the
}

// set target velocity for each wheel (in rad/s)
void setMotorSpeed(float left, float right) {
	speed_l = left;
	speed_r = right;
}
// set target velocities for each wheel based on desired robot dynamics
// lin_vel : desired linear velocity of robot center (m/s)
// ang_vel : desired angular velocity of robot (rad/s)
void drive(float lin_vel, float ang_vel) {
	speed_l =  (lin_vel - ang_vel * WHEEL_BASE/2.0f)/WHEEL_RADIUS;
	speed_r =  (lin_vel + ang_vel * WHEEL_BASE/2.0f)/WHEEL_RADIUS;
}

// stop both motors and cancel any driveTo or turnTo command that is executing
void STOP(void){
	printf("Stop Detected\n\r");
	speed_l = 0.0;
	speed_r = 0.0;
	setMTRSpeed(0.0f,&mot_right);
	setMTRSpeed(0.0f,&mot_left);
	//HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, RESET);
	//HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, RESET);

	// Cancel driving commands
	driving = false;
}

// update the motor controller and robot driving status
// if pid_update is true will update the PID controller also (and associated state like the encoders and output the speeds to the wheels)
// also updates the internal robot pose
// DT is the update period (sec) used for the inverse kinematics to update the internal pose estimate
//
// Returns any events that are triggered like end of driveTo or turnTo command or is a bump sensor is detected
//
// If at any time the motors are driving and an enabled bumb sensor detects a hit both motors are immediately stopped.
MotorEvent updateMotors(bool pid_update, float DT) {

	MotorEvent event = ME_NONE;

	if(pid_update) {// see if we should update the PID this time through
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

		updatePose(DT); // calculate updated pose

		// now test if we have completed a turn to or driveTo command (if one is running)
		float ref_heading = heading; // get current heading

		if(driving && (target_heading != 0.0f)) {  // if doing a turnTo command

			  // see if we will turn through 0 heading and handle wrap around of angles if needed
			if (((ref_heading < 0.0f) && (start_heading >=0.0f)) || ((ref_heading >= 0.0f) && (start_heading < 0.0f))) {

				// handle wrapping around target from + to - angles
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

			// now see if we have turned far enough
			if(fabsf(ref_heading-start_heading) >= target_heading) {
				STOP();
				event = ME_DONE_TURN;
			}
		}

		// check if doing a driveTo command and stop if we have gone far enough
		if(driving && (target_dist_2 != 0.0f)) {

			// calculate squared magnitude of distance we have moved
			float dx=pose_x-start_pose_x;
			float dy=pose_y-start_pose_y;

			if (( dx*dx+dy*dy) >= target_dist_2) { // compare to square magnatude of target distance
				STOP(); // got htere so stop
				event = ME_DONE_DRIVE; // return done event
			}
		}

		if(driving) {
			//printf("sh=%5.2f, th=%5.2f, h=%5.2f, rh=%5.2f\n",start_heading,target_heading,heading,ref_heading);
		}

	}
	// check if either bumper has a hit (if enabled)
	bool leftClif = getEdgeSensorState(BUMP_BIT_LEFT)==ES_HIT;
	bool rightClif = getEdgeSensorState(BUMP_BIT_RIGHT)==ES_HIT;

	if(leftClif || rightClif) {
		STOP(); // stop if bumper hit
		event = leftClif?ME_BUMP_LEFT:ME_BUMP_RIGHT; // return event that bumper is hit
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