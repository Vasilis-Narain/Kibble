/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

#include "stm32h7xx_nucleo.h"
#include <stdio.h>

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
#define AMP_SD_Pin GPIO_PIN_3
#define AMP_SD_GPIO_Port GPIOE
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define BTN_Y_Pin GPIO_PIN_7
#define BTN_Y_GPIO_Port GPIOF
#define BTN_L_Pin GPIO_PIN_8
#define BTN_L_GPIO_Port GPIOF
#define BTN_R_Pin GPIO_PIN_9
#define BTN_R_GPIO_Port GPIOF
#define PH0_MCU_Pin GPIO_PIN_0
#define PH0_MCU_GPIO_Port GPIOH
#define PH1_MCU_Pin GPIO_PIN_1
#define PH1_MCU_GPIO_Port GPIOH
#define LCD_BL_PWM_Pin GPIO_PIN_7
#define LCD_BL_PWM_GPIO_Port GPIOA
#define BTN_START_Pin GPIO_PIN_14
#define BTN_START_GPIO_Port GPIOF
#define BTN_SELECT_Pin GPIO_PIN_15
#define BTN_SELECT_GPIO_Port GPIOF
#define BTN_LEFT_Pin GPIO_PIN_10
#define BTN_LEFT_GPIO_Port GPIOB
#define BTN_RIGHT_Pin GPIO_PIN_11
#define BTN_RIGHT_GPIO_Port GPIOB
#define BTN_A_Pin GPIO_PIN_12
#define BTN_A_GPIO_Port GPIOB
#define BTN_B_Pin GPIO_PIN_13
#define BTN_B_GPIO_Port GPIOB
#define BTN_X_Pin GPIO_PIN_15
#define BTN_X_GPIO_Port GPIOB
#define BTN_POWER_Pin GPIO_PIN_12
#define BTN_POWER_GPIO_Port GPIOD
#define JOY_SEL_Pin GPIO_PIN_13
#define JOY_SEL_GPIO_Port GPIOD
#define LCD_TE_Pin GPIO_PIN_3
#define LCD_TE_GPIO_Port GPIOD
#define LCD_TE_EXTI_IRQn EXTI3_IRQn
#define LCD_RST_Pin GPIO_PIN_6
#define LCD_RST_GPIO_Port GPIOD
#define SD_DET_Pin GPIO_PIN_7
#define SD_DET_GPIO_Port GPIOD
#define SD_DET_EXTI_IRQn EXTI9_5_IRQn
#define PWR_LED_Pin GPIO_PIN_14
#define PWR_LED_GPIO_Port GPIOG
#define BTN_UP_Pin GPIO_PIN_8
#define BTN_UP_GPIO_Port GPIOB
#define BTN_DOWN_Pin GPIO_PIN_9
#define BTN_DOWN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
