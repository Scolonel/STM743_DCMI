#ifndef __DEVICEDATA_H__
#define __DEVICEDATA_H__

#define WAVE_LENGTHS_NUM 5
#define LENGTH_LINE_NUM 7
#define WIDTH_PULSE_NUM 9//8 добавили 5 нС
#define TIME_AVR_NUM 6
#define TIME_LIGHT_NUM 3
#define LSPLACENUM 3
#define CURRENT 0
#define NEXT 1
#define MODELSNUM 5
#define MODERENUM 3
// постоянные времени с фильтрами и без
#define TAUFLTR 820
#define TAUWFLTR 70
#define TAUARX 5
#define TAUARN 10
#define SizePlus 6
// от файла events.c
#define BC_def 800 // уровень отражения задается в зависимости от линии обычно от 80-60 дБ
#define FPO_def 0
#define NF_def 65535
#define NFSF_def 1000
#define PO_def 0
#define LT_def 200  // 0.2dB порог определения события по затуханию
#define RT_def 65000  // 65dB порог определения события по отражению от 0 до 65 дБ
#define ET_def 3000  // 3dB порог затухания для определения конца линии



#define NUMSHIFTZONE 13 // число точек начального смещения для разных длин и импульсов

#define DBNAMESTRUCT 4088 // адрес страницы содержащей структуру альтернативного имени прибора
#define JDSUSTRUCT 4089 // адрес страницы содержащей структуру поддержки JDSU
#define TABLEMEMOTDR 4090 // адрес страницы содержащей таблицу сохраненных рафлектограмм
#define TABLEMEMOTDREXT 4095 // адрес страницы содержащей расширенную таблицу сохраненных рафлектограмм
#define CFG_DEVICE 4091
#define CFG_USER 4092
#define COEF_PM 4093
#define EVEN_SET 4094  // адрес блока установки пределов событий событий

// Структура Установок событий рефлектограммы  и (18.04.2013) конфигурация авторежима - и коэфф. преломления
typedef struct
{
  unsigned short BC;     // BC = 800 предел обратного отражения
  unsigned FPO; // FPO смещение физическое (отступ сбора данных) равен 0
  unsigned short NF ;    // NF нижний уровень шумов равен 0
  unsigned short NFSF ;  // NFSF масштабный коэфф  уровень шумов равен 1000
  unsigned short PO ;    // PO смещение по мощности равен 0 в 0.001 дБ 
  unsigned short LT ;    // LT минимальное значение затухания для события 200 в 0.001 дБ 
  unsigned short RT ;    // RT минимальное значение отражения для события 40000 в -0.001дБ
  unsigned short ET ;    // ET минимальное затухание для конца линии 3000 в 0.001 дБ
  //  добавим 
  //BYTE EnaAutoSetSC[LSPLACENUM]; // конфигурация разрешенных лазеров в авто режиме
  //float K_pr_SC[LSPLACENUM]; // коэфф. преломления для монтажных длин волн
  
} TR_PARAM;  // параметры рефлектограммы


// структура параметров рефлектограммы ( текущего измерения)
extern TR_PARAM ReflParam;
// структура параметров рефлектограммы ( from Memory)
// берем из файла белкор
extern TR_PARAM MemReflParam;
// структура параметров рефлектограммы ( из текущего измерения для востановления)
extern TR_PARAM ResetReflParam;

// Структура Установок рефлектометра (14 byte)
typedef struct
{
  BYTE Index_Ln; // индекс значений длины линии в пользовательском режиме
  BYTE Index_Im; // индекс значений длительности импульса 
  BYTE Index_Vrm; // индекс значений времени работы 
  BYTE Index_Comm;  // местоположения курсора в комментариях 
  float K_pr; // коэфф преломления 
  unsigned NumAvrag;
  BYTE SW_LW;  // признак длинны волны лазера (переключает длину волны лазера)
  BYTE SubModeRef; // рефлектометр авто или нет ( для востановления после включения)
  unsigned short LogNoise;// логарифмический шум (измеряется перед импульсом при накоплении)
  BYTE SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index 
} REFL_SET;

