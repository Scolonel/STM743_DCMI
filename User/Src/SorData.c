// ������ � ������� � "Bellcore" OTDR Data Format
#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "system.h"
//#include "reflect.h"
//#include "translates.h"
//#include "Graph.hpp"
//// disable Warning[Pa039]: use of address of unagligned structure member
//#pragma diag_suppress = Pa039
//#include "SorEvents.hpp"
//
//extern CLossGraph lossGraph;      // ��������� ������� ������

char* g_pszFileNames[LSPLACENUM]; // ������ ���������� �� ������ � ������� ������

uint8_t tempBuffer[256];

#define DEF_REVISION        200
#define COM_SERV_REVISION   100

// ������������ �������
//#define USE_CORDS

//static int  g_NumVer =
//#include "lasti.h"
//;

#pragma pack(push, 1)   // ��������� ������������ ����� �������� ������

// � �������������/�������������� ������ �����
// ������ ���� ����������������� ���� �� ���� ������ �����������, �������� ������� ��������� ����������
// !!! ���� �������� ����������� �� ������������ !!!
#define OTDR_DIST_IN_M      // ���� ��� ���������� ������ [�]!
//#define OTDR_DIST_IN_KM

// � ��� ����������� �� ����� ���������
//#define OTDR_DIST_IN_FT
//#define OTDR_DIST_IN_KF
//#define OTDR_DIST_IN_MI

//enum EDIST_UNITS
//{
//  eDIST_IN_KM = 0,
//  eDIST_IN_M,
//  eDIST_IN_FT,
//  eDIST_IN_KF,
//  eDIST_IN_MI
//};

char g_szBuffer[256];

struct SBlockInfo
{
  uint16_t revision;
  int32_t size;
};

struct SBlockMap
{
  struct SBlockInfo  info;
  uint16_t         cBlocks;        // ���������� ������
};

struct SGeneralParameters
{
  char    achLanguageCode[2];         // LC: EN, RU, CH
  char*   pszCableId;                 // CID
  char*   pszFiberId;                 // FID
  int16_t     sFiberType;                 // FT (2.0) NumValueC<int16_t,int16_t,651,699> 651, 652, 653, 655
  int16_t     sNominalWavelength_nm;      // NW (read only) NumValue<float,int16_t>
  char*   pszOriginatingLocation;     // OL
  char*   pszTerminatingLocation;     // TL
  char*   pszCableCode;               // CCD
  char    achCurrentDataFlag[2];      // CDF
  int32_t     lUserOffset_100ps;          // UO   NumValue<double,int32_t,10,FC_CanBeZero>
  uint32_t     ulUserOffsetDistance;       // UOD (2.0) NumValue<double,uint32_t,10,FC_CanBeZero>
  char*   pszOperator;                // OP
  char*   pszComment;                 // CMT
};

struct SSupplierParameters
{
  char*   pszSupplierName;            // SN, max 32
  char*   pszOTDRMainframeID;         // MFID
  char*   pszOTDRMainframeSN;         // OTDR
  char*   pszOpticalModuleID;         // OMID
  char*   pszOpticalModuleSN;         // OMSN, max 32
  char*   pszSoftwareRevision;        // SR
  char*   pszOther;                   // OT
};

struct SFixedParameters
{
  uint32_t     ulDateTimeStamp;                // DTS  UTC time
  char    achUnitsOfDistance[2];          // UD
  int16_t     sActualWavelength_01nm;         // AW   <float,int16_t,10>
  int32_t     lAcquisitionOffset_100ps;       // AO   <double,int32_t,10> �� ��������� �����
  uint32_t     ulAcquisitionOffsetDistance;    // AOD  <double,uint32_t,10>
  int16_t     sTotalNumberOfPulseWidthUsed;   // TPW
  // ��������� ��������� �.�. ��������� �������� ��� ������� �� TPW
  // --->
  int16_t     sPulseWidthUsed_ns;             // PWU  <float,int16_t>
  int32_t     lDataSpacing_100ps;             // DS   <double,int32_t,100000>
  int32_t     lNumberOfDataPoints;            // NPPW
  // <---
  int32_t     lGroupIndex;                    // GI 1.2...1.6 <double,int32_t,120000,160000,100000>
  int16_t     sBackscatterCoefficient_dB;     // BC   <float,int16_t,-40*-10,-90*-10,-10>
  int32_t     lNumberOfAverages;              // NAV  <size_t,int32_t,1>
  uint16_t     usAveragingTime_01sec;          // AT   <size_t,uint16_t,10>
  int32_t     lAcquisitionRange_100ps;        // AR   <double,int32_t,10>
  int32_t     lAcquisitionRangeDistance;      // ARD  <double,int32_t,1000>
  int32_t     lFrontPanelOffset_100ps;        // FPO  <double,int32_t,10>
  uint16_t     usNoiseFloorLevel_dB;           // NF   <double,uint16_t,-1000>
  int16_t     sNoiseFloorScaleFactor;         // NFSF <float, int16_t, 1000>
  uint16_t     usPowerOffsetFirstPoint_dB;     // PO   <float,uint16_t, 1000>
  uint16_t     usLossThreshold_dB;             // LT   <float,uint16_t, 1000>
  uint16_t     usReflectanceThreshold_dB;      // RT   <float,uint16_t, -1000>
  uint16_t     usEndOfFiberThreshold_dB;       // ET   <float,uint16_t, 1000, FC_CanBeZero>
  char    achTraceType[2];                // TT   (two-charscter string: ST, RT, DT or RF)
  // ������ �� ���� ��������� ���������������� ��� ���������� ������������ ���� "�����" (��) � "��������" (��)
  int32_t     lWinUpperLeftX;
  int32_t     lWinUpperLeftY;
  int32_t     lWinLowerRightX;
  int32_t     lWinLowerRightY;
};

//#if 0
//// ���� ���������� ���� (���������) � ��������� ���������� �������������� �� �����
//
//struct SLandmark    // ��������� ���������
//{
//  int16_t     sLandmarkNum;                   // LMN <int32_t, int16_t, 1>
//  char    achLandmarkCode[2];             // LMC
//  int32_t     lLandmarkDist;                  // LML <int32_t, int32_t, 1>
//  int16_t     sRelatEventNum;                 // REN <int32_t, int16_t, 1, FC_CanBeZero>
//  int32_t     lLongGPS;                       // GPA: longitude <int32_t, int32_t, 1, FC_CanBeZero>
//  int32_t     lLatGPS;                        // GPA: latitude  <int32_t, int32_t, 1, FC_CanBeZero>
//  int16_t     sFiberCorrFactor;               // FCI <float, int16_t, 100, FC_CanBeZero>
//  int32_t     lSheathMarkerEntering;          // SMI <int32_t, int32_t, 1, FC_CanBeZero>
//  int32_t     lSheathMarkerLeaving;           // SML <int32_t, int32_t, 1, FC_CanBeZero>
//  char    achSheathMarkerLeavingUnitsCode[2];// USML: ft or mt
//  int16_t     modeFieldDiamLeaving;           // MFDL <int32_t, int16_t, 1, FC_CanBeZero>
//  char*   Comment;                        // CMT
//};
//
//typedef deque<SLandmarkParameters> tLandmarkParameters;
//
//struct SLinkParameters
//{
//  int16_t                 sNumLandmarks;       // TNL <int32_t, int16_t, 1>
//  tLandmarkParameters landmarks;
//};
//#endif

struct SDataPts
{
  int32_t     lNumPoints;                 // TNDP <int32_t, int32_t, 1>
  int16_t     sNumScaleFactors;           // TSF  <int32_t, int16_t, 1>
  int32_t     lNumPointsUsingScaleFactor; // TPS1 <int32_t, int32_t, 1>
  int16_t     sScaleFactor;               // SF1  <float, int16_t, 1000>
  uint16_t*    pDataPoints;                // DSF11...DSF1z
  // ...
  //int32_t   lNumPointsUsingScaleFactor; // TPSn <int32_t, int32_t, 1,>
  //int16_t   sScaleFactor;               // SFn  <float, int16_t, 1000>
  //vector <uint16_t> dataPonts;           // DSFn1...DSFnz
};

//// ���������������� ���� ������ 01
//struct SComServData01
//{
//  int32_t   lDistToTheLeftMarker_100ps; // ���������� �� ������ �������������� �� ������ �������, 100 ps
//  int32_t   lDistToTheRightMarker_100ps;// ���������� �� ������ �������������� �� ������� �������, 100 ps
//  uint16_t   usApproxMethod;             // ����� ������������� ������ ����� ��������� (0-2P, 1-LS)
//  uint16_t   usLossBetweenMarkres;       // ������ �� ������� ����� ���������, 0.001*��
//  uint16_t   usMeanSlopeBetweenMarkers;  // ������� ������ �� ������� ����� ���������, 0.001*�� / ��
//  int32_t   lMaxReflBetweenMarkers;     // ������������ ���������� ��������� �� ������� ����� ���������, 0.001*��
//  uint16_t   usORL_BetweenMarkers;       // ORL �� ������� ����� ���������, 0.001*��
//  uint16_t   usDisplayFlag;              // ����, �����������, ����� �������� ���������� � ������ � �� ������ (0-MaxReflection, 1-ORL)
//};
//
//// ���������������� ���� ������ 02
//struct SComServData02
//{
//  int32_t   lPrimaryLenOfLink_cm;       // ����� �����, ����������� ��� ������ �������, ��
//
//  int32_t   lLaunchCordLen_cm;          // ����� ������������� ������� � ������ �����, ��
//  int16_t   sAddBeginConnectionLoss;    // ��������� ������ ������� � ������ � �������� ������ ������
//
//  int32_t   lTailCordLen_cm;            // ����� ������������� ������� � ����� �����, ��
//  int16_t   sAddEndConnectionLoss;      // ��������� ������ ������� � ����� � �������� ������ ������
//
//  // ����� �����������/�����������:
//  // 1) �������, ��������� � ������������ �������
//  // SKeyEvent     launchEvent;
//  // SKeyEvent     tailEvent;
//  // 2) ������ �� �����, ����������� ������� �������
//  // SLossesOfLink lossOfLink;         // Note: ������������ g_lineWithCords[iPlaceLS]
//};
char szTest[100];

#pragma pack(pop)

static FRESULT f_readCks(FIL* pFile, void* pvBuffer, UINT cBytesToR);
static FRESULT f_readDummyCks(FIL* pFile, UINT cBytesToR);

// ������� ������� ���������, ���
// ������ ������ ������ ����������� ���������� � ������� ���������� (��� ������?)
// ��������� �� ��������������� ������������ �������
//static uint32_t g_StampTimeSave[LSPLACENUM] = { 1420092001, 1420092002, 1420092003, 1420092004 };

// ������������ ������
const char szBlockMap[]        = "Map";
const char szBlockGenParams[]  = "GenParams";
const char szBlockSupParams[]  = "SupParams";
const char szBlockFixParams[]  = "FxdParams";
const char szBlockKeyEvents[]  = "KeyEvents";
const char szBlockLinkParams[] = "LnkParams";   // ���� �� ����������
const char szBlockDataPts[]    = "DataPts";
//const char szBlockSpecPropr[]  = "UserName";    // default, �� �.�. � ������
const char szBlockCksum[]      = "Cksum";
//const char szBlockComServ01[]  = "NPKSS01";     // RPCCS01 ���������� � ������� �������, � ��������� �������� � �.�.
//const char szBlockComServ02[]  = "NPKSS02";     // RPCCS02 ������ �������-������ ������� � ������ �� ����� � �� ������

static struct SBlockMap   map;
static struct SBlockInfo  genParamsInfo;
static struct SBlockInfo  supParamsInfo;
static struct SBlockInfo  fixParamsInfo;
static struct SBlockInfo  keyEventsInfo;
static struct SBlockInfo  linkParamsInfo; // �� ������������
static struct SBlockInfo  dataPtsInfo;
//static SBlockInfo  specProprInfo;  // �� ������������
static struct SBlockInfo  userDefInfo;
static struct SBlockInfo  chksumInfo;
//static SBlockInfo  comServ01Info;
//static SBlockInfo  comServ02Info;

static char s_szDummy[] = " ";  // ������, ��������������� ������

static char s_szFirstBlockUserDef[32];  // ������������ ������� �� ����������� ������������� ������
static UINT s_uUserDefBlocksSize;       // ��������� ����� ����������� ������������� ������

//extern "C" uint8_t* pKeyBuffer;

static struct SGeneralParameters  genParams;
static struct SSupplierParameters supParams;
static struct SFixedParameters    fixParams;
static struct SDataPts            dataPts;

//static SComServData01      comServ01;
//static SComServData02      comServ02;

static uint16_t          s_chkSum;
static unsigned short NumEventNow; //����� ������� ������� � prePareSor 
// ������ ��� ������������ ...
static char s_szSoftwareRevision[16]; // ������� ��
static char s_szModelDevice[48];      // ������ �������
static char s_szOpticalModule[40];    // ������������ ����������� ������
static char s_szFiberIDNum[48];       // ������� �������� ������ � �������
static char s_szNumDevice[8];         // ������ �������

// �������� ������� � �������������/�������������� ���������� ����� ��� ���������� � ���������� SD-�����

inline FRESULT f_openWTC(FIL* fp, const TCHAR* path, BYTE mode)
{
//  while(*path == ' ' || *path == '\t')
//    ++path;
//
//  bool  bDriveIs0 = (*path == '0');
//
//  if(bDriveIs0)
//    vTaskSuspendAll();

  FRESULT res = f_open(fp, path, mode);

//  if(bDriveIs0)
//    xTaskResumeAll();

  return res;
}

inline FRESULT f_closeWTC(FIL* fp)
{
  // Note: ����� ���������� ��������, �.�. f_close() ����������� ������ �� ��������� fp->fs
//  bool  bDriveIs0 = (fp->fs->drv == 0);
//
//  if(bDriveIs0)
//    vTaskSuspendAll();

  FRESULT res = f_close(fp);

//  if(bDriveIs0)
//    xTaskResumeAll();

  return res;
}

inline FRESULT f_syncWTC(FIL* fp)
{
//  if(fp->fs->drv == 0)
//    vTaskSuspendAll();

  FRESULT res = f_sync(fp);

//  if(fp->fs->drv == 0)
//    xTaskResumeAll();

  return res;
}

inline FRESULT f_unlinkWTC(const TCHAR* path)
{
//  while(*path == ' ' || *path == '\t')
//    ++path;
//
//  bool  bDriveIs0 = (*path == '0');
//
//  if(bDriveIs0)
//    vTaskSuspendAll();

  FRESULT res = f_unlink(path);

//  if(bDriveIs0)
//    xTaskResumeAll();

  return res;
}

inline FRESULT f_readWTC(FIL* fp, void* buff, UINT btr, UINT* br)
{
//  if(fp->fs->drv == 0)
//    vTaskSuspendAll();

  FRESULT res = f_read(fp, buff, btr, br);

//  if(fp->fs->drv == 0)
//    xTaskResumeAll();

  return res;
}

inline FRESULT f_writeWTC(FIL* fp, const void* buff, UINT btw, UINT* bw)
{
//  if(fp->fs->drv == 0)
//    vTaskSuspendAll();
//
  FRESULT res = f_write(fp, buff, btw, bw);

//  if(fp->fs->drv == 0)
//    xTaskResumeAll();

  return res;
}

/*-----------------------------------------------------------------------*/
/* Get a zero-closed string from the file                                */
/*-----------------------------------------------------------------------*/
// ���� ���������!!!  �����
// ����������� �� f_gets()
static TCHAR* f_getsz(TCHAR*  buff,  // Pointer to the string buffer to read
                      int     len,   // Size of string buffer (characters)
                      FIL*    fp)    // Pointer to the file object
{
  int     n = 0;
  TCHAR   c;
  TCHAR*  p = buff;

  while(n < len - 1)  /* Read characters until buffer gets filled */
  {
    /* Read a character without conversion */
    if(f_readCks(fp, &c, 1) != FR_OK)
      break;

    n++;

    if(c == (TCHAR)0)
      break;

    *p++ = c;
  }

  *p = 0;

  return n ? buff : 0;      /* When no data read (eof or error), return with error. */
}

// ������ �������� ���������� ����� ���������
//int CalcParamsBetweenMarkers(int iPlaceLS)
//{
//  if(!g_eventSearch.sUseMarkers)
//     return OTDR_NOTHING_TO_DO;
//
//  // ���������� �� ��������, �
//  float fDistA_m;
//  float fDistB_m;
//
//  if(g_eventSearch.lDistToMarkerL_cm < g_eventSearch.lDistToMarkerR_cm)
//  {
//    fDistA_m = 0.01 * g_eventSearch.lDistToMarkerL_cm;
//    fDistB_m = 0.01 * g_eventSearch.lDistToMarkerR_cm;
//  }
//  else
//  {
//    fDistA_m = 0.01 * g_eventSearch.lDistToMarkerR_cm;
//    fDistB_m = 0.01 * g_eventSearch.lDistToMarkerL_cm;
//  }
//
//  int iGraphItem = 0; // ������ ������� ��� �������� ����� �����
//  int iWaveLengh = g_ConfigDevice.PlaceLS[iPlaceLS];
//
//  // ����� �������
//  for(int iItem = 0; iItem < MAX_GRAPH_ITEM_COUNT; ++iItem)
//  {
//    if(lossGraph.GetItemUserData(iItem) == iWaveLengh)
//    {
//      iGraphItem = iItem;
//      break;
//    }
//  }
//
//  // ������ ������� � ������ ��� ���������, ��
//  float fValueA;
//  float fValueB;
//
//  if(g_eventSearch.sApproxType == OTDR_LS)
//  {
//#if 1
//    lossGraph.CalcLinearAtten(iGraphItem, fDistA_m, fDistB_m, &fValueA, &fValueB);
//#else
//    // ������ ����� ������, ��/��
//    float fSlope = lossGraph.CalcLinearAtten(iGraphItem, fDistA_m, fDistB_m, &fValueA, &fValueB);
//
//    comServ01.usMeanSlopeBetweenMarkers = (uint16_t)(1.e+6 * fSlope);
//#endif
//  }
//  else // OTDR_2P
//  {
//    fValueA = lossGraph.GetValue(iGraphItem, fDistA_m);
//    fValueB = lossGraph.GetValue(iGraphItem, fDistB_m);
//  }
//
//  g_resBetweenMarkers.fDist = fDistB_m - fDistA_m;
//  g_resBetweenMarkers.fLoss = fValueA - fValueB;
//  g_resBetweenMarkers.fMaxReflection = GetMaxDiscreteReflectance(fDistA_m, fDistB_m, iPlaceLS);
//#if 0
//  g_resBetweenMarkers.fORL = CalcORL(fDistA_m, fDistB_m, iPlaceLS);
//#else
//  g_resBetweenMarkers.fORL = 0.;  // ���� �� ����� ������� ORL
//#endif
//
//  return OTDR_OKAY;
//}

// ���������� ������ ��� ���������� � Sor-�������
//
// iPlaceLS     ����� ����������� �����
// pDataPoints  ��������� �� ������ ����� ���������
//
static int PrepareSorData(int iPlaceLS, uint16_t* pDataPoints)
{
  //SKeyEvents& keyEvents = g_keyEvents[iPlaceLS]; // ������ �� ���-�� ������� ��� ����� ������
  float       fGroupIndex = GetIndexWAV();//GetGroupIndexForPlace(iPlaceLS);
  NumEventNow=MakeEvents();// = GetNumEvents(); ����� ������� - ���� 
  
  
  map.cBlocks = 1;
  map.info.revision = DEF_REVISION;
  map.info.size = sizeof(szBlockMap) + sizeof(map);

  map.info.size += sizeof(szBlockGenParams) + sizeof(genParamsInfo);
  ++map.cBlocks;
  map.info.size += sizeof(szBlockSupParams) + sizeof(supParamsInfo);
  ++map.cBlocks;
  map.info.size += sizeof(szBlockFixParams) + sizeof(fixParamsInfo);
  ++map.cBlocks;

  if(NumEventNow) // ���� ���� �������
  {
    map.info.size += sizeof(szBlockKeyEvents) + sizeof(keyEventsInfo);
    ++map.cBlocks;
  }

  map.info.size += sizeof(szBlockDataPts) + sizeof(dataPtsInfo);
  ++map.cBlocks;

// �������� �� ������������� 06.08.2025
//  // ���������� ����� ������� � ������� �������?
//  bool  bComServ02IsRequired;
//
//  if(g_eventSearch.iSwitch == SWITCH_ON)
//    bComServ02IsRequired = (g_eventSearch.launchCord.sUsed || g_eventSearch.tailCord.sUsed);
//  else
//    bComServ02IsRequired = false;
//
//  // ������������ �������?
//  bool  bComServ01IsRequired = g_reportParam.bSaveMarkersInfo && g_eventSearch.sUseMarkers;
//
//  if(bComServ01IsRequired)
//  {
//    // ����� ���� ComServ01
//    map.info.size += sizeof(szBlockComServ01) + sizeof(comServ01Info);
//    ++map.cBlocks;
//  }
//
//  if(bComServ02IsRequired)
//  {
//    // ����� ���� ComServ02
//    map.info.size += sizeof(szBlockComServ02) + sizeof(comServ02Info);
//    ++map.cBlocks;
//  }

  map.info.size += sizeof(szBlockCksum) + sizeof(chksumInfo);
  ++map.cBlocks;
  //g_CabelFileInfo.NumFiber ����� �������, ���������� ������������� ����� ������ � FiberId (��������� 47 �� 30.01.2017)

//  if(g_ConfigDevice.ID_Device == 0)   // i.e., �����-9400
//  {
    genParams.achLanguageCode[0] = 'R';
    genParams.achLanguageCode[1] = 'U';
//  }
//  else  // MOT-940
//  {
//    genParams.achLanguageCode[0] = 'E';
//    genParams.achLanguageCode[1] = 'N';
//  }
  // ��������� ������� GenParams - ������� �� ���������� ���������
  genParams.pszCableId = *GenParams.CID ? GenParams.CID : s_szDummy; // CID
  //genParams.pszFiberId = *g_CabelFileInfo.NameInfoFiber ? g_CabelFileInfo.NameInfoFiber : s_szDummy;
  sprintf(s_szFiberIDNum,"%s", *GenParams.FID ? GenParams.FID : s_szDummy); //FID
  sprintf(s_szFiberIDNum,"%s %d",s_szFiberIDNum,NameDB.FiberID);
  genParams.pszFiberId = s_szFiberIDNum;
  genParams.sFiberType = 652;                      // # ? 652 - default
  //GetLengthWaveLS (0)        // ������������� ����� �����
  genParams.sNominalWavelength_nm =(unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT)); // ��������� ����� ����� �� ������� ������������� �����
  // # ? ����� � g_measConfig.iWaveLenIdx 1310 - def
  genParams.pszOriginatingLocation = *GenParams.OL ? GenParams.OL : s_szDummy;
  genParams.pszTerminatingLocation = *GenParams.TL ? GenParams.TL : s_szDummy;
  genParams.pszCableCode = s_szDummy;
  genParams.achCurrentDataFlag[0] = 'B';  genParams.achCurrentDataFlag[1] = 'C';  // # ? �� ��������������� � GUI


  genParams.pszOperator = *GenParams.OP ? GenParams.OP : s_szDummy;
  genParams.pszComment  = *GenParams.CMT ? GenParams.CMT : s_szDummy;

  genParamsInfo.revision = DEF_REVISION;
  genParamsInfo.size =
    sizeof(szBlockGenParams) +
    sizeof(genParams.achLanguageCode) +
    strlen(genParams.pszCableId) + 1 +
    strlen(genParams.pszFiberId) + 1 +
    sizeof(genParams.sFiberType) +
    sizeof(genParams.sNominalWavelength_nm) +
    strlen(genParams.pszOriginatingLocation) + 1 +
    strlen(genParams.pszTerminatingLocation) + 1 +
    strlen(genParams.pszCableCode) + 1 +
    sizeof(genParams.achCurrentDataFlag) +
    sizeof(genParams.lUserOffset_100ps) +
    sizeof(genParams.ulUserOffsetDistance) +
    strlen(genParams.pszOperator) + 1 +
    strlen(genParams.pszComment) + 1;
    
    // �������� ��������� SupParams
    GetDeviceName( szTest ); // ������ ������ ��������������
    supParams.pszSupplierName = *SupParams.SN ? SupParams.SN : s_szDummy; // ������������� SN
  supParams.pszOTDRMainframeID = *SupParams.MFID ? SupParams.MFID : s_szDummy;
  supParams.pszOTDRMainframeSN =  *SupParams.OTDR ? SupParams.OTDR : s_szDummy;
  supParams.pszOpticalModuleID = *SupParams.OMID ? SupParams.OMID : s_szDummy;    // # ? (�� ������������ � �������) ��������� ���������� ������
  supParams.pszOpticalModuleSN = s_szDummy;           // # ? (�� ������������ � �������) ����� ��� ��, ��� � � ������� s_szNumDevice
  supParams.pszSoftwareRevision = *SupParams.SR ? SupParams.SR : s_szDummy; 
  supParams.pszOther = s_szDummy;   // # ?
  
  // ���� Supplier Params
