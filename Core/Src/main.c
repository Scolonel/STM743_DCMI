/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "dac.h"
#include "dcmi.h"
#include "dma.h"
#include "fatfs.h"
#include "i2c.h"
#include "rtc.h"
#include "sdmmc.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usb_device.h"
#include "usbd_cdc_acm_if.h"
//#include "SysT7Knew.h"
#include "system.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//const uint16_t CodeDAC[64]={4095,64,4032,128,3968,192,3904,256,3840,320,3776,384,3712,448,3648,512,
//3584,576,3520,640,3456,704,3392,768,3328,832,3264,896,3200,960,3136,1024,
//3072,1088,3008,1152,2944,1216,2880,1280,2816,1344,2752,1408,2688,1472,2624,1536,
//2560,1600,2496,1664,2432,1728,2368,1792,2304,1856,2240,1920,2176,1984,2112,2048};
const uint16_t CodeDAC[SizeBuf_DAC]={0,4095,0,64,192,256,320,384,448,4095,//512,
576,640,704,768,832,896,960,4095,//1024,
1088,1152,1216,1280,1344,1408,1472,4095,//1536,
1600,1664,1728,1792,1856,1920,1984,4095,//2048
2112,2176,2240,2304,2368,2432,2496,4095,//2560,//2112,2176,
2624,2688,2752,2816,2880,2944,3008,4095,//3072,
3136,3200,3264,3328,3392,3456,3520,4095,//3584,
3648,3712,3776,3840,3904,3968,4032,4095,0,0,0,0,0,0,0,4095,0};//4095


//2048,2112,1984,2176,1920,2240,1856,2304,1792,2368,1728,2432,1664,2496,1600,2560,
//1536,2624,1472,2688,1408,2752,1344,2816,1280,2880,1216,2944,1152,3008,1088,3072,
//1024,3136,960,3200,896,3264,832,3328,768,3392,704,3456,640,3520,576,3584,
//512,3648,448,3712,384,3776,320,3840,256,3904,192,3968,128,4032,64,4095};

//const char HeaderBelcore[241] = {
//#include "head1.h"
//};

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t TxDMA = 0; // ������� ��������� DMA 
uint8_t TxBufAns[512]; // ������ �������� � USB
uint8_t  CntTx=0;
uint16_t BufADC[SizeBuf_ADC_int]; // ����� ����������� ��� (8), � ���� ����� ��� ����� DMA, ������ �� 8
uint16_t BufADD[SizeBuf_ADC]; // ����� 2���, � ���� ����� ��� ����� DMA, ������ �� 8192
uint32_t BufNAK[SizeBuf_ADC]; // ����� ����������, � ���� ��������� �� ������ ���
uint16_t LogData[SizeBuf_ADC]; // ����� ��������������� ������ ����������, � ���� ��������� �� ������ ���
uint32_t Cnt2Timer[128]; // ����� ������ 2 �� ��������� ����� ���
uint32_t CurrCnt2Timer; // ����� ������ 2 �� ��������� ����� ���
//static uint32_t MediumNoise = 0;
// test QSPI
uint8_t QSPI_TxBuf[]= "Hello world fromQSPI";
uint8_t QSPI_RxBuf[100];
// ���������� UART I2C
uint16_t Dummy; // ������ ������ �������� UART
uint32_t ErrCheck; // �������� ������ ������������
uint8_t N_LS=0; // ����� ������� � ������������ (��� �������� ���� �� ��������)
// �������� ��������� ���������� ��������
//
volatile uint32_t MaxADC = (1<<ResolutionADC)-1; // ������������ ������� ��� - �����������
uint8_t Str[64];

uint32_t BeginShift = 152; // ��������� �������� ��� �������� 
uint32_t BeginSet = 200; // ����� ��������� ������ ������ �������� ��� ��������� ������� ������������ (
uint32_t NumRepit = 8;// ����� ����������, ��� ������ �������� 
//� ���� �������� ��� ��������� � ������ ���� ��������
int SW_TIM1 = 1;
uint32_t CountCC4 = 0; // ����� ����������� ���������� �� TIM4_CH4
uint32_t CountEndCMI = 0; // ����� ���������� ����������� ������ DMA �� DCMI
uint32_t CountDMA = 0; // ����� ����������� ������ DMA 
uint32_t ZondImpulse = 20; // ������������ ������������ �������� � ����� 240 ��� (4.1666 ��)
uint32_t SizeBlockNak = 0; // ������� �������� ������� ����� ��� ( � ����������� �� ������������)
uint32_t SizeBlockNakVar = 100; // ������� �������� ������� ����� ��� ( � ����������� �� ������������)
uint32_t EnaSumm = 0; // ������� ������  ���������� ������������ ��� ����������, ������� �� 
// ����� �����
uint32_t CurrentNak = 0; // ����� ���������� 
uint32_t NumNak = 1000; // ����� ���������� 
uint32_t SumNumNak = 0; // ��������� ����� �������� ��� ������ ����� ����������
uint32_t Sm_Shift = 0; // ������� �������� ������ ����.��������
// 11.04.2024 - ������ ���������
// ���� ����� ������� ���������� � ����� ��� 64 ����� �� 128 �����, ����������� ����������
// ����� � ���� � ������ � ������������ � ����������
// �� ������ ������������ ��������� ��������� �� ����������, ����
// 10 ����������, 
uint32_t CntIzmer = 0;// ����� ����������� ���������

uint8_t EnaNextAvrg = 0;// ������� ������ �������� ���������� ( ����� ��� � �������� �����������)

uint8_t EnaStartRun = 0;// ������� ���������� �������� � ��������������, ���� �� ������
uint32_t StartTime=0;
uint8_t EnaPrintRes = 0;// ������� ���������� ������ ������ �� ��������� ��������������
uint8_t PressKey=0; // ������� ��� �������� ������ ��� ���������� �������� �����
int a = 13,j,i;
uint32_t SM;
uint16_t KeyP; // ������� ������� 
uint8_t CntCMD;
//variable USB
uint32_t RecievUSB=0 ; // ������� �������� ������ �� USB, ����� ������ � ������

// ������� ���������� �� MAIN.c from T7kAR
unsigned int CheckErrMEM; 

Measuring_Stat Head_RAW;
// �� modes.c - ���� ������� �������
//char VerFW_LCD[25] = {"No version LCD          \0"}; //������ �� ���������� NEXION
//volatile BYTE TypeLCD = 0; // ��� ���������� � �������������� v-3.2(=0) s-3.5(=1) 
//volatile BYTE g_NeedChkAnsvNEX=0; // ������� ��������� ������ �� ���������.� �� ��������

uint8_t WIDE_VER=1; // 1- ������� �������� 0 - �����������
DWORD TimeBegin; // ����� ������, ��� �������� ��������� ��������
DWORD CountTimerPA = 0;
// ������ �������� ��������� ����������� � ����������
DWORD RawData[RAWSIZE]; // ���� ���������� � �������� ������
unsigned char NexData[NEXSIZE]; // ���� ������ � ������ USB ��� ���������� 4096 byte
unsigned int PointsPerPeriod; // ����� ����� � ������� 
unsigned int PointInPeriod = 0; // ��������� ����� � ������� ��� ����������
BYTE CurrLang; // ������� ����
BYTE CheckIDMEM=0; // ����� ����� �����? (0- 16 , 1- 32)//01/02/2013
// ������� �������� �������������� JDSU
unsigned int TimerValueJDSU; // ������� �������� ������� ��������� RS

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void SDMMC_SDCard_Test(int Num);

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
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM1_Init();
  MX_DCMI_Init();
  MX_SDMMC2_SD_Init();
  MX_FATFS_Init();
  MX_RTC_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C2_Init();
  MX_UART7_Init();
  MX_UART5_Init();
  MX_DAC1_Init();
  /* USER CODE BEGIN 2 */
  MX_USB_DEVICE_Init();
  // ��� ��� ��������� I2C - ������������ ������� � ���������� ����������� 
  // ����� ������ �������������
  // ��� ��� ��������� ������������ �� 7kAR, �� ������������ �� DataDevice MemFlash(� ��� PCA955x)
  CheckErrMEM =   BeginConfig();
  // Start Uart3 - ������� ���
  uint16_t  Dummy = huart3.Instance->RDR ; // ������ ������ ������ �� SIM
  HAL_UART_Receive_IT(&huart3, RxBufExt,1); // ���� �������� ������� ����� �� �������� ����
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_ERR);
  // �������� ������� �� UART
  sprintf((void*)TxBufAns,"TEst\n"); //  
  HAL_UART_Transmit(&huart3,(void*)TxBufAns, strlen((void*)TxBufAns),100);
  // Start Uart7 - Nextion
  Dummy = huart7.Instance->RDR ; // ������ ������ ������ �� NEXTION
  HAL_UART_Receive_IT(&huart7, RxBufExt,1); // ���� �������� ������� ����� �� �������� ����
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart7, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart7, UART_IT_ERR);
  // Start Uart5 - Optics
  Dummy = huart5.Instance->RDR ; // ������ ������ ������ �� OPTIC
  HAL_UART_Receive_IT(&huart5, RxBufExt,1); // ���� �������� ������� ����� �� �������� ����
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_ERR);
  
  // ���������� ��������� �������, �� ��� �������� �� ������� TIM1, 
  // � ���������� ������� ��� ����� �������������������� � ���������� �� ��� ���������� ���� �����!
  HAL_TIM_PWM_Start_IT (&htim1, TIM_CHANNEL_1 ); // ������ ������ ������ ���������� 
  HAL_TIM_PWM_Start (&htim3, TIM_CHANNEL_4 ); // ������.������� �������� ����� ��� ���� ��������� �������
  HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_4 ); // ��������� ������ ����������� ����������� ������ ������� ��� ADC 
  HAL_TIM_PWM_Start (&htim2, TIM_CHANNEL_1 ); // ��� �������� ������� ���������� ��� � ��������� �����������
  //HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_2 ); // ��������� ������ ������������� ��� 
  //HAL_TIM_Base_Start(&htim4); // ��������� ������ ��� DAC � DAC 
  // ��������� ����������
  
  //
  myBeep(100);
  HAL_Delay(10);
  
  
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  {
    Error_Handler();
  }
  //  if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  //  {
  //    Error_Handler();
  //  }
  
  
  if (HAL_ADC_Start_DMA(&hadc1,
                        (uint32_t *)BufADC,
                        SizeBuf_ADC_int
                          ) != HAL_OK)
  {
    Error_Handler();
  }
  //DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
  
  //  if (HAL_ADC_Start_DMA(&hadc2,
  //                        (uint32_t *)BufADD,
  //                        SizeBuf_ADC
  //                          ) != HAL_OK)
  //  {
  //    Error_Handler();
  //  }
  // ��������� �������� DAC
  //    if (HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)CodeDAC, 64, DAC_ALIGN_12B_R) != HAL_OK)
  //  {
  //    /* Start DMA Error */
  //    Error_Handler();
  //  }
  
  //EnaStartRun = 1;
  // TEST!  QUADSPI -  init, erase, write (to Chip), read (from Chip)
  // if (CSP_QUADSPI_Init() != HAL_OK) Error_Handler();
  
  //if (CSP_QSPI_Erase_Chip() != HAL_OK) Error_Handler();
  
  //if (CSP_QSPI_WriteMemory(QSPI_TxBuf, 0, sizeof(QSPI_TxBuf)) != HAL_OK) Error_Handler();
  
  //if (CSP_QSPI_Read(QSPI_RxBuf, 0, 100) != HAL_OK) Error_Handler();
  //if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) Error_Handler();
  
  //memcpy(QSPI_RxBuf, (uint8_t *) 0x90000000, sizeof(QSPI_TxBuf));
  
  // test SD_Card
  //SDMMC_SDCard_Test(999);
  // ������������ UART1
  huart7.Init.BaudRate = 9600;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_Delay(100);
  sprintf((void*)Str,"bauds=115200���");
  NEX_Transmit(Str);// 
  HAL_Delay(100);
  huart7.Init.BaudRate = 115200;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  
  
  
  HAL_Delay(100);
    sprintf((void*)Str,"page 1���");
  NEX_Transmit(Str);// 
  HAL_Delay(100);
  HAL_Delay(100);
    sprintf((void*)Str,"page 1���");
  NEX_Transmit(Str);// 
  HAL_Delay(100);
    myBeep(100);

    sprintf((void*)Str,"page 23���");
  NEX_Transmit(Str);// 
  HAL_Delay(100);

  InitBtns(); 
  
  // ������ ������..
  TimeBegin = HAL_GetTick();
  
    CmdInitPage(0);// ����� ���� ��������
  HAL_Delay(100);

  SetMode (ModeWelcome);
  CmdInitPage(0);// ������� ������� ������������ ���� �� Welcome � ��������� �������� ������� �����

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    
    // �������� ������ 
    if(GetSysTick(0)>30)// ������ 30 �� ��� ������...
    {
      KeyP = SetBtnStates( GetExpand (), 1 ); // ����� ����������
      GetSysTick(1);// ����� ���������� ��������
      // ���������� ������� �������
      // ���������� �������� � ������ CW*
      // ������������ ���������
      // ���������� ������� PA
      CountTimerPA++;
    }
    // �������� ������ �� UART EXT
          if (RSDecYes) // ����� ��������� ��������� �������� �������� �� UART
      {
        DecodeCommandRS();
      }

    
    // ����� ���������������� ���������� � ������ �� ����������
    if(Uart2DecYes)
    {
      if(ProgFW_LCD)
      {
             CDC_Transmit(0, (uint8_t*)RX_BufNEX, CntRXNEX ); // echo back on same channel
      }
      else
      {
      }
      Uart2DecYes=0;
    }
       
    ModeFuncTmp(); // ���������� �������� ������ 
    //test �������� ������
    if ((PRESS(BTN_OK))&&(getStateButtons(BTN_OK)==SHORT_PRESSED))
      // ��������� ������ ��
    {
      myBeep(55);
      HAL_ADC_Stop_DMA(&hadc1); 
      if (HAL_ADC_Start_DMA(&hadc1,
                            (uint32_t *)BufADC,
                            SizeBuf_ADC_int
                              ) != HAL_OK)
      {
        Error_Handler();
      }
      CntCMD++;
      sprintf((void*)Str,"%s",CmdNextion[CntCMD%0xF]);// 
      //sprintf((void*)Str,"page %d���",CntCMD%0xF);// 
      NEX_Transmit(Str);//
      sprintf((void*)Str,"t1.txt=\"%d\"���",CntCMD);
      NEX_Transmit(Str);//
      if((CntCMD%0xF)==1)
      {
        StartRecievNEX (80);
        sprintf((void*)Str,"get t6.txt���");
        NEX_Transmit(Str);//
        while(!((g_WtRdyNEX)||(ReadyNEX==4)));
        // ����� ������ ����� ��������� �� ���������� ������� �� ����������
        // ��� ����� ��� ������ �����������
        if(RX_BufNEX[0] == 0x70) // ���� �����! ��������� ������
        {
          for(int i=0;i<25;++i)VerFW_LCD[i]=RX_BufNEX[i+1];
          VerFW_LCD[23]=0;
          // ����� ������� ������������� ���������� (���� ��� �������)
          // �� ����� ��� ��������� ����������� ��� ��������� ������������� � � ������
          switch(VerFW_LCD[3])
          {
          case '2':
            TypeLCD=0;
            //KnowLCD = 1;
            break;
          case '5':
            TypeLCD=1;
            //KnowLCD = 1;
            break;
          default:
            TypeLCD=0;
            //KnowLCD = 0;
            break;
          }
        }
      sprintf((void*)Str,"t2.txt=\"%s\"���",VerFW_LCD);
      NEX_Transmit(Str);//
        
      }
      //uint32_t DAC_CODE = BufADC[0]>>2;
      HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, ((CntCMD%0xF)<<8));
      KeyP &=~BTN_OK;
      
    }
    
    // ������ ��������� ������������� �������� ��������� ���������, 
    // ����� ��������, ��� ��������� ��������� ���� ��������
    if(EnaStartRun) //
    {
      //LED_Y(1);
      // ��� ������ ���������� ����, ����������� ������� � ������������ � ��������� ����������� ���������
      // ������� TIM2 - ������ ���������� ����������� ���������
      // ������� �� ������������� ������ �����, ��� ������ 1��� 
      // ��� ������� ������������� � ����������
      memset(&BufADC, 0, sizeof(BufADC));
      memset(&BufADD, 0, sizeof(BufADD));
      memset(&BufNAK, 0, sizeof(BufNAK));
      TIM4->CR1 &=~TIM_CR1_CEN;
      TIM2->CR1 &=~TIM_CR1_CEN;
      TIM3->CR1 &=~TIM_CR1_CEN;
      TIM4->CNT =30000;
      TIM2->CNT =0;
      TIM3->CNT =0;
      Sm_Shift = 0; // ������� �������� ������ ����.��������
      CountDMA = 0; //clear count DMA
      //HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)CodeDAC, SizeBuf_DAC, DAC_ALIGN_12B_R);
      StartTime++;
      // ������� ������� ��� ���������� 
      SumNumNak = NumNak*NumRepit;
      // ��������! ����� ���� ������������� TIM1 ��� ��������������� ������ ������������
      // ������� TIM1 = 1uS ����� ��� �������� ����� ������ �� ����� 100 ����� ������ 
      // ��������� ��������� � ��������  ������ TIM1 (MAXREPIT/NumRepit)
      SW_TIM1 = (MAXREPIT/NumRepit);
      TIM1->ARR = 300;
      if(SW_TIM1>1)
      {
        //for ADC MS9280 
        //( NumRepit == 8 => 240 MHz ~0.4 m , max = 3.2 km => )
        //( NumRepit == 4 => 120 MHz ~0.8 m , max = 6.4 km)
        //( NumRepit == 2 =>  60 MHz ~1.6 m , max = 12.8 km)
        //( NumRepit == 1 =>  30 MHz ~3.2 m , max = 25.6 km)
        TIM1->ARR = 50*SW_TIM1;
      } 
      TIM1->CCR1 = TIM1->ARR - 5;
      TIM1->CNT = TIM1->CCR1 - 5;
      TIM1->CR1 |=TIM_CR1_CEN;
      EnaStartRun = 0;
      
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNakVar);
      
      PressKey=1;
      // ���� �� ��������� ����������
      CountCC4 = 0 ; // ����� ����������� ���������� �� TIM4_CH4
      CountEndCMI = 0;
      SizeBlockNak = (uint32_t)(SizeBuf_ADC/NumRepit);
      HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNak);
      
      //DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
      
      
      // ��������! ������ ���� ����� ��� ������ 1 �����, 
      //���� ������ ����� ��������� ��������� �� 0 ������!
      //LED_Y(0);
      //HAL_Delay(100);
    }
    // ������������ �������� ���������� , ��������� ������ ���� ������������ �������
    // ����� ��������� ����� �������� ��� - �������� ��� ������ ������� �������� �� ����� ����������
    // ���� �������� �� ����� ��������� �������� ������ � �������� ��� ����������
    // ���� ��������� ���������������� �������� ������ � ���� ��������� � ��� ���������
    if(EnaNextAvrg)
    {
      
      LED_G(1);
      StopAllTIM(1);
      // CountDMA ��������� �� �������� � ������� ���� ��������� ������� ����
      //Sm_Shift = (CountDMA)&(NumRepit-1);
      Cnt2Timer[CountDMA%128]=CurrCnt2Timer;
      //uint32_t PointDMA = (CountDMA&(NumRepit-1));
      uint32_t PointDMA = (CountDMA%(NumRepit));
      for(int i=0;i<SizeBlockNak; i++)
      {
        //BufNAK[NumRepit*i+PointDMA] +=BufADC[i]; 
        // for ADC MS9280 buffer BufADD
        BufNAK[NumRepit*i+PointDMA] +=BufADD[i]; 
        //BufNAK[NumRepit*i+PointDMA] +=(BufADC[i]+BufADD[i-1])/2; 
        // BufNAK[2*(NumRepit*i+PointDMA)+1] +=BufADC[i]; 
        //BufNAK[2*(NumRepit*i+PointDMA)] +=BufADD[i-1]; 
        TIM1->CNT = TIM1->CCR1 - 5; 
      }
      if(++CountDMA<SumNumNak)
      {
        if(SW_TIM1>2)
        {
          TIM1->CNT = TIM1->CCR1 - 5;
          // ���������������� �������� ������ ����� � ��������� �����
          
        }
        // � ���������� ��� ��� ������� ���������� �����
        //TIM1->CNT =990;
        TIM1->CR1 |=TIM_CR1_CEN;
        
      }
      else
      {
        TIM1->CR1 &=~TIM_CR1_CEN;
        //  ������������� �������� ������ ��� ����������� ����������
        // ��������� ������� ��������� ��������� ��� ������ �����������
        // ����� ���������� ��� ��������� �������, � ���� ��������
        
        EnaPrintRes = 1;
      }
      
      EnaNextAvrg = 0;
      LED_G(0);
      
    }
    //����� ����� ��������� ��������� DMA ����� �������������� 
    // � ���������� �������� ������������ ��������,
    // �� �������� ����������� ����� ����������� ����� ���
    // �������������� ��������� ���� ��������
    if(EnaPrintRes)
    {
      //LED_Y(0);
      //HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x0);
      //TIM6->CR1 &=~TIM_CR1_CEN;
      //TIM6->CNT =0;
      //TIM2->CR1 &=~TIM_CR1_CEN;
      //TIM2->CNT =0;
      // ������ ��������� ��������
      if(PressKey)
      {
        GetLogData ();
        
        
        //LED_R(1); // delete
        // ������� ����
        SendFileBelcore ();
        // � ������� ���� ����...
        HAL_Delay(200);
        SDMMC_SDCard_Test(CntIzmer);
        CntIzmer++;
        HAL_Delay(200);
        //LED_R(0); //delete
        PressKey =0;
      }
      // ��������� ������ ��� ���
      //HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x0);
      //      int sos = StartTime&0x3;
      //      switch(sos)
      //      {
      //      case 0:
      //        SM = 100;
      //        break;
      //      case 1:
      //        SM = 200;
      //        break;
      //      case 2:
      //        SM = 300;
      //        break;
      //      case 3:
      //        SM = 400;
      //        break;
      //      }
      
      EnaPrintRes=0;
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_ADC
                              |RCC_PERIPHCLK_UART5|RCC_PERIPHCLK_UART7
                              |RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.PLL3.PLL3M = 16;
  PeriphClkInitStruct.PLL3.PLL3N = 96;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 4;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL3;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// ���� ������ ���������� ����� �� �7�_��
uint32_t GetSysTick( int Mode) // ��������� ����� 1 ��. 0 - ��������� �������� �� ����������� ������ 1- �����
{
  static uint32_t MemTick;
  if(Mode) MemTick=HAL_GetTick();
  return HAL_GetTick()-MemTick;
}

// ���������� �������� � ���������� ��������
WORD TimerPA (BYTE Set)
{
  if (Set) CountTimerPA = 0;
  return CountTimerPA;
}


/* 
 * set  beepTick
 */
void myBeep (unsigned sound)
{
  extern unsigned beepTick; // 

	beepTick = sound + 1;
}


void StopAllTIM(int Ext)  // ��������� �������� (OTDR)
{
  if(Ext)
  {
  TIM1->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM1 (��������������� ��������()
  TIM1->CNT = 0;
  }
  //TIM2->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM2 (�������� ��������� �������� ������� ���������� ���������)
  TIM3->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM3 (������������� ������������ ��������)
  TIM4->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM4 (������������� �������� ������ ��� ��� ��� (3.75 ��� �� 240/64)
  TIM2->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM2 (��������������� ��������()
  //TIM12->CR1 &= ~TIM_CR1_CEN; // STop ���������� TIM8 (��������������� ��������()
  // �������� �� ���� ��������� �� ������� TIM1 - (�� �� ����� � �9400 ������ ��� ������ TIM3)
  TIM3->CNT = 0;
  TIM4->CNT = 0;
  TIM2->CNT = 0;
  //TIM12->CNT = 0;
  
}
   // ������ �������� ���������� , ��������� ������ ���� ������������ �������
    // ����� ��������� ��� � ������ ������� ����.�������� � �������������� �����������
    // ����� ��������� ����� ����� ��� - �������� ��� ������ ������� �������� �� ����� ����������
    void CreateNextAvrg(void)
    {
      //SizeBlockNak = (uint32_t)(SizeBuf_ADC/NumRepit);
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNakVar);
      DMA1_Stream2->NDTR = SizeBlockNak; // ������ ����� ������ DMA 
      DMA1_Stream2->CR |= DMA_SxCR_EN; // ��������� ����� ���� ��� for DCMI
      DCMI->CR|=DCMI_CR_ENABLE;          // DCMI capture enable
      DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
      __HAL_DCMI_CLEAR_FLAG(&hdcmi, DCMI_FLAG_VSYNCRI);      //CountDMA++;// ������� ����� ����� �������������
       
       //DMA1_Stream2->CR |= DMA_SxCR_EN; // ��������� ����� ���� ��� for DCMI
     //Sm_Shift = ((NumRepit-1)-((CountDMA)&(NumRepit-1)));
      // ���� ��������� ������������ ������� ���� ��������
      // ��� � (����) - ����� �� 1
      // ��� �/2 - ����� �� 2
      Sm_Shift = (MAXREPIT/NumRepit)*((NumRepit-1)-((CountDMA)%(NumRepit))); 
      // ��� ������ ����� ������������ ������� �� 63 �����
      // �������� ������,����� ������� � ������,
      // � � ������������� ���������� ������ ������ 
      // ������������� ��������� ������������ ��������
      // !!!�������� ���� �������� ���������� ����� ���������!!!
      TIM3->ARR = BeginSet + Sm_Shift + ZondImpulse; //��������� ������ ��������� ������� � ����� 50+���������(40)+�������(2)=90
      // ������ ������� �� 97 ��� ������� 
      TIM3->CCR4 = BeginSet + Sm_Shift;
      // ����� �� ���������� ���, ����� ������������ �� ������� TIM4_CH4
      //HAL_ADC_Start_DMA(&hadc1,
      //                  (uint32_t *)BufADC,
      //                  SizeBlockNak
      //                    );
      // ����� 2 ��� � ������ �����
//      HAL_ADC_Start_DMA(&hadc2,
//                        (uint32_t *)BufADD,
//                        SizeBlockNak
//                          );
      //DMA1_Stream1->
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_CONTINUOUS,(uint32_t)BufADD,SizeBlockNak);
      //DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
  // LED_Y(1); // ������ ������ ����������, ����� �������� ����� ���
      //DCMI_MODE_CONTINUOUS
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNakVar);
      //DCMI_FLAG_VSYNCRI

    }
  //
 void StartADC_DMA (void)
 {
   HAL_ADC_Start_DMA(&hadc1,
                     (uint32_t *)BufADC,
                     8
                       );
   
 }
// ���������� � ��������������� ������ ,
// ���� ��� ������� �� �� �� ���������� �����������
// ������ � 60 ����� , ��� ������� � 5 �� 45 ����� � ������
// ����������� ����� 10 ��������, ���������� �� ����� 3 v
// ���� ��� ������ ��� �������� NumNak
void GetLogData (void)
{
  uint32_t Noise; // 
  uint32_t LocalRaw;
  uint32_t Avrgs = NumNak;
  //uint32_t Beg
  int j=BeginShift; //��������� �������� �������� �� ������� 60
  // ������ ������� �� ������ ����� ������� ������, � �� ������ ���
  // ��� ����. AIN2 
  // INP3 12(8)_40_1.5 - j=103
  // INP3 10(8)_40_2.5 - j=97
  // INP3 10(8)_20_2.5 - j=49 (~97/2)
  // INP3 14(16)_64_8.5 - j=50
  Measuring_Stat Head_RAW;  
  
  for(int u=5;u<30;u++) //45 
  {
    Noise += BufNAK[u];
  }
  Noise = (uint32_t)(Noise/25);//40
  uint32_t CurrentMaxLog =(uint32_t)(5000.0*log10((double)Avrgs*(MaxADC))); // ������������ �������� ������� ����������    
  
  for(int i=0;i<8000;i++)
  {
    LocalRaw = BufNAK[i+j];
    if (LocalRaw<=Noise) LocalRaw=Noise+1;
    LocalRaw= LocalRaw-Noise;
    LogData[i] = (unsigned short)(CurrentMaxLog - (uint32_t)(5000.0*log10((double)LocalRaw))) ;
    if (LogData[i] > CurrentMaxLog)
    { 
      //__no_operation();
      CurrentMaxLog+=1;
    }
  }
  // ������������ ��������� ����� ���������
  // ���������� DS
  Head_RAW.ValDS = (unsigned int)((ADCPeriod*50000)/NumRepit); //  ������������� �������� DS ��� �������������� ������ ���������
  // ����� ���������� NAV
  Head_RAW.NumAvrg = NumNak;
  // ����� ����� ���������, ������������ �������� ������������ AW PWU
  Head_RAW.AW_PWU = (1310)<<16; // ���������� ����� ����� ���������
  Head_RAW.AW_PWU += 10; // ���������� ������������ ��������
  // ������ ������, ��� ������������ (����� �� ����� �� ���� = 0) �������� (������ �����) �� ��� ��������� � ����������
  Head_RAW.FormatData = 1;
  // ������ ���� ����������, ������ ����� ����� �� ������ 
  Head_RAW.SizeFrm = NumRepit;
  //����� �������� NPPW (�� ��������� �������, �� � ��� ����)
  Head_RAW.NumPtsMain = SizeLogBuf;
  
}
// ���������� ��������� ����� ��������� �������
//void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents) // ��������� ����� �������
//{
//  const char *IdnsBC[2]= {"SvyazServis   \0","OPTOKON Co.Ltd\0"};
//
//  char Str[10];
//  char CommentsOTDR[20] = {"                   \0"}; //����������� �������������
//  
//  unsigned short DataShort ; // 
//  unsigned long DataInt ;
//  
//      // ��� ������ ����������������� ��������� ������� (������� ������������� ����� ����� �� ������� ��������)
//      switch (Block)
//      {
//      case 0: //0...55 byte
////        // ������ ���������� ���� ��������� ����
////      if (GetNumTrace()) // ���� �� ������� �� ������ �� �������
////        TraceREAD(GetNumTraceSaved(GetNumTrace()));//  ������ ���� ������� ���� ��������// 27/01/2011 ����������� ��������� ��������������
////      else  TraceREAD(0);
//      memcpy( Name, HeaderBelcore, 56 );
//        if (NumEvents)
//        {
//          //memcpy( &Name[5], 7, 1);
//          Name[2]=0x62; // ������ ������ ������� Map ����� +16
//          Name[6]=7; // ������ ������ ���������� ����� � ��� �����
//          memcpy( &Name[56], &HeaderBelcore[225], 16 ); // �������� ���� ������� � ��� ����
//          DataShort = (unsigned short)(NumEvents*32+24); // ��������� ������� ����� � ����������� �� ����� �������
//          memcpy( &Name[68], &DataShort, 2);
//          
//        }
//        break;
//      case 1: //56...117 (72...133) 62 byte
//      memcpy( Name, &HeaderBelcore[56], 62 );
//        // ������������� ����� �����
//      DataShort = (unsigned short)1310; // ��������� ����� ����� �� ������� ������������� �����
//      // ���������� ������ ����� 1300 ��� �4 ��������!
//      
//      memcpy( &Name[86-56], &DataShort, 2);
//      // ��������� Comments
//      memcpy( &Name[98-56], CommentsOTDR, 19);
//        break;
//      case 2: //118...211 (134...227) 94 byte
//      memcpy( Name, &HeaderBelcore[118], 94 );
//      // ��������� �������������
//      memcpy( &Name[118-118], IdnsBC[0], 14 );
//      // ������������� ������� 12 ���� 
//      sprintf (Str,"TOPAZ_AVRG  ");
//      // Set MDIF
//      memcpy( &Name[133-118], Str, 12 );
//      // Number Device
//      sprintf (Str,"%04d",9999);
//      memcpy( &Name[146-118], Str, 4 );
//      // ����� ���������
//      sprintf (Str,"555x");
//      memcpy( &Name[153-118], Str, 4 );
//      
//      // ����� ���������� �������������� 
//      // ������������� ���� ����������� ��������������
//      // ������ 19.04.2024 16:00 = 
//      DataInt = 1713531924;//1713531924
//      memcpy( &Name[159-118], &DataInt, 4);
//      // (164)AW ����� ����� � 0.1 �� (1310�� = 13100) 0x332c 0x3c8c (164)
//      DataShort = 13100;
//      memcpy( &Name[165-118], &DataShort, 2);
//      // ����� ������������ �������� 
//      DataShort = 12;
//      memcpy( &Name[173-118], &DataShort, 2);
//      // ������������� DS
//      // ((10000points*10(in 1ns 100ps))/2 = 50000 , 333.333 ns - �������� ����� ����������
//      DataInt = (unsigned long)((ADCPeriod*50000)/NumRepit); //  ������������� �������� DS ��� �������������� ������ ���������
//      memcpy( &Name[175-118], &DataInt, 4);
//      // ###(182) GI ����� �����������  146583 (1.46583)  
//      DataInt = (unsigned long)( 1.46810*100000);
//      memcpy( &Name[183-118], &DataInt, 4);
//      // ###(186) BC = 800 ������ ��������� ���������
//      DataInt =  800;
//      memcpy( &Name[187-118], &DataInt, 2);
//      // ###(188) NAV ����� ���������� - ������� �� ������� ���������� � ����� ����� = FinAvrg
//      DataInt = NumNak;
//      memcpy( &Name[189-118], &DataInt, 4);
//      // ###(192) AR  ����� ����������� ������� (����� ����� ��������� �� ���) DS*NPPW/10000
//      DataInt = (unsigned long)((ADCPeriod*5*SizeLogBuf)/NumRepit); //  ������������� �������� DS ��� �������������� ������ ���������
//      memcpy( &Name[193-118], &DataInt, 4);
//      // ###(200) NF ������ ������� ����� ����� 65535
//      DataInt =  65535;//ReflParam.NF;
//      memcpy( &Name[201-118], &DataInt, 2);
//      // ###(206) LT ����������� �������� ��������� ��� ������� 200 � 0.001 ��
//      DataInt =  200;//ReflParam.LT;
//      memcpy( &Name[207-118], &DataInt, 2);
//      // ###(208) RT ����������� �������� ��������� ��� ������� 40000 � -0.001��
//      DataInt =  40000;//ReflParam.RT;
//      memcpy( &Name[209-118], &DataInt, 2);
//      // ###(210) ET ����������� ��������� ��� ����� ����� 3000 � 0.001 ��
//      DataInt =  3000;//ReflParam.ET;
//      memcpy( &Name[211-118], &DataInt, 2);
//        break;
//      case 3: //212...223 (228...239) 12 byte
//      memcpy( Name, &HeaderBelcore[213], 12 );
//       break; 
//      }
//
//}

// �������� ����� ������� ����������� ��� �7� ��� �������
void SendFileBelcore (void)
{
  // ������� ��� ������� ����������� ����� �� �������� 0x1021
  static const short int table[] =
  {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
  };
  
  char StartStr[8]={"#516611\0"}; // 5-� ������� ������!!! �.�. ����� 8000 �����
  //char BufString[225];
  unsigned char *c;
  unsigned char value;
  unsigned short NumEventNow = 0; //��� �������
  unsigned short old_crc = 0xffff; 
  unsigned short new_crc = 0xffff;
  
  // �������� �������� ������ (���������)
  sprintf (StartStr, "#4%4d",16227 + ((NumEventNow)?(NumEventNow*32+40):(0)));
  //UARTSend0 ((BYTE*)StartStr, 7);
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)StartStr,7); // ������ 
  //  TxDMA=1;
  // �������� ���� ��������
  //HAL_UART_Transmit(&huart3, (void*)StartStr,7,1); // ������ 
  //HAL_Delay(2);
  // ��� �������� ������� � ������ ������� ������� (���� 0)
  // ���� ���� ������� �������....
  GetHeaderBelcore (BufString, 0, NumEventNow); // ��������� ����� ������� ������ 56 ���� Block=0
  //UARTSend0 ((BYTE*)BufString, 56+16*((NumEventNow)?(1):(0)));
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,56+16*((NumEventNow)?(1):(0))); // ������ 
  //  TxDMA=1;
  HAL_UART_Transmit(&huart3, (void*)BufString,56+16*((NumEventNow)?(1):(0)),10); // ������ 
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
    CDC_Transmit(0, (void*)BufString, 56+16*((NumEventNow)?(1):(0))); // ������ ����
  //HAL_Delay(3);
  c = (unsigned char*)&BufString;
  for (int i=0;i<56+16*((NumEventNow)?(1):(0));i++)
  {
    /* ������ ������� �������� ����������� ����� - ���������                                             */		
    value = *c;
    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
    old_crc = new_crc;
    c++;
  }
  // ��������� ����� �������  62 ���� Block=1 (����������� ��� ����� + GenParams)
  GetHeaderBelcore (BufString, 1, NumEventNow); 
  //UARTSend0 ((BYTE*)BufString, 62);
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,62); // ������ 
  //  TxDMA=1;
  //HAL_Delay(3);
  HAL_UART_Transmit(&huart3, (void*)BufString,62,10); // ������ 
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
    CDC_Transmit(0, (void*)BufString, 62); // ������ ����
  c = (unsigned char*)&BufString;
  for (int i=0;i<62;i++)
  {
    /* ������� ����������� ����� ����������� �����                                             */		
    value = *c;
    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
    old_crc = new_crc;
    c++;
  }
  // ��������� ����� �������  94 ���� Block=2 - (SupParams FxdParam)
  GetHeaderBelcore (BufString, 2, NumEventNow); 
  //UARTSend0 ((BYTE*)BufString, 95);
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,95); // ������ 
  //  TxDMA=1;
  HAL_UART_Transmit(&huart3, (void*)BufString,95,10); // ������ 
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
    CDC_Transmit(0, (void*)BufString, 95); // ������ ����
  //HAL_Delay(5);
  c = (unsigned char*)&BufString;
  for (int i=0;i<95;i++)
  {
    /* ������� ����������� ����� ����������� �����                                             */		
    value = *c;
    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
    old_crc = new_crc;
    c++;
  }
  // ��������� � �������� ���� ������� ���� �� ���� (���� �������)
  //        if (NumEventNow) // ���� ���� ������� 2 ����� +
  //          // ������� � ������������� ������� ��� ������� 32 �����  +  22 ���� ����� ��� ����� �����
  //        {
  //          // ��������  ����� �������  2 �����
  //          UARTSend0 ((BYTE*)&NumEventNow, 2);
  //          c = (unsigned char*)&NumEventNow;
  //          for (int i=0;i<2;i++)
  //          {
  //            /* ������� ����������� ����� ����������� �����                                             */		
  //            value = *c;
  //            new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
  //            old_crc = new_crc;
  //            c++;
  //          }
  //          // �������� �������������� ����� �������  N*32
  //          for (int s=0; s<NumEventNow; s++)
  //          {
  //            UARTSend0 ((BYTE*)&EvenTrace[s], 32);
  //            c = (unsigned char*)&EvenTrace[s];
  //            for (int i=0;i<32;i++)
  //            {
  //              /* ������� ����������� ����� ����������� �����                                             */		
  //              value = *c;
  //              new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
  //              old_crc = new_crc;
  //              c++;
  //            }
  //            
  //          }
  //          // �������� �������� ���� ������� 22 �����
  //          UARTSend0 ((BYTE*)&EndEvenBlk, 22);
  //          c = (unsigned char*)&EndEvenBlk;
  //          for (int i=0;i<22;i++)
  //          {
  //            /* ������� ����������� ����� ����������� �����                                             */		
  //            value = *c;
  //            new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
  //            old_crc = new_crc;
  //            c++;
  //          }
  //        }
  
  // ��������� ����� ������� 12 ���� Block=3 (DataPts)
  GetHeaderBelcore (BufString, 3, NumEventNow); 
  //UARTSend0 ((BYTE*)BufString, 12);
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,12); // ������ 
  //  TxDMA=1;
  HAL_UART_Transmit(&huart3, (void*)BufString,12,3); // ������ 
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
    CDC_Transmit(0, (void*)BufString, 12); // ������ ����
  //HAL_Delay(1);
  c = (unsigned char*)&BufString;
  for (int i=0;i<12;i++)
  {
    /* ������� ����������� ����� ����������� �����                                             */		
    value = *c;
    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
    old_crc = new_crc;
    c++;
  }
  
  // ���� ������ 
  //UARTSend0 ((BYTE*)LogData, OUTSIZE*2);
  //        while(TxDMA);
  //  HAL_UART_Transmit_DMA(&huart3, (void*)LogData,SizeLogBuf*2); // ������ 
  //  TxDMA=1;
  HAL_UART_Transmit(&huart3, (void*)LogData,SizeLogBuf*2,2000); // ������ ���� ��������������� ������ �� ���� ��������
  
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
    CDC_Transmit(0, (void*)LogData, SizeLogBuf*2); // ������ ���� ��������������� ������ �� ���� ��������

  //HAL_Delay(150);
  c = (unsigned char*)&LogData;
  for (int i=0;i<SizeLogBuf*2;i++)
  {
    /* ������ ������� �������� ����������� ����� - ���������                                             */		
    value = *c;
    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
    old_crc = new_crc;
    c++;
  }
  
  // UARTSend0 ((BYTE*)&new_crc, 2);
  //while(TxCDCFree(0)); // ���� ��������� �������� ����������� �����
  // ��� ����� �������� ��������� ��� �����
  HAL_UART_Transmit(&huart3, (void*)new_crc,2,1); // ������ 
  // ��������� ����� ������ �� ������������ ��� ����� ���� �� ����
  // ������ ���� ���� ��� �������� �����������
    CDC_Transmit(0, (void*)new_crc, 2); // ������ ���� ����������� �����
}

