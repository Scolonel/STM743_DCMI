#ifndef __SYSTEM_H
#define __SYSTEM_H
// INCLUDES FILES
#define ON 1
#define OFF 0
#define YES 1
#define NO 0
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*a))
// языки
#define Rus 0
#define Eng 1
#define Cze 2
#define MAX_LANG 4 // число разрешенных языков без учета 0-русского
//
#define MANUAL 0
#define AUTO 1
#define MaxMemPM 3200
#define MaxMemOTDR 200
#define MaxMemOTDRExt 250
#define MAXMEMALL ((CheckIDMEM)?(MaxMemOTDR+MaxMemOTDRExt):(MaxMemOTDR))
//число сообщений и языков
#define LANG_NUM 6  // число столбцов в таблице языков

#define MSG_NUM 126
#define CMD_NUM 35 //команды Nextion

#define TIMERE 500 //время цикла красного глаза по 500 мС

#define LIGHTSPEED 299792458
// для часов 
  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // число секунд 2000 года

// размеры массивов 
#define POINTSIZE    5200 //4096 // число активных точек, размер числа точек графика

#define NEXSIZE    5200 //4096 //0x1000  //размер для НЕКСТИОН
#define OUTSIZE    5200 //4096 //0x1000  // выходной массив
#define RAWSIZE    5500 //4384 //0x1200  //(4096+288)
#define FLTSIZE    5 // размерность фильтра
// массивы из первичной проверки...перекликаются с размерами объявленными ранее
#define SizeBuf_ADC_int 8
#define SizeBuf_ADC 6000//8192
#define SizeLogBuf 8000
#define SizeBuf_DAC 75
#define ResolutionADC  10


// для управления всякими UART
#define BUFSIZE   0x80
#define BUFSIZEUART1   0x80
#define BUFSIZEUART2   0x80

#define STOP_UART  0
#define START_UART  1
#define END_UART  2

#define BEG_ADD  64 // добавка для определения начала зонд импульса для разных диапазонов

//
#define LENGTH_LINE_NUM 7 // число длин линий (диапазонов)


#define BELCORESIZE 224
#define ENAOLDLCD 0 //разрешение прорисовки на старый LCD 1-разрешено, 0-запрещено


#include "integer.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "pm.h" // функции измерителя
#include "rtc.h" // 
//#include "fatfs.h"
#include "pca955x.h"
#include "buttons.h"   
#include "sdmmc.h"
//#include "ff.h"
// из FATFS библиотеки
#include "fatfs.h"
#include "ff_gen_drv.h"
#include "ff.h"
#include "ffconf.h"

//#include "usbd_cdc_acm_if.h"
#include "OTDR_Meas.h" //from T7kAR
//#include "HardSetDevice.h" // функции управления "железом" 
#include "usart.h" // for UART 

#include "events.h" //from T7kAR
#include "devicedata.h"  //from T7kAR
#include "device.h"  //from T7kAR
#include "modes.h"  //from T7kAR
#include "UartNEX.h"  //from T7kAR
#include "UartEXT.h"  //from T7kAR
#include "drawfunctions.h" //from T7kAR
#include "memflash.h"// //from T7kAR

//const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};
  // таблица для расчета контрольной суммы по полиному 0x1021
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


      // тики опроса клавиатура взято из Т7К_АР
uint32_t GetSysTick( int Mode); // получение тиков 1 мС. 0 - получение счетчика от предыдущего сброса 1- сброс

void NEX_Transmit(uint8_t *Str);
// управление таймером в измерителе АВТОМАТЕ
WORD TimerPA (BYTE Set);

extern char NameDir[100][6];
extern char NameFiles[1000][17];
extern uint32_t NumNameDir; // число имен директорий
extern uint32_t IndexNameDir;// индекс дирректории на которую указываем
extern uint32_t IndexLCDNameDir;// индекс указателя на индикаторе дирректории на которую указываем
extern uint32_t NumNameFiles; // число имен файлов
extern uint32_t IndexNameFiles;// индекс файла на который указываем
extern uint32_t IndexLCDNameFiles;// индекс указателя на индикаторе файла на который указываем
extern uint32_t PageDir; 
extern uint32_t PageFiles; 

