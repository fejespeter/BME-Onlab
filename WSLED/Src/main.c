/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

SPI_HandleTypeDef hspi2;
DMA_HandleTypeDef hdma_spi2_tx;

UART_HandleTypeDef huart4;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define LED_COUNT 84
#define COLOR_COUNT LED_COUNT*3
#define ADC_COUNT 1024
static uint8_t colors[COLOR_COUNT];


uint8_t printbuffer[50];
uint8_t adc_flag = 0;

uint16_t adcValue[ADC_COUNT];
uint16_t adcValueAmpfiled[ADC_COUNT];

uint16_t colheight ;
uint16_t adc_value_min = 2000;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_ADC1_Init(void);
static void MX_UART4_Init(void);
static void MX_ADC2_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */





uint16_t getcolumnheight(){
	uint32_t sum = 0 ;
	for(uint16_t i = 0 ; i < ADC_COUNT ; i++){
		sum = sum + adcValueAmpfiled[i];
	}

	sum = sum / ADC_COUNT;
	sum = sum / 444;

	return sum;
}

void writeLed() {
	uint16_t res = 50;

	uint16_t temp[COLOR_COUNT *8 +res+20];
	uint16_t i, j, k ;

	for(k=0 ; k<20; k++){
		temp[k]=0b0000000000000000;
	}

	for (i = 0 ; i < COLOR_COUNT; i++) {
		for (j = 0; j < 8; j++) {
			if ((colors[i] & (128 >> j)) != 0) {
				temp[i * 8 + j+20] = 0b1111111110000000;
			} else
				temp[i * 8 + j+20] = 0b1111100000000000;
		}
	}

for(k=COLOR_COUNT*8+20 ; k<COLOR_COUNT*8 +res+20; k++){
	temp[k]=0b0000000000000000;
}

	HAL_SPI_Transmit_DMA(&hspi2,temp,COLOR_COUNT*8+res+20);
	//HAL_SPI_Transmit(&hspi2,temp,COLOR_COUNT*8+res,COLOR_COUNT*8*1000000);


}

void resetLed(){
	uint8_t temp[500];
	uint16_t i;
	for(i=0;i<500;i++){
		temp[i]=0;
	}
	HAL_SPI_Transmit(&hspi2,temp,500,100000);



}


void setColor(uint8_t g, uint8_t r, uint8_t b)
{
	uint8_t t;
	  for(t = 0; t < COLOR_COUNT; t++)
	  {
		  if(t%3 == 0)
		  {
			  colors[t] = g;
		  }
		  if(t%3 == 1)
		  {
			  colors[t] = r;
		  }
		  if(t%3 == 2)
		  {
			  colors[t] = b;
		  }
	  }

}

void setLedColorAt(int index,uint8_t g, uint8_t r, uint8_t b)
{
	if(index < LED_COUNT)
	  colors[index*3] = g;

	  colors[(index*3)+1] = r;

	  colors[(index*3)+2] = b;
}


void setColumColorNewPanel(int index,int fill,uint8_t g, uint8_t r, uint8_t b)
{
	uint8_t t = 0;
	if(t<fill)
	{
		colors[0+(index*3)] = g;
		colors[1+(index*3)] = r;
		colors[2+(index*3)] = b;
		t++;
	}
	else
	{
		colors[0+(index*3)] = 0;
		colors[1+(index*3)] = 0;
		colors[2+(index*3)] = 0;
	}

	if(t<fill)
	{
		colors[(27*3)+0-(index*3)] = g;
		colors[(27*3)+1-(index*3)] = r;
		colors[(27*3)+2-(index*3)] = b;
		t++;
	}
	else
	{
		colors[(27*3)+0-(index*3)] = 0;
		colors[(27*3)+1-(index*3)] = 0;
		colors[(27*3)+2-(index*3)] = 0;
	}

	if(t<fill)
	{
		colors[(28*3)+0+(index*3)] = g;
		colors[(28*3)+1+(index*3)] = r;
		colors[(28*3)+2+(index*3)] = b;
		t++;
	}
	else
	{
		colors[(28*3)+0+(index*3)] = 0;
		colors[(28*3)+1+(index*3)] = 0;
		colors[(28*3)+2+(index*3)] = 0;
	}

	if(t<fill)
	{
		colors[(55*3)+0-(index*3)] = g;
		colors[(55*3)+1-(index*3)] = r;
		colors[(55*3)+2-(index*3)] = b;
		t++;
	}
	else
	{
		colors[(55*3)+0-(index*3)] = 0;
		colors[(55*3)+1-(index*3)] = 0;
		colors[(55*3)+2-(index*3)] = 0;
	}

	if(t<fill)
	{
		colors[(56*3)+0+(index*3)] = g;
		colors[(56*3)+1+(index*3)] = r;
		colors[(56*3)+2+(index*3)] = b;
		t++;
	}
	else
	{
		colors[(56*3)+0+(index*3)] = 0;
		colors[(56*3)+1+(index*3)] = 0;
		colors[(56*3)+2+(index*3)] = 0;
	}


	if(t<fill)
	{
		colors[(83*3)+0-(index*3)] = g;
		colors[(83*3)+1-(index*3)] = r;
		colors[(83*3)+2-(index*3)] = b;
		t++;
	}
	else
	{
		colors[(83*3)+0-(index*3)] = 0;
		colors[(83*3)+1-(index*3)] = 0;
		colors[(83*3)+2-(index*3)] = 0;
	}


}


