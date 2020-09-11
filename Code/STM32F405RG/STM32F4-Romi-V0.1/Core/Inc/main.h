/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_BTN_Pin GPIO_PIN_13
#define OLED_BTN_GPIO_Port GPIOC
#define IMU_RST_Pin GPIO_PIN_0
#define IMU_RST_GPIO_Port GPIOC
#define REV_BIT0_Pin GPIO_PIN_1
#define REV_BIT0_GPIO_Port GPIOC
#define REV_BIT1_Pin GPIO_PIN_2
#define REV_BIT1_GPIO_Port GPIOC
#define REV_BIT2_Pin GPIO_PIN_3
#define REV_BIT2_GPIO_Port GPIOC
#define ENCL_CHA_Pin GPIO_PIN_0
#define ENCL_CHA_GPIO_Port GPIOA
#define ENCL_CHB_Pin GPIO_PIN_1
#define ENCL_CHB_GPIO_Port GPIOA
#define Blinky_Pin GPIO_PIN_5
#define Blinky_GPIO_Port GPIOA
#define RGB_INT1_Pin GPIO_PIN_4
#define RGB_INT1_GPIO_Port GPIOC
#define RGB_INT2_Pin GPIO_PIN_5
#define RGB_INT2_GPIO_Port GPIOC
#define TRIGL_Pin GPIO_PIN_0
#define TRIGL_GPIO_Port GPIOB
#define ECHOL_Pin GPIO_PIN_1
#define ECHOL_GPIO_Port GPIOB
#define QTRR_OUT_Pin GPIO_PIN_2
#define QTRR_OUT_GPIO_Port GPIOB
#define ROMI_PWMR_Pin GPIO_PIN_10
#define ROMI_PWMR_GPIO_Port GPIOB
#define QTRL_OUT_Pin GPIO_PIN_12
#define QTRL_OUT_GPIO_Port GPIOB
#define ROMI_DIRR_Pin GPIO_PIN_13
#define ROMI_DIRR_GPIO_Port GPIOB
#define ROMI_SLPR_Pin GPIO_PIN_14
#define ROMI_SLPR_GPIO_Port GPIOB
#define SERVO1_PWM_Pin GPIO_PIN_15
#define SERVO1_PWM_GPIO_Port GPIOB
#define ROMI_SLPL_Pin GPIO_PIN_6
#define ROMI_SLPL_GPIO_Port GPIOC
#define ECHO_CTR_Pin GPIO_PIN_7
#define ECHO_CTR_GPIO_Port GPIOC
#define TRIG_CTR_Pin GPIO_PIN_8
#define TRIG_CTR_GPIO_Port GPIOC
#define ECHOR_Pin GPIO_PIN_9
#define ECHOR_GPIO_Port GPIOA
#define TRIGR_Pin GPIO_PIN_10
#define TRIGR_GPIO_Port GPIOA
#define SPI_CS_AUX_Pin GPIO_PIN_15
#define SPI_CS_AUX_GPIO_Port GPIOA
#define ROMI_DIRL_Pin GPIO_PIN_12
#define ROMI_DIRL_GPIO_Port GPIOC
#define SPI_CS_FLASH_Pin GPIO_PIN_2
#define SPI_CS_FLASH_GPIO_Port GPIOD
#define ENCR_CHA_Pin GPIO_PIN_4
#define ENCR_CHA_GPIO_Port GPIOB
#define ENCR_CHB_Pin GPIO_PIN_5
#define ENCR_CHB_GPIO_Port GPIOB
#define ROMI_PWML_Pin GPIO_PIN_6
#define ROMI_PWML_GPIO_Port GPIOB
#define SERVO2_PWM_Pin GPIO_PIN_9
#define SERVO2_PWM_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
