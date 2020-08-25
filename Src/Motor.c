#include "gpio.h"
#include "motor.h"

#define DIR_Pin GPIO_PIN_1
#define DIR_GPIO_Port GPIOC
#define EN_Pin GPIO_PIN_11
#define EN_GPIO_Port GPIOB

void motor_forward(void)
{
	//DIR = RESET
	HAL_GPIO_WritePin(DIR_GPIO_Port, GPIO_PIN_1, GPIO_PIN_RESET);
	//EN = RESET
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
}

void motor_retrograde(void)
{
	//DIR = SET
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
	//EN = RESET
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
}

void motor_stop(void)
{
	//EN = SET
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
}

void motor_over(void)
{
	motor_forward();
	HAL_Delay(15000);
	motor_stop();
}