void setColumColor(int index,int full,uint8_t g, uint8_t r, uint8_t b)
{
	int act = 0;
	for(act = 0; act < full ; act++)
	{
		  colors[(index*27 + act*3)] = g;

		  colors[(index*27 + act*3)+1] = r;

		  colors[(index*27 + act*3)+2] = b;

	}

	for(act = full; act < 9 ; act++)
	{
		  colors[(index*27 + act*3)] = 0;

		  colors[(index*27 + act*3)+1] = 0;

		  colors[(index*27 + act*3)+2] = 0;

	}

}

void fft_transform(){
	adc_value_min = 2000;
	for(uint16_t i = 0; i<ADC_COUNT; i++)
		{
			uint16_t adc_value_ampfiled = 3;


			if(adcValue[i]<adc_value_min){
				adc_value_min = adcValue[i];
			}

			if(adcValue[i]<1850){
				adc_value_ampfiled = 2;
			}
			if(adcValue[i]>2200){
						adc_value_ampfiled = 4096;
			}
			if(adcValue[i]>1849 && adcValue[i]<2201){
						adc_value_ampfiled = (adcValue[i]*12)-22200 ;
			}

			adcValueAmpfiled[i]=adc_value_ampfiled;
		}




	colheight = getcolumnheight();
	if(colheight > 2){
		colheight = colheight -3;
	}

	setColumColorNewPanel(0,colheight+1,0,0,8);
	setColumColorNewPanel(1,colheight,0,2,8);

	setColumColorNewPanel(2,colheight-1,0,4,8);
	setColumColorNewPanel(3,(colheight-2)*1.5,0,6,8);

	setColumColorNewPanel(4,(colheight-2)*2,0,10,8);
	setColumColorNewPanel(5,colheight-1,0,15,8);

	setColumColorNewPanel(6,colheight+1,0,18,5);
	setColumColorNewPanel(7,colheight+1,0,18,5);

	setColumColorNewPanel(8,colheight-1,0,15,8);
	setColumColorNewPanel(9,(colheight-2)*2,0,10,8);

	setColumColorNewPanel(10,(colheight-2)*1.5,0,6,8);
	setColumColorNewPanel(11,colheight-1,0,4,8);

	setColumColorNewPanel(12,colheight,0,2,8);
	setColumColorNewPanel(13,colheight+1,0,0,8);

/*
	setColumColor(0,colheight,2,0,2);

	setColumColor(1,colheight,2,0,2);
	setColumColor(2,colheight,2,0,2);
	setColumColor(3,colheight,2,0,2);
	setColumColor(4,colheight,2,0,2);
	setColumColor(5,colheight,2,0,2);
*/


	//setColor(2,0,1);
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);



}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi){

	HAL_ADC_Start_DMA(&hadc1, adcValue, ADC_COUNT);

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	HAL_ADC_Stop_DMA(&hadc1);
	writeLed();
	fft_transform();

}


/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */



  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_ADC1_Init();
  MX_UART4_Init();
  MX_ADC2_Init();

  /* USER CODE BEGIN 2 */
  	  //HAL_DMA_RegisterCallback(&hdma_spi2_tx,HAL_DMA_XFER_CPLT_CB_ID, spidmacallback);
  	  HAL_ADC_Start_DMA(&hadc1, adcValue, ADC_COUNT);
  	  setColor(0,0,0);




  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

		//sprintf(printbuffer, "AD=%d\r\n", erosites/360);
		//HAL_UART_Transmit(&huart4, printbuffer, strlen(printbuffer), 5000);



	}
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 51;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* ADC2 init function */
static void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = ENABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* SPI2 init function */
static void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }

}

/* UART4 init function */
static void MX_UART4_Init(void)
{

  huart4.Instance = UART4;
  huart4.Init.BaudRate = 250000;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA3   ------> USART2_RX
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
	}
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