//  //supParams.pszSupplierName = (char*)Ides[GetID_Dev()];//aszSupplierName[g_ConfigDevice.ID_Device];
//  // ����� ���� ������������ ������ ��� ������� ������� ����� ����, ���� �����-9400-�-31-55-85-30-PMH-VFL
//  //supParams.pszOTDRMainframeID = (char*)triDMainframeOTDR[g_ConfigDevice.ID_Device].get();//aszMainframeOTDR[g_ConfigDevice.ID_Device];
//    GetStringPlace(s_szSoftwareRevision, 0);
//  sprintf (s_szModelDevice, "%s-%c-%s%s%s", //-VFL ������ �� ��������! (� ����� ��������)
//           triDMainframeOTDR[g_ConfigDevice.ID_Device].get(),
//           GetSmbDev(0),//(g_ConfigDevice.TypeOM)?("A"):("B"),
//           s_szSoftwareRevision,
//           (g_ConfigDevice.CfgPM)?((g_ConfigDevice.CfgPM-1)?("PMH"):("PM")):(""),
//           (g_ConfigDevice.ID_Device)?("-VFL"):(""));//
//  supParams.pszOTDRMainframeID = s_szModelDevice;
//  sprintf(s_szNumDevice, "%d", g_ConfigDevice.NumDevice);
//  supParams.pszOTDRMainframeSN = s_szNumDevice;
//  GetStringPlace(s_szSoftwareRevision, 1);
//  sprintf (s_szOpticalModule, "%c-%s%s-VFL", // ��������� ���������� ������ (�� �������)
//           GetSmbDev(0),//(g_ConfigDevice.TypeOM)?("A"):("B"),
//           s_szSoftwareRevision,
//           (g_ConfigDevice.CfgPM)?((g_ConfigDevice.CfgPM-1)?("PMH"):("PM")):(""));//
//  supParams.pszOpticalModuleID = s_szOpticalModule;   // # ? (�� ������������ � �������) ��������� ���������� ������
//  supParams.pszOpticalModuleSN = s_szDummy;           // # ? (�� ������������ � �������) ����� ��� ��, ��� � � ������� s_szNumDevice
//  sprintf(s_szSoftwareRevision, "1.%04d", g_NumVer);  //%s, g_ConfigDevice.ID_Device ? "EN" : "RU"
//  supParams.pszSoftwareRevision = s_szSoftwareRevision;
//  supParams.pszOther = s_szDummy;   // # ?

  supParamsInfo.revision = DEF_REVISION;
  supParamsInfo.size =
    sizeof(szBlockSupParams) +
    strlen(supParams.pszSupplierName) + 1 +
    strlen(supParams.pszOTDRMainframeID) + 1 +
    strlen(supParams.pszOTDRMainframeSN) + 1 +
    strlen(supParams.pszOpticalModuleID) + 1 +
    strlen(supParams.pszOpticalModuleSN) + 1 +
    strlen(supParams.pszSoftwareRevision) + 1 +
    strlen(supParams.pszOther) + 1;
 
// ������� Fixed Params   
  fixParams.ulDateTimeStamp = TotalSec (TimeSaveOTDR);    // # DTS
  // UD
#ifdef OTDR_DIST_IN_KM
  fixParams.achUnitsOfDistance[0] = 'k'; fixParams.achUnitsOfDistance[1] = 'm'; // km - [��]
#else
  fixParams.achUnitsOfDistance[0] = 'm'; fixParams.achUnitsOfDistance[1] = 't'; // mt - [�]
#endif

  fixParams.sActualWavelength_01nm = genParams.sNominalWavelength_nm * 10; // AW ???? ������ ������ �� 10 
  fixParams.lAcquisitionOffset_100ps = 0; // AO  ��� �������� 
  fixParams.ulAcquisitionOffsetDistance = 0; // AOD ��� �������� 
//  fixParams.lAcquisitionOffset_100ps = g_FileParamInfo.iAcquisitionOffset_100ps;
//#ifdef OTDR_DIST_IN_KM
//  fixParams.ulAcquisitionOffsetDistance = g_FileParamInfo.uAcquisitionOffsetDistance_10cm * 10 / 100;// [0.1m]->[0.1km]
//#else
//  fixParams.ulAcquisitionOffsetDistance = g_FileParamInfo.uAcquisitionOffsetDistance_10cm; // [0.1m]=[0.1m]
//#endif
  fixParams.sTotalNumberOfPulseWidthUsed = 1;         // TPW # ? ������ � ������� 6000_�����003 ������ 0?

  // ��������� ��������� �.�. ��������� �������� ��� ������� �� TPW
  // --->
  fixParams.sPulseWidthUsed_ns = GetWidthPulse(GetIndexIM());     // PWU g_measConfig.iImpulse;
  fixParams.lDataSpacing_100ps = (int32_t)GetValueDS(); // DS ������������� �������� DS ��� �������������� ������ ���������     // �� ���� �������, ��� �������� ��� ���� * 100000.
  fixParams.lNumberOfDataPoints = OUTSIZE; //  NPPW #define �� system.h
  // <---
  fixParams.lGroupIndex = (int32_t)(fGroupIndex * 100000.); //GI
  
  fixParams.sBackscatterCoefficient_dB = (int16_t)ReflParam.BC;; //BC from TR_PARAM ReflParam  DeviceData.h
  fixParams.lNumberOfAverages = SettingRefl.NumAvrag; //NAV 
  //fixParams.usAveragingTime_01sec = g_FileParamInfo.usAveragingTime;  //AT   ???��� �������� � [0.1sec]???
  fixParams.usAveragingTime_01sec = GetTimeAvrg(GetIndexVRM())*10;//AT   ����� ���������� � ���
 // fixParams.lAcquisitionRange_100ps = (int32_t)((double)(StatusFile.ValueDS/10000.0)*g_FileParamInfo.iNumPoints);  // AR StatusFile.NumPtsMain
  fixParams.lAcquisitionRange_100ps = Get_AR();  // AR StatusFile.NumPtsMain
  // ARD = �������� ��������� � ������ (� ��� ���!?) 
  // �� ���� �� ��� ������ �� ��� ���� ������� ��� ������������� ��������� �.�. 0.5��,2,4,8,16,32,64,128 �� ��� �� � ������
////  fixParams.lAcquisitionRangeDistance = 0;        //g_FileParamInfo.iDistance/1000
//#ifdef OTDR_DIST_IN_KM
//  fixParams.lAcquisitionRangeDistance = (int32_t)(g_fDistStep * (g_FileParamInfo.iNumPoints - 1) / 100.);   // [0.1km]
//#else
//  fixParams.lAcquisitionRangeDistance = (int32_t)(g_fDistStep * (g_FileParamInfo.iNumPoints - 1) * 10.);    // [0.1m]
//#endif
  fixParams.lAcquisitionRangeDistance = (int32_t)(GetLengthLine(GetIndexLN()) * 10000.);    // ARD[0.1m]� �������� ��
  fixParams.lFrontPanelOffset_100ps = 0;
  fixParams.usNoiseFloorLevel_dB = 65535;
  fixParams.sNoiseFloorScaleFactor = 1000;
  fixParams.usPowerOffsetFirstPoint_dB = 0;
  fixParams.usLossThreshold_dB = (uint16_t)(ReflParam.LT); //LT

  // �������� ������ �� ��������� [-69.9; -10.] RT
  // � sor-����� ��� �������� ��������� 2 ����� (uint16_t)
  // ���������� ��� ������ �������� �� -65.5 ��� ��������� (�.�., 65500),
 //  � ������� (������ �� -69.9) ������������ � 35
  // � ��� ����� ��� ���������! ����� ������ ����������
//  if(g_eventSearch.fReflectance < -65.5)
//  {
//    uint16_t usScaledAddn = (uint16_t)((-g_eventSearch.fReflectance - 65.5 + 0.05) * 35. / 4.4);
//
//    if(usScaledAddn > 35)
//      fixParams.usReflectanceThreshold_dB = 65535;
//    else
//      fixParams.usReflectanceThreshold_dB = 65500 + usScaledAddn;
//  }
//  else
//    fixParams.usReflectanceThreshold_dB = (uint16_t)(g_eventSearch.fReflectance * -1000.); // ���� � ��� ������...
 
  fixParams.usReflectanceThreshold_dB = (uint16_t)ReflParam.RT;
  
  
  //fixParams.usEndOfFiberThreshold_dB = (uint16_t)(g_eventSearch.fEndOfLine * 1000.); //ET
  fixParams.usEndOfFiberThreshold_dB = (uint16_t)(ReflParam.ET); //ET

  // fixParams.achTraceType - two-charscter string: ST, RT, RF or DT
//  switch(g_CabelFileInfo.DirectInfo)
//  {
//  case 1:
//    fixParams.achTraceType[0] = 'R'; fixParams.achTraceType[1] = 'T';
//    break;
//  case 2:
//    fixParams.achTraceType[0] = 'R'; fixParams.achTraceType[1] = 'F';
//    break;
//  case 3:
//    fixParams.achTraceType[0] = 'D'; fixParams.achTraceType[1] = 'T';
//    break;
//  default:
//    fixParams.achTraceType[0] = 'S'; fixParams.achTraceType[1] = 'T';
//  }
  // TT
    fixParams.achTraceType[0] = 'S'; fixParams.achTraceType[1] = 'T';

  fixParams.lWinUpperLeftX = 0;
  fixParams.lWinUpperLeftY = 0;
  fixParams.lWinLowerRightX = 0;
  fixParams.lWinLowerRightY = 0;

  fixParamsInfo.revision = DEF_REVISION;
  fixParamsInfo.size =
    sizeof(szBlockFixParams) +
    sizeof(fixParams.ulDateTimeStamp) +
    sizeof(fixParams.achUnitsOfDistance) +
    sizeof(fixParams.sActualWavelength_01nm) +
    sizeof(fixParams.lAcquisitionOffset_100ps) +
    sizeof(fixParams.ulAcquisitionOffsetDistance) +
    sizeof(fixParams.sTotalNumberOfPulseWidthUsed) +
    // ���������� ������� ��������
    fixParams.sTotalNumberOfPulseWidthUsed *
    (sizeof(fixParams.sPulseWidthUsed_ns) + sizeof(fixParams.lDataSpacing_100ps) + sizeof(fixParams.lNumberOfDataPoints)) +
    sizeof(fixParams.lGroupIndex) +
    sizeof(fixParams.sBackscatterCoefficient_dB) +
    sizeof(fixParams.lNumberOfAverages) +
    sizeof(fixParams.usAveragingTime_01sec) +
    sizeof(fixParams.lAcquisitionRange_100ps) +
    sizeof(fixParams.lAcquisitionRangeDistance) +
    sizeof(fixParams.lFrontPanelOffset_100ps) +
    sizeof(fixParams.usNoiseFloorLevel_dB) +
    sizeof(fixParams.sNoiseFloorScaleFactor) +
    sizeof(fixParams.usPowerOffsetFirstPoint_dB) +
    sizeof(fixParams.usLossThreshold_dB) +
    sizeof(fixParams.usReflectanceThreshold_dB) +
    sizeof(fixParams.usEndOfFiberThreshold_dB) +
    sizeof(fixParams.achTraceType) +
    4 * sizeof(fixParams.lWinUpperLeftX);
 // ��� �������... ������ ������ �������
  if(NumEventNow)
  {
    keyEventsInfo.revision = DEF_REVISION;
    // ����� ��������� ������ ����� ��� ��������
    keyEventsInfo.size =
      sizeof(szBlockKeyEvents) +
      sizeof(NumEventNow) +

      NumEventNow * 52 +
//      NumEventNow *
//        (sizeof(keyEvents.aEvent[0].sEventNumber) +
//         sizeof(keyEvents.aEvent[0].lEventPropagationTime) +
//           sizeof(keyEvents.aEvent[0].sAttenuationCoefficient_dBkm) +
//           sizeof(keyEvents.aEvent[0].sEventLoss_dB) +
//        sizeof(keyEvents.aEvent[0].lEventReflectance_dB) +
//        sizeof(keyEvents.aEvent[0].chsEventCode) +
//        sizeof(keyEvents.aEvent[0].chsLossMeasurementTechnique) +
//        sizeof(keyEvents.aEvent[0].lMarkerLocation1) +
//        sizeof(keyEvents.aEvent[0].lMarkerLocation2) +
//        sizeof(keyEvents.aEvent[0].lMarkerLocation3) +
//        sizeof(keyEvents.aEvent[0].lMarkerLocation4) +
//        sizeof(keyEvents.aEvent[0].lMarkerLocation5)) +
      22;
//      sizeof(keyEvents.lEndToEndLoss) +
//      sizeof(keyEvents.lEndToEndMarkerPos1) +
//      sizeof(keyEvents.lEndToEndMarkerPos2) +
//      sizeof(keyEvents.uOpticalReturnLoss) +
//      sizeof(keyEvents.lReturnLossMarkerPos1) +
//      sizeof(keyEvents.lReturnLossMarkerPos2);

    // ���� ����� ������������ -  ��� ������ ��� ������������ (10 ����)
//    for(int16_t i = 0; i < NumEventNow; ++i)
//      keyEventsInfo.size += strlen(keyEvents.aEvent[i].szComment) + 1;
  }
