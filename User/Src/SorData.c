// Работа с данными в "Bellcore" OTDR Data Format
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
//extern CLossGraph lossGraph;      // параметры графика потерь

char* g_pszFileNames[LSPLACENUM]; // массив указателей на строки с именами файлов

uint8_t tempBuffer[256];

#define DEF_REVISION        200
#define COM_SERV_REVISION   100

// использовать катушки
//#define USE_CORDS

//static int  g_NumVer =
//#include "lasti.h"
//;

#pragma pack(push, 1)   // требуется выравнивание полей структур данных

// с приостановкой/возобновлением других задач
// должно быть раскомментировано одно из двух первых определений, задающих единицу измерения расстояний
// !!! пока условная компилляция не используется !!!
#define OTDR_DIST_IN_M      // пока при сохранении только [м]!
//#define OTDR_DIST_IN_KM

// а эти определения не будут корректны
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
  uint16_t         cBlocks;        // количество блоков
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
  int32_t     lAcquisitionOffset_100ps;       // AO   <double,int32_t,10> По реальному файлу
  uint32_t     ulAcquisitionOffsetDistance;    // AOD  <double,uint32_t,10>
  int16_t     sTotalNumberOfPulseWidthUsed;   // TPW
  // следующие параметры д.б. массивами значений для каждого из TPW
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
  // Данные об окне просмотра интерпретируются как координаты относительно осей "точки" (Ох) и "децибелы" (Оу)
  int32_t     lWinUpperLeftX;
  int32_t     lWinUpperLeftY;
  int32_t     lWinLowerRightX;
  int32_t     lWinLowerRightY;
};

//#if 0
//// блок параметров сети (ориентиры) в начальной реализации использоваться не будет
//
//struct SLandmark    // параметры ориентира
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

//// пользовательский блок данных 01
//struct SComServData01
//{
//  int32_t   lDistToTheLeftMarker_100ps; // расстояние от начала рефлектограммы до левого маркера, 100 ps
//  int32_t   lDistToTheRightMarker_100ps;// расстояние от начала рефлектограммы до правого маркера, 100 ps
//  uint16_t   usApproxMethod;             // метод аппроксимации кривой между маркерами (0-2P, 1-LS)
//  uint16_t   usLossBetweenMarkres;       // потери на участке между маркерами, 0.001*дБ
//  uint16_t   usMeanSlopeBetweenMarkers;  // средний наклон на участке между маркерами, 0.001*дБ / км
//  int32_t   lMaxReflBetweenMarkers;     // максимальное дискретное отражение на участке между маркерами, 0.001*дБ
//  uint16_t   usORL_BetweenMarkers;       // ORL на участке между маркерами, 0.001*дБ
//  uint16_t   usDisplayFlag;              // флаг, указывающий, какой параметр отображать в отчёте и на экране (0-MaxReflection, 1-ORL)
//};
//
//// пользовательский блок данных 02
//struct SComServData02
//{
//  int32_t   lPrimaryLenOfLink_cm;       // длина линии, вычисленная при поиске событий, см
//
//  int32_t   lLaunchCordLen_cm;          // длина измерительной катушки в начале линии, см
//  int16_t   sAddBeginConnectionLoss;    // учитывать потери разъёма в начале в величине полных потерь
//
//  int32_t   lTailCordLen_cm;            // длина измерительной катушки в конце линии, см
//  int16_t   sAddEndConnectionLoss;      // учитывать потери разъёма в конце в величине полных потерь
//
//  // далее сохраняются/загружаются:
//  // 1) события, связанные с соединениями катушек
//  // SKeyEvent     launchEvent;
//  // SKeyEvent     tailEvent;
//  // 2) потери на линии, учитывающие наличие катушек
//  // SLossesOfLink lossOfLink;         // Note: используются g_lineWithCords[iPlaceLS]
//};
char szTest[100];

#pragma pack(pop)

static FRESULT f_readCks(FIL* pFile, void* pvBuffer, UINT cBytesToR);
static FRESULT f_readDummyCks(FIL* pFile, UINT cBytesToR);

// отметки времени измерения, сек
// данный массив должен заполняться значениями в моменты завершения (или начала?)
// измерения по соответствующим подключениям лазеров
//static uint32_t g_StampTimeSave[LSPLACENUM] = { 1420092001, 1420092002, 1420092003, 1420092004 };

// наименование блоков
const char szBlockMap[]        = "Map";
const char szBlockGenParams[]  = "GenParams";
const char szBlockSupParams[]  = "SupParams";
const char szBlockFixParams[]  = "FxdParams";
const char szBlockKeyEvents[]  = "KeyEvents";
const char szBlockLinkParams[] = "LnkParams";   // пока не используем
const char szBlockDataPts[]    = "DataPts";
//const char szBlockSpecPropr[]  = "UserName";    // default, но м.б. и другим
const char szBlockCksum[]      = "Cksum";
//const char szBlockComServ01[]  = "NPKSS01";     // RPCCS01 информация о наличии катушек, о положении маркеров и т.д.
//const char szBlockComServ02[]  = "NPKSS02";     // RPCCS02 данные событий-концов катушек и потерь на линии с их учётом

static struct SBlockMap   map;
static struct SBlockInfo  genParamsInfo;
static struct SBlockInfo  supParamsInfo;
static struct SBlockInfo  fixParamsInfo;
static struct SBlockInfo  keyEventsInfo;
static struct SBlockInfo  linkParamsInfo; // не используется
static struct SBlockInfo  dataPtsInfo;
//static SBlockInfo  specProprInfo;  // не используется
static struct SBlockInfo  userDefInfo;
static struct SBlockInfo  chksumInfo;
//static SBlockInfo  comServ01Info;
//static SBlockInfo  comServ02Info;

static char s_szDummy[] = " ";  // строка, соответствующая пустой

