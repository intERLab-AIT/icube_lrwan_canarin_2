/**
  ******************************************************************************
  * @file    test_rf.c
  * @author  MCD Application Team
  * @brief   manages tx tests
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
#include "platform.h"
#include "sys_app.h"
#include "test_rf.h"
#include "radio.h"
#include "stm32_seq.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define F_868MHz                      868000000
#define P_14dBm                       14
#define P_22dBm                       22
#define SF12                          12
#define CR4o5                         1
#define EMISSION_POWER                P_14dBm
#define CONTIUNUOUS_TIMEOUT           0xFFFF
#define LORA_PREAMBLE_LENGTH          8         /* Same for Tx and Rx */
#define LORA_SYMBOL_TIMEOUT           30        /* Symbols */
#define TX_TIMEOUT_VALUE              3000
#define LORA_FIX_LENGTH_PAYLOAD_OFF   false
#define LORA_IQ_INVERSION_OFF         false
#define TX_TEST_TONE                  (1<<0)
#define RX_TEST_RSSI                  (1<<1)
#define TX_TEST_LORA                  (1<<2)
#define RX_TEST_LORA                  (1<<3)
#define RX_TIMEOUT_VALUE              5000
#define RX_CONTINUOUS_ON              1
#define PRBS9_INIT                    ( ( uint16_t) 2 )
#define DEFAULT_PAYLOAD_LEN           16
#define DEFAULT_LDR_OPT               2
#define DEFAULT_FSK_DEVIATION         25000
#define DEFAULT_GAUSS_BT              3 /*Lora default in legacy*/
#define FSK_BANDWIDTH                 50000
#define FSK_DATARATE                  50*1000
#define FSK_BANDWIDTHAFC              83333

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t TestState = 0;

static testParameter_t testParam = { TEST_LORA, F_868MHz, EMISSION_POWER, BW_125kHz, SF12, CR4o5, 0, 0, DEFAULT_PAYLOAD_LEN, DEFAULT_FSK_DEVIATION, DEFAULT_LDR_OPT, DEFAULT_GAUSS_BT};

static __IO uint32_t RadioTxDone_flag = 0;
static __IO uint32_t RadioTxTimeout_flag = 0;
static __IO uint32_t RadioRxDone_flag = 0;
static __IO uint32_t RadioRxTimeout_flag = 0;
static __IO uint32_t RadioError_flag = 0;
static __IO int16_t last_rx_rssi = 0;
static __IO int8_t last_rx_snr = 0;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/*!
 * \brief Generates a PRBS9 sequence
 */
