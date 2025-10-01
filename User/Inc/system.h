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
#define MaxMemPM 512
#define MaxMemOTDR 200
#define MaxMemOTDRExt 250
#define MAXMEMALL ((CheckIDMEM)?(MaxMemOTDR+MaxMemOTDRExt):(MaxMemOTDR))
//����� ��������� � ������
#define LANG_NUM 6  // ����� �������� � ������� ������

#define MSG_NUM 131
#define CMD_NUM 35 //������� Nextion

#define TIMERE 500 //����� ����� �������� ����� �� 500 ��

#define LIGHTSPEED 299792458
// ��� ����� 
  #define YearSecV  31622400
  #define YearSec   31536000
  #define DaySec    86400
  #define Y2000Sec  946684800 // ����� ������ 2000 ����

// ������� �������� 
#define POINTSIZE    5300 //4096 // ����� �������� �����, ������ ����� ����� �������

#define NEXSIZE    5300 //4096 //0x1000  //������ ��� ��������
#define OUTSIZE    5300 //4096 //0x1000  // �������� ������
//#define RAWSIZE    0x1200 //4384 //0x1200  //(4096+288)
#define RAWSIZE    5600 //4384 //0x1200  //(4096+288)
#define FLTSIZE    5 // ����������� �������
// ������� �� ��������� ��������...������������� � ��������� ������������ �����
#define SizeBuf_ADC_int 8
#define SizeBuf_ADC 5700//8192
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

#define BEG_ADD  64 // ������� ��� ����������� ������ ���� �������� ��� ������ ����������

//
#define LENGTH_LINE_NUM 7 // ����� ���� ����� (����������)

#define PNTSNUM 7 // ����� ����� �������� ����� �����

#define BELCORESIZE 224
#define ENAOLDLCD 0 //���������� ���������� �� ������ LCD 1-���������, 0-���������
// from SorData
#define MAX_PATH_LEN 256
#define ArraySize(a)    (sizeof(a)/sizeof(a[0]))
//  ����� ���� ���������� �������
//  ==============================
#define OTDR_OKAY               0   // �������� ����������
#define OTDR_SUCCESS            OTDR_OKAY
//  ���� �������������� ����� ������������� ��������
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OTDR_WARNING            1   // ������������ ��������������
#define OTDR_PARAM_CORRECTED    2   // �������� ��������� ��� ����c������
                                    // ��������� � ���� ����������������
#define OTDR_RESULT_CORRECTED   3   // ��������� ��� ������� ����c�����
                                    // �������� ��� ���������������
#define OTDR_NOTHING_TO_DO      4   // ������ ������
//  ���� ������ ����� ������������� ��������
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define OTDR_ERROR              -1  // ������������ ������
#define OTDR_NULL_PTR           -2  // ������� ��������� == NULL
#define OTDR_MEMORY_ERROR       -3  // ������ ��� ��������� ������
#define OTDR_OUT_OF_MEMORY      -4  // �������� ������ (������, ����������
                                    // � ������� ���-��, ������������)
#define OTDR_FUNCTION_BUSY      -5  // ��������������� ����� �������

#define OTDR_FILE_ERROR         -10 // ������������ �������� ������
#define OTDR_FILE_OPEN_ERROR    -11 // ������������ ������ ��� ������� �����
#define OTDR_PATH_NOT_EXIST     -12 // ���� (�����/��������) �� ����������
#define OTDR_FILE_NAME_ERROR    -13 // ������������ ��� �����
#define OTDR_FILE_NOT_FOUND     -14 // ���� �� ������
#define OTDR_FILE_ACCESS_DENIED -15 // ���������� ������ � �����
#define OTDR_FILE_TYPE_UNKNOWN  -16 // ����������� ��� ����� ������
#define OTDR_FILE_SYNTAX_ERROR  -17 // �������������� ������ � �����
#define OTDR_FILE_STRUCT_ERROR  -18 // ������ � ��������� �����
#define OTDR_FILE_READ_ERROR    -19 // ������������ ������ ������
#define OTDR_FILE_WRITE_ERROR   -20 // ������������ ������ ������
#define OTDR_FILE_BAD_CHECKSUM  -21 // �������� ����������� �����

#define BUFFER_SIZE	256


#include "integer.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "math.h"
#include "pm.h" // ������� ����������
#include "rtc.h" // 
#include "tim.h" // 
//#include "fatfs.h"
#include "pca955x.h"
#include "buttons.h"   
#include "sdmmc.h"
//#include "ff.h"
// �� FATFS ����������
//#include "fatfs.h"
//#include "ff_gen_drv.h"
//#include "ff.h"
//#include "ffconf.h"

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
#include "UartOpt.h"  //from T7kAR
#include "drawfunctions.h" //from T7kAR
#include "memflash.h"// //from T7kAR