static char s_szFirstBlockUserDef[32];  // наименование первого из определённых пользователем блоков
static UINT s_uUserDefBlocksSize;       // суммарная длина определённых пользователем блоков

//extern "C" uint8_t* pKeyBuffer;

static struct SGeneralParameters  genParams;
static struct SSupplierParameters supParams;
static struct SFixedParameters    fixParams;
static struct SDataPts            dataPts;

//static SComServData01      comServ01;
//static SComServData02      comServ02;

static uint16_t          s_chkSum;
static unsigned short NumEventNow; //число событий считаем в prePareSor 
// строки для формирования ...
static char s_szSoftwareRevision[16]; // версиии ПО
static char s_szModelDevice[48];      // модели прибора
static char s_szOpticalModule[40];    // наименования оптического модуля
static char s_szFiberIDNum[48];       // полного названия кабеля с номером
static char s_szNumDevice[8];         // номера прибора

// Варианты функций с приостановкой/возобновлением выполнения задач при обращениях к внутренней SD-карте

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
  // Note: сразу определяем носитель, т.к. f_close() освобождает память по указателю fp->fs
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
// надо разрешить!!!  Потом
// портировано из f_gets()
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

// расчёт значений параметров между маркерами
//int CalcParamsBetweenMarkers(int iPlaceLS)
//{
//  if(!g_eventSearch.sUseMarkers)
//     return OTDR_NOTHING_TO_DO;
//
//  // расстояния до маркеров, м
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
//  int iGraphItem = 0; // индекс графика для заданной длины волны
//  int iWaveLengh = g_ConfigDevice.PlaceLS[iPlaceLS];
//
//  // поиск индекса
//  for(int iItem = 0; iItem < MAX_GRAPH_ITEM_COUNT; ++iItem)
//  {
//    if(lossGraph.GetItemUserData(iItem) == iWaveLengh)
//    {
//      iGraphItem = iItem;
//      break;
//    }
//  }
//
//  // уровни сигнала в точках под маркерами, дБ
//  float fValueA;
//  float fValueB;
//
//  if(g_eventSearch.sApproxType == OTDR_LS)
//  {
//#if 1
//    lossGraph.CalcLinearAtten(iGraphItem, fDistA_m, fDistB_m, &fValueA, &fValueB);
//#else
//    // наклон линии потерь, дБ/км
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
//  g_resBetweenMarkers.fORL = 0.;  // пока не умеем считать ORL
//#endif
//
//  return OTDR_OKAY;
//}

// подготовка данных для сохранения в Sor-формате
//
// iPlaceLS     номер посадочного места
// pDataPoints  указатель на массив точек измерений
//
static int PrepareSorData(int iPlaceLS, uint16_t* pDataPoints)
{
  //SKeyEvents& keyEvents = g_keyEvents[iPlaceLS]; // ссылка на пар-ры событий для места лазера
  float       fGroupIndex = GetIndexWAV();//GetGroupIndexForPlace(iPlaceLS);
  NumEventNow=MakeEvents();// = GetNumEvents(); число событий - если 
  
  
  map.cBlocks = 1;
  map.info.revision = DEF_REVISION;
  map.info.size = sizeof(szBlockMap) + sizeof(map);

  map.info.size += sizeof(szBlockGenParams) + sizeof(genParamsInfo);
  ++map.cBlocks;
  map.info.size += sizeof(szBlockSupParams) + sizeof(supParamsInfo);
  ++map.cBlocks;
  map.info.size += sizeof(szBlockFixParams) + sizeof(fixParamsInfo);
  ++map.cBlocks;

  if(NumEventNow) // если есть события
  {
    map.info.size += sizeof(szBlockKeyEvents) + sizeof(keyEventsInfo);
    ++map.cBlocks;
  }

  map.info.size += sizeof(szBlockDataPts) + sizeof(dataPtsInfo);
  ++map.cBlocks;

// выключил за ненадобностью 06.08.2025
//  // выполнялся поиск событий и имеются катушки?
//  bool  bComServ02IsRequired;
//
//  if(g_eventSearch.iSwitch == SWITCH_ON)
//    bComServ02IsRequired = (g_eventSearch.launchCord.sUsed || g_eventSearch.tailCord.sUsed);
//  else
//    bComServ02IsRequired = false;
//
//  // используются маркеры?
//  bool  bComServ01IsRequired = g_reportParam.bSaveMarkersInfo && g_eventSearch.sUseMarkers;
//
//  if(bComServ01IsRequired)
//  {
//    // будет блок ComServ01
//    map.info.size += sizeof(szBlockComServ01) + sizeof(comServ01Info);
//    ++map.cBlocks;
//  }
//
//  if(bComServ02IsRequired)
//  {
//    // будет блок ComServ02
//    map.info.size += sizeof(szBlockComServ02) + sizeof(comServ02Info);
//    ++map.cBlocks;
//  }

  map.info.size += sizeof(szBlockCksum) + sizeof(chksumInfo);
  ++map.cBlocks;
  //g_CabelFileInfo.NumFiber номер волокна, необходимо присовокупить через пробел к FiberId (замечание 47 от 30.01.2017)

//  if(g_ConfigDevice.ID_Device == 0)   // i.e., Топаз-9400
//  {
    genParams.achLanguageCode[0] = 'R';
    genParams.achLanguageCode[1] = 'U';
//  }
//  else  // MOT-940
//  {
//    genParams.achLanguageCode[0] = 'E';
//    genParams.achLanguageCode[1] = 'N';
//  }
  // заполняем таблицу GenParams - данными из Экспортных перемнных
  genParams.pszCableId = *GenParams.CID ? GenParams.CID : s_szDummy; // CID
  //genParams.pszFiberId = *g_CabelFileInfo.NameInfoFiber ? g_CabelFileInfo.NameInfoFiber : s_szDummy;
  sprintf(s_szFiberIDNum,"%s", *GenParams.FID ? GenParams.FID : s_szDummy); //FID
  sprintf(s_szFiberIDNum,"%s %d",s_szFiberIDNum,NameDB.FiberID);
  genParams.pszFiberId = s_szFiberIDNum;
  genParams.sFiberType = 652;                      // # ? 652 - default
  //GetLengthWaveLS (0)        // устанавливаем длину волны
  genParams.sNominalWavelength_nm =(unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT)); // получение длины волны от индекса установочного места
  // # ? связь с g_measConfig.iWaveLenIdx 1310 - def
  genParams.pszOriginatingLocation = *GenParams.OL ? GenParams.OL : s_szDummy;
  genParams.pszTerminatingLocation = *GenParams.TL ? GenParams.TL : s_szDummy;
  genParams.pszCableCode = s_szDummy;
  genParams.achCurrentDataFlag[0] = 'B';  genParams.achCurrentDataFlag[1] = 'C';  // # ? не устанавливается в GUI


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
    
    // Заполним структуру SupParams
    GetDeviceName( szTest ); // запрос сторки идентификатора
    supParams.pszSupplierName = *SupParams.SN ? SupParams.SN : s_szDummy; // производитель SN
  supParams.pszOTDRMainframeID = *SupParams.MFID ? SupParams.MFID : s_szDummy;
  supParams.pszOTDRMainframeSN =  *SupParams.OTDR ? SupParams.OTDR : s_szDummy;
  supParams.pszOpticalModuleID = *SupParams.OMID ? SupParams.OMID : s_szDummy;    // # ? (не используется в приборе) формируем оптический модуль
  supParams.pszOpticalModuleSN = s_szDummy;           // # ? (не используется в приборе) номер тот же, что и у прибора s_szNumDevice
  supParams.pszSoftwareRevision = *SupParams.SR ? SupParams.SR : s_szDummy; 
  supParams.pszOther = s_szDummy;   // # ?
  
  // было Supplier Params