static int32_t Prbs9_generator(uint8_t *payload, uint8_t len);
/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void);

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t TST_TxTone(void)
{
  /* USER CODE BEGIN TST_TxTone_1 */

  /* USER CODE END TST_TxTone_1 */
  if ((TestState & TX_TEST_TONE) != TX_TEST_TONE)
  {
    TestState |= TX_TEST_TONE;

    APP_TPRINTF("Tx FSK Test\r\n");

    Radio.SetTxContinuousWave(testParam.freq, testParam.power, CONTIUNUOUS_TIMEOUT);

    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_TxTone_2 */

  /* USER CODE END TST_TxTone_2 */
}

int32_t TST_RxRssi(void)
{
  /* USER CODE BEGIN TST_RxRssi_1 */

  /* USER CODE END TST_RxRssi_1 */
  uint32_t timeout = 0;
  int16_t rssiVal = 0;

  /* Test with LNA */
  /* check that test is not already started*/
  if ((TestState & RX_TEST_RSSI) != RX_TEST_RSSI)
  {
    TestState |= RX_TEST_RSSI;

    APP_TPRINTF("Rx FSK Test\r\n");

    Radio.SetChannel(testParam.freq);

    /* RX Continuous */
    Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                      0, FSK_BANDWIDTHAFC, LORA_PREAMBLE_LENGTH,
                      LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_OFF,
                      0, true, 0, 0, LORA_IQ_INVERSION_OFF, true);

    timeout = 0xFFFFFF; /* continuous Rx */
    if (testParam.lna == 0)
    {
      Radio.Rx(timeout);
    }
    else
    {
      Radio.RxBoosted(timeout);
    }

    HAL_Delay(Radio.GetWakeupTime());   /* Wait for 50ms */

    rssiVal = Radio.Rssi(MODEM_FSK);
    APP_TPRINTF(">>> RSSI Value= %d dBm\r\n", rssiVal);

    Radio.Sleep();
    TestState &= ~RX_TEST_RSSI;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_RxRssi_2 */

  /* USER CODE END TST_RxRssi_2 */
}

int32_t  TST_set_config(testParameter_t *Param)
{
  /* USER CODE BEGIN TST_set_config_1 */

  /* USER CODE END TST_set_config_1 */
  UTIL_MEM_cpy_8(&testParam, Param, sizeof(testParameter_t));

  return 0;
  /* USER CODE BEGIN TST_set_config_2 */

  /* USER CODE END TST_set_config_2 */
}

int32_t TST_get_config(testParameter_t *Param)
{
  /* USER CODE BEGIN TST_get_config_1 */

  /* USER CODE END TST_get_config_1 */
  UTIL_MEM_cpy_8(Param, &testParam, sizeof(testParameter_t));
  return 0;
  /* USER CODE BEGIN TST_get_config_2 */

  /* USER CODE END TST_get_config_2 */
}

int32_t TST_stop(void)
{
  /* USER CODE BEGIN TST_stop_1 */

  /* USER CODE END TST_stop_1 */
  TestState = 0;

  /* Set the radio in Sleep*/
  Radio.Sleep();

  return 0;
  /* USER CODE BEGIN TST_stop_2 */

  /* USER CODE END TST_stop_2 */
}

int32_t TST_TX_Start(int32_t nb_packet)
{
  /* USER CODE BEGIN TST_TX_Start_1 */

  /* USER CODE END TST_TX_Start_1 */
  int32_t i;
  uint8_t payload[256] = {0};

  if ((TestState & TX_TEST_LORA) != TX_TEST_LORA)
  {
    TestState |= TX_TEST_LORA;

    APP_TPRINTF("Tx LoRa Test\r\n");

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);
    /*Fill payload with PRBS9 data*/
    Prbs9_generator(payload, testParam.payloadLen);

    /* Launch several times payload: nb times given by user */
    for (i = 1; i <= nb_packet; i++)
    {
      APP_TPRINTF("Tx %d of %d\r\n", i, nb_packet);
      Radio.SetChannel(testParam.freq);
      Radio.SetTxConfig(MODEM_LORA, testParam.power, 0, (testParam.bandwidth - 4),
                        testParam.loraSf_datarate, testParam.codingRate,
                        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_OFF,
                        false, 0, 0, LORA_IQ_INVERSION_OFF, 3000);

      /* Send payload once*/
      Radio.Send(payload, testParam.payloadLen);
      /* Wait Tx done/timeout */
      UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
      Radio.Sleep();

      if (RadioTxDone_flag == 1)
      {
        APP_TPRINTF("OnTxDone\r\n");
      }

      if (RadioTxTimeout_flag == 1)
      {
        APP_TPRINTF("OnTxTimeout\r\n");
      }

      if (RadioError_flag == 1)
      {
        APP_TPRINTF("OnRxError\r\n");
      }

      /*Delay between 2 consecutive Tx*/
      HAL_Delay(500);
      /* Reset TX Done or timeout flags */
      RadioTxDone_flag = 0;
      RadioTxTimeout_flag = 0;
      RadioError_flag = 0;
    }
    TestState &= ~TX_TEST_LORA;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_TX_Start_2 */

  /* USER CODE END TST_TX_Start_2 */
}

int32_t TST_RX_Start(int32_t nb_packet)
{
  /* USER CODE BEGIN TST_RX_Start_1 */

  /* USER CODE END TST_RX_Start_1 */
  int32_t i;
  /* init of PER counter */
  uint32_t count_RxOk = 0;
  uint32_t count_RxKo = 0;
  uint32_t PER = 0;

  if (((TestState & RX_TEST_LORA) != RX_TEST_LORA) && (nb_packet > 0))
  {
    TestState |= RX_TEST_LORA;

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);

    for (i = 1; i <= nb_packet; i++)
    {
      /* Rx config */
      Radio.SetChannel(testParam.freq);
      Radio.SetRxConfig(MODEM_LORA, (testParam.bandwidth - 4), testParam.loraSf_datarate,
                        testParam.codingRate, 0, LORA_PREAMBLE_LENGTH,
                        LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_OFF,
                        0, false, 0, 0, LORA_IQ_INVERSION_OFF, true);
      Radio.Rx(RX_TIMEOUT_VALUE);

      /* Wait Rx done/timeout */
      UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
      Radio.Sleep();

      if (RadioRxDone_flag == 1)
      {
        int16_t rssi = last_rx_rssi;
        int8_t snr = last_rx_snr;
        APP_TPRINTF("OnRxDone\r\n");
        APP_TPRINTF("RssiValue=%d dBm, SnrValue=%d\r\n", rssi, snr);
      }

      if (RadioRxTimeout_flag == 1)
      {
        APP_TPRINTF("OnRxTimeout\r\n");
      }

      if (RadioError_flag == 1)
      {
        APP_TPRINTF("OnRxError\r\n");
      }

      /*check flag*/
      if ((RadioRxTimeout_flag == 1) || (RadioError_flag == 1))
      {
        count_RxKo++;
      }
      if (RadioRxDone_flag == 1)
      {
        count_RxOk++;
      }
      /* Reset timeout flag */
      RadioRxDone_flag = 0;
      RadioRxTimeout_flag = 0;
      RadioError_flag = 0;

      /* Compute PER */
      PER = (100 * (count_RxKo)) / (count_RxKo + count_RxOk);
      APP_TPRINTF("Rx %d of %d  >>> PER= %d %%\r\n", i, nb_packet, PER);
    }
    TestState &= ~RX_TEST_LORA;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_RX_Start_2 */

  /* USER CODE END TST_RX_Start_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone_1 */

  /* USER CODE END OnTxDone_1 */
  /* Set TxDone flag */
  RadioTxDone_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnTxDone_2 */

  /* USER CODE END OnTxDone_2 */
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  /* USER CODE BEGIN OnRxDone_1 */

  /* USER CODE END OnRxDone_1 */
  last_rx_rssi = rssi;
  last_rx_snr = snr;

  /* Set Rxdone flag */
  RadioRxDone_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxDone_2 */

  /* USER CODE END OnRxDone_2 */
}

void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout_1 */

  /* USER CODE END OnTxTimeout_1 */
  /* Set timeout flag */
  RadioTxTimeout_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnTxTimeout_2 */

  /* USER CODE END OnTxTimeout_2 */
}

void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout_1 */

  /* USER CODE END OnRxTimeout_1 */
  /* Set timeout flag */
  RadioRxTimeout_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxTimeout_2 */

  /* USER CODE END OnRxTimeout_2 */
}

void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError_1 */

  /* USER CODE END OnRxError_1 */
  /* Set error flag */
  RadioError_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxError_2 */

  /* USER CODE END OnRxError_2 */
}

static int32_t Prbs9_generator(uint8_t *payload, uint8_t len)
{
  /* USER CODE BEGIN Prbs9_generator_1 */

  /* USER CODE END Prbs9_generator_1 */
  uint16_t prbs9_val = PRBS9_INIT;

  for (int32_t i = 0; i < len * 8; i++)
  {
    /*fill buffer with prbs9 sequence*/
    int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
    prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
    payload[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
  }
  return 0;
  /* USER CODE BEGIN Prbs9_generator_2 */

  /* USER CODE END Prbs9_generator_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