//const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};
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

// from SorData
int   WriteSorFile(const char* pszFileName, int iPlaceLS, unsigned short* pDataPoints);
int ReadSorFileG(const char* pszFileName);


      // ���� ������ ���������� ����� �� �7�_��
uint32_t GetSysTick( int Mode); // ��������� ����� 1 ��. 0 - ��������� �������� �� ����������� ������ 1- �����

void NEX_Transmit(uint8_t *Str);
// ���������� �������� � ���������� ��������
WORD TimerPA (BYTE Set);

extern uint8_t ModeSCWP; // ����� ������� ��� ������������ �������
extern uint16_t TimeSCWP; // ������� ������� ��� ������������ �������
extern DWORD CountTimerPA; // ������� ������ ��� ������ ������� PM 


extern char NameDir[100][6];
extern char NameDirD[32][6];
extern char NameFiles[1200][17];
extern uint32_t NumNameDir; // ����� ���� ����������
extern uint32_t IndexNameDir;// ������ ����������� �� ������� ���������
extern uint32_t IndexLCDNameDir;// ������ ��������� �� ���������� ����������� �� ������� ���������
extern uint32_t NumNameDirD; // ����� ���� ����������
extern uint32_t IndexNameDirD;// ������ ����������� �� ������� ���������
extern uint32_t IndexLCDNameDirD;// ������ ��������� �� ���������� ����������� �� ������� ���������
extern uint32_t NumNameFiles; // ����� ���� ������
extern uint32_t IndexNameFiles;// ������ ����� �� ������� ���������
extern uint32_t IndexLCDNameFiles;// ������ ��������� �� ���������� ����� �� ������� ���������
extern uint32_t PageDir; 
extern uint32_t PageDirD; 
extern uint32_t PageFiles; 

extern BYTE KnowLCD ;
extern uint8_t g_ErrFW_LCD; // �� ���������� �������� ����������
extern uint8_t TimerDraw; // ����� ���������� ������ , ������ �������...

extern uint8_t g_SpeedUart; // 0 - LO(57600), 1-ME(115200), 2-HI(460800)

extern char* g_pszFileNames[LSPLACENUM];  // ������ ���������� �� ������ � ������� ������

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
extern struct tag_PON R_PONI;
// ������� �� ����� *.sor ����� ��� ��������� � ���������, ������� ����� ��� ��� ������������� ����.
#pragma pack(push,1)
typedef struct
{
  char LC[2]; // [82-83] ���� 2 ����� (�����) 
  char CID; // [84] CID Cable ID 
  char FID; // [85] FID Fiber ID
  uint16_t NW; // [86..87] ����� ����� (2 �����) 
  char OL;     // [88] OL Originating Location
  char TL;    // [89] TL Terminating Location
  char CCD;    // [90] CCD Cable Code
  char CDF[2]; // [91] Current Data Flag (BC, RC, OT) 2 ����� (�����) 
  uint32_t UO; // [93..96] DTS ������� ����� (4 �����)
  char OP;  // [97] OP Name or Operator Code 
  char CMT[20]; // [98..117] CMT ����������� �� ������ '�race '  (����� ������ 20 ����) 
  char SN[15]; // [118..132] SN - ��� ��������  (����� ������ 15 ����) 
  char MFID[13]; // [133..145] MFID - ������ ������� TOPAZ-710XR (����� ������ 13 ����) 
  char OTDR[5]; // [146..150] �������� ����� ������� s/n 000  //4-x �������!!! (����� ������ 5 ����) 
  char OMID; // [151] OMID
  char OMSN;  // [152] OMSN
  char SR[5]; // [153..157] SR ������ �� v4.0a (����� ������ 5 ����) 
  char OTHER;  // [158] Other
  uint32_t DTS; // [159..162] DTS ������� ����� (4 �����)
  char UD[2]; // [163..164] UD ������� ��������� km  2 ����� (�����)
  uint16_t AW;//[165..166] AW ����� ����� � 0.1 �� (1310�� = 13100) 0x332c 0x3c8c (2 �����)
  uint32_t AO;   // [167..170] AO �������� ����� ������
  uint16_t TPW;   // [171..172] TPW ���������� ������ ��������� (����)
  uint16_t PWU;//[173..174] PWU ������ ������ �������� (ns) 150ns=0x96, 500ns=0x1f4,  (2 �����)
  uint32_t DS; //[175..178] DS data Spacing ����� ����� ������� ��������� � 100��  (4 �����)
                              // DS = (GI*L)/(C*10e-14) L - ���������� ���������� ����� ������� ��������� �- �������� �����
                              // GI - �����. �����������, �� ������� ������������ ����� ��������� (� ��/2)*100000 = DS
  uint32_t NPPW; // [179..182] NPPW ���������� ����� (5200)//stm32h743  (4 �����)
  uint32_t GI; // [183..186] GI ����� �����������  146583 (1.46583) (4 �����)
  uint16_t BC;//[187..188](BC = 800 ������ ��������� ��������� (2 �����)
  uint32_t NAV; // [189..92] NAV ����� ���������� - ������� �� ������� ���������� � ����� ����� = sycl (4 �����)
  uint32_t AR; // [193..196] AR  ����� ����������� ������� (����� ����� ��������� �� ���) DS*NPPW/10000 (4 �����)
  uint32_t FPO;   // [197..200] FPO �������� ���������� (������ ����� ������) ����� 0
  uint16_t NF;  // [201..202] NF ������ ������� ����� ����� 0

  uint16_t NFSF; // [203..204] NFSF ���������� �����  ������� ����� ����� 1000 (2 �����)
  uint16_t PO; // [205..206] PO �������� �� �������� ����� 0 � 0.001 ��
  uint16_t LT; // [207..208] LT ����������� �������� ��������� ��� ������� 200 � 0.001 �� (2 �����)
  uint16_t RT; // [209..210] RT ����������� �������� ��������� ��� ������� 40000 � -0.001�� (2 �����)
  uint16_t ET; // [211..212] ET ����������� ��������� ��� ����� ����� 3000 � 0.001 �� (2 �����)
  uint32_t TNDP; // [213..216] TNDP ���������� ����� stm32h743 = 5200 DS*NPPW/10000 (4 �����)
  uint16_t TSF;  // [217..218] TSF ����� ����
  uint32_t TNS1; // [219..222] TNS1 ���������� ����� �� ����� (5200)(4 �����)
  uint16_t SF1; // [223..224] SF1 ������� �����
  
} St_File_Sor;
#pragma pack(pop)


