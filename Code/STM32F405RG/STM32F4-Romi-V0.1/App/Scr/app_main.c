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
 * esp link http://192.168.4.1 | 	192.168.50.85
 *
 * To Do:
 * Work on PID Loop
 * Confirm PWM channel timer is 16bit or 32 bit
 * confirm PID Period and Duty is correct
 *
// remove all uneccessary code to check speed and see if delays are errors
// *****************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdbool.h> //this is required to use bool statements
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
#include "sonar.h"
#include "edge_sensor.h"


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
extern TIM_HandleTypeDef htim9; //sonar 1uSec

//******Define Sonar******
//#define uSTIM TIM9

//Speed of sound in air cm/uSec
//const float SpeedOfSound = 0.0343/2; //divided by 2 since its the speed to reach the object and come back

SONAR_STATUS SONARS[] ={{"Left",TRIGL_GPIO_Port,TRIGL_Pin,ECHOL_GPIO_Port,ECHOL_Pin,0,0.0},
						{"Right",TRIGR_GPIO_Port,TRIGR_Pin,ECHOR_GPIO_Port,ECHOR_Pin,0,0.0},
						{"Center",TRIG_CTR_GPIO_Port,TRIG_CTR_Pin,ECHO_CTR_GPIO_Port,ECHO_CTR_Pin,0,0.0}};


//Sonar ticks set to zero
uint32_t numTicks = 0;

#define LED_BLINK_RATE 50 //50*10mS = 1/2 sec

char updatescr[10]; //use to update screen

//******PID******
float speed_l = 0.2;
float speed_r = 0.2;

float duty_l = 0.0;
float duty_r = 0.0;

#define KP 0.02
#define KI 0.0

//#define KP 0.065
//#define KI 2.0

#define TICK_RATE 10 //10mS
#define PID_RATE  4 //2*10mS

#define DT ((float)(TICK_RATE*PID_RATE)/1000)



#define MAX_SPEED 10.0  //rad/s

//#define MAX_VELOCITY 50.0
#define SPEED_CHANGE 0.1

//******PWM Setup****
typedef struct MOTOR_T {
	char *motor;
	uint32_t tim_ch;
	TIM_HandleTypeDef *htim;
	GPIO_TypeDef* gpio_port;
	uint32_t gpio_pin;

}MOTOR_CONF;

static const MOTOR_CONF mot_left = {"Left",TIM_CHANNEL_1, &htim4, ROMI_DIRL_GPIO_Port, ROMI_DIRL_Pin}; //add a status bit?
static const MOTOR_CONF mot_right = {"Right",TIM_CHANNEL_3, &htim2, ROMI_DIRR_GPIO_Port, ROMI_DIRR_Pin};

static PID pid_right = {KP, KI, 0, 0,"Right",DT};
static PID pid_left = {KP, KI, 0, 0,"Left",DT};


ENC_STATUS enc_right = {0,0,0,"Right", 0, &htim3};
ENC_STATUS enc_left = {0,0,0,"Left", 0, &htim5};


// Hardware Revision bits
uint8_t RevBit[3];

// local functions
//static void uSec_Delay(uint32_t uSec);
//static void setPWM(TIM_HandleTypeDef, uint32_t, uint8_t, uint16_t, uint16_t);
static void setMTRSpeed(float speed, const MOTOR_CONF *motor);
void STOP(void);


