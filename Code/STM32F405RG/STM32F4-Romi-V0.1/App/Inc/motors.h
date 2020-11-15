/*
 * motors.h
 *
 *  Created on: Sep 11, 2020
 *      Author: Kyle Rodrigues
 */

#ifndef INC_MOTORS_H_
#define INC_MOTORS_H_

#include <stdint.h> //standard intriguer
#include "app_main.h"
#include "tim.h"
#include "math.h"
#include <stdbool.h>

#define MAX_SPEED 10.0  //rad/s

//#define MAX_VELOCITY 50.0
#define SPEED_CHANGE 5.0

extern const float M_PI_F;

#define MAX_LIN_VEL 0.5f         //  m/s
#define MAX_ANG_VEL (2.0f*M_PI_F)  //  rad/s


//******Motor Setup****
typedef struct MOTOR_T {
	char *motor;
	uint32_t tim_ch;
	TIM_HandleTypeDef *htim;
	GPIO_TypeDef* gpio_port;
	uint32_t gpio_pin;

}MOTOR_CONF;

typedef enum MotorEvents_t {
    ME_NONE=0,
	ME_STOP=1,
	ME_DONE_TURN=2,
	ME_DONE_DRIVE=4,
	ME_BUMP_LEFT=8,
	ME_BUMP_RIGHT=16,

	CE_M1=32,
	CE_M2=64,
	CE_M3=128

} MotorEvent;


//Functions
void setMTRSpeed(float speed, const MOTOR_CONF *motor);

void STOP(void);

void drive(float lin_vel, float ang_vel);

void turnTo(float angle, float ang_vel);
void driveTo(float dist, float lin_vel);

MotorEvent updateMotors(bool pid_update, float DT);


#endif /* INC_MOTORS_H_ */
