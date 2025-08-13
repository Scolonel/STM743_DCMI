#ifndef __DEVICEDATA_H__
#define __DEVICEDATA_H__

#define WAVE_LENGTHS_NUM 5
#define LENGTH_LINE_NUM 7
#define WIDTH_PULSE_NUM 9//8 �������� 5 ��
#define TIME_AVR_NUM 6
#define TIME_LIGHT_NUM 3
#define LSPLACENUM 3
#define CURRENT 0
#define NEXT 1
#define MODELSNUM 5
#define MODERENUM 3
// ���������� ������� � ��������� � ���
#define TAUFLTR 820
#define TAUWFLTR 70
#define TAUARX 5
#define TAUARN 10
#define SizePlus 6
// �� ����� events.c
#define BC_def 800 // ������� ��������� �������� � ����������� �� ����� ������ �� 80-60 ��
#define FPO_def 0
#define NF_def 65535
#define NFSF_def 1000
#define PO_def 0
#define LT_def 200  // 0.2dB ����� ����������� ������� �� ���������
#define RT_def 65000  // 65dB ����� ����������� ������� �� ��������� �� 0 �� 65 ��
#define ET_def 3000  // 3dB ����� ��������� ��� ����������� ����� �����



#define NUMSHIFTZONE 13 // ����� ����� ���������� �������� ��� ������ ���� � ���������

#define DBNAMESTRUCT 4088 // ����� �������� ���������� ��������� ��������������� ����� �������
#define JDSUSTRUCT 4089 // ����� �������� ���������� ��������� ��������� JDSU
#define TABLEMEMOTDR 4090 // ����� �������� ���������� ������� ����������� �������������
#define TABLEMEMOTDREXT 4095 // ����� �������� ���������� ����������� ������� ����������� �������������
#define CFG_DEVICE 4091
#define CFG_USER 4092
#define COEF_PM 4093
#define EVEN_SET 4094  // ����� ����� ��������� �������� ������� �������

// ��������� ��������� ������� ��������������  � (18.04.2013) ������������ ���������� - � �����. �����������
typedef struct
{
  unsigned short BC;     // BC = 800 ������ ��������� ���������
  unsigned FPO; // FPO �������� ���������� (������ ����� ������) ����� 0
  unsigned short NF ;    // NF ������ ������� ����� ����� 0
  unsigned short NFSF ;  // NFSF ���������� �����  ������� ����� ����� 1000
  unsigned short PO ;    // PO �������� �� �������� ����� 0 � 0.001 �� 
  unsigned short LT ;    // LT ����������� �������� ��������� ��� ������� 200 � 0.001 �� 
  unsigned short RT ;    // RT ����������� �������� ��������� ��� ������� 40000 � -0.001��
  unsigned short ET ;    // ET ����������� ��������� ��� ����� ����� 3000 � 0.001 ��
  //  ������� 
  //BYTE EnaAutoSetSC[LSPLACENUM]; // ������������ ����������� ������� � ���� ������
  //float K_pr_SC[LSPLACENUM]; // �����. ����������� ��� ��������� ���� ����
  
} TR_PARAM;  // ��������� ��������������


// ��������� ���������� �������������� ( �������� ���������)
extern TR_PARAM ReflParam;
// ��������� ���������� �������������� ( from Memory)
// ����� �� ����� ������
extern TR_PARAM MemReflParam;
// ��������� ���������� �������������� ( �� �������� ��������� ��� �������������)
extern TR_PARAM ResetReflParam;

// ��������� ��������� ������������� (14 byte)
typedef struct
{
  BYTE Index_Ln; // ������ �������� ����� ����� � ���������������� ������
  BYTE Index_Im; // ������ �������� ������������ �������� 
  BYTE Index_Vrm; // ������ �������� ������� ������ 
  BYTE Index_Comm;  // �������������� ������� � ������������ 
  float K_pr; // ����� ����������� 
  unsigned NumAvrag;
  BYTE SW_LW;  // ������� ������ ����� ������ (����������� ����� ����� ������)
  BYTE SubModeRef; // ������������ ���� ��� ��� ( ��� ������������� ����� ���������)
  unsigned short LogNoise;// ��������������� ��� (���������� ����� ��������� ��� ����������)
  BYTE SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index 
} REFL_SET;

