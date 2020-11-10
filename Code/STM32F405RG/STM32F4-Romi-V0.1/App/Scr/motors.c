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

/* Variables
#include <stdio.h>
#include "motors.h"
#include "main.h"
#include "app_main.h"
#include "PID.h"
#include "encoder.h"
#include "sonar.h"
#include "edge_sensor.h"


//Functions
static void setMTRSpeed(float speed, const MOTOR_CONF *motor);
void STOP(void);



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


void STOP(void){
	printf("Stop Detected\n\r");
	speed_l = 0.0;
	speed_r = 0.0;
	setMTRSpeed(0.0,&mot_right);
	setMTRSpeed(0.0,&mot_left);
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, RESET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, RESET);
	//driving = false;
}*/
