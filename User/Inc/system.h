#ifndef __SYSTEM_H
#define __SYSTEM_H
// INCLUDES FILES
#define ON 1
#define OFF 0
#define YES 1
#define NO 0
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(*a))
// �����
#define Rus 0
#define Eng 1
#define Cze 2
#define MAX_LANG 4 // ����� ����������� ������ ��� ����� 0-��������
//
#define MANUAL 0
#define AUTO 1
#define MaxMemPM 3200
#define MaxMemOTDR 200
#define MaxMemOTDRExt 250
#define MAXMEMALL ((CheckIDMEM)?(MaxMemOTDR+MaxMemOTDRExt):(MaxMemOTDR))
//����� ��������� � ������
#define LANG_NUM 6  // ����� �������� � ������� ������

#define MSG_NUM 126
#define CMD_NUM 33 //������� Nextion

#define TIMERE 500 //����� ����� �������� ����� �� 500 ��

#define LIGHTSPEED 299792458
// ��� ����� 
  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // ����� ������ 2000 ����

// ������� �������� 
#define NEXSIZE    0x1000  //������ ��� ��������
#define OUTSIZE    0x1000  // �������� ������
#define RAWSIZE    0x1200  //(4096+288)
#define FLTSIZE    5 // ����������� �������
// ������� �� ��������� ��������...������������� � ��������� ������������ �����
#define SizeBuf_ADC_int 8
#define SizeBuf_ADC 6000//8192
#define SizeLogBuf 8000
#define SizeBuf_DAC 75
#define ResolutionADC  10


// ��� ���������� ������� UART
#define BUFSIZE   0x80
#define BUFSIZEUART1   0x80
#define BUFSIZEUART2   0x80

#define STOP_UART  0
#define START_UART  1
#define END_UART  2

//
#define LENGTH_LINE_NUM 7 // ����� ���� ����� (����������)


#define BELCORESIZE 224
#define ENAOLDLCD 0 //���������� ���������� �� ������ LCD 1-���������, 0-���������


#include "integer.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "pm.h" // ������� ����������
#include "rtc.h" // 
//#include "fonts.h"
#include "pca955x.h"
#include "buttons.h"   

//#include "usbd_cdc_acm_if.h"
#include "OTDR_Meas.h" //from T7kAR
//#include "HardSetDevice.h" // ������� ���������� "�������" 
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

      // ���� ������ ���������� ����� �� �7�_��
uint32_t GetSysTick( int Mode); // ��������� ����� 1 ��. 0 - ��������� �������� �� ����������� ������ 1- �����

void NEX_Transmit(uint8_t *Str);
// ���������� �������� � ���������� ��������
WORD TimerPA (BYTE Set);

// ��������� �������� ������ ����������
struct tag_PON
{
  unsigned int TotalTimeCell; // ����� ���������                          4
  unsigned int LenWaveKlb[3]; //                                              12      
  //char SuFix[6];  //                                                     0      
  char CommUserPM[16];  // ����������� ����������                         16      
  unsigned int LenWaveMeas; // �������� ����� ����� 1 ���������                    4
  float PowLevel[3];  // �������� ��������                               12
  float BaseLvl[3];  // ������� ������� (��� ���������)                     12
  unsigned short int NumFix; // ����� ������� ������� � ���������         2
  unsigned char CellMod; // ��� ��������� (dB, dBm, W)/��� ������ ����� �����   1      
  unsigned char Rez; //    ����� ���������� 0-���� 1- ������                                        1      
 // unsigned int RZ2; // ������1
}; // blok 64 bytes

extern struct tag_PON PONI;

typedef struct
{
  // ��������� ��������� ��������� �������� ����� (24 �����)
  unsigned char Head[8]; // ������ ����� ������� �� ����� ����� ( #4/5)
  unsigned int ValDS; /* ����������� ����������� 10000 �����, ���� ���������� �������� � 100 ��(0.1��) ������� �����*/
  unsigned int NumAvrg;    // ����� ���������� 
  unsigned int AW_PWU;    // ����� ����� ������ (�� �����), ������������ �������� (������� �����)
  unsigned int FormatData; // ������ ������, �������� - 0  ��� ��� ��������� (���/����) - N- ������� (������� ������������)
  unsigned int SizeFrm; // ������ ���� ���������� � 100��
  unsigned int NumPtsMain; // ����� �������� ����� ������
  
  
} Measuring_Stat;

extern Measuring_Stat Head_RAW;
// ����� ������� �������� ������������ �� MAIN
    // ������ ��������� ������������� �������� ��������� ���������, 
    // ����� ��������, ��� ��������� ��������� ���� ��������
    void StartRunFirst(void ); //
    // ������������ �������� ���������� , ��������� ������ ���� ������������ �������
    // ����� ��������� ����� �������� ��� - �������� ��� ������ ������� �������� �� ����� ����������
    // ���� �������� �� ����� ��������� �������� ������ � �������� ��� ����������
    // ���� ��������� ���������������� �������� ������ � ���� ��������� � ��� ���������
// �������� ������ �������� ������
    void ContNextAvrg(void);
    // for Accum
extern uint32_t SizeBlockNak; // ������� �������� ������� ����� ��� ( � ����������� �� ������������)
extern uint32_t NumRepit;// ����� ����������, ��� ������ �������� = 8
extern uint16_t BufADD[SizeBuf_ADC]; // ����� 2���, � ���� ����� ��� ����� DMA, ������ �� 8192
extern uint32_t BufNAK[SizeBuf_ADC]; // ����� ����������, � ���� ��������� �� ������ ���
extern uint32_t SumNumNak; // ��������� ����� �������� ��� ������ ����� ����������

