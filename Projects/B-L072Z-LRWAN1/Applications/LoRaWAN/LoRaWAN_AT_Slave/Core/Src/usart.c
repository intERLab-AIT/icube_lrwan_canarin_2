/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef hlpuart1;
DMA_HandleTypeDef hdma_lpuart1_tx;

/* LPUART1 init function */

void MX_LPUART1_Init(void)
{
  hlpuart1.Instance = USARTx;
  hlpuart1.Init.BaudRate = UART_BAUDRATE;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;

  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if (uartHandle->Instance == USARTx)
  {
    /* USER CODE BEGIN LPUART1_MspInit 0 */

    /* USER CODE END LPUART1_MspInit 0 */
    /** Initializes the peripherals clocks
      */
#ifdef CANARIN_4
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
#else
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
    PeriphClkInitStruct.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_HSI;
#endif
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    USARTx_CLK_ENABLE();

    /* LPUART1 clock enable */
    USARTx_RX_GPIO_CLK_ENABLE();
    USARTx_TX_GPIO_CLK_ENABLE();

    /**LPUART1 GPIO Configuration
    PA3     ------> LPUART1_RX
    PA2     ------> LPUART1_TX
      */
    GPIO_InitStruct.Pin = USARTx_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;
    HAL_GPIO_Init(USARTx_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USARTx_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;
    HAL_GPIO_Init(USARTx_TX_GPIO_Port, &GPIO_InitStruct);

    /* LPUART1 DMA Init */
    /* LPUART1_TX Init */
    hdma_lpuart1_tx.Instance                 = USARTx_TX_DMA_CHANNEL;
    hdma_lpuart1_tx.Init.Request             = USARTx_TX_DMA_REQUEST;
    hdma_lpuart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_lpuart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_lpuart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_lpuart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_lpuart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_lpuart1_tx.Init.Mode = DMA_NORMAL;
    hdma_lpuart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_lpuart1_tx) != HAL_OK)
    {
      Error_Handler();
    }


    __HAL_LINKDMA(uartHandle, hdmatx, hdma_lpuart1_tx);

    /* LPUART1 interrupt Init */
    HAL_NVIC_SetPriority(USARTx_IRQn, USARTx_Priority, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
    /* USER CODE BEGIN LPUART1_MspInit 1 */

    /* USER CODE END LPUART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

  if (uartHandle->Instance == USARTx)
  {
    /* USER CODE BEGIN LPUART1_MspDeInit 0 */

    /* USER CODE END LPUART1_MspDeInit 0 */
    /* Peripheral clock disable */
	USARTx_CLK_DISABLE();

    /**LPUART1 GPIO Configuration
    PA3     ------> LPUART1_RX
    PA2     ------> LPUART1_TX
      */
    HAL_GPIO_DeInit(GPIOA, USARTx_RX_Pin | USARTx_TX_Pin);

    /* LPUART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* LPUART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
    /* USER CODE BEGIN LPUART1_MspDeInit 1 */

    /* USER CODE END LPUART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