// ���� ������ ��������������
  //dataPts.lNumPoints = g_FileParamInfo.iNumPoints;
  dataPts.lNumPoints = OUTSIZE;
  dataPts.sNumScaleFactors = 1;
  //dataPts.lNumPointsUsingScaleFactor = g_FileParamInfo.iNumPoints;
  dataPts.lNumPointsUsingScaleFactor = OUTSIZE;
  dataPts.sScaleFactor = 1000;
  dataPts.pDataPoints = pDataPoints;

  dataPtsInfo.revision = DEF_REVISION;
  dataPtsInfo.size =
    sizeof(szBlockDataPts) +
    sizeof(dataPts.lNumPoints) +
    sizeof(dataPts.sNumScaleFactors) +
    sizeof(dataPts.lNumPointsUsingScaleFactor) +
    sizeof(dataPts.sScaleFactor) +
    dataPts.lNumPoints * sizeof(*dataPts.pDataPoints);

 // �������� ��������� �������������� �����
//  if(bComServ01IsRequired) // ���������� ��������� �������������� ���� ����������� 01?
//  {
//    comServ01Info.revision = COM_SERV_REVISION;
//    comServ01Info.size = sizeof(szBlockComServ01) + sizeof(comServ01);
//
//    if(NumEventNow)
//      comServ02.lPrimaryLenOfLink_cm = (int32_t)(keyEvents.fPrimaryLenOfLink * 100.);
//    else
//      comServ02.lPrimaryLenOfLink_cm = 0;
//
//    if(g_eventSearch.sUseMarkers)
//    {
//      if(g_eventSearch.lDistToMarkerL_cm < g_eventSearch.lDistToMarkerR_cm)
//      {
//        comServ01.lDistToTheLeftMarker_100ps  = g_eventSearch.lDistToMarkerL_cm;
//        comServ01.lDistToTheRightMarker_100ps = g_eventSearch.lDistToMarkerR_cm;
//      }
//      else
//      {
//        comServ01.lDistToTheLeftMarker_100ps  = g_eventSearch.lDistToMarkerR_cm;
//        comServ01.lDistToTheRightMarker_100ps = g_eventSearch.lDistToMarkerL_cm;
//      }
//
//      float fCmTo100ps = 0.01 * fGroupIndex * 1.e+10 / LIGHTSPEED;  // ����-� �������������� �� �� � ����� ����������
//
//      comServ01.lDistToTheLeftMarker_100ps  *= fCmTo100ps;
//      comServ01.lDistToTheRightMarker_100ps *= fCmTo100ps;
//
//      CalcParamsBetweenMarkers(iPlaceLS);
//
//      comServ01.usApproxMethod = g_eventSearch.sApproxType;
//      comServ01.usDisplayFlag = g_eventSearch.sDisplayFlag;
//
//      comServ01.usLossBetweenMarkres = (uint16_t)(g_resBetweenMarkers.fLoss * 1000.);
//      // [0.001 * dB / km]
//      comServ01.usMeanSlopeBetweenMarkers = (uint16_t)(1.e+6 * g_resBetweenMarkers.fLoss / g_resBetweenMarkers.fDist);
//      comServ01.lMaxReflBetweenMarkers = (int32_t)(g_resBetweenMarkers.fMaxReflection * 1000.);
//      comServ01.usORL_BetweenMarkers = (uint16_t)(g_resBetweenMarkers.fORL * 1000.);
//    }
//    else
//    {
//      comServ01.lDistToTheLeftMarker_100ps = 0;
//      comServ01.lDistToTheRightMarker_100ps = 0;
//      comServ01.usApproxMethod = OTDR_2P;
//      comServ01.usDisplayFlag = OTDR_DISPLAY_MAX_REFLECTION;
//      comServ01.usLossBetweenMarkres = 0;
//      comServ01.usMeanSlopeBetweenMarkers = 0;
//      comServ01.lMaxReflBetweenMarkers = 0;
//      comServ01.usORL_BetweenMarkers = 0;
//    }
//  }
//  else
//    comServ01Info.size = 0;   // ��������, ��� ���� � sor �������� �� �����

//  if(bComServ02IsRequired) // ���������� ��������� ���� � ������� �������-����� ������� �������?
//  {
//    SLineWithCords& lineWithCords = g_lineWithCords[iPlaceLS];
//    SKeyEvent&      launchEvent = lineWithCords.launchEvent;
//    SKeyEvent&      tailEvent = lineWithCords.tailEvent;
//
//    int iEventDataSize =          // ��������� ������ ����� ��������� ��� ������ �����������
//      sizeof(launchEvent.sEventNumber) +
//      sizeof(launchEvent.lEventPropagationTime) +
//      sizeof(launchEvent.sAttenuationCoefficient_dBkm) +
//      sizeof(launchEvent.sEventLoss_dB) +
//      sizeof(launchEvent.lEventReflectance_dB) +
//      sizeof(launchEvent.chsEventCode) +
//      sizeof(launchEvent.chsLossMeasurementTechnique) +
//      sizeof(launchEvent.lMarkerLocation1) +
//      sizeof(launchEvent.lMarkerLocation2) +
//      sizeof(launchEvent.lMarkerLocation3) +
//      sizeof(launchEvent.lMarkerLocation4) +
//      sizeof(launchEvent.lMarkerLocation5);
//
//    comServ02Info.size =
//      sizeof(szBlockComServ02) +
//      sizeof(comServ02) +
//      sizeof(lineWithCords.lEndToEndLoss) +
//      sizeof(lineWithCords.lEndToEndMarkerPos1) +
//      sizeof(lineWithCords.lEndToEndMarkerPos2) +
//      sizeof(lineWithCords.uOpticalReturnLoss) +
//      sizeof(lineWithCords.lReturnLossMarkerPos1) +
//      sizeof(lineWithCords.lReturnLossMarkerPos2);
//
//    if(g_eventSearch.launchCord.sUsed)
//    {
//      comServ02.lLaunchCordLen_cm = g_eventSearch.launchCord.lLen_cm;
//      comServ02.sAddBeginConnectionLoss = g_eventSearch.launchCord.sAddConnectionLoss;
//      comServ02Info.size += iEventDataSize + strlen(launchEvent.szComment) + 1;
//    }
//    else
//    {
//      comServ02.lLaunchCordLen_cm = 0;
//      comServ02.sAddBeginConnectionLoss = 0;
//    }
//
//    if(g_eventSearch.tailCord.sUsed)
//    {
//      comServ02.lTailCordLen_cm = g_eventSearch.tailCord.lLen_cm;
//      comServ02.sAddEndConnectionLoss = g_eventSearch.tailCord.sAddConnectionLoss;
//      comServ02Info.size += iEventDataSize + strlen(tailEvent.szComment) + 1;
//    }
//    else
//    {
//      comServ02.lTailCordLen_cm = 0;
//      comServ02.sAddEndConnectionLoss = 0;
//    }
//
//    comServ02Info.revision = COM_SERV_REVISION;
//  }
//  else
//    comServ02Info.size = 0;
 // ����������� �����
  chksumInfo.revision = DEF_REVISION;
  chksumInfo.size = sizeof(szBlockCksum) + sizeof(uint16_t);

  return 0;
}

//
// ����� �������� (�������������) ����������� �����
//
static void ResetCheckSum(void)
{
  s_chkSum = 0xFFFF;
}

//
// �������� (����������) �������� ����������� �����
//
// buff   ��������� �� ���� ������
// cBytes ���������� ������
//
static uint16_t RecalcCheckSum(const void* buff, UINT cBytes)
{
  UINT  len = cBytes;
  char* pBuff = (char*)buff;

  while(len--)
  {
    s_chkSum ^= *pBuff++ << 8;

    for(int i = 0; i < 8; ++i)
      s_chkSum = s_chkSum & 0x8000 ? (s_chkSum << 1) ^ 0x1021 : s_chkSum << 1;
  }

  return s_chkSum;
}

//
// ������ � ���� � ���������� ����������� �����
//
// pFile        ��������� �� FIL-���������
// buff         ��������� �� ���� ������ ��� ������
// cBytesToW    ���������� ������ ��� ������
//
static FRESULT f_writeCks(FIL* pFile, const void* buff, UINT cBytesToW)
{
  UINT    cBytesW;        // ���������� ���������� ������
  FRESULT res;

  res = f_writeWTC(pFile, buff, cBytesToW, &cBytesW);

  if(res != FR_OK || cBytesW != cBytesToW)
  {
    res = FR_DISK_ERR;
    myBeep(200);
  }
  else
  {
    RecalcCheckSum(buff, cBytesW);

    res = f_syncWTC(pFile);

    if(res != FR_OK)
      myBeep(200);
  }

  return res;
}

//// ������ ������ ������ �������
//FRESULT WriteEvent(FIL* pFile, SKeyEvent &rEvent)
//{
//  // ��������� ��������� ����������� ����� KeyEvent � �����
//  char* pch = g_szBuffer;  // g_szBuffer[BUFFER_SIZE]
//
//  *((int16_t*)pch) = rEvent.sEventNumber;
//  pch += sizeof(rEvent.sEventNumber);
//  *((int32_t*)pch) = rEvent.lEventPropagationTime;
//  pch += sizeof(rEvent.lEventPropagationTime);
//  *((int16_t*)pch) = rEvent.sAttenuationCoefficient_dBkm;
//  pch += sizeof(rEvent.sAttenuationCoefficient_dBkm);
//  *((int16_t*)pch) = rEvent.sEventLoss_dB;
//  pch += sizeof(rEvent.sEventLoss_dB);
//  *((int32_t*)pch) = rEvent.lEventReflectance_dB;
//  pch += sizeof(rEvent.lEventReflectance_dB);
//
//  for(int ich = 0; ich < ArraySize(rEvent.chsEventCode); )
//    *pch++ = rEvent.chsEventCode[ich++];
//
//  *pch++ = rEvent.chsLossMeasurementTechnique[0];
//  *pch++ = rEvent.chsLossMeasurementTechnique[1];
//  *((int32_t*)pch) = rEvent.lMarkerLocation1;
//  pch += sizeof(rEvent.lMarkerLocation1);
//  *((int32_t*)pch) = rEvent.lMarkerLocation2;
//  pch += sizeof(rEvent.lMarkerLocation2);
//  *((int32_t*)pch) = rEvent.lMarkerLocation3;
//  pch += sizeof(rEvent.lMarkerLocation3);
//  *((int32_t*)pch) = rEvent.lMarkerLocation4;
//  pch += sizeof(rEvent.lMarkerLocation4);
//  *((int32_t*)pch) = rEvent.lMarkerLocation5;
//  pch += sizeof(rEvent.lMarkerLocation5);
//  strcpy(pch, rEvent.szComment);
//
//  int iEventDataLen = pch - g_szBuffer + strlen(rEvent.szComment) + 1;
//
//  // ������ ����������� ������
//  return f_writeCks(pFile, g_szBuffer, iEventDataLen);
//}