//  //supParams.pszSupplierName = (char*)Ides[GetID_Dev()];//aszSupplierName[g_ConfigDevice.ID_Device];
//  // здесь надо сформировать полное имя прибора включая длины волн, типа Топаз-9400-А-31-55-85-30-PMH-VFL
//  //supParams.pszOTDRMainframeID = (char*)triDMainframeOTDR[g_ConfigDevice.ID_Device].get();//aszMainframeOTDR[g_ConfigDevice.ID_Device];
//    GetStringPlace(s_szSoftwareRevision, 0);
//  sprintf (s_szModelDevice, "%s-%c-%s%s%s", //-VFL убрали из надписей! (у чехов оставили)
//           triDMainframeOTDR[g_ConfigDevice.ID_Device].get(),
//           GetSmbDev(0),//(g_ConfigDevice.TypeOM)?("A"):("B"),
//           s_szSoftwareRevision,
//           (g_ConfigDevice.CfgPM)?((g_ConfigDevice.CfgPM-1)?("PMH"):("PM")):(""),
//           (g_ConfigDevice.ID_Device)?("-VFL"):(""));//
//  supParams.pszOTDRMainframeID = s_szModelDevice;
//  sprintf(s_szNumDevice, "%d", g_ConfigDevice.NumDevice);
//  supParams.pszOTDRMainframeSN = s_szNumDevice;
//  GetStringPlace(s_szSoftwareRevision, 1);
//  sprintf (s_szOpticalModule, "%c-%s%s-VFL", // Формируем оптический модуль (со слэшами)
//           GetSmbDev(0),//(g_ConfigDevice.TypeOM)?("A"):("B"),
//           s_szSoftwareRevision,
//           (g_ConfigDevice.CfgPM)?((g_ConfigDevice.CfgPM-1)?("PMH"):("PM")):(""));//
//  supParams.pszOpticalModuleID = s_szOpticalModule;   // # ? (не используется в приборе) формируем оптический модуль
//  supParams.pszOpticalModuleSN = s_szDummy;           // # ? (не используется в приборе) номер тот же, что и у прибора s_szNumDevice
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
 
// таблица Fixed Params   
  fixParams.ulDateTimeStamp = TotalSec (TimeSaveOTDR);    // # DTS
  // UD
#ifdef OTDR_DIST_IN_KM
  fixParams.achUnitsOfDistance[0] = 'k'; fixParams.achUnitsOfDistance[1] = 'm'; // km - [км]
#else
  fixParams.achUnitsOfDistance[0] = 'm'; fixParams.achUnitsOfDistance[1] = 't'; // mt - [м]
#endif

  fixParams.sActualWavelength_01nm = genParams.sNominalWavelength_nm * 10; // AW ???? почему множим на 10 
  fixParams.lAcquisitionOffset_100ps = 0; // AO  без смещения 
  fixParams.ulAcquisitionOffsetDistance = 0; // AOD без смещения 