// структура хранения памяти измерителя
struct tag_PON
{
  unsigned int TotalTimeCell; // Время измерения                          4
  unsigned int LenWaveKlb[3]; //                                              12      
  //char SuFix[6];  //                                                     0      
  char CommUserPM[16];  // Комментарии измерителя                         16      
  unsigned int LenWaveMeas; // значение длины волны 1 измерения                    4
  float PowLevel[3];  // значение мощности                               12
  float BaseLvl[3];  // базовый уровень (при измерении)                     12
  unsigned short int NumFix; // номер суфикса менятся в редакторе         2
  unsigned char CellMod; // мод индикации (dB, dBm, W)/или индекс длины волны   1      
  unsigned char Rez; //    режим измерителя 0-авто 1- ручной                                        1      
 // unsigned int RZ2; // резерв1
}; // blok 64 bytes

extern struct tag_PON PONI;

typedef struct
{
  // Структура установок заголовка передачи файла (24 байта)
  unsigned char Head[8]; // первое слово зависит от длины блока ( #4/5)
  unsigned int ValDS; /* разрешающая способность 10000 точек, одно проходного значения в 100 пС(0.1нС) частоты съема*/
  unsigned int NumAvrg;    // число накоплений 
  unsigned int AW_PWU;    // длина волны лазера (ст слово), длительность импульса (младшее слово)
  unsigned int FormatData; // формат данных, основные - 0  или для индикации (мин/макс) - N- масштаб (степень прореживания)
  unsigned int SizeFrm; // размер окна блокировки в 100пС
  unsigned int NumPtsMain; // число отсчетов блока данных
  
  
} Measuring_Stat;

extern Measuring_Stat Head_RAW;
// новые функции сборщика перетащенные из MAIN
    // начало измерения устанавливаем исходные параметры измерения, 
    // число проходов, шаг изменения положения зонд импульса
    void StartRunFirst(unsigned AddIndx); //
    // суммирование текущего накопления , параметры должны быть установленны заранее
    // нужно посчитать число проходов ДМА - возможно это размер массива деленный на число повторений
    // если накопили до конца отключаем основной таймер и тормозим все остаальные
    // если суммируем перустанавливаем основной таймер в пред окончание и его запускаем
// сумматор должен работать быстро
    void ContNextAvrg(void);
    // for Accum
extern uint32_t SizeBlockNak; // текущее значение размера блока ДМА ( в зависимости от прореживания)
extern uint32_t NumRepit;// число повторений, для самого плотного = 8
extern uint16_t BufADD[SizeBuf_ADC]; // буфер 2АЦП, в него пишем при съеме DMA, размер до 8192
extern uint32_t BufNAK[SizeBuf_ADC]; // буфер накопления, в него добавляем из буфера АЦП
extern uint32_t SumNumNak; // суммарное число проходов при данном числе накоплений
extern uint8_t MeasureNow; // признак работы накопителя, для обхода, основного цикла
extern uint32_t Sm_Shift ; // текущее значение сдвига Зонд.Импульса

//void GetPointInPeriod
DWORD CalkZondImpuls (void); // расчет длительности импульса

//const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// число точек на период
void Averaging (int NumAccum,unsigned AddIndexLN, BYTE EnDrawGraph );// функция накопления с установкой задержки после накопления
void DrawPictureMeas (BYTE EnDraw); // рисование картинки при измерении
void FiltrWOW (DWORD* array, unsigned Avrgs );
// check Speed Uart2
int CheckSpeedUart2 (void);
// События конца
//extern unsigned short PosEndEvenBlkELMP; // позиция расположения конца линии в отсчетах (надо продублировать для получения данных

extern UART_HandleTypeDef huart5;

// extern void (*ModeFunc)(void);
//extern unsigned short LogData[OUTSIZE] ; // логарифмические данные
extern uint16_t LogData[SizeBuf_ADC]; // буфер логарифмических данных накопления, в него добавляем из буфера АЦП

extern unsigned char NexData[NEXSIZE] ; // для графика в Nextion
extern DWORD RawData[RAWSIZE] ;// НЕ обработанные данные
extern unsigned int PointsPerPeriod; // шаг дискретизации
extern unsigned int PointInPeriod; // шаг дискретизации
extern int CntAccumulat; // счетчик накоплений
// переменные счетчика Т2, предыдущее и новое
extern volatile DWORD OldTimeAccum; // предыдущее значение
extern volatile DWORD CurrTimeAccum; // текущее значение
extern uint8_t EnaTimerAccum; // текущее значение