//// ������ ��������� ������ ������ �� �����
//FRESULT WriteLossesOfLink(FIL* pFile, SLossesOfLink &rLosses)
//{
//  // ��������� ��������� ����������� ����� SLossesOfLink � �����
//  char* pch = g_szBuffer;
//
//  *((int32_t*)pch) = rLosses.lEndToEndLoss;
//  pch += sizeof(rLosses.lEndToEndLoss);
//  *((int32_t*)pch) = rLosses.lEndToEndMarkerPos1;
//  pch += sizeof(rLosses.lEndToEndMarkerPos1);
//  *((int32_t*)pch) = rLosses.lEndToEndMarkerPos2;
//  pch += sizeof(rLosses.lEndToEndMarkerPos2);
//  *((uint16_t*)pch) = rLosses.uOpticalReturnLoss;
//  pch += sizeof(rLosses.uOpticalReturnLoss);
//  *((int32_t*)pch) = rLosses.lReturnLossMarkerPos1;
//  pch += sizeof(rLosses.lReturnLossMarkerPos1);
//  *((int32_t*)pch) = rLosses.lReturnLossMarkerPos2;
//  pch += sizeof(rLosses.lReturnLossMarkerPos2);
//
//  // ������ ����������� ������
//  return f_writeCks(pFile, g_szBuffer, pch - g_szBuffer);
//}

// ������ Sor-�����
//
// pszFileName  ��� �����, ������� logical drive "0:" ��� "1:"
// iPlaceLS     ������ ����������� �����
// pDataPoints  ��������� �� ������ ����� ���������
//
int WriteSorFile(const char* pszFileName, int iPlaceLS, uint16_t* pDataPoints)
{
  FIL     file;
  FIL*    pFile = &file;
  FRESULT res;
  int     i;
  // �������� ������� �������� ��� ��� ��� ������������� ������
  //SKeyEvents& keyEvents = g_keyEvents[iPlaceLS]; // ������ �� ���-�� ������� ��� ����� ������

  PrepareSorData(iPlaceLS, pDataPoints);

  // ���� �� �������� ������ ��� ������ ��� �����
  if(!g_pszFileNames[iPlaceLS])
    g_pszFileNames[iPlaceLS] = (char*)malloc(MAX_PATH_LEN);

  // Note: ����� ������� ������ ������, �.�. pszFileName ����� ��������� �� ��...

  res = f_openWTC(pFile, pszFileName, FA_CREATE_ALWAYS | FA_WRITE);

  if(res != FR_OK)
  {
    if(g_pszFileNames[iPlaceLS])
      g_pszFileNames[iPlaceLS][0] = 0;

    return res;
  }

  res = f_syncWTC(pFile);

  if(res != FR_OK)
  {
    f_closeWTC(pFile);

    if(g_pszFileNames[iPlaceLS])
      g_pszFileNames[iPlaceLS][0] = 0;

    return res;
  }

  ResetCheckSum();

  do  // once
  {
    // ������ Map
    if ((res = f_writeCks(pFile, szBlockMap, ArraySize(szBlockMap))) != FR_OK ||
        (res = f_writeCks(pFile, &map, sizeof(map))) != FR_OK ||
        (res = f_writeCks(pFile, szBlockGenParams, ArraySize(szBlockGenParams))) != FR_OK ||
        (res = f_writeCks(pFile, &genParamsInfo, sizeof(struct SBlockInfo))) != FR_OK ||
        (res = f_writeCks(pFile, szBlockSupParams, ArraySize(szBlockSupParams))) != FR_OK ||
        (res = f_writeCks(pFile, &supParamsInfo, sizeof(struct SBlockInfo))) != FR_OK ||
        (res = f_writeCks(pFile, szBlockFixParams, ArraySize(szBlockFixParams))) != FR_OK ||
        (res = f_writeCks(pFile, &fixParamsInfo, sizeof(struct SBlockInfo))) != FR_OK)
      break;

    if(NumEventNow)
    {
      if ((res = f_writeCks(pFile, szBlockKeyEvents, ArraySize(szBlockKeyEvents))) != FR_OK ||
          (res = f_writeCks(pFile, &keyEventsInfo, sizeof(struct SBlockInfo))) != FR_OK)
        break;
    }
#if 0   // �� ���� �� ����������
    if ((res = f_writeCks(pFile, szBlockLinkParams, ArraySize(szBlockLinkParams))) != FR_OK ||
        (res = f_writeCks(pFile, &linkParamsInfo, sizeof(struct SBlockInfo))) != FR_OK)
       break;
#endif
    if ((res = f_writeCks(pFile, szBlockDataPts, ArraySize(szBlockDataPts))) != FR_OK ||
        (res = f_writeCks(pFile, &dataPtsInfo, sizeof(struct SBlockInfo))) != FR_OK)
      break;

//    if(comServ01Info.size)
//    {
//      if((res = f_writeCks(pFile, szBlockComServ01, ArraySize(szBlockComServ01))) != FR_OK ||
//         (res = f_writeCks(pFile, &comServ01Info, sizeof(SBlockInfo))) != FR_OK)
//        break;
//    }

//    if(comServ02Info.size)
//    {
//      if((res = f_writeCks(pFile, szBlockComServ02, ArraySize(szBlockComServ02))) != FR_OK ||
//         (res = f_writeCks(pFile, &comServ02Info, sizeof(SBlockInfo))) != FR_OK)
//        break;
//    }

    if((res = f_writeCks(pFile, szBlockCksum, ArraySize(szBlockCksum))) != FR_OK ||
      (res = f_writeCks(pFile, &chksumInfo, sizeof(struct SBlockInfo))) != FR_OK)
      break;

    // ������ GenParams
    if ((res = f_writeCks(pFile, szBlockGenParams, sizeof(szBlockGenParams))) != FR_OK ||
        (res = f_writeCks(pFile, &genParams.achLanguageCode, sizeof(genParams.achLanguageCode))) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszCableId, strlen(genParams.pszCableId) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszFiberId, strlen(genParams.pszFiberId) + 1)) != FR_OK ||
        (res = f_writeCks(pFile,
                          &genParams.sFiberType,
                          sizeof(genParams.sFiberType) +
                            sizeof(genParams.sNominalWavelength_nm))) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszOriginatingLocation, strlen(genParams.pszOriginatingLocation) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszTerminatingLocation, strlen(genParams.pszTerminatingLocation) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszCableCode, strlen(genParams.pszCableCode) + 1)) != FR_OK ||
        (res = f_writeCks(pFile,
                          &genParams.achCurrentDataFlag,
                          sizeof(genParams.achCurrentDataFlag) +
                            sizeof(genParams.lUserOffset_100ps) +
                              sizeof(genParams.ulUserOffsetDistance))) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszOperator, strlen(genParams.pszOperator) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, genParams.pszComment, strlen(genParams.pszComment) + 1)) != FR_OK)
      break;

    // ������ SupParams
    if ((res = f_writeCks(pFile, szBlockSupParams, sizeof(szBlockSupParams))) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszSupplierName, strlen(supParams.pszSupplierName) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOTDRMainframeID, strlen(supParams.pszOTDRMainframeID) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOTDRMainframeSN, strlen(supParams.pszOTDRMainframeSN) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOpticalModuleID, strlen(supParams.pszOpticalModuleID) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOpticalModuleSN, strlen(supParams.pszOpticalModuleSN) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszSoftwareRevision, strlen(supParams.pszSoftwareRevision) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOther, strlen(supParams.pszOther) + 1)) != FR_OK)
      break;

    // ������ FxdParams
    // �������� �����
    //fixParams.ulDateTimeStamp = g_StampTimeSave[iPlaceLS];
    fixParams.ulDateTimeStamp = TotalSec (TimeSaveOTDR);    // # DTS
    // �������� �������� ���������� ��� ������ ����� �����
    //fixParams.lNumberOfAverages = g_NumAveragePoints[iPlaceLS];
    fixParams.lNumberOfAverages = SettingRefl.NumAvrag; //NAV 

    if ((res = f_writeCks(pFile, szBlockFixParams, sizeof(szBlockFixParams))) != FR_OK ||
        (res = f_writeCks(pFile,
                         &fixParams.ulDateTimeStamp,
                         sizeof(fixParams.ulDateTimeStamp) +
                           sizeof(fixParams.achUnitsOfDistance) +
                           sizeof(fixParams.sActualWavelength_01nm) +
                           sizeof(fixParams.lAcquisitionOffset_100ps) +
                           sizeof(fixParams.ulAcquisitionOffsetDistance) +
                           sizeof(fixParams.sTotalNumberOfPulseWidthUsed))) != FR_OK ||
          // �������
          (res = f_writeCks(pFile,
                            &fixParams.sPulseWidthUsed_ns,
                            fixParams.sTotalNumberOfPulseWidthUsed * sizeof(fixParams.sPulseWidthUsed_ns))) != FR_OK ||
            (res = f_writeCks(pFile,
                              &fixParams.lDataSpacing_100ps,
                              fixParams.sTotalNumberOfPulseWidthUsed * sizeof(fixParams.lDataSpacing_100ps))) != FR_OK ||
              (res = f_writeCks(pFile,
                                &fixParams.lNumberOfDataPoints,
                                fixParams.sTotalNumberOfPulseWidthUsed * sizeof(fixParams.lNumberOfDataPoints))) != FR_OK ||
                (res = f_writeCks(pFile,
                                  &fixParams.lGroupIndex,
                                  sizeof(fixParams.lGroupIndex) +
                                    sizeof(fixParams.sBackscatterCoefficient_dB) +
                                    sizeof(fixParams.lNumberOfAverages) +
                                    sizeof(fixParams.usAveragingTime_01sec) +
                                    sizeof(fixParams.lAcquisitionRange_100ps) +
                                    sizeof(fixParams.lAcquisitionRangeDistance) +
                                    sizeof(fixParams.lFrontPanelOffset_100ps) +
                                    sizeof(fixParams.usNoiseFloorLevel_dB) +
                                    sizeof(fixParams.sNoiseFloorScaleFactor) +
                                    sizeof(fixParams.usPowerOffsetFirstPoint_dB) +
                                    sizeof(fixParams.usLossThreshold_dB) +
                                    sizeof(fixParams.usReflectanceThreshold_dB) +
                                    sizeof(fixParams.usEndOfFiberThreshold_dB) +
                                    sizeof(fixParams.achTraceType) +
                                    4 * sizeof(fixParams.lWinUpperLeftX))) != FR_OK)

    break;

    // ������ KeyEvents, ���� ������� �������
    if(NumEventNow)
    {
      if ((res = f_writeCks(pFile, szBlockKeyEvents, sizeof(szBlockKeyEvents))) != FR_OK ||
          (res = f_writeCks(pFile, &NumEventNow, sizeof(NumEventNow))) != FR_OK)
        break;

      // Note: ������� ������ ������� ��������� � sd-ram, ������� ����� � ����������
      //       ���������� ��������� � ���������� � 4 �����. ��������� ����������� �
      //       ����� ������ ��� "�����", � �����, ������ ������ �������.

      for(i = 0; i < NumEventNow; ++i)
        //if((res = WriteEvent(pFile, keyEvents.aEvent[i])) != FR_OK)
        if((res = f_writeCks(pFile, &EvenTrace[i], 52)) != FR_OK)
           break;

      if(res != FR_OK)
        break;

      // ������ ������ �� �����
      //if ((res = WriteLossesOfLink(pFile, keyEvents)) != FR_OK)
      if ((res = f_writeCks(pFile, &EndEvenBlk, 22)) != FR_OK)
         break;
    }

    // ������ DataPts
    if ((res = f_writeCks(pFile, szBlockDataPts, sizeof(szBlockDataPts))) != FR_OK ||
        (res = f_writeCks(pFile,
                          &dataPts.lNumPoints,
                          sizeof(dataPts.lNumPoints) +
                            sizeof(dataPts.sNumScaleFactors) +
                            sizeof(dataPts.lNumPointsUsingScaleFactor) +
                            sizeof(dataPts.sScaleFactor))) != FR_OK ||
        (res = f_writeCks(pFile, dataPts.pDataPoints, dataPts.lNumPoints * sizeof(*dataPts.pDataPoints))) != FR_OK)
      break;