//  fixParams.lAcquisitionOffset_100ps = g_FileParamInfo.iAcquisitionOffset_100ps;
//#ifdef OTDR_DIST_IN_KM
//  fixParams.ulAcquisitionOffsetDistance = g_FileParamInfo.uAcquisitionOffsetDistance_10cm * 10 / 100;// [0.1m]->[0.1km]
//#else
//  fixParams.ulAcquisitionOffsetDistance = g_FileParamInfo.uAcquisitionOffsetDistance_10cm; // [0.1m]=[0.1m]
//#endif
  fixParams.sTotalNumberOfPulseWidthUsed = 1;         // TPW # ? почему в таблице 6000_релиз003 указан 0?

  // следующие параметры д.б. массивами значений для каждого из TPW
  // --->
  fixParams.sPulseWidthUsed_ns = GetWidthPulse(GetIndexIM());     // PWU g_measConfig.iImpulse;
  fixParams.lDataSpacing_100ps = (int32_t)GetValueDS(); // DS устанавливаем значения DS для установленного режима измерения     // не надо множить, уже посчитан как надо * 100000.
  fixParams.lNumberOfDataPoints = OUTSIZE; //  NPPW #define из system.h
  // <---
  fixParams.lGroupIndex = (int32_t)(fGroupIndex * 100000.); //GI
  
  fixParams.sBackscatterCoefficient_dB = (int16_t)ReflParam.BC;; //BC from TR_PARAM ReflParam  DeviceData.h
  fixParams.lNumberOfAverages = SettingRefl.NumAvrag; //NAV 
  //fixParams.usAveragingTime_01sec = g_FileParamInfo.usAveragingTime;  //AT   ???оба значения в [0.1sec]???
  fixParams.usAveragingTime_01sec = GetTimeAvrg(GetIndexVRM())*10;//AT   время накопления в сек
 // fixParams.lAcquisitionRange_100ps = (int32_t)((double)(StatusFile.ValueDS/10000.0)*g_FileParamInfo.iNumPoints);  // AR StatusFile.NumPtsMain
  fixParams.lAcquisitionRange_100ps = Get_AR();  // AR StatusFile.NumPtsMain
  // ARD = диапазон измерений в метрах (у нас так!?) 
  // не знаю на что влияет но для себя напишем как установленные диапазоны т.е. 0.5км,2,4,8,16,32,64,128 но как бы в метрах
////  fixParams.lAcquisitionRangeDistance = 0;        //g_FileParamInfo.iDistance/1000
//#ifdef OTDR_DIST_IN_KM
//  fixParams.lAcquisitionRangeDistance = (int32_t)(g_fDistStep * (g_FileParamInfo.iNumPoints - 1) / 100.);   // [0.1km]
//#else
//  fixParams.lAcquisitionRangeDistance = (int32_t)(g_fDistStep * (g_FileParamInfo.iNumPoints - 1) * 10.);    // [0.1m]
//#endif
  fixParams.lAcquisitionRangeDistance = (int32_t)(GetLengthLine(GetIndexLN()) * 10000.);    // ARD[0.1m]в десятках см
  fixParams.lFrontPanelOffset_100ps = 0;
  fixParams.usNoiseFloorLevel_dB = 65535;
  fixParams.sNoiseFloorScaleFactor = 1000;
  fixParams.usPowerOffsetFirstPoint_dB = 0;
  fixParams.usLossThreshold_dB = (uint16_t)(ReflParam.LT); //LT

  // значения порога по отражению [-69.9; -10.] RT
  // в sor-файле под значение отводится 2 байта (uint16_t)
  // используем при записи значения до -65.5 без изменения (т.е., 65500),
 //  а меньшие (вплоть до -69.9) масштабируем в 35
  // у нас какбы все посчитано! можно просто переписать
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
//    fixParams.usReflectanceThreshold_dB = (uint16_t)(g_eventSearch.fReflectance * -1000.); // знак у нас другой...
 
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
    // прибавляем размеры массивов
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
 // про события... просто задаем размеры
  if(NumEventNow)
  {
    keyEventsInfo.revision = DEF_REVISION;
    // нужно посчитать размер блока при событиях
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

    // учёт строк комментариев -  уже учтены они фиксированые (10 байт)
//    for(int16_t i = 0; i < NumEventNow; ++i)
//      keyEventsInfo.size += strlen(keyEvents.aEvent[i].szComment) + 1;
  }
// блок данных рефлектограммы
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

 // выключим АККУРАТНО дополнительные блоки
//  if(bComServ01IsRequired) // необходимо сохранить дополнительный блок СвязьСервис 01?
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
//      float fCmTo100ps = 0.01 * fGroupIndex * 1.e+10 / LIGHTSPEED;  // коэф-т преобразования из см в сотни пикосекунд
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
//    comServ01Info.size = 0;   // означает, что блок в sor помещать не нужно

//  if(bComServ02IsRequired) // необходимо сохранить блок с данными события-конца входной катушки?
//  {
//    SLineWithCords& lineWithCords = g_lineWithCords[iPlaceLS];
//    SKeyEvent&      launchEvent = lineWithCords.launchEvent;
//    SKeyEvent&      tailEvent = lineWithCords.tailEvent;
//
//    int iEventDataSize =          // суммарный размер полей структуры без строки комментария
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
 // контрольная сумма
  chksumInfo.revision = DEF_REVISION;
  chksumInfo.size = sizeof(szBlockCksum) + sizeof(uint16_t);

  return 0;
}

//
// сброс значения (инициализация) контрольной суммы
//
static void ResetCheckSum(void)
{
  s_chkSum = 0xFFFF;
}

