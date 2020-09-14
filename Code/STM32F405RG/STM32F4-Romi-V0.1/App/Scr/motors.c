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


#include "motors.h"