//    if(comServ01Info.size)
//    {
//      if((res = f_writeCks(pFile, szBlockComServ01, sizeof(szBlockComServ01))) != FR_OK ||
//         (res = f_writeCks(pFile, &comServ01, sizeof(comServ01))) != FR_OK)
//        break;
//    }
//
//    if(comServ02Info.size)
//    {
//      if((res = f_writeCks(pFile, szBlockComServ02, sizeof(szBlockComServ02))) != FR_OK ||
//         (res = f_writeCks(pFile, &comServ02, sizeof(comServ02))) != FR_OK)
//        break;
//
//      if(g_eventSearch.launchCord.sUsed)
//      {
//        if((res = WriteEvent(pFile, g_lineWithCords[iPlaceLS].launchEvent)) != FR_OK)
//           break;
//      }
//
//      if(g_eventSearch.tailCord.sUsed)
//      {
//        if((res = WriteEvent(pFile, g_lineWithCords[iPlaceLS].tailEvent)) != FR_OK)
//           break;
//      }
//
//      if((res = WriteLossesOfLink(pFile, g_lineWithCords[iPlaceLS])) != FR_OK)
//         break;
//    }

    // ������ Cksum
    UINT  cBytesToW;
    UINT  cBytesW;
    //int   iDrive = GetCurrentDrive();

#if 0  // �� ������ �������� ������ �������� s_chkSum
    if ((res = f_writeCks(pFile, szBlockCksum, sizeof(szBlockCksum))) != FR_OK)
      break;

    cBytesToW = sizeof(s_chkSum);
    f_writeWTC(pFile, &s_chkSum, cBytesToW, &cBytesW);
#else
    // ������ �������� ������ � �������� s_chkSum
    RecalcCheckSum(szBlockCksum, sizeof(szBlockCksum));
    memcpy(g_szBuffer, szBlockCksum, sizeof(szBlockCksum));
    *((uint16_t*)(g_szBuffer + sizeof(szBlockCksum))) = s_chkSum;
    cBytesToW = sizeof(szBlockCksum) + sizeof(s_chkSum);
    f_writeWTC(pFile, g_szBuffer, cBytesToW, &cBytesW);
#endif

    f_syncWTC(pFile);

    if(res != FR_OK || cBytesW < cBytesToW)
    {
      res = FR_DISK_ERR;
      myBeep(200);
    }
  } while(0);

  f_closeWTC(pFile);
  //osDelay(30);

  if(res == FR_DISK_ERR)
  {
    if(g_pszFileNames[iPlaceLS])
      g_pszFileNames[iPlaceLS][0] = 0;

    f_unlinkWTC(pszFileName);   // ������� ������� ���� ���� ����� ���������
  }
  else if(res == FR_OK && g_pszFileNames[iPlaceLS] && pszFileName != g_pszFileNames[iPlaceLS])
    strcpy(g_pszFileNames[iPlaceLS], pszFileName);

  return res;
};

//
// ������ �� ���� � ���������� ����������� �����
//
// pFile        ��������� �� FIL-���������
// buff         ��������� �� ���� ������ ��� ������
// cBytesToR    ���������� ������ ��� ������
//
static FRESULT f_readCks(FIL* pFile, void* buff, UINT cBytesToR)
{
  UINT    cBytesR;  // ���������� ����������� ������
  FRESULT res;

  res = f_readWTC(pFile, buff, cBytesToR, &cBytesR);

  if(res != FR_OK || cBytesR != cBytesToR)
  {
    res = FR_DISK_ERR;
    myBeep(200);
  }
  else
    RecalcCheckSum(buff, cBytesR);

  return res;
}

//
// ������ �� ����� ����� ��������� ����� ��� ���������� ����������� �����
//
// pFile        ��������� �� FIL-���������
// cBytesToR    ���������� ������ ��� ������
//
static FRESULT f_readDummyCks(FIL* pFile, UINT cBytesToR)
{
  const UINT cuBlockSize = 256;       // ������������ ������ ����� ��� ������

  UINT    cBytesLeftToR = cBytesToR;  // ���������� ������, ������� �������� ���������
  FRESULT res = FR_OK;

  while(cBytesLeftToR)
  {
    //min(cBytesLeftToR, cuBlockSize);
    cBytesToR = cBytesLeftToR < cuBlockSize ? cBytesLeftToR : cuBlockSize;
    res = f_readCks(pFile, tempBuffer, cBytesToR);

    if(res != FR_OK)
      break;

//    RecalcCheckSum(tempBuffer, cBytesToR);	// ������� ����� ������� FeatEvents&PDF � master (�� ������ ������...)
    cBytesLeftToR -= cBytesToR;
  }

  return res;
}
// �������� ���� ������!!!


// ������ ������ ������ �������
// � ��� ����� ������ � ������� ������ ������ ������ ��� ����������
// �.�. �� ������������� ������� �� ������ ��������������
//int ReadEvent(FIL* pFile, SKeyEvent &rEvent)
//{
//  UINT  cBytesToR;      // ���-�� ������ ��� ������
//  int   iRes = OTDR_OKAY;
//
//  do  // once
//  {
//    cBytesToR = sizeof(rEvent.sEventNumber);
//    if(f_readCks(pFile, &rEvent.sEventNumber, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.lEventPropagationTime);
//    if(f_readCks(pFile, &rEvent.lEventPropagationTime, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.sAttenuationCoefficient_dBkm);
//    if(f_readCks(pFile, &rEvent.sAttenuationCoefficient_dBkm, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.sEventLoss_dB);
//    if(f_readCks(pFile, &rEvent.sEventLoss_dB, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.lEventReflectance_dB);
//    if(f_readCks(pFile, &rEvent.lEventReflectance_dB, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.chsEventCode);
//    if(f_readCks(pFile, &rEvent.chsEventCode, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.chsLossMeasurementTechnique);
//    if(f_readCks(pFile, &rEvent.chsLossMeasurementTechnique, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    cBytesToR = sizeof(rEvent.lMarkerLocation1);
//    if(f_readCks(pFile, &rEvent.lMarkerLocation1, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rEvent.lMarkerLocation2, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rEvent.lMarkerLocation3, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rEvent.lMarkerLocation4, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rEvent.lMarkerLocation5, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//
//    if(!f_getsz(rEvent.szComment, ArraySize(rEvent.szComment), pFile))
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//  } while(0);
//
//  return iRes;
//}
//
//// ������ ��������� ������ ������
//int ReadLossesOfLink(FIL* pFile, SLossesOfLink& rLosses)
//{
//  UINT  cBytesToR;      // ���-�� ������ ��� ������
//  int   iRes = OTDR_OKAY;
//
//  do  // once
//  {
//    cBytesToR = sizeof(rLosses.lEndToEndLoss);
//    if(f_readCks(pFile, &rLosses.lEndToEndLoss, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    cBytesToR = sizeof(rLosses.lEndToEndMarkerPos1);
//    if(f_readCks(pFile, &rLosses.lEndToEndMarkerPos1, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rLosses.lEndToEndMarkerPos2, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    cBytesToR = sizeof(rLosses.uOpticalReturnLoss);
//    if(f_readCks(pFile, &rLosses.uOpticalReturnLoss, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    cBytesToR = sizeof(rLosses.lReturnLossMarkerPos1);
//    if(f_readCks(pFile, &rLosses.lReturnLossMarkerPos1, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//    if(f_readCks(pFile, &rLosses.lReturnLossMarkerPos2, cBytesToR) != FR_OK)
//    {
//      iRes = OTDR_FILE_READ_ERROR;
//      break;
//    }
//  } while(0);
//
//  return iRes;
//}

