/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t prev_bieg = 255; // 255 = żaden bieg wybrany
uint8_t current_bieg = 255;
int8_t Offset_for1 = 0;
int8_t Offset_for2 = 0;
int8_t Offset_for3 = 0;
int8_t Offset_for4 = 0;
uint32_t last_led_toggle = 0;
uint32_t led_interval_ms = 1000;  // domyślnie 1 Hz
uint32_t last_button_time = 0;
uint32_t debounce_time_ms = 200;
typedef struct
{
    uint16_t signature;
    int8_t Offset_for1;
    int8_t Offset_for2;
    int8_t Offset_for3;
    int8_t Offset_for4;
    uint16_t reserved;
} OffsetsFlash_t;

OffsetsFlash_t saved_offsets;
uint8_t offsets_changed = 0;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */


#define SAVE_DEBOUNCE_MS  200
#define FLASH_OFFSETS_ADDR  0x08020000U
#define FLASH_SECTOR        FLASH_SECTOR_5
#define FLASH_SIGNATURE     0x55AA

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HandleSaveButton(void);
uint8_t CheckOffsetsChanged(void);
void SaveOffsetsToFlash(int8_t o1, int8_t o2, int8_t o3, int8_t o4);
OffsetsFlash_t* LoadOffsetsFromFlash(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  OffsetsFlash_t* loaded = LoadOffsetsFromFlash();

  if (loaded != NULL)
  {
      saved_offsets = *loaded;

      Offset_for1 = saved_offsets.Offset_for1;
      Offset_for2 = saved_offsets.Offset_for2;
      Offset_for3 = saved_offsets.Offset_for3;
      Offset_for4 = saved_offsets.Offset_for4;
  }
  else
  {
      Offset_for1 = 0;
      Offset_for2 = 0;
      Offset_for3 = 0;
      Offset_for4 = 0;

      saved_offsets.Offset_for1 = 0;
      saved_offsets.Offset_for2 = 0;
      saved_offsets.Offset_for3 = 0;
      saved_offsets.Offset_for4 = 0;
  }
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	uint32_t pwm_period = 0;

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_RESET) {
		// F_Set_1 założona → 500 Hz
		pwm_period = 2000 - 1; // ARR = 1999 → 1 MHz / 2000 = 500 Hz
	} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET) {
		// F_Set_2 założona → 1 kHz
		pwm_period = 1000 - 1; // ARR = 999 → 1 MHz / 1000 = 1 kHz
	} else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) {
		// F_Set_3 założona → 2 kHz
		pwm_period = 500 - 1; // ARR = 499 → 1 MHz / 500 = 2 kHz
	} else {
		// Tryb testowy → wszystko OFF
		pwm_period = 10000 - 1; // ARR = 9999 → 1 MHz / 10000 = 100 Hz
	}

	// ustawić auto-reload timera
	__HAL_TIM_SET_AUTORELOAD(&htim2, pwm_period);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);

	if (current_bieg == 255)
	{
	    GPIO_InitTypeDef GPIO_InitStruct = {0};
	    GPIO_InitStruct.Pin = GPIO_PIN_0;
	    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	    GPIO_InitStruct.Pull = GPIO_NOPULL;
	    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
	}


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {




		// Odczyt selektora
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == GPIO_PIN_RESET)
			current_bieg = 1;
		else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_RESET)
			current_bieg = 2;
		else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == GPIO_PIN_RESET)
			current_bieg = 3;
		else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == GPIO_PIN_RESET)
			current_bieg = 4;
		else
			current_bieg = 255;

		// ustawienie częstotliwości migania LED w zależności od biegu
		switch (current_bieg) {
		case 1:
			led_interval_ms = 1000; // 1 Hz
			break;
		case 2:
			led_interval_ms = 333;  // ~3 Hz
			break;
		case 3:
			led_interval_ms = 200;  // 5 Hz
			break;
		case 4:
			led_interval_ms = 100;  // 10 Hz
			break;
		default:
			led_interval_ms = 2000; // brak biegu → 0.5 Hz
			break;
		}

		// Odczyt przycisków SP_Up i SP_Dn
		uint8_t sp_up_pressed = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2)
				== GPIO_PIN_RESET);
		uint8_t sp_dn_pressed = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)
				== GPIO_PIN_RESET);

		if ((HAL_GetTick() - last_button_time) > debounce_time_ms)
		{
		    if (sp_up_pressed)
		    {
		        switch (current_bieg)
		        {
		            case 1: if (Offset_for1 < 20) Offset_for1 += 1; break;
		            case 2: if (Offset_for2 < 20) Offset_for2 += 1; break;
		            case 3: if (Offset_for3 < 20) Offset_for3 += 1; break;
		            case 4: if (Offset_for4 < 20) Offset_for4 += 1; break;
		        }
		        last_button_time = HAL_GetTick();
		    }
		    else if (sp_dn_pressed)
		    {
		        switch (current_bieg)
		        {
		        	case 1: if (Offset_for1 > -20) Offset_for1 -= 1; break;
		            case 2: if (Offset_for2 > -20) Offset_for2 -= 1; break;
		            case 3: if (Offset_for3 > -20) Offset_for3 -= 1; break;
		            case 4: if (Offset_for4 > -20) Offset_for4 -= 1; break;
		        }
		        last_button_time = HAL_GetTick();
		    }
		}

		// Ograniczenie offsetów do przedziału -20% ... +20%
		if (Offset_for1 > 20)
			Offset_for1 = 20;
		if (Offset_for1 < -20)
			Offset_for1 = -20;

		if (Offset_for2 > 20)
			Offset_for2 = 20;
		if (Offset_for2 < -20)
			Offset_for2 = -20;

		if (Offset_for3 > 20)
			Offset_for3 = 20;
		if (Offset_for3 < -20)
			Offset_for3 = -20;

		if (Offset_for4 > 20)
			Offset_for4 = 20;
		if (Offset_for4 < -20)
			Offset_for4 = -20;

		// Sprawdzanie, czy zmienił się bieg
		if (current_bieg != prev_bieg) {
			if (current_bieg == 255) {
				// Żaden bieg nie jest wybrany → wyłącz PWM
				HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);

				// Przełącz pin na GPIO LOW
				GPIO_InitTypeDef GPIO_InitStruct = { 0 };
				GPIO_InitStruct.Pin = GPIO_PIN_0;
				GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
			} else {
				// Przełącz pin z powrotem na Alternate Function (PWM)
				GPIO_InitTypeDef GPIO_InitStruct = { 0 };
				GPIO_InitStruct.Pin = GPIO_PIN_0;
				GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
				GPIO_InitStruct.Pull = GPIO_NOPULL;
				GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
				GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
				HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

				if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
				{
				    Error_Handler();
				}

				uint32_t duty_percent = 0;

				switch (current_bieg) {
				case 1:
					duty_percent = 25 + Offset_for1;
					break;
				case 2:
					duty_percent = 50 + Offset_for2;
					break;
				case 3:
					duty_percent = 75 + Offset_for3;
					break;
				case 4:
					duty_percent = 100 + Offset_for4;
					break;
				default:
					duty_percent = 0;
					break;
				}

				// Ogranicz duty_percent do 0…100 %
				if (duty_percent > 100)
					duty_percent = 100;
				if (duty_percent < 0)
					duty_percent = 0;

				uint32_t duty = ((pwm_period + 1) * duty_percent) / 100;
				if (duty > pwm_period)
				    duty = pwm_period;

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
			}

			prev_bieg = current_bieg;
		}

		// miganie LED
		if (HAL_GetTick() - last_led_toggle >= led_interval_ms) {
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			last_led_toggle = HAL_GetTick();
		}



		HandleSaveButton();

		if (CheckOffsetsChanged() && current_bieg != 255)
		{
		    uint32_t duty_percent = 0;

		    switch (current_bieg) {
		        case 1: duty_percent = 25 + Offset_for1; break;
		        case 2: duty_percent = 50 + Offset_for2; break;
		        case 3: duty_percent = 75 + Offset_for3; break;
		        case 4: duty_percent = 100 + Offset_for4; break;
		        default: duty_percent = 0; break;
		    }

		    if (duty_percent > 100) duty_percent = 100;
		    if (duty_percent < 0)   duty_percent = 0;

		    uint32_t duty = ((pwm_period + 1) * duty_percent) / 100;
		    if (duty > pwm_period) duty = pwm_period;

		    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, duty);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void SaveOffsetsToFlash(int8_t o1, int8_t o2, int8_t o3, int8_t o4)
{
    OffsetsFlash_t offsets;
    offsets.signature = FLASH_SIGNATURE;
    offsets.Offset_for1 = o1;
    offsets.Offset_for2 = o2;
    offsets.Offset_for3 = o3;
    offsets.Offset_for4 = o4;

    HAL_FLASH_Unlock();

    // kasuj sektor
    FLASH_EraseInitTypeDef EraseInitStruct = {0};
    uint32_t SectorError;

    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
    EraseInitStruct.Sector = FLASH_SECTOR;
    EraseInitStruct.NbSectors = 1;
    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return;
    }

    // zapisujemy dane słowo po słowie
    uint32_t* pdata = (uint32_t*)&offsets;

    for (uint32_t i = 0; i < (sizeof(OffsetsFlash_t) + 3)/4; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                              FLASH_OFFSETS_ADDR + i*4,
                              pdata[i]) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return;
        }
    }

    HAL_FLASH_Lock();
}

