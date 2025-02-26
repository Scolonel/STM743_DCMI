/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "adc.h"
#include "dcmi.h"
#include "system.h"
#include "usbd_cdc_acm_if.h"
//#include "dac.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
unsigned beepTick = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_dcmi;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;
extern DMA_HandleTypeDef hdma_uart5_tx;
extern DMA_HandleTypeDef hdma_uart7_tx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart7;
extern UART_HandleTypeDef huart3;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  // control USB 
  if(BusyUSB>1) BusyUSB--;
  else if(BusyUSB==1)
  {
    BusyUSB = 0;
    ModeUSB = 1; // надо выключить значек занятости USB
  }
  if(PresentUSB>1) PresentUSB--;
  else if(PresentUSB==1)
  {
    PresentUSB = 0;
    ModeUSB = 2; // надо выключить значек USB
  }
  // my control BEEP
  if (beepTick > 1)
  {
    --beepTick;
    HAL_GPIO_TogglePin(Beep_GPIO_Port, Beep_Pin);
  }
  else if(beepTick == 1)
  {
    beepTick = 0;
    HAL_GPIO_WritePin(Beep_GPIO_Port, Beep_Pin, GPIO_PIN_RESET);
  }
  // Timer for Accum
  if(EnaTimerAccum)
    CurrTimeAccum++;
  // контроль счетчика ожидания окончания приема
  if(CountTimerUart2) // подождали посмотрим что приняли
  {
    CountTimerUart2--;
    if(CountTimerUart2==0) // подождали посмотрим что приняли
    {
      Uart2DecYes = 1;// признак необходимости обработки принятой команды
      RecievNEX=END_UART; // время истекло, приняли что то, пока не принимаем, ждем обработку
      g_WtRdyNEX = 1; // ввзводим признак готовности данных, грубо окончание текущего приема
    }
  }
  
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */
  CntTx++;
  if(DMA1->LISR & DMA_LISR_TCIF0)
  {
    HAL_UART_DMAStop(&huart3);
    TxDMA=0;
  }
  /* USER CODE END DMA1_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */
  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */
  // остановка ДМА при накоплении с работой с внутренней АЦП
//  if(DMA1->LISR & DMA_LISR_TCIF1)
//  {
//    CurrCnt2Timer = TIM2->CNT;
//    HAL_ADC_Stop_DMA(&hadc1);
//    // можно притупить "основной" таймер для суммирования снятого
//    // и вызвать само суммирование
//    LED_Y(0); // закончили текущий цикл ДМА 
//    TIM1->CR1 &= ~TIM_CR1_CEN; // притупим таймер основной
//    EnaNextAvrg = 1;
//
//  }

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc1);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */
  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream2 global interrupt.
  */