//void GetPointInPeriod
DWORD CalkZondImpuls (void); // ������ ������������ ��������

//const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// ����� ����� �� ������
void Averaging (int NumAccum,unsigned DelayAfterAvrg, BYTE EnDrawGraph );// ������� ���������� � ���������� �������� ����� ����������
void DrawPictureMeas (BYTE EnDraw); // ��������� �������� ��� ���������
void FiltrWOW (DWORD* array, unsigned Avrgs );
// check Speed Uart2
int CheckSpeedUart2 (void);
// ������� �����
//extern unsigned short PosEndEvenBlkELMP; // ������� ������������ ����� ����� � �������� (���� �������������� ��� ��������� ������

extern UART_HandleTypeDef huart5;

// extern void (*ModeFunc)(void);
//extern unsigned short LogData[OUTSIZE] ; // ��������������� ������
extern uint16_t LogData[SizeBuf_ADC]; // ����� ��������������� ������ ����������, � ���� ��������� �� ������ ���

extern unsigned char NexData[NEXSIZE] ; // ��� ������� � Nextion
extern DWORD RawData[RAWSIZE] ;// �� ������������ ������
extern unsigned int PointsPerPeriod; // ��� �������������
extern unsigned int PointInPeriod; // ��� �������������
extern int CntAccumulat; // ������� ����������
// ���������� �������� �2, ���������� � �����
extern volatile DWORD OldTimeAccum; // ���������� ��������
extern volatile DWORD CurrTimeAccum; // ������� ��������
extern uint8_t EnaTimerAccum; // ������� ��������

extern volatile unsigned char rawPressure;
extern volatile unsigned char rawPressKeyS; // ������� �������������� ������� ������� S �� ����������
extern volatile unsigned char KeyS; 
//extern char screen[1024] ;
extern volatile BYTE RX_BufNEX[BUFSIZEUART2];
extern char VerFW_LCD[25]; //������ �� ���������� NEXION
extern char NameReadFile[32]; // ���������� ��� ����� ��� ������ � ����������

extern volatile DWORD CntRXNEX;
extern volatile uint32_t RSDecYes; // ����� � ������� ������ �� UART � ��c�� ���� ��� ������ �� USB
extern volatile BYTE Uart2DecYes;
extern volatile DWORD RecievNEX;
extern volatile BYTE ReadyNEX; // ���������� ���������� NEXTION
extern volatile BYTE g_WtRdyNEX;
extern volatile BYTE ProgFW_LCD; // ������� ���������������� ���������� Nexion
extern volatile DWORD UART2Count; // ������� ������ ���� UART2 �� ���������� NEX - ��������� � ������ ����������������
extern volatile DWORD UART0Count; // ������� ������ ���� UART0 ��� ���������� NEX - ��������� � ������ ����������������
extern volatile BYTE UART2Buffer[BUFSIZEUART2];
extern volatile WORD CountTimerUart2;
//extern unsigned short DataADC ;
extern DWORD TimeBegin; // ����� ������, ��� �������� ��������� ��������
extern uint8_t WIDE_VER;
extern BYTE CurrLang; // ������� ����
extern unsigned short KeyP; // ������� ������� 
//extern int CntAccumulat; // ������� ����������
// ������ ����������

// ������� ���������� ��� ������������ ���� ���������� (CW) �������
//extern unsigned long PIN_PHLD;

extern const char *CmdNextion[CMD_NUM]; 
extern const char *MsgMass[MSG_NUM][LANG_NUM]; 
//extern const char MsgMass[MSG_NUM][LANG_NUM][32]; 
extern const DWORD DelayPeriod[LENGTH_LINE_NUM];// �������� ������� � ����� CreatDelay()~ 83.33 ��
extern const DWORD NumPointsPeriod[LENGTH_LINE_NUM];
extern const unsigned char fonts[2048];
extern char BufString[225];
//extern char BufString[60];
//extern const char *NumVer; // ����� ������
extern char NumVer[10]; // ����� ������

extern unsigned int CheckErrMEM; 
extern BYTE CheckIDMEM; // ����� ����� �����? (0- 16 , 1- 32)

extern unsigned int TimerValueJDSU; // ������� �������� ������� ��������� RS
extern unsigned long TimerCR; // �������� ������� 1 (��� �������) ��� �������� ������� ����� 2-�� ���������� �� ����� 
// ��� ����������
extern volatile BYTE g_GetStr; // ������� �������� ��������� ������ �� ���������. 
extern volatile BYTE g_NeedScr; // ����������� ������ ! ���������! ���� ��������� 
extern volatile BYTE g_FirstScr; // ������ ���� � �����, ����� ������ ����������� (����������), ����� �������� ������ ������ ����, ��� NEXTION
extern volatile BYTE IndexSmall; // ������������� ��� 2 �� ,0 - 0.5 ��, 1 - 2�� 
extern volatile BYTE TypeLCD ; // ��� ���������� � �������������� v-3.2(=0) s-3.5(=1) 
extern volatile BYTE g_NeedChkAnsvNEX; // ������� ��������� ������ �� ���������.� �� ��������
extern volatile BYTE g_AutoSave; // ������� ������ ���� ���������� ��������� �� �������� ������ ����
extern volatile int NeedReturn; // ������������� ��������� � ���� ����������
extern BYTE g_SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index 
extern BYTE mg_SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index ��� � ������

extern BYTE MemTable[MaxMemOTDR+1]; // ������� ������������� ������ ������ �������� � ������ �� MaxMemOTDR
extern BYTE MemTableExt[MaxMemOTDRExt+1]; // ������� ������������� ����������� ������ ������ �������� � ������ �� MaxMemOTDRExt
extern uint8_t BusyUSB ; // ������� �������� ������ �� USB, � SD ������


#endif
