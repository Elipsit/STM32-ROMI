/** ****************************************************************************
//
//  @file  app_main.cpp
//
//  Project:  STM32-Romi.
//
//  @brief   handle main application loop
//
//  Created: Kyle Rodrigues
//
//Change Notes:
 * 8/29/2020
 * added Sonar left and right
 * added header and main files for oled screen
 * esp link http://192.168.50.85/console.html
 *
 * To Do:
 * Work on PID Loop
 * Confirm PWM channel timer is 16bit or 32 bit
 * confirm PID Period and Duty is correct
 *
//
// *****************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "can.h"
#include "dac.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_tim_ex.h"

//******Custom Files******
#include "app_main.h"
#include "PID.h"
#include "encoder.h"


//OLED Includes
#include "fonts.h"
#include "ssd1306.h"
#include "test.h"
#include "bitmap.h"

//******timers******
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

//******Define Sonar******
#define uSTIM TIM9
//Sonar ticks set to zero
uint32_t numTicks = 0;

//******PID******
#define FORWARD 0
#define REVERSE 1
#define KP 1.0
#define KI 0.0
//int32_t MTR_PWM_PERIOD = 100;
const int32_t MAX_SPEED = 100;
const int32_t MAX_VELOCITY = 360;
const int32_t SPEED_CHANGE = 5;

//char position[10]; // used to write a int to char
//uint8_t oddeven = 0; //used to flip left and right oled screen location
//*******Sonar Setup******
typedef struct SONAR_STATUS_t {
	float distanceL;
	float distanceR;
	float distanceC;
}SONAR_STATUS;

//declare the variable for sonar status
SONAR_STATUS sonar ={0.0,0.0,0.0};

//create array for sonar tirgger pins

//Speed of sound in air cm/uSec
const float SpeedOfSound = 0.0343/2; //divided by 2 since its the speed to reach the object and come back

//******PWM Setup****
typedef struct MOTOR_T {
	char *motor;
	uint32_t tim_ch;
	TIM_HandleTypeDef *htim;
	GPIO_TypeDef* gpio_port;
	uint32_t gpio_pin;

}MOTOR_CONF;

static const  MOTOR_CONF mot_left = {"Left",TIM_CHANNEL_1, &htim4, ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin};
static const MOTOR_CONF mot_right = {"Right",TIM_CHANNEL_3, &htim2, ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin};

// Hardware Revision bits
uint8_t RevBit[3];

// local functions
static void uSec_Delay(uint32_t uSec);
static void checksonar(SONAR_STATUS *sonar);
static void setPWM(TIM_HandleTypeDef, uint32_t, uint8_t, uint16_t, uint16_t);
static void setMTRSpeed(int16_t speed, const MOTOR_CONF *motor);

PID pid_right = {KP, KI, 0.0, 0.0};
PID pid_left = {KP, KI, 0.0, 0.0};


ENC_STATUS enc_right = {0,0,0,"Right", 0, &htim3};
ENC_STATUS enc_left = {0,0,0,"Left", 0, &htim5};


// main application loop
void appMain(void){


	/* Check Hardware Revision Bits*/
	RevBit[0] =	HAL_GPIO_ReadPin(REV_BIT0_GPIO_Port, REV_BIT0_Pin);
	RevBit[1] =	HAL_GPIO_ReadPin(REV_BIT1_GPIO_Port, REV_BIT1_Pin);
	RevBit[2] =	HAL_GPIO_ReadPin(REV_BIT2_GPIO_Port, REV_BIT2_Pin);


	//hal pwm start
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);  //Start PWM
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);  //Start PWM

	//hal encoder start
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

	int16_t speed_l = 25;
	int16_t speed_r = 25;

	float duty_l = 0;
	float duty_r = 0;

	uint32_t BLINK_RATE = 50;
	uint32_t BlinkTimer = BLINK_RATE;

	uint32_t PID_RATE = 10; //Changed to a #Define function at top
	uint32_t PIDTimer = PID_RATE;

	printf("Power up initiated...\r\n");
	printf("All systems nominal..\r\n");
	printf("Hardware Revision: %d%d%d\r\n",RevBit[2],RevBit[2],RevBit[2]);

	//Initialize OLED
	SSD1306_Init();
	SSD1306_Clear();
	SSD1306_DrawBitmap(0, 0, ologic, 128, 64, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(2000);
	SSD1306_Clear();
	SSD1306_GotoXY(30, 0);
	SSD1306_Puts("STM32-ROMI", &Font_7x10, 1);
	SSD1306_UpdateScreen();
	HAL_Delay(100);

	uint32_t tick = HAL_GetTick();


	//Set Direction bits to 0 for forward
	//HAL_GPIO_WritePin(ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin, SET);
	//HAL_GPIO_WritePin(ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin, SET);
	//Set Sleep bits to 1 for enable
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, SET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, SET);


	//Main program to loop forever
	while(1){
		//printf("Check Sonar\r\n");
		//checksonar(&sonar);
		//uint32_t tock = HAL_GetTick();

		/*
		if(tock-tick>10){
			BlinkTimer--;
			if(BlinkTimer==0){
				BlinkTimer = BLINK_RATE;
				HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin);
			}

		}*/

		/// use this to adjust the pwm

		int c = getchar();
			if(c != EOF){
				putchar(c);

					if(speed_l < MAX_SPEED){
						speed_l += SPEED_CHANGE;
					}

					if(c == '-'){
						if(speed_l > -MAX_SPEED){
							speed_l -= SPEED_CHANGE;
						}
					}

					if(c == '>'){
						if(speed_r < MAX_SPEED){
							speed_r += SPEED_CHANGE;
						}
					}
					if(c == '<'){
						if(speed_r > -MAX_SPEED){
							speed_r -= SPEED_CHANGE;
						}
					}
					if(c == ' '){
						speed_r = 0;
						speed_l = 0;
						}

				}else{
				clearerr(stdin); // Reset the EOF Condition
				}

			PIDTimer--;
			//printf("PIDTimer = %ld\r\n",PIDTimer);
			if(PIDTimer == 0){ //every 10 Hz
				PIDTimer = PID_RATE;

				/* Update the encoders*/
				updateEncoder(&enc_right);
				updateEncoder(&enc_left);

				duty_l = PID_update((float)speed_l/MAX_SPEED, (float)enc_left.vel/MAX_VELOCITY, &pid_left);
				duty_r = PID_update((float)speed_r/MAX_SPEED, (float)enc_right.vel/MAX_VELOCITY, &pid_right);


				/* Update the motors*/
				//setPWM(htim2, TIM_CHANNEL_3, 0, MTR_PWM_PERIOD, duty_r);
				//setPWM(htim4, TIM_CHANNEL_1, 0, MTR_PWM_PERIOD, duty_l);


				setMTRSpeed(duty_r*MOTOR_PWM_PERIOD,&mot_right);
				setMTRSpeed(duty_l*MOTOR_PWM_PERIOD,&mot_left);
				printf("Left Motor = %d\t Right Motor =%d\n\r",speed_l,speed_r);

			}



				//tick = tock;

			} //end of while loop



} //end of main loop