// структура коэффициентов измерителя мощности
typedef struct
{
int ShZeroRng[4];// начальные смещения диапазонов
float CoefStykRange[4]; // коэффициенты стыковки диапазонов={1.0,1.0,1.0,1.0};
unsigned int PointKalib[WAVE_LENGTHS_NUM];// длины волн калибровки если 0 то ее нет={850,1310,1490,1550,1625};
float CoefPointKlb[WAVE_LENGTHS_NUM]; // коэффициенты привязки длин волн калибровки={1,1,1,1,1}
float CoefSpctrL[6]; // коэффициенты спектральной харр. 800-900={16,15,14,13,12,11}
float CoefSpctrM[10]; // коэффициенты спектральной харр. 1210-1390 (было 9 с 1370)={1,1,1,1,0.95,1,0.95,0.90,0.85,1}
float CoefSpctrH[13]; // коэффициенты спектральной харр. 1410-1650={1,1,1,1,1,1,1,1,1,1,1,1,1}
} TAB_SET;

// структура конфигурации прибора
typedef struct
{
  WORD NumDevice;  // (999) № прибора,
  WORD PlaceLS[LSPLACENUM]; //  Место 1, (1310) Конфигурация установленных лазеров по местам
  BYTE TypeDevice; // (5)  Признак в типе прибора только для ТОПАЗ (последняя цифра)
  BYTE CfgPM;       //	Признак измерителя (0-нет,1- простой(-70..+6), 2- расширенный (-55..+20)
  BYTE CfgRE;       //	Признак красного глаза (0-нет, 1-есть)
  BYTE ApdiSet;     //	Признак лавинного диода (0-нет, 1 есть)
  WORD InputCheckLvl;	//порог контроля уровня на входе (10- степень двойки) 
  WORD EndCheckLvl; // 	порог контроля конца линии (8500)
  BYTE ID_Device; // прибор 0-ТОПАЗ или 1-MOT_700
  float BatStep; // шаг АЦП батарейки
  // 18/04/2013 
} CFG_DEV;

//Структура поддержки JDSU
typedef struct
{
  unsigned int CompMode;  // режим совместимости ( 0- топаз, 1-JDSU)
  unsigned int LambdaNum[6]; //  длины волн частотного определения
  unsigned int FreqLambda[6]; //  частота соответствующей длины волн частотного определения
  // 21/10/2014 
} JDSU_COM;

//Структура альтернативного имени ПРИБОРА (Д.Б. 26.04.2017)
typedef struct
{
  BYTE Ena_DB;  // разрешение альтернативного имени( 0- разрешено, 1-запрещено)
  unsigned char AltName[16]; //  альтернативное имя
  float ph_A[3]; // поправочный коэфф для длинн волн (из-за "плохого" фотодиода) 
  char UserComm[20];// пользовательский комментарий раньше хранился в 
  // нулевой рефлектограмме
  uint16_t FiberID; //Счетчик волокна? 
} DB_NAME_ENA;

typedef struct  // основные конфигурационные настройки прибора режимы работы
{
  BYTE CurrLang;  // текущий язык Rus, Eng, Cze
  BYTE SubModeRef; // подрежим рефлектометра 0- ручнойЮ 1-авто
  BYTE EnIncFiber; // разрешение авто инкремента волокон при сохранении измерений
  WORD CellMem; // число сохраненых ячеек измерителя
  float CurrLvldB; // текущее значение уровня привязки
  float LevelRef[WAVE_LENGTHS_NUM]; // уровни привязки длин волн калибровки
  DWORD BegShiftZone[NUMSHIFTZONE]; // начальные смешения нулевого метра для разных длин линий
  BYTE TimeLight; // время подсветки
  //
  //  добавим 19.04.2013
  BYTE EnaAutoSetSC[LSPLACENUM]; // конфигурация разрешенных лазеров в авто режиме
  float K_pr_SC[LSPLACENUM]; // коэфф. преломления для монтажных длин волн
  
} CFG_USR;

