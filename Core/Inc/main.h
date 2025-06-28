/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOC
#define Save_Led_Pin GPIO_PIN_1
#define Save_Led_GPIO_Port GPIOA
#define Save_Bt_Pin GPIO_PIN_2
#define Save_Bt_GPIO_Port GPIOA
#define Selektor_1_Pin GPIO_PIN_3
#define Selektor_1_GPIO_Port GPIOA
#define Selektor_2_Pin GPIO_PIN_4
#define Selektor_2_GPIO_Port GPIOA
#define Selektor_3_Pin GPIO_PIN_5
#define Selektor_3_GPIO_Port GPIOA
#define Selektor_4_Pin GPIO_PIN_6
#define Selektor_4_GPIO_Port GPIOA
#define F_Set_1_Pin GPIO_PIN_7
#define F_Set_1_GPIO_Port GPIOA
#define F_Set_2_Pin GPIO_PIN_0
#define F_Set_2_GPIO_Port GPIOB
#define F_Set_3_Pin GPIO_PIN_1
#define F_Set_3_GPIO_Port GPIOB
#define SP_Up_Pin GPIO_PIN_2
#define SP_Up_GPIO_Port GPIOB
#define SP_Dn_Pin GPIO_PIN_10
#define SP_Dn_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
