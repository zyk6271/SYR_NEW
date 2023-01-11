/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-07-01     Rick       the first version
 */
#include <stm32l4xx.h>
#include "ADCwork.h"
#include "rtthread.h"
#include "rtdevice.h"
#include "flashwork.h"
#include "moto.h"
#include "pin_config.h"

#define DBG_TAG "adc"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

uint32_t adc_value[3];

rt_timer_t Moto_overload_delay_timer = RT_NULL;

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */
    /** Common config
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV256;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.NbrOfConversion = 3;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {

    }
    /** Configure Analog WatchDog 1
    */
    AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
    AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
    AnalogWDGConfig.Channel = ADC_CHANNEL_16;
    AnalogWDGConfig.ITMode = DISABLE;
    AnalogWDGConfig.HighThreshold = 45;
    AnalogWDGConfig.LowThreshold = 0;
    if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
    {

    }
    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {

    }
    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_3;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {

    }
    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_16;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {

    }
    /* USER CODE BEGIN ADC1_Init 2 */

    HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &adc_value, 3);
    /* USER CODE END ADC1_Init 2 */
}
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
}
void ADC_DMA_Init(void)
{
    __HAL_RCC_ADC_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
}
void ADC_DMA_DeInit(void)
{
    __HAL_RCC_ADC_CLK_DISABLE();
    __HAL_RCC_DMA1_CLK_DISABLE();
}
void ADC1_IRQHandler(void)
{
  /* USER CODE BEGIN ADC1_IRQn 0 */
    Disable_MotoINT();
  /* USER CODE END ADC1_IRQn 0 */
    HAL_ADC_IRQHandler(&hadc1);
  /* USER CODE BEGIN ADC1_IRQn 1 */
    Moto_Current_Detect();
  /* USER CODE END ADC1_IRQn 1 */
}
uint32_t Get_Moto_Current(void)
{
    return adc_value[2];
}
uint32_t Get_BAT_Voltage(void)
{
    return adc_value[1];
}
uint32_t Get_DC_Voltage(void)
{
    return adc_value[0];
}
void Enable_MotoINT(void)
{
    rt_timer_start(Moto_overload_delay_timer);
}
void Disable_MotoINT(void)
{
    rt_timer_stop(Moto_overload_delay_timer);
    LL_ADC_ClearFlag_AWD1(hadc1.Instance);
    LL_ADC_DisableIT_AWD1(hadc1.Instance);
}
void Moto_overload_delay_timer_callback(void *parameter)
{
    LL_ADC_ClearFlag_AWD1(hadc1.Instance);
    LL_ADC_EnableIT_AWD1(hadc1.Instance);
}
void ADC_Init(void)
{
    MX_DMA_Init();
    MX_ADC1_Init();
    Moto_overload_delay_timer = rt_timer_create("Moto_overload_delay_timer",Moto_overload_delay_timer_callback,RT_NULL,500,RT_TIMER_FLAG_ONE_SHOT|RT_TIMER_FLAG_SOFT_TIMER);
}
uint8_t Get_DC_Level(void)
{
    uint32_t value = Get_DC_Voltage();
    LOG_D("Get_DC_Voltage is %ld\r\n",value);
    return value>2800?1:0;
}
uint32_t Get_DC_Value(void)
{
    uint32_t value = Get_DC_Voltage();
    LOG_D("DC Value is %ld\r\n",value);
    return value;
}
uint32_t Get_Bat_Value(void)
{
    uint32_t value = Get_BAT_Voltage();
    LOG_D("BAT Value is %ld\r\n",value);
    return value;
}
