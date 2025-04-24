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
#define BeginTest  64 // начальное смещение для рассчета 
    // !!!В Н И М А Н И Е!!!
    // идентификатор платы должен совпасть с программным
#define ID_PLATE  0x3 //0x3 - Т7к  0x2 - Т5к
    

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

static int  g_NumVer =
#include "lasti.h"
;
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
uint8_t TxDMA = 0; // признак занятости DMA 
uint8_t TxBufAns[512]; // буффер передачи в USB
uint8_t  CntTx=0;
uint16_t BufADC[SizeBuf_ADC_int]; // буфер внутреннего АЦП (8), в него пишем при съеме DMA, размер до 8
uint16_t BufADD[SizeBuf_ADC]; // буфер 2АЦП, в него пишем при съеме DMA, размер до 8192
uint32_t BufNAK[SizeBuf_ADC]; // буфер накопления, в него добавляем из буфера АЦП
uint16_t LogData[SizeBuf_ADC]; // буфер логарифмических данных накопления, в него добавляем из буфера АЦП
uint16_t LogDataSplice[SizeBuf_ADC]; // буфер логарифмических данных накопления, в него добавляем из буфера АЦП
uint32_t Cnt2Timer[128]; // время тамера 2 по окончании цикла ДМА
uint32_t CurrCnt2Timer; // время тамера 2 по окончании цикла ДМА
//static uint32_t MediumNoise = 0;
// test QSPI
uint8_t QSPI_TxBuf[]= "Hello world fromQSPI";
uint8_t QSPI_RxBuf[100];
// переменные UART I2C
uint16_t Dummy; // пустое чтение буфферов UART
uint32_t ErrCheck; // контроль памяти конфигурации
uint8_t N_LS=0; // число лазеров в конфигурации (для контроля есть ли источник)
// основные установки параметров сборщика
//
volatile uint32_t MaxADC = (1<<ResolutionADC)-1; // максимальный уровень АЦП - разрядность
uint8_t Str[64];

uint32_t DBPRO = 48;

uint32_t SizeStrob = 264; // строба

uint32_t BeginShift = 36; // начальное смещение для рассчета 
uint32_t BeginSet = 200; // место установки начала зондир импульса при различных режимах прореживания (
//И него получаем шаг изменения в сдвиге зонд импульса
int SW_TIM1 = 1;
uint32_t CountKeyS = 0; // число нажатых кнопок
// Параметры установок накопителя
uint32_t NumRepit = 1;// число повторений, для самого плотного = 8
uint32_t T_Dist_Arr = 1000;// длительность повторения импульсов (рассчетно-проверенная для разных режимов)
// устанавливается от параметров измерения и рассчетов в тиках таймера TIM1- 1мкС
uint32_t NumNak = 5; // число накоплений , сколько надо копить в единицах
uint32_t PointForDist = 5600; // число точек съема (зависит от уст диапазона (дистанция)
uint32_t ZondImpulse = 5000; // длительность зондирующего импульса в тиках 240 МГц (4.1666 нС)
// признаки работающего сборщика
volatile uint8_t Ena_AVRG = 0; // разрешаем накопления, пока циклимся в while
volatile uint8_t Ena_CFG = 0;// ждем начала установк перед стартом ТИМ1
volatile uint8_t Ena_SUMM = 0; // можно запускать суммирование ( зависит от линии) выбор времени начала суммирования
uint32_t CountWait = 0; // счетчик Ожиданий
uint8_t IndexDist =0;
uint8_t EndDMA_DCMI =0; // признак окончания цикла ДМА, устанавливается в прерывании


// участвует в установках DICM DMA и рассчета суммирования
uint32_t CountCC4 = 0; // число совершенных прерываний по TIM4_CH4
uint32_t CountEndCMI = 0; // число прерываний совершенных циклов DMA по DCMI
uint32_t CountDMA = 0; // число совершенных циклов DMA 
uint32_t SizeBlockNak = 0; // текущее значение размера блока ДМА ( в зависимости от прореживания)
uint32_t SizeBlockNakVar = 100; // текущее значение размера блока ДМА ( в зависимости от прореживания)
uint32_t EnaSumm = 0; // признак начала  разрешения суммирования при накоплении, зависит от 
// длины линии
uint32_t CurrentNak = 0; // число накоплений 
uint32_t SumNumNak = 0; // суммарное число проходов при данном числе накоплений
uint32_t Sm_Shift = 0; // текущее значение сдвига Зонд.Импульса
// 11.04.2024 - Делаем следующее
// пока копим плотное накопление в буфер АЦП 64 блока по 128 точек, особенность заполнения
// пишем в блок в начало и подсуммируем в накопление
// по кнопке устаналиваем начальные установки по накоплению, пока
// 10 накоплений, 
uint32_t CntIzmer = 0;// число совершенных измерений

uint8_t MeasureNow = 0; // признак работы накопителя, для обхода, основного цикла
uint8_t EnaNextAvrg = 0;// признак начала текущего накопления ( старт ДМА с текущими параметрами)

uint8_t EnaStartRun = 0;// признак разрешения импульса и преобразования, пока от кнопки
uint32_t StartTime=0;
uint8_t EnaPrintRes = 0;// признак разрешения печати данных по окончании преобразования
uint8_t PressKey=0; // признак что нажимали кнопку для разрешения передачи файла
int a = 13,j,i;
uint32_t SM;
uint16_t KeyP; // клавиши нажатые 
uint8_t CntCMD;
//variable USB
uint32_t RecievUSB=0 ; // признак принятия данных по USB, число данных в буфере
uint8_t BusyUSB=0 ; // признак передачи данных по USB, с SD картой
// при приеме передаче взводим на 10 мС , и перезаводим при следующей передаче/приеме
uint16_t PresentUSB = 0; // признак подключенного USB
uint8_t ModeUSB = 0; // признак работы USB для индикации

// перенос переменных из MAIN.c from T7kAR
unsigned int CheckErrMEM; 

  // контроль идентификатора платы
uint8_t CheckErrID_Plate=0; 

Measuring_Stat Head_RAW;
// из modes.c - надо вернуть обратно
//char VerFW_LCD[25] = {"No version LCD          \0"}; //версия ПО индикатора NEXION
//volatile BYTE TypeLCD = 0; // тип индикатора в идентификаторе v-3.2(=0) s-3.5(=1) 
//volatile BYTE g_NeedChkAnsvNEX=0; // признак получения строки из редактора.и ее проверка

uint8_t WIDE_VER=1; // 1- обычный диапазон 0 - расширенный
DWORD TimeBegin; // время начала, для контроля начальной заставки
DWORD CountTimerPA = 0;
// Группа основных перемнных участвующих в накоплении
DWORD RawData[RAWSIZE]; // блок накопления в основной памяти
unsigned char NexData[NEXSIZE]; // блок данных в памяти USB для индикатора 4096 byte
unsigned int PointsPerPeriod; // Число точек в периоде 
unsigned int PointInPeriod = 0; // указатель точки в периоде при накоплении
BYTE CurrLang; // текущий язык
BYTE CheckIDMEM=0; // какая флэшь стоит? (0- 16 , 1- 32)//01/02/2013
// счетчик частотты преобразования JDSU
unsigned int TimerValueJDSU; // текущее значение частоты приемника RS
//  FATFS FatFsMain;
//  FIL Fil;
//  FRESULT FR_Status;
//  FRESULT res;
// для батарейки
  char LvlBatInd=0; //индикатор уровня батарейки