extern St_File_Sor F_SOR; // ���������� �������� ���������� ����� SOR


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
    void StartRunFirst(unsigned AddIndx); //
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
extern uint8_t MeasureNow; // ������� ������ ����������, ��� ������, ��������� �����
extern uint32_t Sm_Shift ; // ������� �������� ������ ����.��������

//void GetPointInPeriod
DWORD CalkZondImpuls (void); // ������ ������������ ��������

//const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// ����� ����� �� ������
void Averaging (int NumAccum,unsigned AddIndexLN, BYTE EnDrawGraph );// ������� ���������� � ���������� �������� ����� ����������
void DrawPictureMeas (BYTE EnDraw); // ��������� �������� ��� ���������
void FiltrWOW (DWORD* array, unsigned Avrgs );
// check Speed Uart2
int CheckSpeedUart2 (void);
// ������� �����
//extern unsigned short PosEndEvenBlkELMP; // ������� ������������ ����� ����� � �������� (���� �������������� ��� ��������� ������
// ���������� UART ����������
extern UART_HandleTypeDef huart5;
extern uint8_t NeedFreq; // ���������� ������������� ��������� ����� ���
extern uint8_t ModeLS; // ����� ���������
extern uint8_t TxOptBusy; // ������� ��������� UatrOptic

// extern void (*ModeFunc)(void);
//extern unsigned short LogData[OUTSIZE] ; // ��������������� ������
extern uint16_t LogData[SizeBuf_ADC]; // ����� ��������������� ������ ����������, � ���� ��������� �� ������ ���
extern uint16_t LogDataSplice[SizeBuf_ADC]; // ����� ��������������� ������ ����������, � ���� ��������� �� ������ ���

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
extern uint8_t RX_BufNEX[BUFSIZEUART2];
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
extern const DWORD MultIndex[LENGTH_LINE_NUM];//��������� ���������� ������� �����

extern const int TimeRepitOfLN[PNTSNUM]; // ������� �������� ���������� ��� ������ ���������� � ��� ��� ���1
extern const int KeyPoints[PNTSNUM]; // ����� ����������� ������� ������������� ����� 4096
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
//extern BYTE g_SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index 

extern BYTE MemTable[MaxMemOTDR+1]; // ������� ������������� ������ ������ �������� � ������ �� MaxMemOTDR
extern BYTE MemTableExt[MaxMemOTDRExt+1]; // ������� ������������� ����������� ������ ������ �������� � ������ �� MaxMemOTDRExt
extern uint8_t BusyUSB ; // ������� �������� ������ �� USB, � SD ������
extern uint16_t PresentUSB ; // ������� ������������� USB
extern uint8_t ModeUSB ; // ������� ������������� USB
 // ����� ������� �������� ���������� ��� �������������
extern char NumCurrPage ; // 

//extern   FIL Fil;
//extern   FRESULT FR_Status;
//extern   FRESULT res;
//extern  DIR dir;

#endif