void uSec_Delay(uint32_t uSec)
{
	if(uSec < 2)uSec = 2;
	uSTIM->ARR = uSec - 1; 	//Sets the value in the auto reload register
	uSTIM -> EGR = 1;		//Re-initialize the Timer
	uSTIM -> SR &= ~1;  	//Resets the flag
	uSTIM ->CR1 |= 1;		//Enables the counter
	while((uSTIM -> SR&0x0001) != 1);
	uSTIM -> SR &= ~(0x0001);

}

void checksonar(SONAR_STATUS *sonar){
	//Left Sonar - Update wiith hardware timer to count the input pulses
		//Set the trigger pin low for a few uSec
			  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_RESET);
			  uSec_Delay(3);

			  ///Start the ultrasonic measurement routine
			  //1. output 10us pulse on trigger pin
			  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_SET);
			  uSec_Delay(10);
			  HAL_GPIO_WritePin(TRIGL_GPIO_Port, TRIGL_Pin, GPIO_PIN_RESET);

			  //2. Wait for echo pin rising edge
			  while(HAL_GPIO_ReadPin(ECHOL_GPIO_Port, ECHOL_Pin) == GPIO_PIN_RESET);

			  //3. Start measuring echo pause width until the falling edge
			  numTicks = 0;
			  while(HAL_GPIO_ReadPin(ECHOL_GPIO_Port, ECHOL_Pin) == GPIO_PIN_SET)
			  {
				  numTicks++;
				  uSec_Delay(2);  //actually around 2.8 uSec, measures every 2.8us
			  }
			//4. Estimate distance. 0.0f type casts as a float, multiply by actual delay 2.8uS
			sonar->distanceL = (numTicks + 0.0f)*2.8*SpeedOfSound;
			printf("Left Sonar Distance (cm): %f",sonar->distanceL);

};