void DMA1_Stream2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream2_IRQn 0 */
  //DMA1_Stream2->CR &= ~DMA_SxCR_EN; // запрещаем ДМА DCMI
    CountEndCMI++; 
  if(DMA1->LISR & DMA_LISR_TCIF2)
  {
    //DCMI->CR &= ~DCMI_CR_CAPTURE;
    //while(DCMI->CR&0X01);               // Waiting for transmission to complete
    //DMA1_Stream2->CR &= ~DMA_SxCR_EN; // выключаем цикл ДМА for DCMI
    //DMA1_Stream2->CR |= DMA_SxCR_EN; // запускаем новый цикл ДМА for DCMI
    //TIM1->CR1 &= ~TIM_CR1_CEN; // притупим таймер основной
    CurrCnt2Timer = TIM1->CNT;
    //HAL_DCMI_Stop(&hdcmi);
    DMA1_Stream2->CR &=~DMA_SxCR_EN; // запрещаем  ДМА for DCMI
    
    // можно притупить "основной" таймер для суммирования снятого
    // и вызвать само суммирование
    //LED_Y(0); // закончили текущий цикл ДМА 
    DMA1->LIFCR |= (uint32_t)(DMA_LIFCR_CTCIF2);
    
    //EnaNextAvrg = 1;
    // возможно здесь не надо тормозить таймер, так как это втянем в цикл без выхода в остонвной цикл
    // чисто накопление бе отвлечений
    // ПОКА сюда попадаем когда закончисли цикл накопления и выйдем в основной цикл 
    // где подцепим признак продолжения накопления
    //TIM2->CR1 &= ~TIM_CR1_CEN; // притупим таймер основной
       // StopAllTIM(2);

  }
  /* USER CODE END DMA1_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_dcmi);
  /* USER CODE BEGIN DMA1_Stream2_IRQn 1 */
  /* USER CODE END DMA1_Stream2_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart7_tx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart5_tx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles TIM1 capture compare interrupt.
  */
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */

  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */
  if(TIM1->DIER & TIM_IT_CC1) // прерывание по "почти" окончании счета частоты повторения импульсов
    // здесь перестроим Таймеры на новые положения если надо
  {
    TIM1->CR1 &= ~TIM_CR1_CEN; // притупим таймер основной
    // можно тормознуть все остальные таймеры, и даже сбросить
    StopAllTIM(0);
    // сформируем сдвиг зондирующего импульса
    // и там же пробуем предустановить новый цикл ДМА АЦП
    CreateNextAvrg();      
    // продолжим тики основного генератора
    CountCC4++;
    
    TIM1->CR1 |= TIM_CR1_CEN; // снова разрешим
    //LED_Y(1); // начало одного накопления, старт текущего цикла ДМА
    
  }

  /* USER CODE END TIM1_CC_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
  // это запросы из вне по RS
      if (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_RXNE))
  {
  Dummy = (uint16_t)(huart3.Instance->RDR); // 
  RS_com((BYTE)Dummy); 
  }
  
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */
   HAL_NVIC_ClearPendingIRQ(USART3_IRQn);

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI15_10_IRQn 0 */
  if (__HAL_GPIO_EXTI_GET_IT(BUT_START_Pin) != 0x00U)
  {
//EnaStartRun = 1;
// из старой проги, признак нажатой кнопки "S"
              rawPressKeyS = 1;

CountKeyS++; // число нажатых кнопок
    
  }
  /* USER CODE END EXTI15_10_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(BUT_START_Pin);
  /* USER CODE BEGIN EXTI15_10_IRQn 1 */

  /* USER CODE END EXTI15_10_IRQn 1 */
}

/**
  * @brief This function handles TIM5 global interrupt.
  */
void TIM5_IRQHandler(void)
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
  if (__HAL_TIM_GET_FLAG(&htim5, TIM_FLAG_CC4) != RESET)
     Ena_SUMM=0;

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}

/**
  * @brief This function handles UART5 global interrupt.
  */
void UART5_IRQHandler(void)
{
  /* USER CODE BEGIN UART5_IRQn 0 */
  /* USER CODE END UART5_IRQn 0 */
  HAL_UART_IRQHandler(&huart5);
  /* USER CODE BEGIN UART5_IRQn 1 */
   HAL_NVIC_ClearPendingIRQ(UART5_IRQn);

  /* USER CODE END UART5_IRQn 1 */
}

/**
  * @brief This function handles UART7 global interrupt.
  */
void UART7_IRQHandler(void)
{
  /* USER CODE BEGIN UART7_IRQn 0 */
        /* Check if RXNE flag is set */
  // это ответы из индикатора
      if (__HAL_UART_GET_FLAG(&huart7, UART_FLAG_RXNE))
  {
  Dummy = (uint16_t)(huart7.Instance->RDR); // 
//  TRD = (uint8_t)(Dummy); 
//  if(ProgFW_LCD)
//  {
//     CDC_Transmit(0, (uint8_t*)(TRD), 1); // echo back on same channel
//  }
//  else // 
//  {
   RS_LCD((BYTE)Dummy); 
//  }
  }
  /* USER CODE END UART7_IRQn 0 */
  HAL_UART_IRQHandler(&huart7);
  /* USER CODE BEGIN UART7_IRQn 1 */
   HAL_NVIC_ClearPendingIRQ(UART7_IRQn);

  /* USER CODE END UART7_IRQn 1 */
}

/**
  * @brief This function handles USB On The Go FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */
   HAL_GPIO_TogglePin(KTS_GPIO_Port, KTS_Pin);

  PresentUSB = 1500; // признак что крутится USB - тики когда ничего нет 
  // приблизительно через секунду
  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */

  /* USER CODE END OTG_FS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