//����� ��������� ����� ������ FATFS in SDMMC2 with SD_Card
void SDMMC_SDCard_Test(int Num)
{
  FATFS FatFs;
  FIL Fil;
  FRESULT FR_Status;
  FATFS *FS_Ptr;
  UINT RWC, WWC; // Read/Write Word Counter
  DWORD FreeClusters;
  uint32_t TotalSize, FreeSpace;
    char FileNameS[20];
  char RW_Buffer[200];
  char TxBuffer[250]; // �������� ������ �������� ���������� ��������� � UART3 (�� ������� ���)
  do
  {
    //------------------[ Mount The SD Card ]--------------------
    FR_Status = f_mount(&FatFs, SDPath, 1);
    if (FR_Status != FR_OK)
    {
      sprintf(TxBuffer, "Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
      break;
    }
    sprintf(TxBuffer, "00000\nSD Card Mounted Successfully! \r\n\n");
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
    //------------------[ Get & Print The SD Card Size & Free Space ]--------------------
    f_getfree("", &FreeClusters, &FS_Ptr);
    TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
    FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
    sprintf(TxBuffer, "Total SD Card Size: %lu Bytes\r\n", TotalSize);
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),100); // ������ 
    sprintf(TxBuffer, "Free SD Card Space: %lu Bytes\r\n\n", FreeSpace);
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),100); // ������ 
    //------------------[ Open A Text File For Write & Write Data ]--------------------
    //Open the file
    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    sprintf(FileNameS,"MyFile%03d.txt",Num);
    FR_Status = f_open(&Fil, FileNameS, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
    if(FR_Status != FR_OK)
    {
      sprintf(TxBuffer, "Error! While Creating/Opening A New Text File, Error Code: (%i)\r\n", FR_Status);
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
      break;
    }
    sprintf(TxBuffer, "Text File Created & Opened! Writing Data To The Text File..\r\n\n");
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),100); // ������ 
    // (1) Write Data To The Text File [ Using f_puts() Function ]
    f_puts("Hello! From STM32 To SD Card Over SDMMC, Using f_puts()\n", &Fil);
    // (2) Write Data To The Text File [ Using f_write() Function ]
    strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SDMMC, Using f_write()\r\n");
    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
    // Close The File
    f_close(&Fil);
    //------------------[ Open A Text File For Read & Read Its Data ]--------------------
    // Open The File
    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ);
    FR_Status = f_open(&Fil, FileNameS, FA_READ);
    if(FR_Status != FR_OK)
    {
      sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Read.. \r\n");
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
      break;
    }
    // (1) Read The Text File's Data [ Using f_gets() Function ]
    f_gets(RW_Buffer, sizeof(RW_Buffer), &Fil);
    sprintf(TxBuffer, "Data Read From (MyTextFile.txt) Using f_gets():%s", RW_Buffer);
    //sprintf(TxBuffer, "Data Read From (%s) Using f_gets():%s",FileNameS, RW_Buffer);
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
    // (2) Read The Text File's Data [ Using f_read() Function ]
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    sprintf(TxBuffer, "Data Read From (%s) Using f_read():%s",FileNameS, RW_Buffer);
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
    // Close The File
    f_close(&Fil);
    sprintf(TxBuffer, "File Closed! \r\n\n");
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
    //------------------[ Open An Existing Text File, Update Its Content, Read It Back ]--------------------
    // (1) Open The Existing File For Write (Update)
    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_OPEN_EXISTING | FA_WRITE);
    FR_Status = f_open(&Fil, FileNameS, FA_READ); // Open The File For Read
    FR_Status = f_lseek(&Fil, f_size(&Fil)); // Move The File Pointer To The EOF (End-Of-File)
    if(FR_Status != FR_OK)
    {
      sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Update.. \r\n");
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
      break;
    }
    // (2) Write New Line of Text Data To The File
    FR_Status = f_puts("This New Line Was Added During File Update!\r\n", &Fil);
    f_close(&Fil);
    memset(RW_Buffer,'\0',sizeof(RW_Buffer)); // Clear The Buffer
    // (3) Read The Contents of The Text File After The Update
    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ); // Open The File For Read
    FR_Status = f_open(&Fil, FileNameS, FA_READ); // Open The File For Read
    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
    sprintf(TxBuffer, "Data Read From (MyTextFile.txt) After Update:\r\n%s", RW_Buffer);
    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
    f_close(&Fil);
    //------------------[ Delete The Text File ]--------------------
    // Delete The File
    /*
    FR_Status = f_unlink(MyTextFile.txt);
    if (FR_Status != FR_OK){
        sprintf(TxBuffer, "Error! While Deleting The (MyTextFile.txt) File.. \r\n");
        USC_CDC_Print(TxBuffer);
    }
    */
  } while(0);
  //------------------[ Test Complete! Unmount The SD Card ]--------------------
  FR_Status = f_mount(NULL, "", 0);
  // ����� �� ��������� ������
  //        sprintf(TxBuffer, "\r\nSD Card NO   Un-mounted Successfully! \r\n");
  //    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
  if (FR_Status != FR_OK)

  {
      sprintf(TxBuffer, "\r\nError! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
  } else{
      sprintf(TxBuffer, "\r\nSD Card Un-mounted Successfully! \r\n");
      HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // ������ 
  }
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
  while (1)
  {
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