extern volatile unsigned char rawPressure;
extern volatile unsigned char rawPressKeyS; // признак необработанной нажатой клавиши S по прерыванию
extern volatile unsigned char KeyS; 
//extern char screen[1024] ;
extern uint8_t RX_BufNEX[BUFSIZEUART2];
extern char VerFW_LCD[25]; //версия ПО индикатора NEXION
extern char NameReadFile[32]; // глобальная имя файла при чтении в сохранении

extern volatile DWORD CntRXNEX;
extern volatile uint32_t RSDecYes; // здесь и признак приема по UART и чиcло байт при приеме от USB
extern volatile BYTE Uart2DecYes;
extern volatile DWORD RecievNEX;
extern volatile BYTE ReadyNEX; // готовность управления NEXTION
extern volatile BYTE g_WtRdyNEX;
extern volatile BYTE ProgFW_LCD; // признак программирования индикатора Nexion
extern volatile DWORD UART2Count; // счетчик приема байт UART2 от индикатора NEX - интересны в режиме программирования
extern volatile DWORD UART0Count; // счетчик приема байт UART0 для индикатора NEX - интересны в режиме программирования
extern volatile BYTE UART2Buffer[BUFSIZEUART2];
extern volatile WORD CountTimerUart2;
//extern unsigned short DataADC ;
extern DWORD TimeBegin; // время начала, для контроля начальной заставки
extern uint8_t WIDE_VER;
extern BYTE CurrLang; // текущий язык
extern unsigned short KeyP; // клавиши нажатые 
//extern int CntAccumulat; // счетчик накоплений
// кнопки клавиатуры

// внешняя переменная для конфигурации пина управления (CW) лазером
//extern unsigned long PIN_PHLD;

extern const char *CmdNextion[CMD_NUM]; 
extern const char *MsgMass[MSG_NUM][LANG_NUM]; 
//extern const char MsgMass[MSG_NUM][LANG_NUM][32]; 
extern const DWORD DelayPeriod[LENGTH_LINE_NUM];// задержка периода в тиках CreatDelay()~ 83.33 нс
extern const DWORD NumPointsPeriod[LENGTH_LINE_NUM];
extern const DWORD MultIndex[LENGTH_LINE_NUM];//множитель уменьшения частоты съема

extern const unsigned char fonts[2048];
extern char BufString[225];
//extern char BufString[60];
//extern const char *NumVer; // номер версии
extern char NumVer[10]; // номер версии

extern unsigned int CheckErrMEM; 
extern BYTE CheckIDMEM; // какая флэшь стоит? (0- 16 , 1- 32)

extern unsigned int TimerValueJDSU; // текущее значение частоты приемника RS
extern unsigned long TimerCR; // значение таймера 1 (его разница) при подсчете периода между 2-мя импульсами на входе 
// для индикатора
extern volatile BYTE g_GetStr; // признак ожидания получения строки из редактора. 
extern volatile BYTE g_NeedScr; // перерисовки экрана ! Аккуратно! надо проверить 
extern volatile BYTE g_FirstScr; // первый вход в экран, нужна полная перерисовка (заполнение), далее изменяем только нужные поля, для NEXTION
extern volatile BYTE IndexSmall; // переключатель для 2 км ,0 - 0.5 км, 1 - 2км 
extern volatile BYTE TypeLCD ; // тип индикатора в идентификаторе v-3.2(=0) s-3.5(=1) 
extern volatile BYTE g_NeedChkAnsvNEX; // признак получения строки из редактора.и ее проверка
extern volatile BYTE g_AutoSave; // признак режима авто сохранения измерений на выбраных длинах волн
extern volatile int NeedReturn; // необходимость вернуться в окно сохранения
//extern BYTE g_SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index 

extern BYTE MemTable[MaxMemOTDR+1]; // таблица рефлектограмм ячейки памяти меняется в памяти до MaxMemOTDR
extern BYTE MemTableExt[MaxMemOTDRExt+1]; // таблица рефлектограмм расширенной ячейки памяти меняется в памяти до MaxMemOTDRExt
extern uint8_t BusyUSB ; // признак передачи данных по USB, с SD картой
extern uint16_t PresentUSB ; // признак подключенного USB
extern uint8_t ModeUSB ; // признак подключенного USB

extern   FIL Fil;
extern   FRESULT FR_Status;
extern   FRESULT res;
extern  DIR dir;
#endif