// ��������� ������������� ���������� ��������
typedef struct
{
int ShZeroRng[4];// ��������� �������� ����������
float CoefStykRange[4]; // ������������ �������� ����������={1.0,1.0,1.0,1.0};
unsigned int PointKalib[WAVE_LENGTHS_NUM];// ����� ���� ���������� ���� 0 �� �� ���={850,1310,1490,1550,1625};
float CoefPointKlb[WAVE_LENGTHS_NUM]; // ������������ �������� ���� ���� ����������={1,1,1,1,1}
float CoefSpctrL[6]; // ������������ ������������ ����. 800-900={16,15,14,13,12,11}
float CoefSpctrM[10]; // ������������ ������������ ����. 1210-1390 (���� 9 � 1370)={1,1,1,1,0.95,1,0.95,0.90,0.85,1}
float CoefSpctrH[13]; // ������������ ������������ ����. 1410-1650={1,1,1,1,1,1,1,1,1,1,1,1,1}
} TAB_SET;

// ��������� ������������ �������
typedef struct
{
  WORD NumDevice;  // (999) � �������,
  WORD PlaceLS[LSPLACENUM]; //  ����� 1, (1310) ������������ ������������� ������� �� ������
  BYTE TypeDevice; // (5)  ������� � ���� ������� ������ ��� ����� (��������� �����)
  BYTE CfgPM;       //	������� ���������� (0-���,1- �������(-70..+6), 2- ����������� (-55..+20)
  BYTE CfgRE;       //	������� �������� ����� (0-���, 1-����)
  BYTE ApdiSet;     //	������� ��������� ����� (0-���, 1 ����)
  WORD InputCheckLvl;	//����� �������� ������ �� ����� (10- ������� ������) 
  WORD EndCheckLvl; // 	����� �������� ����� ����� (8500)
  BYTE ID_Device; // ������ 0-����� ��� 1-MOT_700
  float BatStep; // ��� ��� ���������
  // 18/04/2013 
} CFG_DEV;

//��������� ��������� JDSU
typedef struct
{
  unsigned int CompMode;  // ����� ������������� ( 0- �����, 1-JDSU)
  unsigned int LambdaNum[6]; //  ����� ���� ���������� �����������
  unsigned int FreqLambda[6]; //  ������� ��������������� ����� ���� ���������� �����������
  // 21/10/2014 
} JDSU_COM;

//��������� ��������������� ����� ������� (�.�. 26.04.2017)
typedef struct
{
  BYTE Ena_DB;  // ���������� ��������������� �����( 0- ���������, 1-���������)
  unsigned char AltName[16]; //  �������������� ���
  float ph_A[3]; // ����������� ����� ��� ����� ���� (��-�� "�������" ���������) 
  char UserComm[20];// ���������������� ����������� ������ �������� � 
  // ������� ��������������
  uint16_t FiberID; //������� �������? 
} DB_NAME_ENA;

typedef struct  // �������� ���������������� ��������� ������� ������ ������
{
  BYTE CurrLang;  // ������� ���� Rus, Eng, Cze
  BYTE SubModeRef; // �������� ������������� 0- ������� 1-����
  BYTE EnIncFiber; // ���������� ���� ���������� ������� ��� ���������� ���������
  WORD CellMem; // ����� ���������� ����� ����������
  float CurrLvldB; // ������� �������� ������ ��������
  float LevelRef[WAVE_LENGTHS_NUM]; // ������ �������� ���� ���� ����������
  DWORD BegShiftZone[NUMSHIFTZONE]; // ��������� �������� �������� ����� ��� ������ ���� �����
  BYTE TimeLight; // ����� ���������
  //
  //  ������� 19.04.2013
  BYTE EnaAutoSetSC[LSPLACENUM]; // ������������ ����������� ������� � ���� ������
  float K_pr_SC[LSPLACENUM]; // �����. ����������� ��� ��������� ���� ����
  
} CFG_USR;

typedef struct  // �������� ���������������� ��������� ������� ������ ������ GenParams
{
  char LC[2]; // Language Code EN-English RU- Russian 
  char CID[16];  // Cable ID  
  char FID[16];  // Cable ID  
  short int FT;  // Fiber Type (652)
  short int NW;  // Nominal WaveLenght (1310)
  char OL[16];   // Originating Location
  char TL[16];   // Terminating Location
  char CCD[16];  // Cable Code
  char CDF[2];    // Current Data Flag (CC,BC,RT or OT)
  int32_t UO;    // User Offset � ��� 0 (�������� � 100 �� �� ������
  int32_t UOD;    // User Offset Distance � ��� 0 (�������� � ������������ ��������� (km,m,ft...) �� ������
  char OP[16];   // Operator Name or Operator Code
  char CMT[20];  // Comment User Added information about trace 
} GPB_STR;

