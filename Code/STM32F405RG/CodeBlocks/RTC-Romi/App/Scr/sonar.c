/*
 * sonar.c
 *
 *  Created on: Sep 19, 2020
 *      Author: Kyle Rodrigues
 *
 *      To Do: fix timer scale factor
 */
#include "sonar.h"
#include "main.h"
#include "tim.h"

#define SR04_MAX_READING 0.05f  // 50ms
#define PING_MAX_READING 0.018f  // 18ms

uint32_t uSec = 10;

const float SpeedOfSound = 0.0343/2; //cm/uSec divided by 2 since its the speed to reach the object and come back
const float SONAR_TIM_SCALE = 2.8; //Define how much the time scales by

void checkSonar(SONAR_STATUS *sonar){
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET); //Set the Trigger pin low
	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,SET);//keep high for 10uS
	/* This is a 10uS delay*/
	if(uSec < 2)uSec = 2;
	TIM7->ARR = uSec - 1; 	//Sets the value in the auto reload register
	TIM7 -> EGR = 1;		//Re-initialize the Timer
	TIM7 -> SR &= ~1;  	//Resets the flag
	TIM7 ->CR1 |= 1;		//Enables the counter
	while((TIM7 -> SR&0x0001) != 1);
	TIM7 -> SR &= ~(0x0001);

	HAL_GPIO_WritePin(sonar->trig_port,sonar->trig_pin,RESET);//Set to low again to start reading
	//sonar->tick = __HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register
	//printf(" %c Sonar tick: %ld\n\r",sonar->sonar_ch,sonar->tick);
}

void updateSonar(SONAR_STATUS *sonar){
	//4. Estimate distance. 0.0f type casts as a float, multiply by actual delay 2.8uS
	//sonar->distance = (sonar->tick + 0.0f)*2.8*SpeedOfSound;
	//sonar->distance = (sonar->tock-sonar->tick + 0.0f)*SONAR_TIM_SCALE*SpeedOfSound;
	printf("Sonar tick: %ld \t Sonar tock: %ld\n\r",sonar->tick,sonar->tock);
	//printf("%s Sonar Distance (cm): %f\n\n\r",sonar->sonar_ch,sonar->distance);

}
//This is called as an interrupt controller, do minimal stuff in here and leave
void sonarISR(SONARID id){
	SONAR_STATUS *sonar = &SONARS[id];
	sonar->tick = __HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register

	while(HAL_GPIO_ReadPin(sonar->echo_port, sonar->echo_pin)== GPIO_PIN_SET)
	{
		// Wait for pin to go low
	}
	sonar->tock = __HAL_TIM_GET_COUNTER(&htim9); //grab the count value in the counter register
	updateSonar(sonar->tock);

}
/*
switch(sonarState) { // update sonar state machine

    case SONAR_IDLE: // no sonars busy
      sonarTimeout = 0;
      if(restart) {
         sonarState = CHECK_SONAR_1; // next try sonar 1
         sonarTimeout = SONAR_TIMEOUT; // start timer
      }
      break;

    case CHECK_SONAR_1: // if sonar 1 enabled, start a reading
      if(sonarTimeout > (SONAR_TIMEOUT-TRIGGER_DELAY) && sonar1.isEnabled()) { // wait for at least MIN_TRIGGER_DELAY before next trigger
    	  break;
      }


      if(sonar1.isEnabled()) {
        sonar1.trigger(echo1,echo2,0x55);
        sonarTimeout = SONAR_TIMEOUT; // start timer
        sonarState = SONAR_1_ACTIVE;  // next wait for echo

      }
      else {
        sonarState = CHECK_SONAR_2;  // not enabled so try sonar 2
      }
      break;

    case CHECK_SONAR_2: // if sonar 2 enabled, start a reading

      if(sonarTimeout > (SONAR_TIMEOUT-TRIGGER_DELAY) && sonar2.isEnabled()) { // wait for at least MIN_TRIGGER_DELAY before next trigger
      	 break;
      }

      if(sonar2.isEnabled()) {
        sonar2.trigger(echo2,echo1,0x65);
        sonarTimeout = SONAR_TIMEOUT; // start timer
        sonarState = SONAR_2_ACTIVE;  // next wait for echo
      }
      else {
        sonarState = SONAR_IDLE; // not enabled so try sonar 1 again
      }
      break;


    case SONAR_1_ACTIVE: // waiting for echo for sonar 1 or timeout
      if(sonar1.doEvent(gotEcho,timeout)) {
        //sonarTimeout = 0;
        sonarState = CHECK_SONAR_2;
        sonarChanged = true;
      }
      break;

    case SONAR_2_ACTIVE: // waiting for echo for sonar 2 or timeout
      if(sonar2.doEvent(gotEcho,timeout)) {
        //sonarTimeout = 0;
        sonarState = SONAR_IDLE;
        sonarChanged = true;
      }
      break;

  }

  return sonarChanged;

}*/

/*
void uSec_Delay(uint32_t uSec)
{
	if(uSec < 2)uSec = 2;
	uSTIM->ARR = uSec - 1; 	//Sets the value in the auto reload register
	uSTIM -> EGR = 1;		//Re-initialize the Timer
	uSTIM -> SR &= ~1;  	//Resets the flag
	uSTIM ->CR1 |= 1;		//Enables the counter
	while((uSTIM -> SR&0x0001) != 1);
	uSTIM -> SR &= ~(0x0001);

}*/