// main application loop
void appMain(void){


	/* Check Hardware Revision Bits*/
	RevBit[0] =	HAL_GPIO_ReadPin(REV_BIT0_GPIO_Port, REV_BIT0_Pin);
	RevBit[1] =	HAL_GPIO_ReadPin(REV_BIT1_GPIO_Port, REV_BIT1_Pin);
	RevBit[2] =	HAL_GPIO_ReadPin(REV_BIT2_GPIO_Port, REV_BIT2_Pin);

	//Start the 1uSec timer for sonar
	HAL_TIM_Base_Start(&htim9);

	//hal pwm start
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);  //Start PWM
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);  //Start PWM

	//hal encoder start
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);

	uint32_t ledTimer=LED_BLINK_RATE;
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
	//SSD1306_GotoXY(40, 20);
	//SSD1306_Puts("OWO", &Font_16x26, 1);
	//SSD1306_UpdateScreen();
	//HAL_Delay(2000);
	SSD1306_Clear();
	SSD1306_GotoXY(30, 0);
	SSD1306_Puts("STM32-ROMI", &Font_7x10, 1);
	SSD1306_GotoXY(0, 30);
	SSD1306_Puts("E", &Font_7x10, 1);
	SSD1306_GotoXY(0, 40);
	SSD1306_Puts("D", &Font_7x10, 1);
	SSD1306_GotoXY(0, 50);
	SSD1306_Puts("S", &Font_7x10, 1);
	SSD1306_UpdateScreen();


	uint32_t tick = HAL_GetTick();

	//Set Sleep bits to 1 for enable
	HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, SET);
	HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, SET);

	//Edge Sensors
	enableEdgeSensors(BUMP_BIT_LEFT | BUMP_BIT_RIGHT);


	//Main program to loop forever
	while(1){
		uint32_t tock = HAL_GetTick();

		if(tock-tick>TICK_RATE){ //10mS

			ledTimer--;
			if(ledTimer==0){
				ledTimer = LED_BLINK_RATE;
				HAL_GPIO_TogglePin(Blinky_GPIO_Port, Blinky_Pin);
			}

			PIDTimer --;
			if(PIDTimer==0) {
				PIDTimer=PID_RATE;

				//set the motor drivers on
				HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, SET);
				HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, SET);

				/* Update the encoders*/
				updateEncoder(&enc_right);
				updateEncoder(&enc_left);
				printf("%s Encoder is %f\n\r",enc_left.tag,enc_left.vel);
				printf("%s Encoder is %f\n\r",enc_right.tag,enc_right.vel);

				duty_l = PID_update(speed_l,enc_left.vel,&pid_left);
				duty_r = PID_update(speed_r,enc_right.vel,&pid_right);
				//duty_l = 0.2;
				//duty_r = 0.2;
				setMTRSpeed(duty_r*MOTOR_PWM_PERIOD,&mot_right);
				setMTRSpeed(duty_l*MOTOR_PWM_PERIOD,&mot_left);

				//printf("Left Speed = %f\t Right Speed =%f\n\r",speed_l,speed_r);
				//printf("Left Duty = %f\t Right Duty =%f\n\r",duty_l,duty_r);

				//update screen
				SSD1306_GotoXY(10, 40);
				sprintf(updatescr, "%ld",duty_l); //this is used to convert to the char array position[10]
				SSD1306_Puts(updatescr, &Font_7x10, 1);
				SSD1306_GotoXY(75, 40);
				sprintf(updatescr, "%ld",duty_r); //this is used to convert to the char array position[10]
				SSD1306_Puts(updatescr, &Font_7x10, 1);
				SSD1306_GotoXY(10, 50);
				sprintf(updatescr, "%ld",speed_l); //this is used to convert to the char array position[10]
				SSD1306_Puts(updatescr, &Font_7x10, 1);
				SSD1306_GotoXY(75, 50);
				sprintf(updatescr, "%ld",speed_r); //this is used to convert to the char array position[10]
				SSD1306_Puts(updatescr, &Font_7x10, 1);

				//Check the sonars
				checkSonar(&SONARS[SONAR1]);
				checkSonar(&SONARS[SONAR2]);

				//check Edge Sensors
				updateEdgeSensors();  //update the state of the edge sensors

			}
			tick = tock;

		}

		/// use this to adjust the pwm

		int c = getchar();
			if(c != EOF){
				putchar(c);
				switch (c) {
					case 'w':
						if((speed_l < MAX_SPEED)&&(speed_r < MAX_SPEED)){
							speed_l += SPEED_CHANGE;
							//speed_r += SPEED_CHANGE;
						}
						break;
					case 'a':
						if((speed_l < MAX_SPEED)&&(speed_r < MAX_SPEED)){
							speed_l += SPEED_CHANGE;
							speed_r -= SPEED_CHANGE;
						}
						break;
					case 'd':
						if((speed_l < MAX_SPEED)&&(speed_r < MAX_SPEED)){
							speed_r += SPEED_CHANGE;
							speed_l -= SPEED_CHANGE;
						}
						break;
					case 's':
						if((speed_l > -MAX_SPEED)&&(speed_r > -MAX_SPEED)){
							speed_l -= SPEED_CHANGE;
							speed_r -= SPEED_CHANGE;
						}
						break;

					case ' ':
						STOP();
						break;
					default:
						break;
				}

				}else{
				clearerr(stdin); // Reset the EOF Condition
				}

		//	updateEdgeSensors();  //update the state of the edge sensors
		bool leftClif = getEdgeSensorState(BUMP_BIT_LEFT)==ES_HIT;
		bool rightClif = getEdgeSensorState(BUMP_BIT_RIGHT)==ES_HIT;

			if(leftClif || rightClif){
				STOP();
			}


			} //end of while loop



} //end of main loop


void setMTRSpeed(float speed, const MOTOR_CONF *motor){

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
	//HAL_GPIO_WritePin(ROMI_SLPL_GPIO_Port, ROMI_SLPL_Pin, RESET);
	//HAL_GPIO_WritePin(ROMI_SLPR_GPIO_Port, ROMI_SLPR_Pin, RESET);
	//driving = false;
}
/*
void drive(float lin_vel, float ang_vel){
	//speed_l = (lin_vel);
}*/