typedef struct  // �������� ������ ������� SupplierParams
{
  char SN[24]; // OTDR Manufacturer's Name 
  char MFID[16];  // OTDR Mainframe Model  
  char OTDR[16];  // Mainfraim Serial number 
  char OMID[20];   // Plug-In Model Number
  char OMSN[16];   // Plug-In Serial Number
  char SR[20];  // Vercion of software/firmware
  char OT[16];    // Other infomation (trace, hardware, software,firmware)
} SPB_STR;

typedef struct  // �������� ������������� ��������� ������� FixedParams
{
  uint32_t DTS; // Current Time/Date Seconds since 00:00 1 January 1970 
  char UD[2];  // unit Distance (km, mt, ft, kf, mi)  
  uint16_t AW;  // actual measured wavelength (nm) = NW from GeneralParams
  int32_t AO;   
  int32_t AOD;
  int16_t TPW; // ����� ��������� (���� �� ���� ������ �� ���������)
  int16_t PWU; // ������������ �������� � nS
  int32_t DS;  // Data Spacing 
  int32_t NPPW;  // ����� ����� ��� ������� �������� 
  int32_t GI;  // Group Index - ����� �����������, � ������� ������� 
  int16_t BC; //Backscatter Coefficient
  int32_t NAV; // Number of Averages
  uint16_t AT; // Averaging Time in seconds
  int32_t AR; // Acquisition Range (100 pS - ��������
  int32_t ARD; // Acquisition Range in units distance (UD)
  int32_t FPO; // Front Panel Offset ��������� ��������
  uint16_t NF; //Noise Floor Level (65535)
  int16_t NFSF; //Noise Floor Scale Factor 1000=1.000
  uint16_t PO; // PowerOffset First Point Add Attenuators 10000=10.0dB
  uint16_t LT; // Loss Threshold 200=0.2dB (200)
  uint16_t RT; // Reflectance Threshold 40000=40.0 dB(40000)
  uint16_t ET; // Enf-Of-Fibet Threshold 3000=3.0 dB (3000)
  char TT[2]; // Trace Type (ST, RT, DT, RT)
  int32_t WC[4]; // Window Coordinates
} FPB_STR;

typedef struct  // Block Data Points � ��� ��������� �������
{
  int32_t TNDP; //Number of Data Points
  int16_t TSF; // Total Number Scale Factor Used
  int32_t TPS; // Total Data points Using 
  int16_t SF; // Scale Factor
} DPNTS_STR;
// ��������� ������� DS
unsigned GetValueDS (void);


float GetSetBatStep (float Dir); // ���������� ��� ������������� �������� ���� ��� ���������

// ��������� ��������� ������� � ������ ���������� ������� �� ������ ��������� �����
void SetIndxSeqLS (void);

unsigned StructSize(unsigned data);
unsigned char * StructPtr(unsigned data);
void SetLang( BYTE lang );
BYTE GetLang(int Dir); // ��������� �������� �����/ ��� ������������
BYTE GetID_Dev(void); // ��������� �������������� ��� ���� ������
BYTE SetID_Dev(BYTE Data); // ��������� �������������� ��� ���� ������
WORD GetNumDevice(void); // ��������� ������  �������
WORD SetNumDevice(WORD Num); // ��������� ������  �������
// Check param DBNAME
unsigned InvalidDBNAME(); //boolean actually
void InitDBNAME(unsigned Err);
// Check param JDSU
unsigned InvalidJDSU(); //boolean actually
void InitJDSU(unsigned Err);

unsigned InvalidDevice(); //boolean actually
void InitDevice(unsigned Err);
void GetDeviceLabel( char* label, int Mode ); // Label on Main device page
void GetDeviceName( char* name ); // from Computer
// ��������� �������������� "������" , ��� � ��� ����������������
void GetDeviceHW( char* name ); // from Computer

void GetTesterName( char* name ); // Tester
void GetMFID( char* name ); // for Belcore
int32_t Get_AR (void);

//void GetHeaderBelcore (char* Name);  ��������� ����� �������
void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents); // ��������� ����� �������
BYTE GetEnIncFiber (BYTE Dir);// ��������� - ������ ��������� �������� ���������� ���� ���������� �������� �������
WORD GetCellMem (BYTE Dir);// ��������� ��������� �������� �������� �������� ���������� ���������

DWORD FindErrCoeff (void); // �������� ������� �������������
void FixErrCoeff (DWORD Err); // ����������� ������� �������������
// �������� ������� ��������� ����������
DWORD CheckErrCoeff (void);

DWORD CheckUserGonfig (void);  // �������� ���������������� �������� 
BYTE SetTypeDevice (BYTE Data); // ��������� ���� ������� ��� �������
BYTE GetPlaceLS (BYTE Dir); // ��������� ������� ����� ����� ����� �� ������ ������������� ������� CURRENT -�������, NEXT- ����� (���������)
WORD GetLengthWaveLS (BYTE SetPlace); // ��������� ����� ����� �� ������� ������������� �����
void SetPlaceLS (BYTE SetPlace); // ��������� ���������� ������ 
WORD SetupWavePlaceLS (BYTE Place, WORD Data); // Setup num wave for place laser
BYTE SetSeqCombLW (void); // ������������� ������ ���������� ��������� �������
BYTE GetSubModRefl (void); // ��������� ������ ������ �������������
void SetSubModRefl (BYTE Mode); // ��������� ������ �������������
BYTE GetIndexLN (void); // ��������� ������� ����� �����
BYTE SetIndexLN (BYTE Index); // ��������� ������� ����� �����
WORD GetLengthLine (BYTE Index); // ��������� ����� ����� �� �������
BYTE GetIndexIM (void); // ��������� ������� ����� ��������
void SetIndexIM (BYTE Index); // ��������� ������� ����� ��������
WORD GetTimeAvrg (BYTE Index); // ��������� ������� ���������� �� �������
BYTE GetIndexVRM (void); // ��������� ������� ������� ����������
void SetIndexVRM (BYTE Index); // ��������� ������� ������� ����������
WORD GetWidthPulse (BYTE Index); // ��������� ����� �������� �� �������
float GetIndexWAV (void); // ��������� ������� �����������
void SetIndexWAV (float Data); // ��������� ������� �����������
float GetWAV_SC(BYTE Index);// ��������� ������� ����������� � ����������� �� ����� ������
float SetWAV_SC(float Data, BYTE Index);// ������ ����������� ����� � ���������
void ReSaveWAV_SC (void);// ���������� �������� ����� � ������������� ����� (��� ����������� ����������)

// ���������� �������  ����� �������� �� ��� ��������
BYTE CalkIndexIM (uint16_t IM); // ��������� ������� ����� Pulse
// ���������� �������  ����� ����� ��  � �������� (� ��� � ������)
BYTE CalkIndexLN (long int LN); // ��������� ������� ����� ����� (���������)
// ���������� ������� ����������� ����� �� ����� �����, ��  � �������� (nm)
BYTE CalkIndexSC (long int LW); // ��������� ������� ����������� �����
// ���������� ������� ������� ���������� �� �� �������� (��������� �� ������ 4)
BYTE CalkIndexWRM (uint16_t WRM); // ��������� ������� ������� ����������

void InitMemReflSet (void); // ������������� ��������� ������������� ���������� �� �����

void InitReflSet (void); // ������������� ��������� �������������
BYTE GetCfgPM (void); // ��������� ��������� ����������
BYTE SetCfgPM (BYTE Data); // Setup ��������� ����������
BYTE GetCfgRE (void);// ��������� �������� ��������� �������� �����
BYTE SetCfgRE (BYTE Data);// Setup �������� ��������� �������� �����
BYTE SetModeLS (char* Str, int Dir, BYTE Lang); // ���������� ����� ������ ���������, � ����� ������ ��� ����������� �� ����������
BYTE SetModeRE (char* Str, int Dir, BYTE Lang); // ���������� ����� ������ VFL, � ����� ������ ��� ����������� �� ����������
void CntrlRE (void); // ������� �������� ������� ������
BYTE OffModeRE(void); // ��������� ������� �����

DWORD GetCurrentBegShiftZone (void); //��������� �������� �������� �� �������
void SetIndexShadowIM (BYTE Index); // ��������� ������� ���� ��� �������� ��������

BYTE GetIndexCurLengthWave (WORD LengthWave);// ��������� ������� ����� ����� � ����������� �� �������� ����� �����
float GetLSACoef (void); // ��������� �����. ������� ����� �� ������������� ����������


unsigned GetTimeLight (BYTE Index); // ��������� ������������ ��������� �� �������
BYTE SetIndexLight (int Dir); // ���������(���������) ������� ������������ ��������� 0-�������, 1-����������, -1-����������
DWORD SetBegShiftZone (BYTE Index, DWORD Data);// ������ ������� ���� �� �������
DWORD GetBegShiftZone (BYTE Index);// ������ ������� ���� �� �������
DWORD GetEndCheckLvl (void); // ������� ������ ��� �������� ����� �����

BYTE SetIndexCommOTDR (BYTE Index); // ��������� ������� ����������� � ��������� ��� ����������
void CheckCommOTDR (void); // �������� � ������������� ������ ������������ OTDR
BYTE CheckSavedTrace(void); // ������� �������� ����������� �������������� 
uint32_t CheckBelcore(void); // ������� �������� ����������� �������������� 

BYTE GetApdiSet (void); //��������� �������� ������� ��������� ���������
BYTE SetupApdiSet (BYTE Data); //Setup �������� ������� ��������� ���������
void SetNumAverag (unsigned Data); // ������ ����� ���������� � ������ �������������
unsigned GetNumAverag (void); // ��������� ����� ���������� ��������������
void DeletedAllCell (void); // �������� ���� ������� ����������
// ���������� ����� ����� � �������� �� ���������� ��������� (1-real 0-wided)
unsigned short PointsInImpulse (int Real);
// ���������� �������� ��� ��� ������������� ������������ ��������
float TauImp (void);
// ������ ����� ����� �������� ��� ��������� �������
unsigned short GetPointsShift (void);
// ��������� ������ ����� ��� ���� ���������
unsigned short GetNoiseLvl (void);
// ������� ��������� ���������� ������� �������������
unsigned short GetSet_BC (float Dir); // ���������� ��������� ���������
unsigned short GetSet_LT (float Dir); // ��������� ������� �������� �������
unsigned short GetSet_ET (float Dir); // ����� ����� �� ����� 10 ��
unsigned short GetSet_RT (float Dir); // ��������� �� ����� -55��
//���� �.�.
int GetIndxPON (int Lambda);

float SetCurrLvldB(int Lambda, float LvldBm);  // ������������� ������� LvldBm � �������� ��������
float GetCurrLvldB(int Lambda);         // ���������� ������� ������� ��������
                                        // ��� ��������  ������� ������� �������� � ����������� �� ����� �����
BYTE PreSetModeLS (BYTE Index); // �������������� ��������� ������ ���������
void  SetupSource (BYTE ModeLS); // ��������� ������ ������ ������
BYTE GetModeLS (void); // �������� ����� ���������
// ����������� ��������� ��� ��������� ���������
void ContinueModulation(void);

void GetNumVer (char* Str); // ��������� ������ ������
void SetNewLeveldB (float RPON, int i); // ��������� ������ ������ �������� �� �������� ������
void SetLogNoise (unsigned short Data);// ������� ���������������� ����, ���������
void SaveLogNoise (void); // ��������� ������� ���������������� ���� ����� ���������
unsigned short CalkCheckSum (void);// ������� ����������� ����� ������������ �������
// ��������� ���������� ������� ������ �� �������
BYTE GetCombLS (BYTE Index);
// ������� ������������ ������� �� ����� � ���������� ���������� � ����
void Run_SCWP(void);

// ��������� ��������� �������� ��������������� ������ ����� ����� ���������
BYTE GetSetModeLW (int a);
// ��������� ��������� ������� ������� �����???
BYTE GetSetNumLS (BYTE a);


// ��������/��������� �������� �������� ������ �������������, ����� � 200 
unsigned long GetSetCntFiles (BYTE Dir);
void SetCurCntFiles (void);

extern char MsgErrMem[20]; // ��������� �� ������ ������ �����

// ��������� ��������� ������������� ( �������� ���������)
extern REFL_SET SettingRefl;
// ��������� ��������� ������������� ( �� ������)
extern REFL_SET MemSetRefl;
// ��������� ��������� ������������� ( �� ��������� ��� �������������)
extern REFL_SET ReSetRefl;
// ��������� ������������� ���������� ��������
extern TAB_SET CoeffPM;

extern unsigned long CurCountFiles; // ���������� ����� �������� ����� (������ �� ������)

extern DB_NAME_ENA NameDB;
extern JDSU_COM SetJDSU;

extern CFG_DEV ConfigDevice;
extern CFG_USR UserSet;
extern GPB_STR GenParams;
extern SPB_STR SupParams;
extern FPB_STR FixParams;
extern DPNTS_STR HeadDataPoints;

extern const char *IdnsBC[2];

#endif  /* __DEVICEDATA_H__ */