char PeriodIntADC=0; // счетчик запуска внутреннего АЦП и измерения уровня батарейки
//float Ubat=4.1; // начальное напряжение батареи

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
//void SDMMC_SDCard_Test(int Num);
uint8_t GetID_Plate(void);

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
  MX_GPIO_Init();
  if(ID_PLATE != GETIDPLT)
  {
    CheckErrID_Plate=1;
  }
  if(CheckErrID_Plate)
  {
    MX_DMA_Init();
    MX_UART7_Init();
    // Start Uart7 - Nextion
    uint16_t  Dummy = huart7.Instance->RDR ; // чистим буффер приема от NEXTION
    HAL_UART_Receive_IT(&huart7, RX_BufNEX,1); // ждем принятия первого байта из внешнего мира
    /* disable the UART Parity Error Interrupt */
    __HAL_UART_DISABLE_IT(&huart7, UART_IT_PE);
    /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_DISABLE_IT(&huart7, UART_IT_ERR);
    
    // перенастроим UART7  для NEXTION
    huart7.Init.BaudRate = 9600;
    if (HAL_UART_Init(&huart7) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_Delay(10);
    sprintf((void*)Str,"bauds=115200яяя");
    HAL_UART_Transmit(&huart7, (void*)Str,strlen((void*)Str),20); // выдаем 
    
    //NEX_Transmit(Str);// 
    HAL_Delay(10);
    huart7.Init.BaudRate = 115200;
    if (HAL_UART_Init(&huart7) != HAL_OK)
    {
      Error_Handler();
    }
    //  myBeep(100);
    HAL_Delay(10);
    sprintf((void*)Str, "t0.txt=\"! ОШИБКА !\"яяя"); // auto
    NEX_Transmit((void*)Str);    // 
    sprintf((void*)Str, "t1.txt=\"!прибор не тот!\"яяя"); // auto
    NEX_Transmit((void*)Str);    // 
    
    while(1);
    
  }
  
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
  MX_I2C2_Init();
  MX_RTC_Init();
  MX_UART7_Init();
  MX_UART5_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_DAC1_Init();
  MX_TIM5_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
  // проверяем конфигурацию платы, чтобы не запустить программу по исправленю
  // если вдруг зашили чужую программу, попытаемся написать в индикатор и зациклится
  
  //  if(ID_PLATE != GETIDPLT)
  //  {
  //    CheckErrID_Plate=1;
  //  }
  // Start Uart7 - Nextion
  Dummy = huart7.Instance->RDR ; // чистим буффер приема от NEXTION
  HAL_UART_Receive_IT(&huart7, RX_BufNEX,1); // ждем принятия первого байта из внешнего мира
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart7, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart7, UART_IT_ERR);
  
  // перенастроим UART7  для NEXTION
  huart7.Init.BaudRate = 9600;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_Delay(10);
  sprintf((void*)Str,"bauds=115200яяя");
  HAL_UART_Transmit(&huart7, (void*)Str,strlen((void*)Str),20); // выдаем 
  
  //NEX_Transmit(Str);// 
  HAL_Delay(10);
  huart7.Init.BaudRate = 115200;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  //  myBeep(100);
  HAL_Delay(10);
  sprintf((void*)Str, "page 0яяя"); // < START>
  NEX_Transmit((void*)Str);    //
  
  // так как появилось I2C - конфигурация прибора и управление клавиатурой 
  // будет первым настраиваться
  // так как повторяем конфигурацию из 7kAR, то скомбинируем из DataDevice MemFlash(у нас PCA955x)
  
  CheckErrMEM = BeginConfig();
  
  CheckErrMEM |= StartInitSDcard();
  
  //  // попробуем проинициализировать SD Card
  //  // пустое подключение с первого раза не берет
  //  res = f_mount(&FatFsMain, SDPath, 1);
  //  FR_Status = f_mount(NULL, "", 0);
  //  //
  //  HAL_Delay(100);
  //  res = f_mount(&FatFsMain, SDPath, 1);
  //      if (res != FR_OK) // какие-то проблемы с SD Card
  //    {
  //      CheckErrMEM |= ERR_SDCard;
  //      //sprintf(TxBuffer, "Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
  //      //UARTSendExt((void*)TxBuffer,strlen(TxBuffer)); // выдаем 
  //    }
  //
  //    res = f_mkdir("0:/_OTDR");
  //    if(res != FR_EXIST)
  //    {
  //      CheckErrMEM |= CLR_SDCard;
  //    }
  //        // почитаем директории...только что созданные 
  //    res = f_opendir(&dir, "0:/_OTDR");
  //    f_closedir(&dir);
  //    
  //    sprintf((void*)TxBufAns,"TOP_N%04d",ConfigDevice.NumDevice);
  //    
  //    res = f_setlabel (TxBufAns);							/* Set volume label */
  //
  //
  //  FR_Status = f_mount(NULL, "", 0);
  //      // создаем или проверяем наличие дирректории _OTDR
  
  
  // Start Uart3 - внешний мир
  Dummy = huart3.Instance->RDR ; // чистим буффер приема 
  HAL_UART_Receive_IT(&huart3, RxBufExt,1); // ждем принятия первого байта из внешнего мира
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_ERR);
  // Тестовая посылка по UART
  sprintf((void*)TxBufAns,"TEst\n"); //  
  HAL_UART_Transmit(&huart3,(void*)TxBufAns, strlen((void*)TxBufAns),100);
  // Start Uart5 - Optics
  Dummy = huart5.Instance->RDR ; // чистим буффер приема от OPTIC
  HAL_UART_Receive_IT(&huart5, RX_BufOpt,1); // ждем принятия первого байта из оптики
  /* disable the UART Parity Error Interrupt */
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_PE);
  /* disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_ERR);
  /* disable the UART Error Interrupt: (готовность данных) */
  //__HAL_UART_DISABLE_IT(&huart5, UART_IT_RXNE); // пока запретим а то мешает.
  
  //HAL_TIM_PWM_Start (&htim12, TIM_CHANNEL_2 ); // запускаем таймер синхронизации ЦАП 
  //HAL_TIM_Base_Start(&htim4); // запускаем таймер для DAC и DAC 
  // прочитаем клавиатуру
  
  //
  // myBeep(100);
  //  HAL_Delay(500);
  //  порождение строки версии
  sprintf(NumVer,"%03d",g_NumVer); // новый список версий
  
  
  
  if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  {
    myBeep(100);
    
    Error_Handler();
  }
  //  if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED) != HAL_OK)
  //  {
  //    Error_Handler();
  //  }
  
  //  myBeep(100);
  //  HAL_Delay(500);
  
  if (HAL_ADC_Start_DMA(&hadc1,
                        (uint32_t *)BufADC,
                        3
                          ) != HAL_OK)
  {
    myBeep(100);
    
    Error_Handler();
  }
  //  myBeep(100);
  //  HAL_Delay(500);
  //DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
  
  //  if (HAL_ADC_Start_DMA(&hadc2,
  //                        (uint32_t *)BufADD,
  //                        SizeBuf_ADC
  //                          ) != HAL_OK)
  //  {
  //    Error_Handler();
  //  }
  // попробуем запусить DAC
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
  // перенастроим UART7  для NEXTION
  huart7.Init.BaudRate = 9600;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_Delay(100);
  sprintf((void*)Str,"bauds=115200яяя");
  HAL_UART_Transmit(&huart7, (void*)Str,strlen((void*)Str),20); // выдаем 
  
  //NEX_Transmit(Str);// 
  HAL_Delay(100);
  huart7.Init.BaudRate = 115200;
  if (HAL_UART_Init(&huart7) != HAL_OK)
  {
    Error_Handler();
  }
  //  myBeep(100);
  HAL_Delay(100);
  
  HAL_TIM_Base_Start(&htim7); // запуск таймера 7
  
  // необходимо запустить таймеры, но они работают от мастера TIM1, 
  // в прерывании которго они могут перекофигурироваться и запускатся по его Перезаписи чуть позже!
  HAL_TIM_PWM_Start_IT (&htim1, TIM_CHANNEL_1 ); // мастер таймер Период повторения 
  HAL_TIM_PWM_Start (&htim3, TIM_CHANNEL_4 ); // Зондир.Импульс Тетсовый вроде как надо запустить таймеры
  HAL_TIM_PWM_Start (&htim4, TIM_CHANNEL_4 ); // запускаем таймер обеспечения перезапуска синхро таймера для ADC 
  HAL_TIM_PWM_Start (&htim2, TIM_CHANNEL_1 ); // для подсчета времени выполнения ДМА с заданными параметрами
  HAL_TIM_PWM_Start_IT (&htim5, TIM_CHANNEL_4 ); // для подсчета времени когда начать ссумировать
  //HAL_Delay(10);
  TIM7->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM7 (вспомогательный генратор()

  StopAllTIM(1);  // остановка таймеров (OTDR)
  
  //инициализация DCMI DMA (пока 10 точек но это не важно)
  // важно определить режим и буфер
  HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,10);
  
  //  HAL_Delay(100);
  //    sprintf((void*)Str,"page 1яяя");
  //  NEX_Transmit(Str);// 
  //  HAL_Delay(100);
  //  HAL_Delay(100);
  //    sprintf((void*)Str,"page 1яяя");
  //  NEX_Transmit(Str);// 
  //  HAL_Delay(100);
  //    myBeep(100);
  //
  //    sprintf((void*)Str,"page 23яяя");
  //  NEX_Transmit(Str);// 
  //  HAL_Delay(100);
  
  InitBtns(); 
  //вызов функции установки лазеров по местам согласно конфигурации
  SetIndxSeqLS();
  // востановим из памяти коментарий
  memcpy( CommentsOTDR,NameDB.UserComm, 20 );
  
  //myBeep(100);
  //HAL_Delay(100);
  // начало работы..
  TimeBegin = HAL_GetTick();
  
  CmdInitPage(0);// вызов окна заставки
  HAL_Delay(100);
  SetMode (ModeWelcome);
  CmdInitPage(0);// посылка команды переключения окна на Welcome и установка признака первого входа
  MX_USB_DEVICE_Init();
  
  //  MX_USB_DEVICE_Init(); //инициализация USB - долгий процесс
  // повисим возможно сдесь если подключены  к линии писаноем туда сообщение
  // здесь запускается "долгий" процесс связи с компьютером, и мешает инициализации, стоит
  // что-то предпринять
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { 
    //ContinueModulation(); // продолжение модуляции источника для режимов 270Гц и 2кГц
    //HAL_GPIO_TogglePin(KTS_GPIO_Port, KTS_Pin);
    // проверка кнопок 
    if((GetSysTick(0)>30)&&(!ProgFW_LCD))// каждые 30 мС или больше...
    {
      LED_KTS(1);
      // здесь кроме опроса
      KeyP = SetBtnStates( GetExpand (), 1 ); // опрос клавиатуры
      GetSysTick(1);// сброс системного ожидания
      // управление красным лазером
      // управление лазерами в режиме CW*
      // поконтролить батарейку
      // инекремент таймаре PA
      CountTimerPA++;
            // управление лазером в режиме CW*
      if(ModeLS == 4) // ВАЖНО при переходе в другие режимы без исользования источником, сбрасывать режим ЛАЗЕРА
      {
        Run_SCWP(); // вызовем функцию управления каждые 30 mS
      }

      if (!CheckLevelBattery ()) BadBattery();;

      if(++PeriodIntADC > 15)// 450 mS
      {
        // здесь можно запустить Измерение АЦП
        if (HAL_ADC_Start_DMA(&hadc1,(uint32_t *)&BufADC,3) != HAL_OK)
        {
          myBeep(100);
          Error_Handler();
        }
        PeriodIntADC = 0;
      }
      // если измеритель то можно почитать медленное АЦП 7782
      // вызвать раз в +30 мС для апроса и проверки готовности данных
      if(GetModePowerMeter() != NOTMODE)
      {
              // запуск и контоль измерителя P1
      GetPMData(); // получаем текущие значения АЦП
      }
            LED_KTS(0);

    }
    // проверка приема по UART EXT
    if (RSDecYes) // вызов программы обработки комманды принятой по UART
    {
      DecodeCommandRS();
    }
    
    
    // режим программирования индикатора и ответы от индикатора
    if(Uart2DecYes)
    {
      if(ProgFW_LCD)
      {
        CDC_Transmit(0, (uint8_t*)RX_BufNEX, CntRXNEX ); // echo back on same channel
      }
      else
      {
        // что-то приняли в ответ от индикатора, можно посмотреть
        CheckStrNEX (); // проверка принятой строки  
        
      }
      Uart2DecYes=0;
      RecievNEX=STOP_UART;
    }
    
    // надо объединить в непрерывную функцию сбора по установленным параметрм
    
    //    // начало измерения устанавливаем исходные параметры измерения, 
    //    // число проходов, шаг изменения положения зонд импульса
    //    if(EnaStartRun)
    //      {//
    //      StartRunFirst(); // подготовка перед запуском измерений
    //      // первое прерывание от TIM1_CCR1 пред начало счета ТИМ1
    //      while(Ena_AVRG)
    //      {
    //        //CountWait=0;
    //        //LED_START(1);
    //        while(Ena_CFG) // ждем установок для измерения
    //        {
    //        //  CountWait++;
    //        asm("NOP");
    //        }
    //       // LED_START(0);
    //       // BufNAK[0] = CountWait;
    //       // CountWait=0;
    //       // LED_START(1);
    //        while(Ena_SUMM)
    //        {
    //       //   CountWait++;
    //        asm("NOP");
    //        }
    //       // LED_START(0);
    //       // BufNAK[1] = CountWait;
    //             ContNextAvrg();
    //
    //        
    //      }
    //      EnaStartRun=0;
    //      }
    
    //    // суммирование текущего накопления , параметры должны быть установленны заранее
    //    // нужно посчитать число проходов ДМА - возможно это размер массива деленный на число повторений
    //    // если накопили до конца отключаем основной таймер и тормозим все остаальные
    //    // если суммируем перустанавливаем основной таймер в пред окончание и его запускаем
    //    if(EnaNextAvrg)
    //    {
    //      EnaNextAvrg = 0;
    //    }
    //здесь когда закончили очередной DMA можно проссумировать 
    // с параметром смещения зондирующего импульса,
    // по окнчании ссумрования можно перезустить новый ДМА
    // переустановить положение зонд импульса
    //    if(EnaPrintRes)
    //    {
    //      //LED_Y(0);
    //      //HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x0);
    //      //TIM6->CR1 &=~TIM_CR1_CEN;
    //      //TIM6->CNT =0;
    //      //TIM2->CR1 &=~TIM_CR1_CEN;
    //      //TIM2->CNT =0;
    //      // быстро посчитаем Логарифм
    //      if(PressKey)
    //      {
    //        GetLogData ();
    //        
    //        
    //        //LED_R(1); // delete
    //        // выдадим файл
    //       // SendFileBelcore ();
    //        // и запишем тест файл...
    //        HAL_Delay(200);
    //     //   SDMMC_SDCard_Test(CntIzmer);
    //        CntIzmer++;
    //        HAL_Delay(200);
    //        //LED_R(0); //delete
    //        PressKey =0;
    //        MeasureNow =0; //выключаем ограничение на прорисовку режима
    //      }
    //      // остановим таймер для ЦАП
    //      //HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x0);
    //      //      int sos = StartTime&0x3;
    //      //      switch(sos)
    //      //      {
    //      //      case 0:
    //      //        SM = 100;
    //      //        break;
    //      //      case 1:
    //      //        SM = 200;
    //      //        break;
    //      //      case 2:
    //      //        SM = 300;
    //      //        break;
    //      //      case 3:
    //      //        SM = 400;
    //      //        break;
    //      //      }
    //      
    //      EnaPrintRes=0;
    //    }
    // все параметры включим до вызова сновного цикала
    if((!MeasureNow)&&(!ProgFW_LCD))
    {
            LED_KTT(1);

      ModeFuncTmp(); // прорисовка текущего режима - основной цикл
      LED_KTT(0);
      // проверим занятость USB  и соотв высветим значек
      if(ModeUSB)
      {
        switch (ModeUSB)
        {
        case 1:
          sprintf((void*)Str,"pic 460,0,11яяя");//black
          break;
        case 2: // отключили USB
          sprintf((void*)Str,"pic 460,0,12яяя");//empty
          break;
        case 3:
          sprintf((void*)Str,"pic 460,0,9яяя");//RED
          break;
        }
        ModeUSB = 0;
        NEX_Transmit((void*)Str);// 
      }
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 8;
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
                              |RCC_PERIPHCLK_SDMMC|RCC_PERIPHCLK_UART5
                              |RCC_PERIPHCLK_UART7|RCC_PERIPHCLK_USART3;
  PeriphClkInitStruct.PLL2.PLL2M = 16;
  PeriphClkInitStruct.PLL2.PLL2N = 144;
  PeriphClkInitStruct.PLL2.PLL2P = 3;
  PeriphClkInitStruct.PLL2.PLL2Q = 8;
  PeriphClkInitStruct.PLL2.PLL2R = 6;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_1;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.PLL3.PLL3M = 16;
  PeriphClkInitStruct.PLL3.PLL3N = 96;
  PeriphClkInitStruct.PLL3.PLL3P = 2;
  PeriphClkInitStruct.PLL3.PLL3Q = 4;
  PeriphClkInitStruct.PLL3.PLL3R = 12;
  PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_1;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
  PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL2;
  PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_PLL2;
  PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// тики опроса клавиатура взято из Т7К_АР
uint32_t GetSysTick( int Mode) // получение тиков 1 мС. 0 - получение счетчика от предыдущего сброса 1- сброс
{
  static uint32_t MemTick;
  if(Mode) MemTick=HAL_GetTick();
  return HAL_GetTick()-MemTick;
}

// управление таймером в измерителе АВТОМАТЕ
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


void StopAllTIM(int Ext)  // остановка таймеров (OTDR)
{
  if(Ext==1)
  {
  TIM1->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM1 (вспомогательный генратор()
  TIM1->CNT = 0;
  }
  if(Ext!=2) 
  {
  TIM5->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM5 (время когда надо суммировать)
  TIM5->CNT = 0;
  }
  //TIM2->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM2 (Основной генератор задающий частоту повторения измерений)
  TIM2->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM2 (формирователь основных тактов для ДМА АЦП (30 мГц из 240/8)
  TIM3->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM3 (формирователь зондирующего импульса)
  TIM4->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM4 ()строб VSYNC HSYNC(
  //TIM12->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM8 (вспомогательный генратор()
  // пытаемся их всех запустить от мастера TIM1 - (то же самое в Т9400 только там мастер TIM3)
  TIM2->CNT = 0;
  TIM3->CNT = 0;
  TIM4->CNT = 0;
  //TIM12->CNT = 0;
  
}
   // Запуск текущего накопления , параметры должны быть установленны заранее
    // Здесь запускаем ДМА и работу таймера Зонд.Импульса с установленными параметрами
    // нужно посчитать число точек ДМА - возможно это размер массива деленный на число повторений
    void CreateNextAvrg(void)
    {
      //TIM4->CNT =30000;
      //SizeBlockNak = (uint32_t)(SizeBuf_ADC/NumRepit);
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNak/2-1);
       
      // надо правильно рассчитывать сдвижку зонд импульса
      // при Х (макс) - сдвиг на 1
      // при Х/2 - сдвиг на 2
      Sm_Shift = (MAXREPIT/NumRepit)*((NumRepit-1)-((CountDMA)%(NumRepit))); 
      // при первом съеме устанвливаем импульс на 63 точку
      // обратный индекс,далее двигаем к началу,
      // а в ссуммировании используем прямой индекс 
      // устанавливаем положение зондирующего импульса
      // !!!ВНИМАНИЕ НАДО ПОДРОБНО РАССЧИТАТЬ МЕСТО УСТАНОВКИ!!!
      TIM3->ARR = BeginSet + Sm_Shift + ZondImpulse; //расчетное первое положение ипульса в съеме 50+макссдвиг(40)+импульс(2)=90
      // реально снимаем на 97 при плотном 
      TIM3->CCR4 = BeginSet + Sm_Shift;
      DMA1_Stream2->NDTR = (SizeBlockNak/2)-1; // размер блока данных DMA 
      DMA1_Stream2->CR |= DMA_SxCR_EN; // запускаем новый цикл ДМА for DCMI
      //DCMI->CR|=DCMI_CR_ENABLE;          // DCMI capture enable
      DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
      //__HAL_DCMI_CLEAR_FLAG(&hdcmi, DCMI_FLAG_VSYNCRI);      //CountDMA++;// добавим позже после ссуммирования

      Ena_CFG = 0;
        //LED_KT(0); // закончили удержание сброса линии

      //TIM4->CNT =0;

    }
  //
 void StartADC_DMA (void)
 {
   HAL_ADC_Start_DMA(&hadc1,
                     (uint32_t *)BufADC,
                     8
                       );
   
 }
// Перерасчет в логарифмические данные ,
// пока без анализа но на со следующими параметрами
// начало с 60 точки , шум считаем с 5 по 45 точку в начале
// особенности всего 10 разрядов, напряжение не более 3 v
// пока ВСЕ данные что накопили NumNak
void GetLogData (void)
{
  uint32_t Noise; // 
  uint32_t LocalRaw;
  uint32_t Avrgs = NumNak;
  //uint32_t Beg
  int j=BeginShift; //получение текущего смещения по индексу 60
  // сильно зависит от какого входа снимаем сигнал, и от режима АЦП
  // для Дифф. AIN2 
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
  uint32_t CurrentMaxLog =(uint32_t)(5000.0*log10((double)Avrgs*(MaxADC))); // максимальный логарифм текщего накопления    
  
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
  // перенесенный заголовок дампа измерений
  // разрешение DS
  //Head_RAW.ValDS = (unsigned int)((ADCPeriod*50000)/NumRepit); //  устанавливаем значения DS для установленного режима измерения
  Head_RAW.ValDS = GetValueDS(); //  устанавливаем значения DS для установленного режима измерения
  // число накоплений NAV
  Head_RAW.NumAvrg = NumNak;
  // длина волны источника, длительность импульса зондирования AW PWU
  Head_RAW.AW_PWU = (1310)<<16; // записываем длину волны источника
  Head_RAW.AW_PWU += 10; // записываем длительность импульса
  // Формат данных, при прореживании (будет ли здесь не знаю = 0) смещение (начало линии) то что прописано в настройках
  Head_RAW.FormatData = 1;
  // размер окна блокировки, сейчас число точек на период 
  Head_RAW.SizeFrm = NumRepit;
  //число отсчетов NPPW (на выбранный импульс, он у нас один)
  Head_RAW.NumPtsMain = RAWSIZE;//SizeLogBuf;
  
}
// заполнение структуры блока заголовка белкора
//void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents) // заполняем шапку белкора
//{
//  const char *IdnsBC[2]= {"SvyazServis   \0","OPTOKON Co.Ltd\0"};
//
//  char Str[10];
//  char CommentsOTDR[20] = {"                   \0"}; //комментарии рефлектометра
//  
//  unsigned short DataShort ; // 
//  unsigned long DataInt ;
//  
//      // при чтении переустанавливаем настройки прибора (поэтому перезаполняем длину волны на которой измеряли)
//      switch (Block)
//      {
//      case 0: //0...55 byte
////        // первое заполнение надо прочитать файл
////      if (GetNumTrace()) // если не нулевая то читаем по таблице
////        TraceREAD(GetNumTraceSaved(GetNumTrace()));//  читаем файл который надо передать// 27/01/2011 неадекватно считывала рефлектограмму
////      else  TraceREAD(0);
//      memcpy( Name, HeaderBelcore, 56 );
//        if (NumEvents)
//        {
//          //memcpy( &Name[5], 7, 1);
//          Name[2]=0x62; // запись нового размера Map блока +16
//          Name[6]=7; // запись нового количества полей в Мар блоке
//          memcpy( &Name[56], &HeaderBelcore[225], 16 ); // дозапись поля Событий в Мар блок
//          DataShort = (unsigned short)(NumEvents*32+24); // получение размера блока в зависимости от числа событий
//          memcpy( &Name[68], &DataShort, 2);
//          
//        }
//        break;
//      case 1: //56...117 (72...133) 62 byte
//      memcpy( Name, &HeaderBelcore[56], 62 );
//        // устанавливаем длину волны
//      DataShort = (unsigned short)1310; // получение длины волны от индекса установочного места
//      // перезапись длинны волны 1300 для х4 приборов!
//      
//      memcpy( &Name[86-56], &DataShort, 2);
//      // заполняем Comments
//      memcpy( &Name[98-56], CommentsOTDR, 19);
//        break;
//      case 2: //118...211 (134...227) 94 byte
//      memcpy( Name, &HeaderBelcore[118], 94 );
//      // заполняем производителя
//      memcpy( &Name[118-118], IdnsBC[0], 14 );
//      // идентификатор изделия 12 байт 
//      sprintf (Str,"TOPAZ_AVRG  ");
//      // Set MDIF
//      memcpy( &Name[133-118], Str, 12 );
//      // Number Device
//      sprintf (Str,"%04d",9999);
//      memcpy( &Name[146-118], Str, 4 );
//      // релиз программы
//      sprintf (Str,"555x");
//      memcpy( &Name[153-118], Str, 4 );
//      
//      // время сохранения рефлектограммы 
//      // устанавливаем дату сохраненной рефлектограммы
//      // бросим 19.04.2024 16:00 = 
//      DataInt = 1713531924;//1713531924
//      memcpy( &Name[159-118], &DataInt, 4);
//      // (164)AW длина волны в 0.1 нм (1310нм = 13100) 0x332c 0x3c8c (164)
//      DataShort = 13100;
//      memcpy( &Name[165-118], &DataShort, 2);
//      // длина зондирующего импульса 
//      DataShort = 12;
//      memcpy( &Name[173-118], &DataShort, 2);
//      // устанавливаем DS
//      // ((10000points*10(in 1ns 100ps))/2 = 50000 , 333.333 ns - интервал съема информации
//      DataInt = (unsigned long)((ADCPeriod*50000)/NumRepit); //  устанавливаем значения DS для установленного режима измерения
//      memcpy( &Name[175-118], &DataInt, 4);
//      // ###(182) GI коэфф преломления  146583 (1.46583)  
//      DataInt = (unsigned long)( 1.46810*100000);
//      memcpy( &Name[183-118], &DataInt, 4);
//      // ###(186) BC = 800 предел обратного отражения
//      DataInt =  800;
//      memcpy( &Name[187-118], &DataInt, 2);
//      // ###(188) NAV число накоплений - зависит от времени накопления и длины линии = FinAvrg
//      DataInt = NumNak;
//      memcpy( &Name[189-118], &DataInt, 4);
//      // ###(192) AR  длина измеряемого участка (грубо число измерений на шаг) DS*NPPW/10000
//      DataInt = (unsigned long)((ADCPeriod*5*SizeLogBuf)/NumRepit); //  устанавливаем значения DS для установленного режима измерения
//      memcpy( &Name[193-118], &DataInt, 4);
//      // ###(200) NF нижний уровень шумов равен 65535
//      DataInt =  65535;//ReflParam.NF;
//      memcpy( &Name[201-118], &DataInt, 2);
//      // ###(206) LT минимальное значение затухания для события 200 в 0.001 дБ
//      DataInt =  200;//ReflParam.LT;
//      memcpy( &Name[207-118], &DataInt, 2);
//      // ###(208) RT минимальное значение отражения для события 40000 в -0.001дБ
//      DataInt =  40000;//ReflParam.RT;
//      memcpy( &Name[209-118], &DataInt, 2);
//      // ###(210) ET минимальное затухание для конца линии 3000 в 0.001 дБ
//      DataInt =  3000;//ReflParam.ET;
//      memcpy( &Name[211-118], &DataInt, 2);
//        break;
//      case 3: //212...223 (228...239) 12 byte
//      memcpy( Name, &HeaderBelcore[213], 12 );
//       break; 
//      }
//
//}
//<5555> убрал за ненадобностью что бы не мешало
//// <5555>Отправка файла Белкора построенных как Т7к без событий
//void SendFileBelcore (void)
//{
////  // таблица для расчета контрольной суммы по полиному 0x1021
////  static const short int table[] =
////  {
////    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
////    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
////    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
////    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
////    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
////    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
////    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
////    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
////    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
////    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
////    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
////    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
////    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
////    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
////    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
////    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
////    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
////    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
////    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
////    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
////    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
////    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
////    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
////    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
////    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
////    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
////    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
////    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
////    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
////    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
////    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
////    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
////  };
//  
//  char StartStr[8]={"#516611\0"}; // 5-х значные номера!!! т.к. более 8000 точек
//  //char BufString[225];
//  unsigned char *c;
//  unsigned char value;
//  unsigned short NumEventNow = 0; //без событий
//  unsigned short old_crc = 0xffff; 
//  unsigned short new_crc = 0xffff;
//  
//  // начинаем передачу трассы (Заголовок)
//  sprintf (StartStr, "#4%4d",16227 + ((NumEventNow)?(NumEventNow*32+40):(0)));
//  //UARTSend0 ((BYTE*)StartStr, 7);
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)StartStr,7); // выдаем 
//  //  TxDMA=1;
//  // ВНИМАНИЕ ПОКА ВЫЛЮЧИЛИ
//  //HAL_UART_Transmit(&huart3, (void*)StartStr,7,1); // выдаем 
//  //HAL_Delay(2);
//  // Мар страница белкора с учетом Таблицы событий (блок 0)
//  // если есть таблица событий....
//  GetHeaderBelcore (BufString, 0, NumEventNow); // заполняем шапку белкора первые 56 байт Block=0
//  //UARTSend0 ((BYTE*)BufString, 56+16*((NumEventNow)?(1):(0)));
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,56+16*((NumEventNow)?(1):(0))); // выдаем 
//  //  TxDMA=1;
//  HAL_UART_Transmit(&huart3, (void*)BufString,56+16*((NumEventNow)?(1):(0)),10); // выдаем 
//  // попробуем здесь выдать по виртуальному ком порту этот же блок
//    CDC_Transmit(0, (void*)BufString, 56+16*((NumEventNow)?(1):(0))); // выдаем блок
//  //HAL_Delay(3);
//  c = (unsigned char*)&BufString;
//  for (int i=0;i<56+16*((NumEventNow)?(1):(0));i++)
//  {
//    /* первый вариант подсчета контрольной суммы - табличный                                             */		
//    value = *c;
//    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//    old_crc = new_crc;
//    c++;
//  }
//  // заполняем шапку белкора  62 байт Block=1 (продолжение Мар блока + GenParams)
//  GetHeaderBelcore (BufString, 1, NumEventNow); 
//  //UARTSend0 ((BYTE*)BufString, 62);
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,62); // выдаем 
//  //  TxDMA=1;
//  //HAL_Delay(3);
//  HAL_UART_Transmit(&huart3, (void*)BufString,62,10); // выдаем 
//  // попробуем здесь выдать по виртуальному ком порту этот же блок
//    CDC_Transmit(0, (void*)BufString, 62); // выдаем блок
//  c = (unsigned char*)&BufString;
//  for (int i=0;i<62;i++)
//  {
//    /* Считаем контрольную сумму переданного блока                                             */		
//    value = *c;
//    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//    old_crc = new_crc;
//    c++;
//  }
//  // заполняем шапку белкора  94 байт Block=2 - (SupParams FxdParam)
//  GetHeaderBelcore (BufString, 2, NumEventNow); 
//  //UARTSend0 ((BYTE*)BufString, 95);
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,95); // выдаем 
//  //  TxDMA=1;
//  HAL_UART_Transmit(&huart3, (void*)BufString,95,10); // выдаем 
//  // попробуем здесь выдать по виртуальному ком порту этот же блок
//    CDC_Transmit(0, (void*)BufString, 95); // выдаем блок
//  //HAL_Delay(5);
//  c = (unsigned char*)&BufString;
//  for (int i=0;i<95;i++)
//  {
//    /* Считаем контрольную сумму переданного блока                                             */		
//    value = *c;
//    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//    old_crc = new_crc;
//    c++;
//  }
//  // Проверяем и передаем блок событий если он есть (блок событий)
//  //        if (NumEventNow) // если есть события 2 байта +
//  //          // события в фиксированном размере для каждого 32 байта  +  22 байт общее для всего блока
//  //        {
//  //          // передаем  число событий  2 байта
//  //          UARTSend0 ((BYTE*)&NumEventNow, 2);
//  //          c = (unsigned char*)&NumEventNow;
//  //          for (int i=0;i<2;i++)
//  //          {
//  //            /* Считаем контрольную сумму переданного блока                                             */		
//  //            value = *c;
//  //            new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//  //            old_crc = new_crc;
//  //            c++;
//  //          }
//  //          // передаем информационные блоки событий  N*32
//  //          for (int s=0; s<NumEventNow; s++)
//  //          {
//  //            UARTSend0 ((BYTE*)&EvenTrace[s], 32);
//  //            c = (unsigned char*)&EvenTrace[s];
//  //            for (int i=0;i<32;i++)
//  //            {
//  //              /* Считаем контрольную сумму переданного блока                                             */		
//  //              value = *c;
//  //              new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//  //              old_crc = new_crc;
//  //              c++;
//  //            }
//  //            
//  //          }
//  //          // передаем конечный блок событий 22 байта
//  //          UARTSend0 ((BYTE*)&EndEvenBlk, 22);
//  //          c = (unsigned char*)&EndEvenBlk;
//  //          for (int i=0;i<22;i++)
//  //          {
//  //            /* Считаем контрольную сумму переданного блока                                             */		
//  //            value = *c;
//  //            new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//  //            old_crc = new_crc;
//  //            c++;
//  //          }
//  //        }
//  
//  // заполняем шапку белкора 12 байт Block=3 (DataPts)
//  GetHeaderBelcore (BufString, 3, NumEventNow); 
//  //UARTSend0 ((BYTE*)BufString, 12);
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)BufString,12); // выдаем 
//  //  TxDMA=1;
//  HAL_UART_Transmit(&huart3, (void*)BufString,12,3); // выдаем 
//  // попробуем здесь выдать по виртуальному ком порту этот же блок
//    CDC_Transmit(0, (void*)BufString, 12); // выдаем блок
//  //HAL_Delay(1);
//  c = (unsigned char*)&BufString;
//  for (int i=0;i<12;i++)
//  {
//    /* Считаем контрольную сумму переданного блока                                             */		
//    value = *c;
//    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//    old_crc = new_crc;
//    c++;
//  }
//  
//  // блок данных 
//  //UARTSend0 ((BYTE*)LogData, OUTSIZE*2);
//  //        while(TxDMA);
//  //  HAL_UART_Transmit_DMA(&huart3, (void*)LogData,SizeLogBuf*2); // выдаем 
//  //  TxDMA=1;
//  HAL_UART_Transmit(&huart3, (void*)LogData,SizeLogBuf*2,2000); // выдаем блок логарифмических данных от двух байтовый
//  
//  // попробуем здесь выдать по вертуальному ком порту этот же блок
//    CDC_Transmit(0, (void*)LogData, SizeLogBuf*2); // выдаем блок логарифмических данных от двух байтовый
//
//  //HAL_Delay(150);
//  c = (unsigned char*)&LogData;
//  for (int i=0;i<SizeLogBuf*2;i++)
//  {
//    /* первый вариант подсчета контрольной суммы - табличный                                             */		
//    value = *c;
//    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
//    old_crc = new_crc;
//    c++;
//  }
//  
//  // UARTSend0 ((BYTE*)&new_crc, 2);
//  //while(TxCDCFree(0)); // ждем окончания передачи предыдущего блока
//  // без этого терялись последние два байта
//  HAL_UART_Transmit(&huart3, (void*)new_crc,2,1); // выдаем 
//  // попробуем здесь выдать по вертуальному ком порту этот же блок
//  // теряем этот блок при передаче предыдущего
//    CDC_Transmit(0, (void*)new_crc, 2); // выдаем блок контрольной суммы
//}

