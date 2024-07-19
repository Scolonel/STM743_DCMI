/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */

  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // секунды до 2000 года

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 10;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JUNE;
  sDate.Date = 28;
  sDate.Year = 24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
// здесь может быть код перевода времени в разные форматы
/**
  * @brief  Backup save parameter 
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                         specify the register.
  * @param  Data:    Data to be written in the specified RTC Backup data register.
  * @retval None
  */
void k_BkupSaveParameter(uint32_t address, uint32_t data)
{
  HAL_RTCEx_BKUPWrite(&hrtc,address,data);  
}

/**
  * @brief  Backup restore parameter. 
  * @param  address: RTC Backup data Register number.
  *                  This parameter can be: RTC_BKP_DRx where x can be from 0 to 19 to 
  *                                         specify the register. 
  * @retval None
  */
uint32_t k_BkupRestoreParameter(uint32_t address)
{
   return HAL_RTCEx_BKUPRead(&hrtc,address);  
}

/**
  * @brief  RTC Get time. 
  * @param  Time: Pointer to Time structure
  * @retval None
  */
void k_GetTime(  RTC_TimeTypeDef *Time)
{
   HAL_RTC_GetTime(&hrtc, Time, FORMAT_BIN);
}

/**
  * @brief  RTC Set time. 
  * @param  Time: Pointer to Time structure
  * @retval None
  */
void k_SetTime(  RTC_TimeTypeDef *Time)
{
   Time->StoreOperation = 0;
   Time->SubSeconds = 0;
   Time->DayLightSaving = 0;
   HAL_RTC_SetTime(&hrtc, Time, FORMAT_BIN);
}

/**
  * @brief  RTC Get date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void k_GetDate(  RTC_DateTypeDef *Date)
{
   HAL_RTC_GetDate(&hrtc, Date, FORMAT_BIN);
   
   if((Date->Date == 0) || (Date->Month == 0))
   {
     Date->Date = Date->Month = 1;
   }    
}

/**
  * @brief  RTC Set date
  * @param  Date: Pointer to Date structure
  * @retval None
  */
void k_SetDate(  RTC_DateTypeDef *Date)
{
   HAL_RTC_SetDate(&hrtc, Date, FORMAT_BIN);
}


uint32_t get_fattime_RTC (void)
{
  uint32_t res;
  RTC_DateTypeDef          Date;  
  RTC_TimeTypeDef          Time; 
  k_GetDate(&Date);  
  k_GetTime(&Time);   
  
  
  //res =  (((uint32_t)rtcdate.RTC_Year - 1980) << 25) // oae auei e ienaei a aia 2061
  res =  (((uint32_t)(Date.Year + 20 ) ) << 25)// так как храним год более 2000
    | ((uint32_t)Date.Month << 21)
      | ((uint32_t)Date.Date << 16)
        | (uint16_t)(Time.Hours << 11)
          | (uint16_t)(Time.Minutes << 5)
			| (uint16_t)(Time.Seconds >> 1);
  
  return res;
}

uint32_t TotalSec( void) // перевод текущего времени в секунды
{
  uint32_t secs;
  RTC_DateTypeDef          Datas;  
  RTC_TimeTypeDef          Time; 
  k_GetDate(&Datas);  
  k_GetTime(&Time);   

  int Year;
  unsigned int Month;
  unsigned int Date, days;
  unsigned int Hour;
  unsigned int Minut;
  unsigned int Sec;
  unsigned int _Days[]={31,28,31,30,31,30,31,31,30,31,30,31};
  days = 0;
  secs = 0;
  Sec = Time.Seconds; 
  Minut = Time.Minutes; 
  Hour = Time.Hours; 
  Date = Datas.Date; 
  Month = Datas.Month; 
 if ((Month > 12) || (Month == 0)) Month =5; 
 Year = Datas.Year%100 ;//+ 14; 
 	if((!((Year) % 4))&&(Month>2)) days++;
        
	if (Year > 99) Year = 1;

	Year--;
        
	while(Year>=0)
	{
		if(!((Year)%4))days+=366;
		else days+=365;
		Year--;
	}
	while(Month-1>0)
		days+=_Days[(--Month)-1];

	days+=Date-1;
	
	secs=days*24*3600+Hour*3600+Minut*60+Sec;
        secs = secs + 946684800; // 2000aia naeoia
//(946684800)

    return(secs);
}

void Sec2Date( uint32_t TimeSec,  RTC_DateTypeDef *Dates, RTC_TimeTypeDef *Time) // перевод секунд в структуры времени
{

  unsigned int _Days[]={31,28,31,30,31,30,31,31,30,31,30,31};
  
  int Year = 2000;
  int Month;
  int Day;
  
  int Hour;
  int Minute;
  int Second;
  
  int tmp;
  
  TimeSec = TimeSec - Y2000Sec; // вычитаем секунды до 2000 года
  TimeSec = TimeSec - DaySec; // вычитаем секунды высокосного дня 2000 года
  if (TimeSec>0x80000000) TimeSec = 10*YearSecV-1; // зачем это не понятно???
  tmp = YearSec;
  while(TimeSec > tmp)
  {
    TimeSec = TimeSec - tmp;
    Year++;
    if(Year%4) tmp = YearSec;
    else tmp = YearSecV;
  }
  // получили год 
  Dates->Year = Year - 2000;
  Month = 1;

  while(TimeSec>_Days[Month-1]*DaySec)
  {
    TimeSec -= _Days[Month-1]*DaySec;
    Month++;
  }
  if((Month>1)&&(!(Year%4))) TimeSec -=DaySec;
  // Mounth
  Dates->Month = Month;
  
  Day = 1;
  
  while(TimeSec>DaySec)
  {
    TimeSec -= DaySec;
    Day++;    
  }
  // Day 
  Dates->Date = Day;
  
  Hour = TimeSec/3600;
  Time->Hours = Hour;

  TimeSec -= Hour*3600;
  
  Minute = TimeSec/60;
  Time->Minutes = Minute;
  
  TimeSec -= Minute*60;
  
  Second = TimeSec;
  Time->Seconds = Second;
  

  return;
}

/* USER CODE END 1 */