/* This function uses interrupts to toggle Blinky*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim6)
{
	 HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin);
}



/*
//Update Encoder
void updateEncoder(ENC_STATUS *enc){

	enc-> pos = __HAL_TIM_GET_COUNTER(enc->htim);

	//calculate the velocity
	enc-> vel = enc -> pos - enc -> last;
	printf("encoder  %s: pos = %ld, vel = %ld, last = %ld\r\n\n",enc->tag, enc->pos, enc->vel, enc->last);

	//Save enc pause into inc last
	enc -> last  =  enc -> pos;

	if(oddeven < 1){
		SSD1306_GotoXY(50, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(50, 40);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven++;

	}else if(2 > oddeven >= 1) {
		SSD1306_GotoXY(0, 20);
		SSD1306_Puts(enc->tag, &Font_7x10, 1);
		SSD1306_GotoXY(0, 40);
		sprintf(position, "%ld",enc->pos); //this is used to convert to the char array position[10]
		SSD1306_Puts(position, &Font_7x10, 1);
		SSD1306_UpdateScreen();
		oddeven = 0;
	}else{
		oddeven = 0;
	}


	}*/


void setMTRSpeed(int16_t speed, const MOTOR_CONF *motor){

	uint32_t direction = speed > 0?0:1; //if assignment, ternary operator
	speed = abs(speed); //takes speed and returns absolute value
	HAL_GPIO_WritePin(motor->gpio_port, motor->gpio_pin, direction==1 ?SET:RESET);

	if(speed > MOTOR_PWM_PERIOD){
		speed = MOTOR_PWM_PERIOD;
	}
	__HAL_TIM_SET_COMPARE(motor->htim,motor->tim_ch,speed); //sets capture/compare register for the the duty; how fast the
}


void setPWM(TIM_HandleTypeDef timer,uint32_t channel, uint8_t dir, uint16_t period, uint16_t pulse){
	if(dir == 1){
		HAL_GPIO_WritePin(ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin, SET);
		HAL_GPIO_WritePin(ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin, SET);
	}else{
		HAL_GPIO_WritePin(ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin, RESET);
		HAL_GPIO_WritePin(ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin, RESET);
	}
	HAL_TIM_PWM_Stop(&timer, channel); // stop the current timer
	TIM_OC_InitTypeDef sConfigOC;
	timer.Init.Period = period;   //load period duration
	HAL_TIM_PWM_Init(&timer); //reinit the timer

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = pulse;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	HAL_TIM_PWM_ConfigChannel(&timer, &sConfigOC, channel);

	HAL_TIM_PWM_Start(&timer,channel);  //start PWM

}