//
// ������ Sor-�����
//
// pszFileName  ��� �����, ������� logical drive "0:" ��� "1:"
// pDataPoints  ��������� �� ������ ����� ���������
// nPointsMax   ������������ ���������� ����� � �������
//
int ReadSorFile(const char* pszFileName, uint16_t* pDataPoints, int32_t nPointsMax)
{
  extern int16_t  g_aCurrEvent[LSPLACENUM];
  //SKeyEvents  &keyEvents = g_keyEvents[0];
  FIL         file;           // ��������� ������ �����
  FIL*        pFile = &file;  // ��������� ��� ���������
  UINT        cBytesToR;      // ���-�� ������ ��� ������
  int         iRes = OTDR_OKAY;
  FRESULT     fRes;

  //keyEvents.sNumberOfEvents = 0;
  //g_aCurrEvent[0] = 0;

  do  // Once
  {
    if(f_openWTC(pFile, pszFileName, FA_OPEN_EXISTING | FA_READ) != FR_OK)
    {
      pFile = 0;
      iRes = OTDR_FILE_OPEN_ERROR;
      break;
    }

    ResetCheckSum();

    // �������� ������
    cBytesToR = sizeof(szBlockMap);
    // ��������� ��� ����
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ������� ��� ������ ����� "Map"
    if(memcmp(g_szBuffer, szBlockMap, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;  // It is not sor-file
      break;
    }

    cBytesToR = sizeof(map);
    // ��������� ���������� ��� ����� (1)
    if(f_readCks(pFile, &map, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ��������� ����������, ��������� ������
    if(map.info.revision < 200 || 299 < map.info.revision)
    {
      iRes = OTDR_FILE_SYNTAX_ERROR;  // Unknown version of sor-file
      break;
    }
    // � ����� ������ ����� ������ ����������, �� ���� 
    // 7 -���� ������� � 6- ��� ������� � ������ Map 
    // ������ ���� GenParams - 
    cBytesToR = sizeof(szBlockGenParams);
    // ��������� �������� ����� "GenParams" (2 ����)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ���������� ��������
    if(memcmp(g_szBuffer, szBlockGenParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;  // It is not sor-file
      break;
    }
    // 
    cBytesToR = sizeof(struct SBlockInfo);
    // ��������� ���� ���� - ������ � ������ (2)
    if(f_readCks(pFile, &genParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
     // ����� �� �������� ������ ����� GenParams ��� ������� "������"?
    cBytesToR = sizeof(szBlockSupParams);
    
    // ��������� �������� ����� "SupParams" (3 ����)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ���������� ��������
    if(memcmp(g_szBuffer, szBlockSupParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    cBytesToR = sizeof(struct SBlockInfo);
    // ��������� ���� ���� - ������ � ������ (3)
    if(f_readCks(pFile, &supParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
     // ����� �� �������� ������ ����� GenParams ��� ������� "������"?

    cBytesToR = sizeof(szBlockFixParams);
    // ��������� �������� ����� "FixParams" (4 ����)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ���������� ��������
    if(memcmp(g_szBuffer, szBlockFixParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    cBytesToR = sizeof(struct SBlockInfo);
    // ��������� ���� ���� - ������ � ������ (4)
    if(f_readCks(pFile, &fixParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ����� ������ ��������� 256 ����, � �����
    // ��� ���� �� �������� 0 ()
    // ���� ��������� 0 �� ������� � ������ ������ ������� ������
    // � �� �������� � �� ��� 256, ���� ���� ��� ������ ����...
    // ������ ��������
    // �������������� ����, ����� ������� DataPts
    if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ��������� �� ��������� �� ��� ����� "KeyEvents"
    if(memcmp(g_szBuffer, szBlockKeyEvents, sizeof(szBlockKeyEvents)))
    {
      // ��� �����-�� ������ ����; ���� ����������, �����
      keyEventsInfo.revision = DEF_REVISION;
      keyEventsInfo.size = 0;
    }
    else
    { // ��� ���������� ����� "KeyEvents" (���� 5 �� 7)
      cBytesToR = sizeof(struct SBlockInfo);
    // ��������� ���� ���� - ������ � ������ 
      if(f_readCks(pFile, &keyEventsInfo, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
      // ����� ��������� ���� �� ���������� ����(0) ,
      // ������ ����� ���������� �����
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }
    // � ���� ���� �������� ��� ��� ��� �� ������ ���� � ����� ������
    // ����� ������ ��� ���������...
    // ��������� ��� ��� 
    // �������������� ����  "LnkParams";   // ���� �� ����������
    if(memcmp(g_szBuffer, szBlockLinkParams, sizeof(szBlockLinkParams)))
    {
      // ��� �����-�� ������ ����; ���� ����������, �����
      linkParamsInfo.revision = DEF_REVISION;
      linkParamsInfo.size = 0;
    }
    else
    { // ���� ��� "LnkParams" ��������� ���
      cBytesToR = sizeof(struct SBlockInfo);

      if(f_readCks(pFile, &linkParamsInfo, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      // ������ ����� ���������� �����
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // �������������� ����, ����� ������� KeyEvents
    if(memcmp(g_szBuffer, szBlockDataPts, sizeof(szBlockDataPts)))
    {
      if(keyEventsInfo.size == 0)    // ����� KeyEvents �� ����?
      {
        iRes = OTDR_FILE_STRUCT_ERROR;
        break;
      }

      // ��� �����-�� ������ ����; ���� ����������, �����
      dataPtsInfo.revision = DEF_REVISION;
      dataPtsInfo.size = 0;
    }
    else
    {
      cBytesToR = sizeof(struct SBlockInfo);

      if(f_readCks(pFile, &dataPtsInfo, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      // ������ ����� ���������� �����
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // ����� BlockCksum ����� �������������� �������������� �����, �������� ��������������
    // ------------------------------------------------------------------
//
//    // �������������� ���� ComServ01
//    if(memcmp(g_szBuffer, szBlockComServ01, sizeof(szBlockComServ01)))
//    {
//      // ��� �����-�� ������ ����; ���� ����������, �����
//      comServ01Info.revision = DEF_REVISION;
//      comServ01Info.size = 0;
//    }
//    else
//    {
//      cBytesToR = sizeof(SBlockInfo);
//
//      if(f_readCks(pFile, &comServ01Info, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      // ������ ����� ���������� �����
//      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//    }
//
//    // �������������� ���� ComServ02
//    if(memcmp(g_szBuffer, szBlockComServ02, sizeof(szBlockComServ02)))
//    {
//      // ��� �����-�� ������ ����; ���� ����������, �����
//      comServ02Info.revision = DEF_REVISION;
//      comServ02Info.size = 0;
//    }
//    else
//    {
//      cBytesToR = sizeof(SBlockInfo);
//
//      if(f_readCks(pFile, &comServ02Info, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      // ������ ����� ���������� �����
//      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//    }

    // ���-���, ���������� ����������� ������������� ������
    s_szFirstBlockUserDef[0] = 0;
    s_uUserDefBlocksSize = 0;

    while(1)
    {
      if(memcmp(g_szBuffer, szBlockCksum, sizeof(szBlockCksum)) == 0)
      {
        cBytesToR = sizeof(struct SBlockInfo);

        if(f_readCks(pFile, &chksumInfo, cBytesToR) != FR_OK)
        {
          iRes = OTDR_FILE_READ_ERROR;
          break;
        }

        break;
      }
      else
      {
        if(s_uUserDefBlocksSize == 0)
          strcpy(s_szFirstBlockUserDef, g_szBuffer);

        cBytesToR = sizeof(struct SBlockInfo);

        if(f_readCks(pFile, &userDefInfo, cBytesToR) != FR_OK)
        {
          iRes = OTDR_FILE_READ_ERROR;
          break;
        }

        s_uUserDefBlocksSize += userDefInfo.size;

        // ������ ����� ���������� �����
        if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
        {
          iRes = OTDR_FILE_READ_ERROR;
          break;
        }
      }
    }
    // ����� �����-�� ������...
    if(iRes != OTDR_OKAY)
      break;
    // �������� ����� ��� �����, ��� ������ ���� ����� ������� ����������� �������
    DWORD dwFilePos = f_tell(pFile);
    // ���� ��������� ������ ��� ����������� ������ ����� Map, �� ������ ������ ����������
    //�� ������� � ��������� ����������� �����. 
    if(dwFilePos < map.info.size)
    {
      if(f_readDummyCks(pFile, (UINT)(map.info.size - dwFilePos)) != FR_OK)  // ������ ��� �������� CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }
    // ��������� ��������� GenParams
    cBytesToR = sizeof(szBlockGenParams);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ��������� ��� �� ����������
    if(memcmp(g_szBuffer, szBlockGenParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }
    // ����� ����� ��������� ��� ���������� �� ����� ��� ������� � "�������" ���� ����
    cBytesToR = sizeof(genParams.achLanguageCode);
    // ��������� LC
    if(f_readCks(pFile, genParams.achLanguageCode, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // ��������� �� �����, � ���. ����� ��������������� ������������ ���������� �����
    char* pchBuffer = (char*)tempBuffer;

    *pchBuffer = 0;
    genParams.pszCableId = pchBuffer;
    // CID
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszFiberId = pchBuffer;
    //FID
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    cBytesToR = sizeof(genParams.sFiberType) + sizeof(genParams.sNominalWavelength_nm);
    // ��������� ��� Short Int FT, NW
    if(f_readCks(pFile, &genParams.sFiberType, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszOriginatingLocation = pchBuffer;
    // get OL
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszTerminatingLocation = pchBuffer;
    // get TL
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszCableCode = pchBuffer;
    // get CCD
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ���������������� � ���������� 10 ���� = 2+4+4
    cBytesToR =
      sizeof(genParams.achCurrentDataFlag) +
      sizeof(genParams.lUserOffset_100ps) +
      sizeof(genParams.ulUserOffsetDistance);
    // ��������� CDF, UO,  UOD
    if(f_readCks(pFile, &genParams.achCurrentDataFlag, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    
    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszOperator = pchBuffer;
     // get OP (string)
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    genParams.pszComment = pchBuffer;
    // get CMT - ����������� ������� ����� ����������!
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // ���� SupParams
    cBytesToR = sizeof(szBlockSupParams);
    
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ����������
    if(memcmp(g_szBuffer, szBlockSupParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszSupplierName = pchBuffer;
    // get SN
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszOTDRMainframeID = pchBuffer;
    // get MFID
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszOTDRMainframeSN = pchBuffer;
    // get OTDR
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszOpticalModuleID = pchBuffer;
    // get OMID
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszOpticalModuleSN = pchBuffer;
    // get OMSN
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszSoftwareRevision = pchBuffer;
    // get SR
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    pchBuffer += strlen(pchBuffer) + 1;
    *pchBuffer = 0;
    supParams.pszOther = pchBuffer;
    // get OT
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // ���� FixParams
    cBytesToR = sizeof(szBlockFixParams);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // ����������
    if(memcmp(g_szBuffer, szBlockFixParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }
    // �������������� ���������� ����� ������ 18 ����
    // DTS, UD, AW, AO, AOD, TPW
    cBytesToR =
      sizeof(fixParams.ulDateTimeStamp) +
      sizeof(fixParams.achUnitsOfDistance) +
      sizeof(fixParams.sActualWavelength_01nm) +
      sizeof(fixParams.lAcquisitionOffset_100ps) +
      sizeof(fixParams.ulAcquisitionOffsetDistance) +
      sizeof(fixParams.sTotalNumberOfPulseWidthUsed);
    // get: DTS, UD, AW, AO, AOD, TPW
    if(f_readCks(pFile, &fixParams.ulDateTimeStamp, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

#if 0
    float fDistFactor;  // ��������� ��� ��������� �������� ����������, [�], ��� ��������� �������� ���������

    if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'm')      // [��]
      fDistFactor = 1000.;
    else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 't') // [�]
      fDistFactor = 1.;
    else if(fixParams.achUnitsOfDistance[0] == 'f' && fixParams.achUnitsOfDistance[1] == 't') // [����]
      fDistFactor = 0.3048;
    else if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'f') // [�����]
      fDistFactor = 304.8;
    else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 'i') // [����]
      fDistFactor = 1609.344;
    else
    {
      iRes = OTDR_FILE_SYNTAX_ERROR;
      break;
    }

    // �������� ����� ������� ������������ � ���� ComServData02, � �������� �������� ������ ����� 0!
    // Note: �������� � sor ��������� � 1/10 ������ ���������
    g_eventSearch.launchCord.lLen_cm = (int32_t)(genParams.ulUserOffsetDistance * fDistFactor * 0.1f);
#endif

#if 0
    // �������
    cBytesToR = fixParams.sTotalNumberOfPulseWidthUsed * sizeof(*fixParams.psPulseWidthUsed_ns);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    cBytesToR = fixParams.sTotalNumberOfPulseWidthUsed * sizeof(*fixParams.lDataSpacing_ns);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    cBytesToR = fixParams.sTotalNumberOfPulseWidthUsed * sizeof(*fixParams.plNumberOfDataPoints);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
#else
    // �� ������ �������� ���������� ��� ������������ ������ ��������
    // fixParams.sTotalNumberOfPulseWidthUsed �.�. =1
    //PWU, DS, NPPW
    cBytesToR =
      sizeof(fixParams.sPulseWidthUsed_ns) +
      sizeof(fixParams.lDataSpacing_100ps) +
      sizeof(fixParams.lNumberOfDataPoints);
    // get PWU, DS, NPPW
    if(f_readCks(pFile, &fixParams.sPulseWidthUsed_ns, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
#endif
    
    //g_FileParamInfo.sPulseW = fixParams.sPulseWidthUsed_ns;
    
    // ��������� ���� FixParams
    // GI, BC, NAV, AT, AR, ARD, FPO, NF .. WCi
    cBytesToR =
      sizeof(fixParams.lGroupIndex) + //GI
      sizeof(fixParams.sBackscatterCoefficient_dB) + //BC
      sizeof(fixParams.lNumberOfAverages) +  //NAV
      sizeof(fixParams.usAveragingTime_01sec) +  //AT
      sizeof(fixParams.lAcquisitionRange_100ps) + //AR
      sizeof(fixParams.lAcquisitionRangeDistance) +  //ARD
      sizeof(fixParams.lFrontPanelOffset_100ps) +  //FPO
      sizeof(fixParams.usNoiseFloorLevel_dB) +  //NF
      sizeof(fixParams.sNoiseFloorScaleFactor) +  //NFSF
      sizeof(fixParams.usPowerOffsetFirstPoint_dB) + //PO
      sizeof(fixParams.usLossThreshold_dB) +   //LT
      sizeof(fixParams.usReflectanceThreshold_dB) +  //RT
      sizeof(fixParams.usEndOfFiberThreshold_dB) +   //ET
      sizeof(fixParams.achTraceType) +  //TT
      4 * sizeof(fixParams.lWinUpperLeftX); //WC1, WC2, WC3, vWC4

    if(f_readCks(pFile, &fixParams.lGroupIndex, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // ���� KeyEvents (����� �������������)
    if(keyEventsInfo.size)
    {
            if(f_readDummyCks(pFile, keyEventsInfo.size) != FR_OK)  // ��������� CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

//      // ������ g_keyEvents � aKeyEvent[iPlaceLS][i], ���� ������� �������
//      cBytesToR = sizeof(szBlockKeyEvents);
//
//      if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      if(memcmp(g_szBuffer, szBlockKeyEvents, cBytesToR))
//      {
//        iRes = OTDR_FILE_STRUCT_ERROR;
//        break;
//      }
//      // ��������� ����� ��������
//      cBytesToR = sizeof(keyEvents.sNumberOfEvents);
//
//      if(f_readCks(pFile, &keyEvents.sNumberOfEvents, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      for(int16_t i = 0; i < keyEvents.sNumberOfEvents; ++i)
//      {
//        if((iRes = ReadEvent(pFile, keyEvents.aEvent[i])) != OTDR_OKAY)
//          break;
//      }
//
//      if(iRes != OTDR_OKAY)
//        break;
//
//      if((iRes = ReadLossesOfLink(pFile, keyEvents)) != OTDR_OKAY)
//        break;
    }

    if(linkParamsInfo.size)
    {
      if(f_readDummyCks(pFile, linkParamsInfo.size) != FR_OK)  // ��������� CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // ���� DataPts
    if(dataPtsInfo.size)
    {
      cBytesToR = sizeof(szBlockDataPts);

      if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      if(memcmp(g_szBuffer, szBlockDataPts, cBytesToR))
      {
        iRes = OTDR_FILE_STRUCT_ERROR;
        break;
      }

      // �������� ������ ���-�� ����� � �������������� ����-�
      cBytesToR = sizeof(dataPts.lNumPoints) + sizeof(dataPts.sNumScaleFactors);

      if(f_readCks(pFile, &dataPts.lNumPoints, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      // �������� ��� ������ ������ ������ (���-�� ����� � �������������� ����-�)
      cBytesToR =
        sizeof(dataPts.lNumPointsUsingScaleFactor) + sizeof(dataPts.sScaleFactor);

      if(f_readCks(pFile, &dataPts.lNumPointsUsingScaleFactor, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      dataPts.pDataPoints = pDataPoints; // �������� �� ��� ������

      if(dataPts.lNumPointsUsingScaleFactor > nPointsMax) // �������� �� �������� ���������� ������� ���������� ������
      {
        iRes = OTDR_OUT_OF_MEMORY;
        break;
      }

      cBytesToR = dataPts.lNumPointsUsingScaleFactor * sizeof(*dataPts.pDataPoints);
      // ���������� ������ � ��� ������ (LogData)
      if(f_readCks(pFile, dataPts.pDataPoints, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }
    else if(!keyEventsInfo.size)
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

// ��������� ��������� �� �������������...
//    if(comServ01Info.size)
//    {
//      cBytesToR = sizeof(szBlockComServ01);
//
//      if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      if(memcmp(g_szBuffer, szBlockComServ01, cBytesToR))
//      {
//        iRes = OTDR_FILE_STRUCT_ERROR;
//        break;
//      }
//
//      cBytesToR = sizeof(comServ01);
//
//      if(f_readCks(pFile, &comServ01, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//    }
//
//    keyEvents.fPrimaryLenOfLink = 0.;
//
//    SLineWithCords& lineWithCords = g_lineWithCords[0];
//    SKeyEvent&      launchEvent = lineWithCords.launchEvent;
//    SKeyEvent&      tailEvent = lineWithCords.tailEvent;
//
//    if(comServ02Info.size)
//    {
//      cBytesToR = sizeof(szBlockComServ02);
//
//      if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      if(memcmp(g_szBuffer, szBlockComServ02, cBytesToR))
//      {
//        iRes = OTDR_FILE_STRUCT_ERROR;
//        break;
//      }
//
//      cBytesToR = sizeof(comServ02);
//
//      if(f_readCks(pFile, &comServ02, cBytesToR) != FR_OK)
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//
//      if(keyEvents.sNumberOfEvents)
//        keyEvents.fPrimaryLenOfLink = comServ02.lPrimaryLenOfLink_cm * 0.01;
//
//      if(comServ02.lLaunchCordLen_cm)
//      {
//        if((iRes = ReadEvent(pFile, lineWithCords.launchEvent)) != OTDR_OKAY)
//          break;
//      }
//      else
//      {
//        // "��������" ���������� ���������
//        memset(&launchEvent, 0, sizeof(launchEvent));
//        launchEvent.szComment[0] = ' '; // ������������ ������ ��-��������� " \0"
//      }
//
//      if(comServ02.lTailCordLen_cm)
//      {
//        if((iRes = ReadEvent(pFile, lineWithCords.tailEvent)) != OTDR_OKAY)
//          break;
//      }
//      else
//      {
//        // "��������" ���������� ���������
//        memset(&tailEvent, 0, sizeof(tailEvent));
//        tailEvent.szComment[0] = ' '; // ������������ ������ ��-��������� " \0"
//      }
//
//      if((iRes = ReadLossesOfLink(pFile, lineWithCords)) != OTDR_OKAY)
//        break;
//    }
//    else
//    {
//      // ��������� ����������� �������� ������ �������-������ �������
//      memset(&launchEvent, 0, sizeof(launchEvent));
//      launchEvent.szComment[0] = ' ';
//      memset(&tailEvent, 0, sizeof(tailEvent));
//      tailEvent.szComment[0] = ' ';
//
//      // Note: ������������ ����������� �������������� ���������� � ������������ �� ������� �����
//      memset(static_cast<SLossesOfLink*>(&lineWithCords), 0, sizeof(SLossesOfLink));
//    }

    if(s_uUserDefBlocksSize)
    {
      // ������� �����, �������� �������������
      cBytesToR = strlen(s_szFirstBlockUserDef) + 1;

      // ����������� ������������ ������ ������� �� ������, �������� �������������
      if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      if(memcmp(g_szBuffer, s_szFirstBlockUserDef, cBytesToR))
      {
        iRes = OTDR_FILE_STRUCT_ERROR;
        break;
      }

      // ����� ������ ����� ��������� ����� s_uUserDefBlocksSize
      if(f_readDummyCks(pFile, s_uUserDefBlocksSize - cBytesToR) != FR_OK) // ��������� CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // ���� Cksum
    cBytesToR = sizeof(szBlockCksum);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    if(memcmp(g_szBuffer, szBlockCksum, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    uint16_t   uChkSum;  // ����������� �� ����� �������� CRC
    UINT  cBytesR;

    cBytesToR = sizeof(uChkSum);

    // ������ ����� ����������� ����� (��� � ���������)
    fRes = f_readWTC(pFile, &uChkSum, cBytesToR, &cBytesR);

    if(fRes != FR_OK || cBytesToR != cBytesR)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    if(uChkSum != s_chkSum)   // ��������� � ���������� CRC
    {
      iRes = OTDR_FILE_BAD_CHECKSUM;  // Bad checksum
      break;
    }
  } while(0);


  if(pFile)
  {
    f_closeWTC(pFile);
 // ����� ������ ������ � ��������� ��� ������ �����,
    // �����-�� ����� ��� ��������� ������
    if(iRes == OTDR_FILE_BAD_CHECKSUM) // ������ ����������� ����� 
    {
      // ����� ��������� � ������ ����������� ����� �� ������������ ������
      sprintf(MsgErrMem,"Bad CheckSum");
      //if(MessageBox(trBadCheckSum.get(), trWarning.get(), MB_YESNO) == IDYES)
        iRes = OTDR_OKAY;
    }

    if(iRes != OTDR_OKAY)
    {
      if(iRes != OTDR_FILE_BAD_CHECKSUM)   // �.�. �������������� � CRC ��� ����������
      sprintf(MsgErrMem,"Bad File Err=%d",iRes);
      // ����� ��� �����. ����� ��������� �� ������ �� ������������ � ���� �� ���������...
    }
    else
    {
      InitMemReflSet (); // ������������� ��������� ������������� ���������� �� �����
      // ���������� �������� ����������� �����, � ��������� �������� �������,
      // ���������� ������� ��������� ��� ������������� ����� ���������
      // ���� ������������ � ����� � ����������� Mem
      // ���� ����� ����, �������� �� �������� ������������ ����� ����� (��������) � �����...
      int32_t iDistance=0;
            if(fixParams.lAcquisitionRangeDistance > 0)
      {
        float fDistFactor;

        if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'm')      // [��]
          fDistFactor = 1000.;
        else if(fixParams.achUnitsOfDistance[0] == 'f' && fixParams.achUnitsOfDistance[1] == 't') // [����]
          fDistFactor = 0.3048;
        else if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'f') // [�����]
          fDistFactor = 304.8;
        else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 'i') // [����]
          fDistFactor = 1609.344;
        else // if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 't') // [�]
          fDistFactor = 1.;

        iDistance = (int32_t)(fixParams.lAcquisitionRangeDistance * fDistFactor * 0.1);
      }
      else
      {
        iDistance =
          (int32_t)((fixParams.lNumberOfDataPoints - 1) *
                ((double)LIGHTSPEED * fixParams.lDataSpacing_100ps) /
                  (fixParams.lGroupIndex * 1.e+9));
      }
      // �������� ��������� � ��������� � ��������� FixParams ��� ���������
      FixParams.ARD = iDistance; // � ������!!!
      MemSetRefl.Index_Ln = CalkIndexLN(iDistance); // ��������� ������ ����� ����� (��������) 
      FixParams.AW = genParams.sNominalWavelength_nm; // ����� �����
      MemSetRefl.SW_LW = CalkIndexSC(genParams.sNominalWavelength_nm); // ������ ����������� ����� �� ����� �����
      FixParams.PWU = fixParams.sPulseWidthUsed_ns; // ������������ ��������
      MemSetRefl.Index_Im = CalkIndexIM(fixParams.sPulseWidthUsed_ns); //  ������ ������������ ��������
      FixParams.AT = fixParams.usAveragingTime_01sec; // ����� ���������� � 0.1 ��������
      MemSetRefl.Index_Vrm = CalkIndexWRM(fixParams.usAveragingTime_01sec); //  ������ ������� ����������
      FixParams.GI = fixParams.lGroupIndex; // ������������ ����������� (�����������) 
      MemSetRefl.K_pr = (float)(fixParams.lGroupIndex/100000.); // �����. �����������
      
      MemReflParam.BC = fixParams.sBackscatterCoefficient_dB;
      MemReflParam.LT= fixParams.usLossThreshold_dB ; //LT
      MemReflParam.RT = fixParams.usReflectanceThreshold_dB;
      MemReflParam.ET = fixParams.usEndOfFiberThreshold_dB; //ET
      //g_FileParamInfo.iGroupIndex = fixParams.lGroupIndex;
      //g_FileParamInfo.sBackscatter = fixParams.sBackscatterCoefficient_dB;
//      g_FileParamInfo.iNumPoints = fixParams.lNumberOfDataPoints;
//      g_FileParamInfo.iNumAver = fixParams.lNumberOfAverages;
//      g_FileParamInfo.iDS = fixParams.lDataSpacing_100ps;
//      g_FileParamInfo.sPulseW = fixParams.sPulseWidthUsed_ns;
//      g_FileParamInfo.sWaveLengh = genParams.sNominalWavelength_nm;
//      g_FileParamInfo.uTimeStamp = fixParams.ulDateTimeStamp;
//      g_FileParamInfo.usAveragingTime = fixParams.usAveragingTime_01sec;
//      g_FileParamInfo.iAcquisitionOffset_100ps = fixParams.lAcquisitionOffset_100ps;
//      g_FileParamInfo.uAcquisitionOffsetDistance_10cm = fixParams.ulAcquisitionOffsetDistance;

  // �������� �����������
  memcpy(GenParams.CMT, genParams.pszComment, (strlen(genParams.pszComment)<19)?(strlen(genParams.pszComment)):(19));
  GenParams.CMT[19]=0;



    }
  }

  return iRes;
}

//
// ������ ������ �� SOR-����� � ��������� ������
//
// Note: ��� ���������� ������ ��� �������, � ����� ����� �����,
//       ������������ ��������������� ������ � ������� ��������.
int ReadSorFileG(const char* pszFileName)
{
  // ���� �� �������� ������ ��� ������ ��� �����?
  if(!g_pszFileNames[0])
    g_pszFileNames[0] = (char*)malloc(MAX_PATH_LEN);

  // ����� ����������� ������, �.�. pszFileName ����� ��������� � g_szBuffer, ���.
  // ������������ � ReadSorFile()
  if(g_pszFileNames[0] && pszFileName != g_pszFileNames[0])
    strcpy(g_pszFileNames[0], pszFileName);

  // ����� ������ ����� ����������� ����� �����!
  int iRes = ReadSorFile(g_pszFileNames[0], LogData, 5300);

  if(iRes == OTDR_OKAY)
  {
    // ����� �������� ������ 
//    float fDistStep =
//            (float)(((double)LIGHTSPEED*g_FileParamInfo.iDS) / (g_FileParamInfo.iGroupIndex*1.e+9));

  }
  else
  {
    if(g_pszFileNames[0])
      g_pszFileNames[0][0] = 0; // �������� ������
  }

  return iRes;
}


