/*
 * edge_sensor.c
 *
 *  Created on: Oct 2, 2020
 *      Author: livin
 */

#include <stdio.h>
#include "main.h"
#include "gpio.h"
#include "edge_sensor.h"
#define EDGE_SENSOR_ACTIVE GPIO_PIN_SET
static uint32_t sensor_state=0;
static uint32_t sensor_changed=0;
static uint32_t sensor_enabled=0;
static uint32_t debounce(uint32_t sample);
static uint32_t readSensors(void);

void enableEdgeSensors(uint32_t sensor) {
	sensor_enabled |= sensor;
}
void disableEdgeSensors(uint32_t sensor) {
	sensor_enabled &= ~sensor;
}
EDGE_SENSOR_STATE getEdgeSensorState(uint32_t sensor) {
	uint32_t hit =  (sensor_state & sensor)?ES_HIT:ES_CLEAR;
	if(sensor_changed & sensor) {
		printf("Edge Sensor %ld: %s\n",sensor,((hit==ES_HIT)?"Hit":"Clear"));
		sensor_changed &= ~sensor;
	}
	return hit & sensor_enabled;
}
void updateEdgeSensors(void) {
	uint32_t new_state = readSensors();
	//uint32_t state = debounce(state);
	sensor_changed = new_state ^ sensor_state;
	sensor_state = new_state;
}
uint32_t readSensors(void) {

	uint32_t bump1=HAL_GPIO_ReadPin(QTRL_GPIO_Port, QTRL_Pin)==EDGE_SENSOR_ACTIVE?BUMP_BIT_LEFT:0;
	uint32_t bump2=HAL_GPIO_ReadPin(QTRR_GPIO_Port, QTRR_Pin)==EDGE_SENSOR_ACTIVE?BUMP_BIT_RIGHT:0;

	return bump1 | bump2;
}
uint32_t debounce(uint32_t sample)
{
    static uint32_t state, x0, x1;
    uint32_t delta;
    delta = sample ^ state;
    x1 = (x1 ^ x0) & delta;
    x0 = ~x0 & delta;
    state ^= (x0 & x1);
    return state;
}
