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
#include "system.h"

/* USER CODE BEGIN 0 */

  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // ������� �� 2000 ����

RTCTime TimeSaveOTDR;
RTCTime TimeSaveOLT;

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
// ����� ����� ���� ��� �������� ������� � ������ �������
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
//������� �� t7kAR
// ��������� �������� �������, ������ ��������, ������������ � ���������
RTCTime RTCGetTime( void )
{
  RTC_TimeTypeDef CsTime = {0};
  RTC_DateTypeDef CsDate = {0};
  
  RTCTime LocalTime; // �������� ���������
  // ������� ������� �����
  k_GetDate(&CsDate);
  k_GetTime(&CsTime);
  
  LocalTime.RTC_Sec = CsTime.Seconds;
  LocalTime.RTC_Min = CsTime.Minutes;
  LocalTime.RTC_Hour = CsTime.Hours;
  LocalTime.RTC_Mday = CsDate.Date;
  LocalTime.RTC_Wday = CsDate.WeekDay;
  LocalTime.RTC_Yday = 200;
  LocalTime.RTC_Mon = CsDate.Month;
  LocalTime.RTC_Year = CsDate.Year;
  return ( LocalTime );
}
 

uint32_t get_fattime_RTC (void)
{
  uint32_t res;
  RTC_DateTypeDef          Date;  
  RTC_TimeTypeDef          Time; 
  k_GetDate(&Date);  
  k_GetTime(&Time);   
  
  
  //res =  (((uint32_t)rtcdate.RTC_Year - 1980) << 25) // oae auei e ienaei a aia 2061
  res =  (((uint32_t)(Date.Year + 20 ) ) << 25)// ��� ��� ������ ��� ����� 2000
    | ((uint32_t)Date.Month << 21)
      | ((uint32_t)Date.Date << 16)
        | (uint16_t)(Time.Hours << 11)
          | (uint16_t)(Time.Minutes << 5)
            | (uint16_t)(Time.Seconds >> 1);
  
  return res;
}

unsigned int TotalSec( RTCTime CurrentTime) // ������� ������ ������� � ���
{
  int Year;
  unsigned int Month;
  unsigned int Date, days;
  unsigned int Hour;
  unsigned int Minut;
  unsigned int Sec, secs;
  unsigned int _Days[]={31,28,31,30,31,30,31,31,30,31,30,31};
  days = 0;
  secs = 0;
  Sec = CurrentTime.RTC_Sec; 
  Minut = CurrentTime.RTC_Min; 
  Hour = CurrentTime.RTC_Hour; 
  Date = CurrentTime.RTC_Mday; 
  Month = CurrentTime.RTC_Mon; 
  if ((Month > 12) || (Month == 0)) Month = 5; 
  Year = CurrentTime.RTC_Year%100; 
  if((!((Year) % 4))&&(Month>2)) days++;
  
  if (Year > 32) Year = 19; // ��� ������ ���� �������� 2019 (���������)
  
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
  secs = secs + 946684800; // 2000��� ������
  //(946684800)
  
  return(secs);
  
  
}

void Sec2Date( unsigned long TimeSec, RTCTime* CurrentTime) // ������� ������ � ����
{
  
  unsigned int _Days[]={31,28,31,30,31,30,31,31,30,31,30,31};
  
  int Year = 2000;
  int Month;
  int Day;
  
  int Hour;
  int Minute;
  int Second;
  
  int tmp;
  
  TimeSec = TimeSec - Y2000Sec; // �������� ������� �� 2000 ����
  TimeSec = TimeSec - DaySec; // �������� ������� ����������� ��� 2000 ����
  
  if (TimeSec>0x80000000) TimeSec = 10*YearSecV-1; // ��� ���� ������� ������� ������ 19 ������ 2038
  tmp = YearSec;
  while(TimeSec > tmp)
  {
    TimeSec = TimeSec - tmp;
    Year++;
    if(Year%4) tmp = YearSec;
    else tmp = YearSecV;
  }
  // �������� ���
  CurrentTime->RTC_Year = Year;
  Month = 1;
  
  while(TimeSec>_Days[Month-1]*DaySec)
  {
    TimeSec -= _Days[Month-1]*DaySec;
    Month++;
  }
  // Mounth
  CurrentTime->RTC_Mon = Month;
  
  Day = 1;
  
  while(TimeSec>DaySec)
  {
    TimeSec -= DaySec;
    Day++;    
  }
  // Day 
  CurrentTime->RTC_Mday = Day;
  
  Hour = TimeSec/3600;
  CurrentTime->RTC_Hour = Hour;
  
  TimeSec -= Hour*3600;
  
  Minute = TimeSec/60;
  CurrentTime->RTC_Min = Minute;
  
  TimeSec -= Minute*60;
  
  Second = TimeSec;
  CurrentTime->RTC_Sec = Second;
  
  
  return;
}

/* USER CODE END 1 */