//
// пересчёт (пополнение) значения контрольной суммы
//
// buff   указатель на блок данных
// cBytes количество байтов
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
// запись в файл и пополнение контрольной суммы
//
// pFile        указатель на FIL-структуру
// buff         указатель на блок данных для записи
// cBytesToW    количество байтов для записи
//
static FRESULT f_writeCks(FIL* pFile, const void* buff, UINT cBytesToW)
{
  UINT    cBytesW;        // количество записанных байтов
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

//// запись данных одного события
//FRESULT WriteEvent(FIL* pFile, SKeyEvent &rEvent)
//{
//  // побайтное помещение содержимого полей KeyEvent в буфер
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
//  // запись содержимого буфера
//  return f_writeCks(pFile, g_szBuffer, iEventDataLen);
//}

//// запись структуры данных потерь на линии
//FRESULT WriteLossesOfLink(FIL* pFile, SLossesOfLink &rLosses)
//{
//  // побайтное помещение содержимого полей SLossesOfLink в буфер
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
//  // запись содержимого буфера
//  return f_writeCks(pFile, g_szBuffer, pch - g_szBuffer);
//}

// запись Sor-файла
//
// pszFileName  имя файла, включая logical drive "0:" или "1:"
// iPlaceLS     индекс посадочного места
// pDataPoints  указатель на массив точек измерений
//
int WriteSorFile(const char* pszFileName, int iPlaceLS, uint16_t* pDataPoints)
{
  FIL     file;
  FIL*    pFile = &file;
  FRESULT res;
  int     i;
  // пропишем события напрямую так как они фиксированной длинны
  //SKeyEvents& keyEvents = g_keyEvents[iPlaceLS]; // ссылка на пар-ры событий для места лазера

  PrepareSorData(iPlaceLS, pDataPoints);

  // была ли выделена память под строку для имени
  if(!g_pszFileNames[iPlaceLS])
    g_pszFileNames[iPlaceLS] = (char*)malloc(MAX_PATH_LEN);

  // Note: сразу урезать строку нельзя, т.к. pszFileName может указывать на неё...

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
    // запись Map
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
#if 0   // мы пока не используем
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

    // запись GenParams
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

    // запись SupParams
    if ((res = f_writeCks(pFile, szBlockSupParams, sizeof(szBlockSupParams))) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszSupplierName, strlen(supParams.pszSupplierName) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOTDRMainframeID, strlen(supParams.pszOTDRMainframeID) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOTDRMainframeSN, strlen(supParams.pszOTDRMainframeSN) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOpticalModuleID, strlen(supParams.pszOpticalModuleID) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOpticalModuleSN, strlen(supParams.pszOpticalModuleSN) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszSoftwareRevision, strlen(supParams.pszSoftwareRevision) + 1)) != FR_OK ||
        (res = f_writeCks(pFile, supParams.pszOther, strlen(supParams.pszOther) + 1)) != FR_OK)
      break;

    // запись FxdParams
    // пропишем время
    //fixParams.ulDateTimeStamp = g_StampTimeSave[iPlaceLS];
    fixParams.ulDateTimeStamp = TotalSec (TimeSaveOTDR);    // # DTS
    // пропишем значение накоплений для данной длины волны
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
          // массивы
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

    // запись KeyEvents, если имеются события
    if(NumEventNow)
    {
      if ((res = f_writeCks(pFile, szBlockKeyEvents, sizeof(szBlockKeyEvents))) != FR_OK ||
          (res = f_writeCks(pFile, &NumEventNow, sizeof(NumEventNow))) != FR_OK)
        break;

      // Note: Массивы данных событий размещены в sd-ram, границы полей в структурах
      //       вынужденно выровнены с кратностью в 4 байта. Применено копирование в
      //       буфер данных без "дырок", а затем, запись буфера целиком.

      for(i = 0; i < NumEventNow; ++i)
        //if((res = WriteEvent(pFile, keyEvents.aEvent[i])) != FR_OK)
        if((res = f_writeCks(pFile, &EvenTrace[i], 52)) != FR_OK)
           break;

      if(res != FR_OK)
        break;

      // запись потерь на линии
      //if ((res = WriteLossesOfLink(pFile, keyEvents)) != FR_OK)
      if ((res = f_writeCks(pFile, &EndEvenBlk, 22)) != FR_OK)
         break;
    }

    // запись DataPts
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

    // запись Cksum
    UINT  cBytesToW;
    UINT  cBytesW;
    //int   iDrive = GetCurrentDrive();

#if 0  // не всегда проходит запись значения s_chkSum
    if ((res = f_writeCks(pFile, szBlockCksum, sizeof(szBlockCksum))) != FR_OK)
      break;

    cBytesToW = sizeof(s_chkSum);
    f_writeWTC(pFile, &s_chkSum, cBytesToW, &cBytesW);
#else
    // запись ключевой строки и значения s_chkSum
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

    f_unlinkWTC(pszFileName);   // пробуем удалить файл если плохо записался
  }
  else if(res == FR_OK && g_pszFileNames[iPlaceLS] && pszFileName != g_pszFileNames[iPlaceLS])
    strcpy(g_pszFileNames[iPlaceLS], pszFileName);

  return res;
};