//Чужая программа теста работы FATFS in SDMMC2 with SD_Card
//void SDMMC_SDCard_Test(int Num)
//{
//  FATFS FatFs;
//  FIL Fil;
//  FRESULT FR_Status;
//  FATFS *FS_Ptr;
//  UINT RWC, WWC; // Read/Write Word Counter
//  DWORD FreeClusters;
//  uint32_t TotalSize, FreeSpace;
//  char FileNameS[20];
//  char RW_Buffer[200];
//  char TxBuffer[250]; // тестовый буффер передачи справочной нофрмации в UART3 (во внешний мир)
//  do
//  {
//    //------------------[ Mount The SD Card ]--------------------
//    FR_Status = f_mount(&FatFs, SDPath, 1);
//    if (FR_Status != FR_OK)
//    {
//      sprintf(TxBuffer, "Error! While Mounting SD Card, Error Code: (%i)\r\n", FR_Status);
//      UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//      break;
//    }
//    sprintf(TxBuffer, "00000\nSD Card Mounted Successfully! \r\n\n");
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    //------------------[ Get & Print The SD Card Size & Free Space ]--------------------
//    f_getfree("", &FreeClusters, &FS_Ptr);
//    TotalSize = (uint32_t)((FS_Ptr->n_fatent - 2) * FS_Ptr->csize * 0.5);
//    FreeSpace = (uint32_t)(FreeClusters * FS_Ptr->csize * 0.5);
//    sprintf(TxBuffer, "Total SD Card Size: %lu Bytes\r\n", TotalSize);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    sprintf(TxBuffer, "Free SD Card Space: %lu Bytes\r\n\n", FreeSpace);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    //------------------[ Open A Text File For Write & Write Data ]--------------------
//    //Open the file
//    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
//    sprintf(FileNameS,"MyFile%03d.txt",Num);
//    FR_Status = f_open(&Fil, FileNameS, FA_WRITE | FA_READ | FA_CREATE_ALWAYS);
//    if(FR_Status != FR_OK)
//    {
//      sprintf(TxBuffer, "Error! While Creating/Opening A New Text File, Error Code: (%i)\r\n", FR_Status);
//      UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//      break;
//    }
//    sprintf(TxBuffer, "Text File Created & Opened! Writing Data To The Text File..\r\n\n");
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    // (1) Write Data To The Text File [ Using f_puts() Function ]
//    f_puts("Hello! From STM32 To SD Card Over SDMMC, Using f_puts()\n", &Fil);
//    // (2) Write Data To The Text File [ Using f_write() Function ]
//    strcpy(RW_Buffer, "Hello! From STM32 To SD Card Over SDMMC, Using f_write()\r\n");
//    f_write(&Fil, RW_Buffer, strlen(RW_Buffer), &WWC);
//    // Close The File
//    f_close(&Fil);
//    //------------------[ Open A Text File For Read & Read Its Data ]--------------------
//    // Open The File
//    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ);
//    FR_Status = f_open(&Fil, FileNameS, FA_READ);
//    if(FR_Status != FR_OK)
//    {
//      sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Read.. \r\n");
//      UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//      break;
//    }
//    // (1) Read The Text File's Data [ Using f_gets() Function ]
//    f_gets(RW_Buffer, sizeof(RW_Buffer), &Fil);
//    sprintf(TxBuffer, "Data Read From (MyTextFile.txt) Using f_gets():%s", RW_Buffer);
//    //sprintf(TxBuffer, "Data Read From (%s) Using f_gets():%s",FileNameS, RW_Buffer);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    // (2) Read The Text File's Data [ Using f_read() Function ]
//    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
//    sprintf(TxBuffer, "Data Read From (%s) Using f_read():%s",FileNameS, RW_Buffer);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    // Close The File
//    f_close(&Fil);
//    sprintf(TxBuffer, "File Closed! \r\n\n");
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    //------------------[ Open An Existing Text File, Update Its Content, Read It Back ]--------------------
//    // (1) Open The Existing File For Write (Update)
//    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_OPEN_EXISTING | FA_WRITE);
//    FR_Status = f_open(&Fil, FileNameS, FA_READ); // Open The File For Read
//    FR_Status = f_lseek(&Fil, f_size(&Fil)); // Move The File Pointer To The EOF (End-Of-File)
//    if(FR_Status != FR_OK)
//    {
//      sprintf(TxBuffer, "Error! While Opening (MyTextFile.txt) File For Update.. \r\n");
//      UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//      break;
//    }
//    // (2) Write New Line of Text Data To The File
//    FR_Status = f_puts("This New Line Was Added During File Update!\r\n", &Fil);
//    f_close(&Fil);
//    memset(RW_Buffer,'\0',sizeof(RW_Buffer)); // Clear The Buffer
//    // (3) Read The Contents of The Text File After The Update
//    //FR_Status = f_open(&Fil, "MyTextFile.txt", FA_READ); // Open The File For Read
//    FR_Status = f_open(&Fil, FileNameS, FA_READ); // Open The File For Read
//    f_read(&Fil, RW_Buffer, f_size(&Fil), &RWC);
//    sprintf(TxBuffer, "Data Read From (MyTextFile.txt) After Update:\r\n%s", RW_Buffer);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//    f_close(&Fil);
//    //------------------[ Delete The Text File ]--------------------
//    // Delete The File
//    /*
//    FR_Status = f_unlink(MyTextFile.txt);
//    if (FR_Status != FR_OK){
//    sprintf(TxBuffer, "Error! While Deleting The (MyTextFile.txt) File.. \r\n");
//    USC_CDC_Print(TxBuffer);
//  }
//    */
//  } while(0);
//  //------------------[ Test Complete! Unmount The SD Card ]--------------------
//  FR_Status = f_mount(NULL, "", 0);
//  // думал не закрывать флэшку
//  //        sprintf(TxBuffer, "\r\nSD Card NO   Un-mounted Successfully! \r\n");
//  //    HAL_UART_Transmit(&huart3, (void*)TxBuffer,strlen(TxBuffer),50); // выдаем 
//  if (FR_Status != FR_OK)
//    
//  {
//    sprintf(TxBuffer, "\r\nError! While Un-mounting SD Card, Error Code: (%i)\r\n", FR_Status);
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//  } else{
//    sprintf(TxBuffer, "\r\nSD Card Un-mounted Successfully! \r\n");
//    UARTSendExt ((BYTE*)TxBuffer, strlen (TxBuffer));
//  }
//}
    // начало измерения устанавливаем исходные параметры измерения, 
    // число проходов, шаг изменения положения зонд импульса
    void StartRunFirst(unsigned AddIndx) //
    {
      uint32_t ClkADC_ARR;
      uint32_t ClkADC_CCR1;
      StopAllTIM(1);

      MeasureNow = 1; 
      //LED_Y(1);
      // при старте накоплений надо, перестроить таймеры в соответствии с заданными параметрами измерений
      // счетчик TIM2 - период повторения зондирующих импульсов
      // зависит от установленной длинны линии, шаг задали 1мкС 
      // все таймеры останавливаем и сбрасываем
      //memset(&BufADC, 0, sizeof(BufADC));
      //memset(&BufADD, 0, sizeof(BufADD));
      memset(&BufNAK, 0, sizeof(BufNAK));
      TIM4->CR1 &=~TIM_CR1_CEN;
      TIM2->CR1 &=~TIM_CR1_CEN;
      TIM3->CR1 &=~TIM_CR1_CEN;
      TIM5->CR1 &=~TIM_CR1_CEN;
      TIM4->CNT =30000;
      TIM2->CNT =0; // clk ADC (30,15,7.5,3.75)
      TIM3->CNT =0;
      TIM5->CNT =0;
      Sm_Shift = 0; // текущее значение сдвига Зонд.Импульса
      CountDMA = 0; //clear count DMA
      //HAL_DAC_Start_DMA(&hdac1, DAC_CHANNEL_1, (uint32_t *)CodeDAC, SizeBuf_DAC, DAC_ALIGN_12B_R);
      StartTime++;
      // считаем сколько нам сравнивать при прореживании число проходов увеличивается кратно числу повторений
      // так как будем измерять по одному накоплению, число проходов кратно прореживанию
      //SumNumNak = NumRepit;//NumNak*NumRepit;
      // Установка 
       IndexDist = GetIndexLN(); // индекс измерения длины выбранный пользователем (или заданный программой
       // для установки параметров измерения кроме периода измерения
       // период определяется если необходимо удлиннение
       // по индексу устанавливается Число повторений (NumRepit)
       // частота таймера ТИМ2 (такты АЦП)
       // начальный сдвиг положения зондирующего импульса BeginSet
       // Местоположение начала суммирования ТИМ5 
      // предустанавливается TIM1 в зависимости от диапазна (индекс)
       // Возможно будет изменен если необходимо удлиннение
       
      // устанавливаем длительности импульса ТИМ3 
      ZondImpulse = CalkZondImpuls();
      
      // важно установить место расположения зонд импульса в зависимости от частоты Съема
       //BeginSet = BEG_ADD + (64<<IndexDist);// установка местоположения зонд импульса в зависимости от диапазона
       //BeginSet = BeginShift+(64<<IndexDist);// установка местоположения зонд импульса в зависимости от диапазона
       if(IndexDist>3) //32..128 km
       {
         SizeStrob = 32<<IndexDist;
         //BeginSet = (BeginTest<<IndexDist)-((8<<IndexDist)- 95);//- (32<<(IndexDist-3));
         BeginSet = SizeStrob +(BeginTest<<IndexDist)-(60<<(IndexDist-3));//- (32<<(IndexDist-3));
         ClkADC_ARR = (1<<IndexDist)-1;
         ClkADC_CCR1 = 1<<(IndexDist-1);
         NumRepit = 1; 

       }
       else // 0.5 ... 16 km
       {
         SizeStrob = 256;
         BeginSet = SizeStrob+((BeginTest)<<IndexDist)-60;// установка местоположения зонд импульса в зависимости от диапазона
         // где 60 - это задержка на выдачу данных после преобразования
         ClkADC_ARR = 7;
         ClkADC_CCR1 = 4;
         NumRepit = 8>>IndexDist; 
       }
      
      // ТИМ4 - установка строба тот есть АЦП уже должно как минимум 30 тактов
      // пройти, что бы всё устаканилось
      // соответственно при плотном съеме 2-16 км это величина 256 = 32*8
      // 32 км - 512   (2.2 мкС)
      // 64 км - 1024  (4.3мкС)
      // 128 км - 2048 ( тиков 240 мГц) (8.5 мкс добавка для рассчета периода)
      TIM4->CCR4 = 1;
      TIM4->ARR = SizeStrob; 
      
      //TIM2 (clkADC)
         TIM2->ARR = ClkADC_ARR;
         TIM2->CCR1 = ClkADC_CCR1;
      
      // установить! TIM5 на время начала суммирования шаг 1/240 мкС , одиночный импульс
      // 5000 точек суммируется за приблизительно 305 мкС
      // соотв при более плотных съемах
      // 0.5 2 км - время суммирования ~ 40uS
      // 4 км - время суьмирования ~ 80uS
      // 8 км - время суьмирования ~ 160uS
      // 16 км - время суьмирования ~ 320uS
      TIM5->ARR = 15;
      TIM5->CCR4 = 5;
      // восстановим частоту - так на всякий случай

       // здесь надо разделить период повторения и параметр съема
       switch(IndexDist) // устанвливаем параметры Съема
       {
       case 0: //0.5 2.0 km - 33.3nS*700=24uS = Тнак (прореживание = 4)
         // Тсумм = 42 мкС > Тнак !!! поэтому можем суммировать сразу 
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 65;
         //NumRepit = 8; 
         //TIM2->PSC = 0;
         //TIM2->ARR = 7;
         //TIM2->CCR1 = 4;
         //BeginSet = BEG_ADD + (8<<IndexDist);

         //TIM4->PSC = 0;
         //TIM4->ARR = TIM4->CCR4 + (8+1)*2;
         TIM5->CCR4 = 5; //через 1.666 мкС + 42 мкС(сумм 700) меньше 105 мкС
         TIM5->ARR = 6;
         break;
       case 1: //4 km - 33.3nS*1400=47uS = Тнак (прореживание = 4)
         // Тсумм = 84 мкС > Тнак !!! поэтому можем суммировать сразу 
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 100;
         //NumRepit = 4; 
         //TIM2->PSC = 0;
         //TIM2->ARR = 7;
         //TIM2->CCR1 = 4;
         //TIM4->PSC = 0;
         //TIM4->ARR = TIM4->CCR4 + (8+1)*2;
         TIM5->CCR4 = 11; //через 1.666 мкС + 84 мкС(сумм 1400) меньше 100 мкС
         TIM5->ARR = 12;
         break;
       case 2: //8 km - 33.3nS*2800=94uS = Тнак (прореживание = 2)
         // Тсумм = 168 мкС > Тнак !!! поэтому можем суммировать сразу 
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 200;
         //NumRepit = 2; 
         //TIM2->PSC = 0;
         //TIM2->ARR = 7;
         //TIM2->CCR1 = 4;
         //TIM4->PSC = 0;
         //TIM4->ARR = TIM4->CCR4 + (8+1)*2;
         TIM5->CCR4 = 239; // +168 мкС (сумм 2800) меньше 200мкС
         TIM5->ARR = 240;
         break;
       case 3: //16 km - 186uS 33.3nS*5600=186uS  = Тнак время накопления
         // Тсумм = 336 мкС > Тнак !!! поэтому можем суммировать сразу 
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 350;
         //NumRepit = 1; 
         //TIM2->PSC = 0;
         //TIM2->ARR = 7;
         //TIM2->CCR1 = 4;
         //TIM4->PSC = 0;
         //TIM4->ARR = TIM4->CCR4 + (8+1)*2;
         TIM5->CCR4 = 489; // через 1.666 мкС + 336 мкС (сумм 5600) меньше  350мкс
         TIM5->ARR = 490;
         break;
       case 4: //32km - 330uS 66.6nS*5600=373uS = Тнак
         // Тсумм = 336 мкС
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 420;
         //NumRepit = 1; 
         //TIM2->PSC = 1;
         //TIM2->ARR = 15;
         //TIM2->CCR1 = 8;
         //TIM4->PSC = 1;
         //TIM4->ARR = TIM4->CCR4 + DBPRO;//(16)*3;
         TIM5->CCR4 = 12000;//50*240; 373uS = Тнак < через 50 мкС + 336 мкС (сумм 5600) < 420 мкС
         TIM5->ARR = 12001;//51*240;
         break;
       case 5: //64km - 660uS 133.3nS*5600=750uS = Тнак
         // Тсумм = 336 мкС
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 800;
         //NumRepit = 1; 
         //TIM2->PSC = 3;
         //TIM2->ARR = 31;
         //TIM2->CCR1 = 16;
         //TIM4->PSC = 3;
         //TIM4->ARR = 100+24;
         TIM5->CCR4 = 100800;//240*420; 750uS < через 420 мкС + 336 мкС (сумм 5600) < 800 мкС
         TIM5->ARR = 100801;//361;
         break;
       case 6: //128km - 1300uS 266.6нС *5600=1493uS = Тнак
         // Тсумм = 336 мкС
         TIM1->ARR = TimeRepitOfLN[IndexDist];
         //TIM1->ARR = 1600; // Тповт
         //NumRepit = 1; 
         //TIM2->PSC = 7;
         //TIM2->ARR = 63;
         //TIM2->CCR1 = 32;
         //TIM4->PSC = 7;
         //TIM4->ARR = 64;
         TIM5->CCR4 = 288000;//240*1200;1493 мкС < через 1200 мкС + 336 мкС (сумм 5600) < 1493 мкС
         TIM5->ARR = 288001;//1201;
         break;
       default:
         TIM1->ARR = 75;
         TIM5->CCR4 = 8;
         TIM5->ARR = 9;
         
         break;         
       }
 // переустановим период повторения если надо
       // ранее рассчитали диапазон который получили по длине
        // то устанавливать AddIndx = AddIndexLN = IndxAddBad будем как рассчитанный при
        //   
       if(AddIndx)
       {
         // было
//         switch(AddIndx)
//         {
//         case 1:
//         TIM1->ARR = 120;
//           break;
//         case 2:
//         TIM1->ARR = 200;
//           break;
//         case 3:
//         TIM1->ARR = 350;
//           break;
//         case 4:
//         TIM1->ARR = 450;
//           break;
//         case 5:
//         TIM1->ARR = 750;
//           break;
//         case 6:
//         TIM1->ARR = 1500;
//           break;
//         }
         // стало
         TIM1->ARR = TimeRepitOfLN[AddIndx];
        //TIM1->ARR = AddIndx*10; 
       }
      
      // ВНИМАНИЕ! Здесь надо перенастроить TIM1 для устаногвленного режима прореживания
      // тайминг TIM1 = 1uS Соотв для коротких линий ставим не менее 100 далее кратно 
      // отношению МаксРепит к текущему  ставим TIM1 (MAXREPIT/NumRepit)
      // здесь устанавливаем период повторения зонд импульсов
      // рассчитывается по заданному диапазону, и времени суммировния,
//      SW_TIM1 = (MAXREPIT/NumRepit);
//      TIM1->ARR = 2500;
//      if(SW_TIM1>1)
//      {
//        //for ADC MS9280 30MHz 6000 points
//        //( NumRepit == 8 => 240 MHz ~0.4 m , max = 2.4 km => ) sum 40 uS ~ 4 km
//        //( NumRepit == 4 => 120 MHz ~0.8 m , max = 4.8 km)sum 80 uS ~ 8 km
//        //( NumRepit == 2 =>  60 MHz ~1.6 m , max = 9.2 km) sum 160 uS ~ 16 km
//        //( NumRepit == 1 =>  30 MHz ~3.2 m , max = 18.4 km) sum 320 uS ~ 32 km
//     
//        
//        TIM1->ARR = 100*SW_TIM1;
//      } 
      //TIM1->CCR1 = TIM1->ARR - 5*(1+IndexDist);
      //TIM1->CNT = TIM1->CCR1 - 5*(1+IndexDist);
      TIM1->CCR1 = TIM1->ARR - 5;
      TIM1->CNT = TIM1->CCR1 - 5;
       //EnaStartRun = 0;
      
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNakVar);
      
      PressKey=1;
      // тест по счетчикам прерываний
      CountCC4 = 0 ; // число совершенных прерываний по TIM4_CH4
      CountEndCMI = 0;
      
      SizeBlockNak = (uint32_t)(PointForDist/NumRepit);
      //HAL_DCMI_Start_DMA(&hdcmi,DCMI_MODE_SNAPSHOT,(uint32_t)BufADD,SizeBlockNak/2-1);
      //LED_START(1);
      Ena_AVRG = 1; // для цикла накоплений
      Ena_CFG = 1; // для установок цикла одного накопления
      Ena_SUMM = 1; // для ожидания начала суммирования накоплений
      // для боевой , это пока не запускаем, таймер СТОИТ
      //TIM1->CR1 |=TIM_CR1_CEN;
      // тут полетело накопление, по окончании сбора
      // попадем в прерывание где сформируем повый запуск если необхоимо
      
      //DCMI->CR|=DCMI_CR_CAPTURE;          // DCMI capture enable
      
      
      // ВНИМАНИЕ! первый съем будет при сдвиге 1 сразу, 
      //надо учесть чтобы дособрать последний на 0 сдвиге!
      //LED_Y(0);
      //HAL_Delay(100);
    }


    // суммирование текущего накопления , параметры должны быть установленны заранее
    // нужно посчитать число проходов ДМА - возможно это размер массива деленный на число повторений
    // если накопили до конца отключаем основной таймер и тормозим все остаальные
    // если суммируем перустанавливаем основной таймер в пред окончание и его запуск




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