typedef struct  // основные конфигурационные настройки прибора режимы работы GenParams
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
  int32_t UO;    // User Offset у нас 0 (смещение в 100 пС от начала
  int32_t UOD;    // User Offset Distance у нас 0 (смещение в размерностях дистанции (km,m,ft...) от начала
  char OP[16];   // Operator Name or Operator Code
  char CMT[20];  // Comment User Added information about trace 
} GPB_STR;

typedef struct  // основные данные прибора SupplierParams
{
  char SN[24]; // OTDR Manufacturer's Name 
  char MFID[16];  // OTDR Mainframe Model  
  char OTDR[16];  // Mainfraim Serial number 
  char OMID[20];   // Plug-In Model Number
  char OMSN[16];   // Plug-In Serial Number
  char SR[20];  // Vercion of software/firmware
  char OT[16];    // Other infomation (trace, hardware, software,firmware)
} SPB_STR;

typedef struct  // основные фиксированные параметры прибора FixedParams
{
  uint32_t DTS; // Current Time/Date Seconds since 00:00 1 January 1970 
  char UD[2];  // unit Distance (km, mt, ft, kf, mi)  
  uint16_t AW;  // actual measured wavelength (nm) = NW from GeneralParams
  int32_t AO;   
  int32_t AOD;
  int16_t TPW; // число импульсов (если не один дальше не разбираем)
  int16_t PWU; // длительность импульса в nS
  int32_t DS;  // Data Spacing 
  int32_t NPPW;  // число точек для каждого импульса 
  int32_t GI;  // Group Index - коэфф преломления, в формате белкора 
  int16_t BC; //Backscatter Coefficient
  int32_t NAV; // Number of Averages
  uint16_t AT; // Averaging Time in seconds
  int32_t AR; // Acquisition Range (100 pS - диапазон
  int32_t ARD; // Acquisition Range in units distance (UD)
  int32_t FPO; // Front Panel Offset начальное смещение
  uint16_t NF; //Noise Floor Level (65535)
  int16_t NFSF; //Noise Floor Scale Factor 1000=1.000
  uint16_t PO; // PowerOffset First Point Add Attenuators 10000=10.0dB
  uint16_t LT; // Loss Threshold 200=0.2dB (200)
  uint16_t RT; // Reflectance Threshold 40000=40.0 dB(40000)
  uint16_t ET; // Enf-Of-Fibet Threshold 3000=3.0 dB (3000)
  char TT[2]; // Trace Type (ST, RT, DT, RT)
  int32_t WC[4]; // Window Coordinates
} FPB_STR;

typedef struct  // Block Data Points у нас одиночный масштаб
{
  int32_t TNDP; //Number of Data Points
  int16_t TSF; // Total Number Scale Factor Used
  int32_t TPS; // Total Data points Using 
  int16_t SF; // Scale Factor
} DPNTS_STR;
// получение индекса DS
unsigned GetValueDS (void);


float GetSetBatStep (float Dir); // возвращает или устанавливает значение шага АЦП батарейки

// установка получение индекса в списке комбинации лазеров от номера лазерного места
void SetIndxSeqLS (void);

unsigned StructSize(unsigned data);
unsigned char * StructPtr(unsigned data);
void SetLang( BYTE lang );
BYTE GetLang(int Dir); // получение текущего языка/ или переключение
BYTE GetID_Dev(void); // получение идентификатора для кого прибор
BYTE SetID_Dev(BYTE Data); // установка идентификатора для кого прибор
WORD GetNumDevice(void); // получение номера  прибора
WORD SetNumDevice(WORD Num); // установка номера  прибора
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
// получение идентификатора "железа" , что и как сконфигурировано
void GetDeviceHW( char* name ); // from Computer

void GetTesterName( char* name ); // Tester
void GetMFID( char* name ); // for Belcore
int32_t Get_AR (void);

//void GetHeaderBelcore (char* Name);  заполняем шапку белкора
void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents); // заполняем шапку белкора
BYTE GetEnIncFiber (BYTE Dir);// установка - запрос состояния признака разрешения авто инкремента счетчика волокна
WORD GetCellMem (BYTE Dir);// установка получение текущего значения счетчика сохранения измерений