OffsetsFlash_t* LoadOffsetsFromFlash(void)
{
    OffsetsFlash_t* ptr = (OffsetsFlash_t*)FLASH_OFFSETS_ADDR;
    if (ptr->signature == FLASH_SIGNATURE)
    {
        return ptr;
    }
    else
    {
        return NULL;
    }
}


uint8_t CheckOffsetsChanged(void)
{
    if (Offset_for1 != saved_offsets.Offset_for1) return 1;
    if (Offset_for2 != saved_offsets.Offset_for2) return 1;
    if (Offset_for3 != saved_offsets.Offset_for3) return 1;
    if (Offset_for4 != saved_offsets.Offset_for4) return 1;
    return 0;
}



void HandleSaveButton(void)
{
    static uint32_t last_save_time = 0;

    // zapal LED jeśli zmienione offsety
    if (CheckOffsetsChanged())
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }

    uint8_t save_pressed = (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) == GPIO_PIN_RESET);
    static uint8_t prev_save_pressed = 0;
    if (save_pressed && !prev_save_pressed && (HAL_GetTick() - last_save_time > SAVE_DEBOUNCE_MS))
    {
        if (CheckOffsetsChanged())
        {
            // Zapisz offsety do flash
            SaveOffsetsToFlash(Offset_for1, Offset_for2, Offset_for3, Offset_for4);

            // Zaktualizuj saved_offsets
            saved_offsets.Offset_for1 = Offset_for1;
            saved_offsets.Offset_for2 = Offset_for2;
            saved_offsets.Offset_for3 = Offset_for3;
            saved_offsets.Offset_for4 = Offset_for4;

            // Zgaś SaveLed po zapisie
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
        }

        last_save_time = HAL_GetTick();
    }
    prev_save_pressed = save_pressed;
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
