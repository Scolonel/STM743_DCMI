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
#define CMD_NUM 33 //команды Nextion

#define TIMERE 500 //время цикла красного глаза по 500 мС

#define LIGHTSPEED 299792458
// для часов 
  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // число секунд 2000 года

// размеры массивов 
#define NEXSIZE    0x1000  //размер для НЕКСТИОН
#define OUTSIZE    0x1000  // выходной массив
#define RAWSIZE    0x1200  //(4096+288)
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
//#include "fonts.h"
#include "pca955x.h"
#include "buttons.h"   

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

      // тики опроса клавиатура взято из Т7К_АР
uint32_t GetSysTick( int Mode); // получение тиков 1 мС. 0 - получение счетчика от предыдущего сброса 1- сброс

void NEX_Transmit(uint8_t *Str);
// управление таймером в измерителе АВТОМАТЕ
WORD TimerPA (BYTE Set);

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
    void StartRunFirst(void ); //
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

//void GetPointInPeriod
DWORD CalkZondImpuls (void); // расчет длительности импульса

//const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// число точек на период
void Averaging (int NumAccum,unsigned DelayAfterAvrg, BYTE EnDrawGraph );// функция накопления с установкой задержки после накопления
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
extern volatile BYTE RX_BufNEX[BUFSIZEUART2];
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
extern BYTE g_SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index 
extern BYTE mg_SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index это в памяти

extern BYTE MemTable[MaxMemOTDR+1]; // таблица рефлектограмм ячейки памяти меняется в памяти до MaxMemOTDR
extern BYTE MemTableExt[MaxMemOTDRExt+1]; // таблица рефлектограмм расширенной ячейки памяти меняется в памяти до MaxMemOTDRExt
extern uint8_t BusyUSB ; // признак передачи данных по USB, с SD картой


#endif