//
// чтение из файл и пополнение контрольной суммы
//
// pFile        указатель на FIL-структуру
// buff         указатель на блок данных для чтения
// cBytesToR    количество байтов для чтения
//
static FRESULT f_readCks(FIL* pFile, void* buff, UINT cBytesToR)
{
  UINT    cBytesR;  // количество прочитанных байтов
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
// чтение из файла блока указанной длины для пополнения контрольной суммы
//
// pFile        указатель на FIL-структуру
// cBytesToR    количество байтов для чтения
//
static FRESULT f_readDummyCks(FIL* pFile, UINT cBytesToR)
{
  const UINT cuBlockSize = 256;       // максимальный размер блока для чтения

  UINT    cBytesLeftToR = cBytesToR;  // количество байтов, которые осталось прочитать
  FRESULT res = FR_OK;

  while(cBytesLeftToR)
  {
    //min(cBytesLeftToR, cuBlockSize);
    cBytesToR = cBytesLeftToR < cuBlockSize ? cBytesLeftToR : cuBlockSize;
    res = f_readCks(pFile, tempBuffer, cBytesToR);

    if(res != FR_OK)
      break;

//    RecalcCheckSum(tempBuffer, cBytesToR);	// оставил после слияния FeatEvents&PDF и master (на всякий случай...)
    cBytesLeftToR -= cBytesToR;
  }

  return res;
}
// выключим пока чтение!!!


// чтение данных одного события
// у нас можно убрать и сделать пустое чтение блоков без заполнения
// т.к. мы пересчитываем события по данным рефлектограммы
//int ReadEvent(FIL* pFile, SKeyEvent &rEvent)
//{
//  UINT  cBytesToR;      // кол-во байтов для чтения
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
//// чтение структуры данных потерь
//int ReadLossesOfLink(FIL* pFile, SLossesOfLink& rLosses)
//{
//  UINT  cBytesToR;      // кол-во байтов для чтения
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
// чтение Sor-файла
//
// pszFileName  имя файла, включая logical drive "0:" или "1:"
// pDataPoints  указатель на массив точек измерений
// nPointsMax   максимальное количество точек в массиве
//
int ReadSorFile(const char* pszFileName, uint16_t* pDataPoints, int32_t nPointsMax)
{
  extern int16_t  g_aCurrEvent[LSPLACENUM];
  //SKeyEvents  &keyEvents = g_keyEvents[0];
  FIL         file;           // структура данных файла
  FIL*        pFile = &file;  // указатель для краткости
  UINT        cBytesToR;      // кол-во байтов для чтения
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

    // ключевая строка
    cBytesToR = sizeof(szBlockMap);
    // считываем мар блок
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // сверяем что строка равна "Map"
    if(memcmp(g_szBuffer, szBlockMap, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;  // It is not sor-file
      break;
    }

    cBytesToR = sizeof(map);
    // считываем содержимое мар блока (1)
    if(f_readCks(pFile, &map, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // разбираем содержимое, проверяем версию
    if(map.info.revision < 200 || 299 < map.info.revision)
    {
      iRes = OTDR_FILE_SYNTAX_ERROR;  // Unknown version of sor-file
      break;
    }
    // у наших файлов число блоков ограничено, то есть 
    // 7 -если события и 6- без событий с блоком Map 
    // первый блок GenParams - 
    cBytesToR = sizeof(szBlockGenParams);
    // считываем название блока "GenParams" (2 блок)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // сравниваем название
    if(memcmp(g_szBuffer, szBlockGenParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;  // It is not sor-file
      break;
    }
    // 
    cBytesToR = sizeof(struct SBlockInfo);
    // считываем блок инфо - версию и размер (2)
    if(f_readCks(pFile, &genParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
     // здесь бы получить размер блока GenParams для расчета "прыжка"?
    cBytesToR = sizeof(szBlockSupParams);
    
    // считываем название блока "SupParams" (3 блок)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // сравниваем название
    if(memcmp(g_szBuffer, szBlockSupParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    cBytesToR = sizeof(struct SBlockInfo);
    // считываем блок инфо - версию и размер (3)
    if(f_readCks(pFile, &supParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
     // здесь бы получить размер блока GenParams для расчета "прыжка"?

    cBytesToR = sizeof(szBlockFixParams);
    // считываем название блока "FixParams" (4 блок)
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // сравниваем название
    if(memcmp(g_szBuffer, szBlockFixParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }

    cBytesToR = sizeof(struct SBlockInfo);
    // считываем блок инфо - версию и размер (4)
    if(f_readCks(pFile, &fixParamsInfo, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // вроде просто считываем 256 байт, в буфер
    // или пока не встретим 0 ()
    // если встретили 0 то считали в буффер только символы строки
    // с ее размером а не все 256, даже если это другой блок...
    // другое название
    // необязательный блок, когда имеется DataPts
    if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // проверяем не заголовок ли это блока "KeyEvents"
    if(memcmp(g_szBuffer, szBlockKeyEvents, sizeof(szBlockKeyEvents)))
    {
      // это какой-то другой блок; ниже выясняется, какой
      keyEventsInfo.revision = DEF_REVISION;
      keyEventsInfo.size = 0;
    }
    else
    { // это объявление блока "KeyEvents" (блок 5 из 7)
      cBytesToR = sizeof(struct SBlockInfo);
    // считываем блок инфо - версию и размер 
      if(f_readCks(pFile, &keyEventsInfo, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
      // опять считываем блок до следующего нуля(0) ,
      // чтение имени следующего блока
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }
    // я этот блок исключил так как его не должно быть в наших файлах
    // можно вообще его исключить...
    // проверяем что это 
    // необязательный блок  "LnkParams";   // пока не используем
    if(memcmp(g_szBuffer, szBlockLinkParams, sizeof(szBlockLinkParams)))
    {
      // это какой-то другой блок; ниже выясняется, какой
      linkParamsInfo.revision = DEF_REVISION;
      linkParamsInfo.size = 0;
    }
    else
    { // если это "LnkParams" считываем его
      cBytesToR = sizeof(struct SBlockInfo);

      if(f_readCks(pFile, &linkParamsInfo, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      // чтение имени следующего блока
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // необязательный блок, когда имеется KeyEvents
    if(memcmp(g_szBuffer, szBlockDataPts, sizeof(szBlockDataPts)))
    {
      if(keyEventsInfo.size == 0)    // блока KeyEvents не было?
      {
        iRes = OTDR_FILE_STRUCT_ERROR;
        break;
      }

      // это какой-то другой блок; ниже выясняется, какой
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

      // чтение имени следующего блока
      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // перед BlockCksum могут присутствовать дополнительные блоки, заданные производителем
    // ------------------------------------------------------------------
//
//    // необязательный блок ComServ01
//    if(memcmp(g_szBuffer, szBlockComServ01, sizeof(szBlockComServ01)))
//    {
//      // это какой-то другой блок; ниже выясняется, какой
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
//      // чтение имени следующего блока
//      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//    }
//
//    // необязательный блок ComServ02
//    if(memcmp(g_szBuffer, szBlockComServ02, sizeof(szBlockComServ02)))
//    {
//      // это какой-то другой блок; ниже выясняется, какой
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
//      // чтение имени следующего блока
//      if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
//      {
//        iRes = OTDR_FILE_READ_ERROR;
//        break;
//      }
//    }

    // инф-ция, касающаяся определённых пользователем блоков
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

        // чтение имени следующего блока
        if(!f_getsz(g_szBuffer, ArraySize(g_szBuffer), pFile))
        {
          iRes = OTDR_FILE_READ_ERROR;
          break;
        }
      }
    }
    // вдруг какие-то ошибки...
    if(iRes != OTDR_OKAY)
      break;
    // получаем место где стоим, это должно быть конец таблицы содержимого белкора
    DWORD dwFilePos = f_tell(pFile);
    // если положение меньше чем объявленный размер блока Map, то делаем пустое считывание
    //до размера с подсчетом контрольной суммы. 
    if(dwFilePos < map.info.size)
    {
      if(f_readDummyCks(pFile, (UINT)(map.info.size - dwFilePos)) != FR_OK)  // чтение для подсчёта CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }
    // Считываем заголовок GenParams
    cBytesToR = sizeof(szBlockGenParams);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // проверяем его на совпадение
    if(memcmp(g_szBuffer, szBlockGenParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }
    // далее можно разобрать его содержимое по полям или считать в "пустоту" весь блок
    cBytesToR = sizeof(genParams.achLanguageCode);
    // считываем LC
    if(f_readCks(pFile, genParams.achLanguageCode, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // указатель на буфер, в кот. будет последовательно зачитываться содержимые строк
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
    // считываем два Short Int FT, NW
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
    // подготавливаемся к считыванию 10 байт = 2+4+4
    cBytesToR =
      sizeof(genParams.achCurrentDataFlag) +
      sizeof(genParams.lUserOffset_100ps) +
      sizeof(genParams.ulUserOffsetDistance);
    // считываем CDF, UO,  UOD
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
    // get CMT - комментарий который можно отобразить!
    if(!f_getsz(pchBuffer, BUFFER_SIZE, pFile))
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    // блок SupParams
    cBytesToR = sizeof(szBlockSupParams);
    
    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // совпадение
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

    // блок FixParams
    cBytesToR = sizeof(szBlockFixParams);

    if(f_readCks(pFile, g_szBuffer, cBytesToR) != FR_OK)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }
    // совпадение
    if(memcmp(g_szBuffer, szBlockFixParams, cBytesToR))
    {
      iRes = OTDR_FILE_STRUCT_ERROR;
      break;
    }
    // подготавливаем считывание блока данных 18 байт
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
    float fDistFactor;  // множитель для пересчёта значений расстояний, [м], при различных единицах измерения

    if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'm')      // [км]
      fDistFactor = 1000.;
    else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 't') // [м]
      fDistFactor = 1.;
    else if(fixParams.achUnitsOfDistance[0] == 'f' && fixParams.achUnitsOfDistance[1] == 't') // [футы]
      fDistFactor = 0.3048;
    else if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'f') // [кфуты]
      fDistFactor = 304.8;
    else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 'i') // [мили]
      fDistFactor = 1609.344;
    else
    {
      iRes = OTDR_FILE_SYNTAX_ERROR;
      break;
    }

    // значения длины катушек записываются в блок ComServData02, а значения смещения всегда равны 0!
    // Note: значения в sor сохранены в 1/10 единиц измерения
    g_eventSearch.launchCord.lLen_cm = (int32_t)(genParams.ulUserOffsetDistance * fDistFactor * 0.1f);
#endif

#if 0
    // массивы
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
    // по одному значению параметров для единственной ширины импульса
    // fixParams.sTotalNumberOfPulseWidthUsed д.б. =1
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
    
    // остальные поля FixParams
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

    // блок KeyEvents (может отсутствовать)
    if(keyEventsInfo.size)
    {
            if(f_readDummyCks(pFile, keyEventsInfo.size) != FR_OK)  // пополняем CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

//      // чтение g_keyEvents и aKeyEvent[iPlaceLS][i], если имеются события
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
//      // заголовок блока прочитан
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
      if(f_readDummyCks(pFile, linkParamsInfo.size) != FR_OK)  // пополняем CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // блок DataPts
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

      // значения общего кол-ва точек и масштабирующих коэф-в
      cBytesToR = sizeof(dataPts.lNumPoints) + sizeof(dataPts.sNumScaleFactors);

      if(f_readCks(pFile, &dataPts.lNumPoints, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      // значения для первой группы данных (кол-во точек и масштабирующий коэф-т)
      cBytesToR =
        sizeof(dataPts.lNumPointsUsingScaleFactor) + sizeof(dataPts.sScaleFactor);

      if(f_readCks(pFile, &dataPts.lNumPointsUsingScaleFactor, cBytesToR) != FR_OK)
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }

      dataPts.pDataPoints = pDataPoints; // указалка на наш массив

      if(dataPts.lNumPointsUsingScaleFactor > nPointsMax) // проверка на параметр превышения размера получаемых данных
      {
        iRes = OTDR_OUT_OF_MEMORY;
        break;
      }

      cBytesToR = dataPts.lNumPointsUsingScaleFactor * sizeof(*dataPts.pDataPoints);
      // считывание данных в наш массив (LogData)
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

// попробуем выключить за ненадобностью...
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
//        // "затираем" содержимое структуры
//        memset(&launchEvent, 0, sizeof(launchEvent));
//        launchEvent.szComment[0] = ' '; // формирование строки по-умолчанию " \0"
//      }
//
//      if(comServ02.lTailCordLen_cm)
//      {
//        if((iRes = ReadEvent(pFile, lineWithCords.tailEvent)) != OTDR_OKAY)
//          break;
//      }
//      else
//      {
//        // "затираем" содержимое структуры
//        memset(&tailEvent, 0, sizeof(tailEvent));
//        tailEvent.szComment[0] = ' '; // формирование строки по-умолчанию " \0"
//      }
//
//      if((iRes = ReadLossesOfLink(pFile, lineWithCords)) != OTDR_OKAY)
//        break;
//    }
//    else
//    {
//      // обнуление содержимого структур данных событий-концов катушек
//      memset(&launchEvent, 0, sizeof(launchEvent));
//      launchEvent.szComment[0] = ' ';
//      memset(&tailEvent, 0, sizeof(tailEvent));
//      tailEvent.szComment[0] = ' ';
//
//      // Note: используется стандартное преобразование указателей с производного на базовый класс
//      memset(static_cast<SLossesOfLink*>(&lineWithCords), 0, sizeof(SLossesOfLink));
//    }

    if(s_uUserDefBlocksSize)
    {
      // имеются блоки, заданные пользователем
      cBytesToR = strlen(s_szFirstBlockUserDef) + 1;

      // проверяется наименование только первого из блоков, заданных пользователем
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

      // блоки должны иметь суммарную длину s_uUserDefBlocksSize
      if(f_readDummyCks(pFile, s_uUserDefBlocksSize - cBytesToR) != FR_OK) // пополняем CRC
      {
        iRes = OTDR_FILE_READ_ERROR;
        break;
      }
    }

    // блок Cksum
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

    uint16_t   uChkSum;  // прочитанное из файла значение CRC
    UINT  cBytesR;

    cBytesToR = sizeof(uChkSum);

    // чтение самой контрольной суммы (без её изменения)
    fRes = f_readWTC(pFile, &uChkSum, cBytesToR, &cBytesR);

    if(fRes != FR_OK || cBytesToR != cBytesR)
    {
      iRes = OTDR_FILE_READ_ERROR;
      break;
    }

    if(uChkSum != s_chkSum)   // сравнение с записанным CRC
    {
      iRes = OTDR_FILE_BAD_CHECKSUM;  // Bad checksum
      break;
    }
  } while(0);


  if(pFile)
  {
    f_closeWTC(pFile);
 // далее разбор ошибок и сообщений при чтении файла,
    // какой-то выход или окончение чтения
    if(iRes == OTDR_FILE_BAD_CHECKSUM) // плохая контрольная сумма 
    {
      // пишем сообщение о плохой контрольной сумме но переписываем данные
      sprintf(MsgErrMem,"Bad CheckSum");
      //if(MessageBox(trBadCheckSum.get(), trWarning.get(), MB_YESNO) == IDYES)
        iRes = OTDR_OKAY;
    }

    if(iRes != OTDR_OKAY)
    {
      if(iRes != OTDR_FILE_BAD_CHECKSUM)   // т.к. предупреждение о CRC уже выводилось
      sprintf(MsgErrMem,"Bad File Err=%d",iRes);
      // здесь все плохо. Пишем сообщения но данные не переписываем и файл не открываем...
    }
    else
    {
      InitMemReflSet (); // инициализация установок рефлектометра полученных из файла
      // заполнение структур отображения файла, и настройки подсчета событий,
      // сохранение текущих установок для востановления после просмотра
      // пока переписываем в блоки с маркировкой Mem
      // если чужой файл, пытаемся по парметру восстановить длину линии (диапазон) в метры...
      int32_t iDistance=0;
            if(fixParams.lAcquisitionRangeDistance > 0)
      {
        float fDistFactor;

        if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'm')      // [км]
          fDistFactor = 1000.;
        else if(fixParams.achUnitsOfDistance[0] == 'f' && fixParams.achUnitsOfDistance[1] == 't') // [футы]
          fDistFactor = 0.3048;
        else if(fixParams.achUnitsOfDistance[0] == 'k' && fixParams.achUnitsOfDistance[1] == 'f') // [кфуты]
          fDistFactor = 304.8;
        else if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 'i') // [мили]
          fDistFactor = 1609.344;
        else // if(fixParams.achUnitsOfDistance[0] == 'm' && fixParams.achUnitsOfDistance[1] == 't') // [м]
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
      // зполняем индексами и дублируем в стркутуру FixParams для индикации
      FixParams.ARD = iDistance; // в метрах!!!
      MemSetRefl.Index_Ln = CalkIndexLN(iDistance); // вычисляем индекс длины линии (диапазон) 
      FixParams.AW = genParams.sNominalWavelength_nm; // длина волны
      MemSetRefl.SW_LW = CalkIndexSC(genParams.sNominalWavelength_nm); // индекс посадочного места по длине волны
      FixParams.PWU = fixParams.sPulseWidthUsed_ns; // длительность импульса
      MemSetRefl.Index_Im = CalkIndexIM(fixParams.sPulseWidthUsed_ns); //  индекс длительности импульса
      FixParams.AT = fixParams.usAveragingTime_01sec; // время накопления в 0.1 секундах
      MemSetRefl.Index_Vrm = CalkIndexWRM(fixParams.usAveragingTime_01sec); //  индекс времени накопления
      FixParams.GI = fixParams.lGroupIndex; // спектральный коэффициент (преломления) 
      MemSetRefl.K_pr = (float)(fixParams.lGroupIndex/100000.); // коэфф. преломления
      
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

  // копируем комментарии
  memcpy(GenParams.CMT, genParams.pszComment, (strlen(genParams.pszComment)<19)?(strlen(genParams.pszComment)):(19));
  GenParams.CMT[19]=0;



    }
  }

  return iRes;
}

//
// чтение данных из SOR-файла с указанным именем
//
// Note: для сохранения данных для графика, а также имени файла,
//       используются соответствующие буферы с нулевым индексом.
int ReadSorFileG(const char* pszFileName)
{
  // была ли выделена память под строку для имени?
  if(!g_pszFileNames[0])
    g_pszFileNames[0] = (char*)malloc(MAX_PATH_LEN);

  // сразу копирование строки, т.к. pszFileName может совпадать с g_szBuffer, кот.
  // используется в ReadSorFile()
  if(g_pszFileNames[0] && pszFileName != g_pszFileNames[0])
    strcpy(g_pszFileNames[0], pszFileName);

  // вызов только после копирования имени файла!
  int iRes = ReadSorFile(g_pszFileNames[0], LogData, 5300);

  if(iRes == OTDR_OKAY)
  {
    // можно рисовать график 
//    float fDistStep =
//            (float)(((double)LIGHTSPEED*g_FileParamInfo.iDS) / (g_FileParamInfo.iGroupIndex*1.e+9));

  }
  else
  {
    if(g_pszFileNames[0])
      g_pszFileNames[0][0] = 0; // урезание строки
  }

  return iRes;
}