DWORD FindErrCoeff (void); // проверка таблицы коэффициентов
void FixErrCoeff (DWORD Err); // исправление таблицы коэффициентов
// контроль таблицы настройки измерителя
DWORD CheckErrCoeff (void);

DWORD CheckUserGonfig (void);  // Проверка пользовательских настроек 
BYTE SetTypeDevice (BYTE Data); // установка типа прибора для ТОПАЗОВ
BYTE GetPlaceLS (BYTE Dir); // установка индекса новой длины волны из списка установленных лазеров CURRENT -текущий, NEXT- новый (следующий)
WORD GetLengthWaveLS (BYTE SetPlace); // получение длины волны от индекса установочного места
void SetPlaceLS (BYTE SetPlace); // установка требуемого лазера 
WORD SetupWavePlaceLS (BYTE Place, WORD Data); // Setup num wave for place laser
BYTE SetSeqCombLW (void); // инициализация списка комбинаций установок лазеров
BYTE GetSubModRefl (void); // получение режима работы рефлектометра
void SetSubModRefl (BYTE Mode); // установка режима рефлектометра
BYTE GetIndexLN (void); // получение индекса длины линии
BYTE SetIndexLN (BYTE Index); // установка индекса длины линии
WORD GetLengthLine (BYTE Index); // получение длины линии по индексу
BYTE GetIndexIM (void); // получение индекса длины импульса
void SetIndexIM (BYTE Index); // установка индекса длины импульса
WORD GetTimeAvrg (BYTE Index); // получение времени накопления по индексу
BYTE GetIndexVRM (void); // получение индекса времени накопления
void SetIndexVRM (BYTE Index); // установка индекса времени накопления
WORD GetWidthPulse (BYTE Index); // получение длины импульса по индексу
float GetIndexWAV (void); // получение индекса преломления
void SetIndexWAV (float Data); // установка индекса преломления
float GetWAV_SC(BYTE Index);// получение индекса преломления в зависимости от места лазера
float SetWAV_SC(float Data, BYTE Index);// запись измененного коэфф в структуру
void ReSaveWAV_SC (void);// сохранение текущего коэфф в установленном месте (для динамичного сохранения)

// вычисление индекса  длины импульса по его значению
BYTE CalkIndexIM (uint16_t IM); // получение индекса длины Pulse
// вычисление индекса  длины линии по  её значению (у нас в метрах)
BYTE CalkIndexLN (long int LN); // получение индекса длины линии (диапазона)
// вычисление индекса посадочного места по длине волны, по  её значению (nm)
BYTE CalkIndexSC (long int LW); // получение индекса посадочного места
// вычисление индекса времени усреднения по ее значению (проверяем на первые 4)
BYTE CalkIndexWRM (uint16_t WRM); // получение индекса времени накопления

void InitMemReflSet (void); // инициализация установок рефлектометра полученных из файла

void InitReflSet (void); // инициализация установок рефлектометра
BYTE GetCfgPM (void); // получение установки измерителя
BYTE SetCfgPM (BYTE Data); // Setup установки измерителя
BYTE GetCfgRE (void);// получение признака установки красного глаза
BYTE SetCfgRE (BYTE Data);// Setup признака установки красного глаза
BYTE SetModeLS (char* Str, int Dir, BYTE Lang); // возвращает номер режима источника, и пишет строку для отображения на индикаторе
BYTE SetModeRE (char* Str, int Dir, BYTE Lang); // возвращает номер режима VFL, и пишет строку для отображения на индикаторе
void CntrlRE (void); // функция контроля красным глазом
BYTE OffModeRE(void); // выключает красный гдлаз

DWORD GetCurrentBegShiftZone (void); //получение текущего смещения по индексу
void SetIndexShadowIM (BYTE Index); // установка мертвой зоны для теневого импульса

BYTE GetIndexCurLengthWave (WORD LengthWave);// получение индекса длины волны в зависимости от значения длины волны
float GetLSACoef (void); // получение коэфф. наклона линии по установленным параметрам


unsigned GetTimeLight (BYTE Index); // получение длительности подсветки от индекса
BYTE SetIndexLight (int Dir); // установка(получение) индекса длительности подсветки 0-текущий, 1-увеличение, -1-уменьшение
DWORD SetBegShiftZone (BYTE Index, DWORD Data);// запись мертвой зоны по индексу
DWORD GetBegShiftZone (BYTE Index);// чтение мертвой зоны по индексу
DWORD GetEndCheckLvl (void); // возврат порога при контроле конца линии

BYTE SetIndexCommOTDR (BYTE Index); // установка индекса коммкнтария в структкру для сохранения
void CheckCommOTDR (void); // проверка и корректировка строки комментариев OTDR
BYTE CheckSavedTrace(void); // функция контроля сохраненной рефлектограммы 
uint32_t CheckBelcore(void); // функция контроля сохраненной рефлектограммы 

BYTE GetApdiSet (void); //получение признака наличия лавинного фотодиода
BYTE SetupApdiSet (BYTE Data); //Setup признака наличия лавинного фотодиода
void SetNumAverag (unsigned Data); // Запись числа накоплений в память рефлектограмм
unsigned GetNumAverag (void); // получение числа накоплений рефлектограммы
void DeletedAllCell (void); // удаление всех записей измерителя
// определяет число точек в импульсе по параметрам измерения (1-real 0-wided)
unsigned short PointsInImpulse (int Real);
// возвращает значение ТАУ для установленной длительности импульса
float TauImp (void);
// расчет числа точек смещения при измерении событий
unsigned short GetPointsShift (void);
// получение порога шумов для типа лавинника
unsigned short GetNoiseLvl (void);
// функции изменения параметров анализа рефлктограммы
unsigned short GetSet_BC (float Dir); // коэффциент обратного отражения
unsigned short GetSet_LT (float Dir); // мнимльный перепад контроля события
unsigned short GetSet_ET (float Dir); // конец линии не более 10 дБ
unsigned short GetSet_RT (float Dir); // отражение не более -55дБ
//Зуев И.А.
int GetIndxPON (int Lambda);

float SetCurrLvldB(int Lambda, float LvldBm);  // устанавливает уровень LvldBm в качестве привязки
float GetCurrLvldB(int Lambda);         // Возвращает текущий уровень привязки
                                        // или изменяет  текущий уровень привязки в зависимости от длины волны
BYTE PreSetModeLS (BYTE Index); // принудительная установка режима источника
void  SetupSource (BYTE ModeLS); // установка режима работы лазера
BYTE GetModeLS (void); // получить режим источника
// продолжение модуляции при выбранной модуляции
void ContinueModulation(void);

void GetNumVer (char* Str); // получение номера версии
void SetNewLeveldB (float RPON, int i); // установка нового уроаня привязки по принятым волнам
void SetLogNoise (unsigned short Data);// уровень логарифмического шума, установка
void SaveLogNoise (void); // сохраняем уровень логарифмического шума перед импульсом
unsigned short CalkCheckSum (void);// подсчет контрольной суммы конфигурации прибора
// получение комбинации лазеров исходя из индекса
BYTE GetCombLS (BYTE Index);
// функция переключения лазеров по кругу с излучением информации о себе
void Run_SCWP(void);

// получение установка признака автоматического выбора длины волны измерения
BYTE GetSetModeLW (int a);
// установка получение сколько лазеров стоит???
BYTE GetSetNumLS (BYTE a);


// получить/инкремент значение счетчика файлов рефлектограмм, старт с 200 
unsigned long GetSetCntFiles (BYTE Dir);
void SetCurCntFiles (void);

extern char MsgErrMem[20]; // сообщение об ошибке чтения файла

// структура настройки рефлектометра ( текущего измерения)
extern REFL_SET SettingRefl;
// структура настройки рефлектометра ( из памяти)
extern REFL_SET MemSetRefl;
// структура настройки рефлектометра ( из установок для востановления)
extern REFL_SET ReSetRefl;
// структура коэффициентов измерителя мощности
extern TAB_SET CoeffPM;

extern unsigned long CurCountFiles; // порядковый номер текущего файла (читаем из памяти)

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



