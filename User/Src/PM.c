#include "main.h"

#define SIZE_BUF 8


//#define AUTO      1
//#define MANUAL    0

//static BYTE Range = 0;                  // диапазон работы усилителей
//static uint8_t CurrRange = 0;                  //  текущий диапазон работы усилителей чуть запаздывает за переключением (до готовности данных)
//static BYTE TypeRslt=0;                 // единицы измерений : дБм - 0, дБ -1, Вт-2
const uint16_t CalibrLW[WAVE_LENGTHS_NUM]={850,1300,1310,1490,1550,1625,1650};

float volatile RPON[WAVE_LENGTHS_NUM]; // ячейки значений в режиме автомат P1a PON 0-850,1-1300,2-1310,3-1490,4-1550,5-1625,6-1650  

static uint32_t Watchdog;
uint8_t StateADC = FREEADC;
static int DataADC_PM=0x00;
static uint8_t ModePM = 1;                  // режим переключения диапазонов
static int FreeCodeADC_PM=0x00;       // приведенное значение кодов АЦП для текущего измерения

uint16_t PMWavelenght=1310;  // Текущая длина волны P1
uint16_t P1aWavelenght=0;  // Текущая длина волны P1a

static double PMCurrValue=0;                 // Текущее значение мощности в зависимости от установленного диапазона без привязки к длине волны
double OptPow; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
float Opt_dB; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
float Opt_dBm; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
uint8_t IndxNumWave; // индекс длины волны для получения длины волны при переключених по калибровочным
// и специальным Делаем туда-сюда преобразование Num2Lengh Lengh2Num
float CurrSpectrKoeff; // текущий спектральный коэфф, расчитывается и устанавливается при изменении длины волны
// приустановках, а также выбирается и устанавливается базовый уровень
static float PMdBmValue=0.0;                 // Текущее значение мощности в зависимости от установленного диапазона и выбранной длины волны 
static float PMdBValue=0.0;                 // Текущее значение мощности в зависимости от установленного диапазона и выбранной длины волны
float CurrBaseLevel=-84.0; // текущее значение базового уровня для данной длины волны
float OldBaseLevel=0.0; // старое значение базового уровня
// сюда же пропишем всякие переменные для работы с Р2
float P2dBmValue=0.0;                 // Текущее значение мощности для Р2 в зависимости от установленного диапазона и выбранной длины волны 
float P2dBValue=0.0;                 // Текущее значение мощности Р2 зависимости от установленного диапазона и выбранной длины волны
float P2ORLValue; // текущее значение ORL по данным P2 (от кодов АЦП)
float CurrP2BaseLvl=0.0; // текущий базовый уровень без привязки к длине волны
uint8_t IndxP2LW = 2; // индекс длины волны калибровки для переключения в Р2
// переменные фильтра ДБ для индикации и отображения
uint32_t BuffP2[ADCSIZE]; // буффер данных для P2 (Fifo)
float MediumP2;  // среднее значение АЦП Р2
uint32_t P2SUM;
float P2UP_Lim = 0.0; // верхний порог смены индикации по кодам DAC
float P2DN_Lim =4095.0; // нижний порог смены индикации
float MediumP2_Ind = 0.0; // среднее значение АЦП Р2 для рассчета и индикации

// переменные для оценки измерений P2 в мВт (для ORL)
float BuffP2mW[ADCSIZE]; // буффер данных для P2 mW(Fifo)
float MedP2mW;  // среднее Р2 в миливатах
float P2SUMmW;
// в районе 14. дБ 1 код АЦП дает 
float P2UPmW_Lim = 0.0; // верхний порог смены индикации при миливатах
float P2DNmW_Lim = 10.0; // (100pW)  нижний порог смены индикации при миливатах
float MedP2mW_Ind = 0.001; // среднее Р2 в миливатах для расчета индикации
float MedP2mW_corr;  // среднее Р2 в миливатах с коррекцией

// ORL - считаем всегда по где P2 - 
float BaseRetCurrORL[WAVE_LENGTHS_NUM]; // текущий уровень привязки для расчета ORL вяжется на длину волны, нужен для оперативной
// корректировки в "верху" (13-17 дБ ORL)
float ORLCalc; // расчетное значение ORL
float ORLCalcFree; // расчетное значение ORL
float ORLCalcCorr; // расчетное значение ORL
double PowP2_mW; // мощность на входе P2 в мВт
float PowClbr_mW; // мощность калибровочная для малых значений ORL в мВт
uint16_t indx_LW_ORL; // индекс длины волны измерения ORL, 
// P1a
uint32_t CntWaitLW=0;// счетчик ожидания длины волны в режиме P1a
uint32_t YesLW=0; // счетчик готовности длины волны для управления измерителем и получения данных, если 0 не считаем
uint8_t EnaOpticUART=0; // признак разрешения UARt2 (Optic

TAB_SET CoeffPM; // таблица коэффициентов измерителя (смещение, стыковка, спектралка...)
//unsigned int g_CountRepetCWADD=0; // счетчик повтора переключения в режиме CW_ADD
TAB_SPECIFIC SpecCoeffPM; // таблица коэффициентов измерителя (специфических длин волн)

uint32_t GetPMData(void)       
{
  
  
  switch (StateADC)
  {
  case FREEADC: 
    PM_CS(0);               // Запускаем цикл измерения выбрав CS
    Watchdog=0; // обнуляем счетчик от зависонов (25 мС)
    StateADC = BUSYADC;
    break;
  case BUSYADC: // 
    Watchdog++; // ждем готовности АЦП
    
    if (Watchdog > 10) // не дождались более 250 мС
    {
      // чистим буфер на всякий случай (делаем пустое чтение
      ReadDataADC7782 ();
      StateADC = FREEADC;
      PM_CS(1);
    }
    else
    {
      if (!(GET_PM_DATA)) // данные готовы можно считывать
      {
        DataADC_PM =  ReadDataADC7782 ();
        Watchdog=0; //
        StateADC = READYDATA;
      }
    }
    
    break;
  case READYDATA: // данные готовы (здесь попробуем фильтрануть их)
    Watchdog=0; // 
    PM_CS(1);
    //CurrRange = Range;
    PMCurrValue = GetPower(PMWavelenght);      // Получаем мощность в мВт не зависимо от длины волны
    StateADC = FREEADC;
    break;
  }
  
  //FreeCodeADC_PM = 0x800000-DataADC_PM;
  //FreeCodeADC_PM = 0xFFFFFF-DataADC_PM; // для маленьких тесторов
  if(DeviceConfig.CfgRE>>1) // если новая плата аналоговая 03.03.2023
    FreeCodeADC_PM = 0x800000 - DataADC_PM;
  else // старая аналоговая плата
    FreeCodeADC_PM = DataADC_PM-0x800000;
  return  FreeCodeADC_PM;                           // 
}

void MyDe_t (int lin) // подпрограмма формирования программной задержки
{
  for (int i=0;i<lin;i++);
}

double GetPMValue (void) //получение текущей мощности в Милливатах
{
  return PMCurrValue;
}

float GetPMValuedBm (double PowOpt) //получение текущей мощности в dBm
{
  float lim=-85.0;
  PMdBmValue = (float)(10.0*log10(PowOpt));

  switch (DeviceConfig.CfgPM)
  {
  case 1: // простой фотодиод
    lim = -85.0;
    break;
  case 2: // простой фотодиод
    lim = -60.0;
    break;
  default:
    lim = -85.0;
    break;
  
  }
  if(PMdBmValue<lim) PMdBmValue = lim;
     
  return PMdBmValue; 
}

float GetPMValuedB (double PowOpt) //получение текущей мощности в dB 
{
  PMdBValue = (GetPMValuedBm(PowOpt) - CurrBaseLevel);
  return PMdBValue;
}

uint32_t GetPMDataNorm(void)   // получение кодов АЦП нормированные     
{
  return  FreeCodeADC_PM;                           // 
}

// ногоДрыганное чтение данных из АЦП
uint32_t ReadDataADC7782 (void)
{
  uint32_t Data=0;
  for (int i=0; i<24; i++)
  {
    PM_CLK(0);
    MyDe_t(2);
    if ((GET_PM_DATA)!=0) Data++;
    PM_CLK(1);  
    Data = Data<<1;
    MyDe_t(4);
  }
  return Data>>1;
}


uint8_t GetRange(void)                        // Возвращает текущий диапазон работы измерительного усилителя Получение
{
  return Range;//CurrRange;
}

uint8_t GetModePM (void) // получить режим измерителя
{
  return ModePM;
}

// Устанавливает режим переключения коэффициентов усиления 
// измерителя мощности (ручной или автоматический)
uint8_t SetModePM( uint8_t Mode) // установить  режим измерителя
{
  if (Mode) ModePM = 1; // auto
  else ModePM = 0; // manual
  return ModePM;
}



// поиск индекса в списке специальных
uint8_t SearchIndxNW (uint16_t LW)
{
  uint8_t i=0,yesLW=0;
  for (i=0; i<WAVE_LENGTHS_SPEC; i++)
  {
    if(LW == SpecCoeffPM.PointSpecKalib[i])
    {
      yesLW=1;
      break;
    }
  }
  if(yesLW) yesLW=WAVE_LENGTHS_NUM+i;
  return yesLW;
}

// Получение спектрального коэффициента в зависимости от длины волны
// диапазоны 800-900, 1210-1390, 1410-1650 
// расчет спектрального коэфф. по длине волны, установка текущего IndxNumWave - в зависимости от длины волны
// Переключение длины волны по калиброваным и спец волнам
float GetCoeffSpctr(uint16_t Lambda, int8_t Mode)
{
  static float k_c =1.0;    // Спектральный коэффициент
  unsigned int i;    // Индекс
  double a,b;        // индексы по которым расчитывают коэфф.  a*x+b=y
  uint8_t YesNumWave; // индекс текущей длины волны из списков калибровочных и специальных
  k_c =1.0;
  // 1. определим по длине волны текущий индекс
  if (Lambda < 800)
  {
    YesNumWave = SearchIndxNW(Lambda);
    if(YesNumWave)  IndxNumWave = YesNumWave;// пропишем соотв. CurrNumWave
    else
    {
      IndxNumWave = 6;
      Lambda = 1650;
    }
  }
  else if (Lambda < 900)
  {
    IndxNumWave =0;
  }
  else if (Lambda < 1210)
  {
    YesNumWave = SearchIndxNW(Lambda);
    if(YesNumWave)  IndxNumWave = YesNumWave;// пропишем соотв. CurrNumWave
    else // нет спец волн в диапазоне "900-1210" надо переключатся через ветвление
    {
      if(Lambda==905)
      {
      IndxNumWave =1;
      Lambda = 1210;
      }
      else
      {
      IndxNumWave =0;
      Lambda = 900;
      }
    }
  }
  else if (Lambda <= 1300)
  {
    IndxNumWave =1;
  }
  else if (Lambda <= 1310)
  {
    IndxNumWave =2;
  }
  else if (Lambda <= 1490)
  {
    IndxNumWave =3;
  }
  else if (Lambda <= 1550)
  {
    IndxNumWave =4;
  }
  else if (Lambda <= 1625)
  {
    IndxNumWave =5;
  }
  else if (Lambda <= 1650)
  {
    IndxNumWave =6;
  }
  else
  {
    YesNumWave = SearchIndxNW(Lambda);
    if(YesNumWave)  IndxNumWave = YesNumWave;// пропишем соотв. CurrNumWave
    else
    {
      IndxNumWave =0;
      Lambda = 800;
    }
  }
  // здесь имеем текущий индекс
  // 2. Что за режим, если надо переключатся по калибровочным и спец длинам , то устанавливаем 
  // лямбду и базовый уровень смещения
  
  
  // сначала проверим и попрыгаем, переключим длины волн по калиброванным или специальным
  // контроль Mode
  if(Mode) // переключаемся по индексу, пересчитываем и выводим, переключение по калиброванным длинам волн
  {
    if(Mode == 1) // increment
    {
      if(IndxNumWave<WAVE_LENGTHS_NUM-1) // до 1625, основные длины волн 
      {
        if(Lambda>=CalibrLW[IndxNumWave])IndxNumWave++;
        //IndxNumWave = (IndxNumWave)?(IndxNumWave):(IndxNumWave++);
        //if(IndxNumWave == 7) IndxNumWave=0; // перешагнули через 1650, перескок на 850
      }
      else // или 1650 или есть не стандартные
      {
        YesNumWave=0;// поиск нестандартной
        for(i=IndxNumWave; i<(WAVE_LENGTHS_NUM+WAVE_LENGTHS_SPEC-1); i++)
        {
          if( SpecCoeffPM.PointSpecKalib[i-(WAVE_LENGTHS_NUM-1)])
          {
            YesNumWave = i+1;
            break;
          }
        }
        IndxNumWave = YesNumWave;
      }
    }
    // в обратную сторону
    if(Mode == -1)
    {
            if((IndxNumWave>0) && (IndxNumWave<WAVE_LENGTHS_NUM)) //1...7
      {
        IndxNumWave--;
      }
      else
      {
        YesNumWave=0;
        for(i=(IndxNumWave)?(IndxNumWave-WAVE_LENGTHS_NUM):(WAVE_LENGTHS_SPEC); i>0; i--)//IndxNumWave;(WAVE_LENGTHS_NUM+WAVE_LENGTHS_SPEC)
        {
          if(SpecCoeffPM.PointSpecKalib[i-1])
          {
            YesNumWave = i;
            break;
          }
        }
        IndxNumWave = WAVE_LENGTHS_NUM + YesNumWave -1;
      }
    }
    // проверка после изменения что за длина волны и установка соотв параметров, сначала из списка калибровонных длин волн и специфических
//    if(IndxNumWave<WAVE_LENGTHS_NUM) // если длина волны из основного списка калибровонных длин волн, то Д.Б. сказал что надо рассчитывать как обычно
//      // то есть через спектральный коэффициент и поправку на диапазоне
//    {
//      Lambda = CalibrLW[IndxNumWave];
//      CurrBaseLevel = UserMeasConfig.BaseLvl[IndxNumWave];
//      k_c = CoeffPM.CoefPointKlb[IndxNumWave];
//    }
//    else
//    {
//      CurrBaseLevel = UserMeasConfig.BaseSpecKlb[IndxNumWave-WAVE_LENGTHS_NUM];
//      Lambda =  SpecCoeffPM.PointSpecKalib[IndxNumWave-WAVE_LENGTHS_NUM];
//      k_c =  SpecCoeffPM.CoefSpecKlb[IndxNumWave-WAVE_LENGTHS_NUM];
//    }
//    //  UserMeasConfig.BaseLevel = CurrBaseLevel; 
//    PMWavelenght = Lambda;
//    return k_c;  
    
    // проверка на специфическую длину волны, и установка соотв параментов, иначе расчет спектр. коэфф. по длине волны как обычно
    if(IndxNumWave>=WAVE_LENGTHS_NUM) // если длина волны из спец списка списка калибровонных длин волн, то Д.Б. сказал что надо рассчитывать как обычно
      // то есть через спектральный коэффициент и поправку на диапазоне
    {
      CurrBaseLevel = UserMeasConfig.BaseSpecKlb[IndxNumWave-WAVE_LENGTHS_NUM];
      Lambda =  SpecCoeffPM.PointSpecKalib[IndxNumWave-WAVE_LENGTHS_NUM];
      k_c =  SpecCoeffPM.CoefSpecKlb[IndxNumWave-WAVE_LENGTHS_NUM];
    PMWavelenght = Lambda;
    return k_c;  
    }
    //  UserMeasConfig.BaseLevel = CurrBaseLevel; 
    //попереключались по длинам волн калибровки, установим длину волны, базовый уровень 
      Lambda = CalibrLW[IndxNumWave];
      CurrBaseLevel = UserMeasConfig.BaseLvl[IndxNumWave];
      //k_c = CoeffPM.CoefPointKlb[IndxNumWave];
    PMWavelenght = Lambda;
  }
  // здесь установлена лямбда для которой надо рассчитать спектральный коэффициент, и установить базовый уровень
  // проверим LAmbda не равна одной из длин волн калибровки, если да то перепишем CurrBaseLevel
  for(int j=0;j<WAVE_LENGTHS_NUM;j++)
  {
    if(Lambda == CalibrLW[j])
    {
            CurrBaseLevel = UserMeasConfig.BaseLvl[j];
    }
  }
  // разбор и установка спектрального коэфф для данной длины волны
  if(Lambda<800) // Если указана  длина волны  вне основного диапазона
  {
    // переберем список специальных длин волн
    YesNumWave = SearchIndxNW(Lambda);
    CurrBaseLevel = UserMeasConfig.BaseSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    Lambda =  SpecCoeffPM.PointSpecKalib[YesNumWave-WAVE_LENGTHS_NUM];
    k_c =  SpecCoeffPM.CoefSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    PMWavelenght = Lambda;
    return k_c;
  }
  
  // 800 ... 900 
  if(Lambda<=900)                      // Первый диапазон L-диапазон 800...900нм
  {
    i=(int)((Lambda-800)/20);          // Шаг между спектральными коэффициетами - 20 нм
    
    
    a = (CoeffPM.CoefSpctrL[i+1]-CoeffPM.CoefSpctrL[i])/20;   //a = (y2-y1)/(x2-x1)=(y2-y1)/20
    b = CoeffPM.CoefSpctrL[i]-a*(800+i*20);                   //b = y1-a*x1 = y - a*(820нм)
    
    k_c = a*Lambda+b;
    k_c = k_c*CoeffPM.CoefTune[0]; // for 850 nm
    PMWavelenght = Lambda;
    return k_c;
  }
  // 
  //не существующий диапазон расчитаем из точек 900 - 1210 
  //
  if(Lambda<1210)                     // 
  {
    // переберем список специальных длин волн
    YesNumWave = SearchIndxNW(Lambda);
    CurrBaseLevel = UserMeasConfig.BaseSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    Lambda =  SpecCoeffPM.PointSpecKalib[YesNumWave-WAVE_LENGTHS_NUM];
    k_c =  SpecCoeffPM.CoefSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    PMWavelenght = Lambda;
    return k_c;
  }
  // 1210.... 1390
  if(Lambda<=1390)                     // Второй диапазон M-диапазон 1210...1390нм
  {
    i=(int)((Lambda-1210)/20);          // Шаг между спектральными коэффициетами - 20 нм
    
    
    a = (CoeffPM.CoefSpctrM[i+1]-CoeffPM.CoefSpctrM[i])/20;   //a = (y2-y1)/(x2-x1)=(y2-y1)/20
    b = CoeffPM.CoefSpctrM[i]-a*(1210+i*20);                   //b = y1-a*x1 = y - a*(820нм)
    
    k_c = a*Lambda+b;
    k_c = k_c*CoeffPM.CoefTune[1];
    PMWavelenght = Lambda;
    return k_c;
  }
  // 1390... 1410
  if(Lambda<1410)                       // "Окно" 1391...1409 нм
  {
    a = (CoeffPM.CoefSpctrH[0]*CoeffPM.CoefPointKlb[4]-CoeffPM.CoefSpctrM[9]*CoeffPM.CoefPointKlb[3])/20;   //a = (y2-y1)/(x2-x1)=(y2-y1)/20
    b = CoeffPM.CoefSpctrM[9]*CoeffPM.CoefPointKlb[3]-a*1390;                   //b = y1-a*x1 = y - a*(820нм)
    
    k_c = a*Lambda+b;
    //k_c = k_c;	// #???
    k_c = k_c*CoeffPM.CoefTune[1];
    PMWavelenght = Lambda;
    
    return k_c;
  }  
  // 1410... 1650
  if(Lambda<=1650)                     // Третий диапазон H-диапазон 1410...1650нм
  {
    i=(int)((Lambda-1410)/20);          // Шаг между спектральными коэффициетами - 20 нм
    
    
    a = (CoeffPM.CoefSpctrH[i+1]-CoeffPM.CoefSpctrH[i])/20;   //a = (y2-y1)/(x2-x1)=(y2-y1)/20
    b = CoeffPM.CoefSpctrH[i]-a*(1410+i*20);                   //b = y1-a*x1 = y - a*(820нм)
    
    k_c = a*Lambda+b;
    k_c = k_c*CoeffPM.CoefTune[2];
    PMWavelenght = Lambda;
    
    return k_c;
  }
  // 1650... 1700
  if(Lambda<=1700)                     // не существующий диапазон расчитаем из точек 1650 - 1700(1650*1.1) прямой апроксимацией
  {
    // переберем список специальных длин волн
    YesNumWave = SearchIndxNW(Lambda);
    CurrBaseLevel = UserMeasConfig.BaseSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    Lambda =  SpecCoeffPM.PointSpecKalib[YesNumWave-WAVE_LENGTHS_NUM];
    k_c =  SpecCoeffPM.CoefSpecKlb[YesNumWave-WAVE_LENGTHS_NUM];
    PMWavelenght = Lambda;
    return k_c;
  }
  return k_c;
  
}

double GetPower(unsigned int Lambda)      // Получаем мощность в нВт в зависимости от длины волны
{
  //  
  //  char M_PON;
  //  float clck_K;
  //  clkP++;
  
  int i;                          // Счётчик
  int Res_Now;                    // Текущее значение АЦП с учетом смещения
  static float Lev_x;                    // Усредненные значения АЦП: среднее значение Level[0..39]
  static double Lev_W=1e-9;
  static int Res_Old=0;                 // Предыдущее усредненное значение АЦП с учетом смещения
  static int Level[SIZE_BUF];         // Накопленные значения АЦП с учетом смещения
  static uint8_t OldRange=0;
  //HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
   //   GPIOE->BSRRL = GPIO_PIN_5; // (PE5) set pin
  
  Res_Now = FreeCodeADC_PM - CoeffPM.ShZeroRng[Range];        // Получаем результат с учетом смещения
  
  //  данные готовы!
  
  if (abs(Res_Old - Res_Now)>10000)                      // Перезапишем буфера при скачке показаний измерения
  {
    for (i=0; i<SIZE_BUF; i++)
      Level[i]=Res_Now;    
  }
  
  for (i=SIZE_BUF-1; i>=1; i--)
    Level[i]=Level[i-1];
  Level[0] = Res_Now; 
  
  
  Lev_x = 0.;                                            // Считаем среднее значение показаний АЦП
  for (i=0; i<SIZE_BUF; i++)Lev_x +=Level[i];
  Lev_x = Lev_x/SIZE_BUF;
  // замена строк местами, сначала проверяем потом устанавливаем
  Res_Old = (int)Lev_x;                                      // Запомнили значение, как "старое"
  if (Lev_x<=0)     Lev_x = 1.0;                          // Если вдруг значение меньше или равно нулю
  
  //Lev_W = ( 7.9261495712e-7 * Lev_x  * CoeffPM.CoefStykRange[Range] * GetCoeffSpctr(Lambda)) / pow( 100.0, (double)(3-Range) ); // собственно расчет мощности в миливаттах 
  Lev_W =  (double)(Lev_x);
  //Lev_W =  Lev_W * ADC_STEP ;
  Lev_W =  Lev_W * 5.0e-7 ; // как у константиныча! без учета пересчета значения кода АЦП
  Lev_W = Lev_W  * CoeffPM.CoefStykRange[Range];
  //Lev_W = Lev_W * GetCoeffSpctr(Lambda); // без привязки к длине волны
  Lev_W = Lev_W  / pow( 100.0, (double)(3-Range)); // собственно расчет мощности в миливаттах 
  
  if (GetModePM())                          // Автоматическое переключение диапазонов
  {

    // Границы гистерезиса переключений
    // чтобы работал с 270 Гц надо снизить уровень переключения до 6000000 = 5B8000
    // чтобы работал с 270 Гц надо снизить уровень переключения до 5000000 = 4C4B40 03.02.2016 не работало на высоком
    if ((Lev_x>=4150000) && (Range<3))                     //Ежели переполнение не на самом грубом диапазоне, (более 8000000 с вычетом смещения)
    {
      Range++;                                          //переключаемся на менее чувствительный диапазон
      SetRange(Range);
      //CreatDelay (5000000);
    }
    if (Lev_x < 30000 && Range>0)                       //Ежели получена слишком малая величина не на самом чувствительном пределе,
    {
      Range--;                                          //переключаемся на более чувствительный диапазон
      SetRange(Range);
      //CreatDelay (5000000);
    }
  }
  if (Range!=OldRange)
  {
  //    GPIOE->BSRRH = GPIO_PIN_5; // (PE5) reset pin
  //   HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_5);
     OldRange=Range;
  }
  
  
  
  
  //    switch(GetCfgPM())
  //    {
  //    case 1:
  //      if (Lev_W<1e-9)Lev_W = 1e-9;                          //ограничение показаний на уровне 1pW 
  //      break;
  //    case 2:
  //      if (Lev_W<1e-6)Lev_W = 1e-6;                          //ограничение показаний на уровне 1pW 
  //      break;
  //    }
  
  return Lev_W;
}

// Установка и запись нового базового уровня при привязке для P1
void SetNewBaseLevel (void)
{
           // поиск индекса в списке специальных
         uint8_t yLW = SearchIndxNW (PMWavelenght);
         if(yLW) 
         {
           SpecCoeffPM.PointSpecKalib[yLW-(WAVE_LENGTHS_NUM-1)]= CurrBaseLevel;
         }
         else  
           // поищем среди калибованных
         {
           for(int j=0;j<WAVE_LENGTHS_NUM;j++)
           {
             if(PMWavelenght == CalibrLW[j])
             {
               UserMeasConfig.BaseLvl[j] = CurrBaseLevel;
               break;
             }
           }
         }

  
}

// Настройка нулевого смещения
//void SettingZeroPoints (void)
//{
//  int BufMed[20];
//  int Medium[4];
//  // Print Msg Go
//  sprintf((void*)Tx_Buff,"Go "); // // получение диапазона текущего; //
//  HAL_UART_Transmit_DMA(&huart3, (void*)Tx_Buff,strlen((void*)Tx_Buff)); // выдаем ответ установки
//  TransmitU = DATA_TX;
//  // set MANUAL Mode
//  SetModePM(0);
//  SetRange(0);
//  osDelay(3000);
//  for (int i=0; i<4; ++i)
//  {
//    // set New Range
//    SetRange(i);
//    osDelay(1500);
//    memset(&BufMed, 0, sizeof(BufMed));  
//    Medium[i]=0;
//    
//    for (int j=0; j<20; ++j)
//    {
//      BufMed[i] = GetPMDataNorm();
//      Medium[i] += BufMed[i];
//      //      sprintf((void*)Tx_Buff,"%d\n",BufMed[i]); // // получение диапазона текущего; //
//      //      HAL_UART_Transmit_DMA(&huart3, (void*)Tx_Buff,strlen((void*)Tx_Buff)); // выдаем ответ установки
//      //      TransmitU = DATA_TX;
//      osDelay(500);
//    }
//    // get Level on Current Range
//    CoeffPM.ShZeroRng[i]=Medium[i]/20;
//    // Print Level on LowRange
//    //      sprintf((void*)Tx_Buff,"%d,k=%d\n ",Medium[i],CoeffPM.ShZeroRng[i]); // // получение диапазона текущего; //
//    sprintf((void*)Tx_Buff,"%d, ",CoeffPM.ShZeroRng[i]); // // получение диапазона текущего; //
//    HAL_UART_Transmit_DMA(&huart3, (void*)Tx_Buff,strlen((void*)Tx_Buff)); // выдаем ответ установки
//    TransmitU = DATA_TX;
//    
//  }
//  // set AUTO Mode
//  SetModePM(1);
//  // Save Setting
//  EEPROM_write(&CoeffPM, ADR_CoeffPM, sizeof(CoeffPM));
//  
//  osDelay(50);
//  // Print Msg End
//  sprintf((void*)Tx_Buff,"End\r"); // // получение диапазона текущего; //
//  HAL_UART_Transmit_DMA(&huart3, (void*)Tx_Buff,strlen((void*)Tx_Buff)); // выдаем ответ установки
//  TransmitU = DATA_TX;
//}
//
//
void InitTableCoeff (void) // инициализация коэффициентов настройки измерителя
{
  int i;
  for (i=0; i<4;i++)
  {
    CoeffPM.ShZeroRng[i] = 0;
    CoeffPM.CoefStykRange[i] = 1.0;
  }
  for (i=0; i<6;i++)
  {
    CoeffPM.CoefSpctrL[i] = 1.0;
  }
  for (i=0; i<10;i++)
  {
    CoeffPM.CoefSpctrM[i] = 1.0;
  }
  for (i=0; i<13;i++)
  {
    CoeffPM.CoefSpctrH[i] = 1.0;
  }
  for (i=0; i<WAVE_LENGTHS_NUM;i++)
  {
    CoeffPM.CoefPointKlb[i] = 1.0;
  }

}

        // среднее значение и буффер для Р2
void CalkP2Mode (void)
{
      // среднее значение и буффер для Р2
      P2SUM=0;
      P2SUMmW=0;
      
      for (int n=0;n<ADCSIZE-1;n++)
      {
        BuffP2[n] = BuffP2[n+1];
        P2SUM +=BuffP2[n];
      }
      BuffP2[ADCSIZE-1]=IN_BuffADC[1];       //au?aneaiiia a aAi , aey aaeuiaeoae ia?aaioee
      //BuffP2[ADCSIZE-1]=(IN_BuffADC[1]>ADC_Calibr)?(IN_BuffADC[1]-ADC_Calibr):(IN_BuffADC[1]);       //22.08.2023 - перезапишем значение по новому, так как есть калибровка АЦП
      P2SUM +=BuffP2[ADCSIZE-1];
      MediumP2 = (float)P2SUM/ADCSIZE; // среднее значение кодов АЦП
      if(MediumP2<4096.0)// посчитаем значения для индикации и памяти!
      {
        // перасчитаем пороги и новое значение для индикации по Д.Б. (09.08.2022)
        if(MediumP2>P2UP_Lim)
        {
          MediumP2_Ind = MediumP2_Ind + (MediumP2 - P2UP_Lim);
          P2UP_Lim = MediumP2;
          P2DN_Lim = P2UP_Lim - DELTAP2; 
        }
        else if (MediumP2<P2DN_Lim)
        {
          MediumP2_Ind = MediumP2_Ind - (P2DN_Lim - MediumP2 );
          P2DN_Lim = MediumP2;
          P2UP_Lim = P2DN_Lim  + DELTAP2; 
        }
        // значение Р2 в дБм для индикации прошло через фильтр от порога изменений в 
        P2dBmValue = (MediumP2_Ind - (float)DeviceConfig.SmPx[IndxP2LW])*DeviceConfig.Kprop;
        P2dBValue =  P2dBmValue - UserMeasConfig.BaseLvlP2[IndxP2LW];
      }
      //
      TestCntWait++; // инкремент счетчика измерения Р2
      // посчитаем значения ORL (с привязкой к длине волны ORL (хитро для измерений в режиме автомат)
      PowP2_mW = pow(10.0,(P2dBmValue/10.0));
      //calk medium P2 in mW
            for (int n=0;n<ADCSIZE-1;n++)
      {
        BuffP2mW[n] = BuffP2mW[n+1];
        P2SUMmW +=BuffP2mW[n];
      }
      BuffP2mW[ADCSIZE-1]=PowP2_mW;       //в последнюю пишем что только что посчитали
      P2SUMmW +=BuffP2mW[ADCSIZE-1];
      MedP2mW = (float)P2SUMmW/ADCSIZE;
      // переназначим среднее
      //PowP2_mW = MedP2mW;
      if ((MedP2mW - PowClbr_mW)>0)
        MedP2mW_corr = MedP2mW - PowClbr_mW;
      else
        MedP2mW_corr = 0.00000001; // 10pW)
        
      // теперь можно просдеить изменения по мощности
      //if(MedP2mW<0.001)// посчитаем значения для индикации и памяти!(<1мкВт)
      //{
        if((MedP2mW_Ind>MedP2mW+DELTAP2mW)||(MedP2mW_Ind<MedP2mW-DELTAP2mW))
          MedP2mW_Ind = MedP2mW;
        // перасчитаем пороги и новое значение для индикации по Д.Б. (09.08.2022)
        if(MedP2mW>P2UPmW_Lim)
        {
          MedP2mW_Ind = MedP2mW_Ind + (MedP2mW - P2UPmW_Lim);
          P2UPmW_Lim = MedP2mW;
          P2DNmW_Lim = P2UPmW_Lim - DELTAP2mW; 
        }
        else if (MedP2mW<P2DNmW_Lim)
        {
          MedP2mW_Ind = MedP2mW_Ind - (P2DNmW_Lim - MedP2mW );
          P2DNmW_Lim = MedP2mW;
          P2UPmW_Lim = P2DNmW_Lim  + DELTAP2mW; 
        }
        // значение Р2 в дБм для индикации прошло через фильтр от порога изменений в 
        //P2dBmValue = (MediumP2_Ind - (float)DeviceConfig.SmPx[IndxP2LW])*DeviceConfig.Kprop;
        //P2dBValue =  P2dBmValue - UserMeasConfig.BaseLvlP2[IndxP2LW];
        //  PowP2_mW = MedP2mW_Ind;
      //}
//      // старый рассчет от
//      ORLCalcFree = CoeffPM.BaseRet[indx_LW_ORL]-(10*log10(PowP2_mW - PowClbr_mW)); // мгновенное
//      ORLCalc = ORL_LVL_P; // присваиваем макс уровень
//      if ((PowP2_mW - PowClbr_mW)>0)
//      {
//        ORLCalc = CoeffPM.BaseRet[indx_LW_ORL]-(10*log10(PowP2_mW - PowClbr_mW)); // мгновенное
//        // если вдруг посчиталось как то не так и получили более 60 снова зарубим до 60
//        if(ORLCalc >= ORL_LVL_P) 
//          ORLCalc = ORL_LVL_P; // присваиваем макс уровень
//      }
      // старый рассчет от
      //ORLCalcFree = CoeffPM.BaseRet[indx_LW_ORL]-(10*log10(PowP2_mW)); // мгновенное
      ORLCalcFree = BaseRetCurrORL[indx_LW_ORL]-(10*log10(PowP2_mW)); // мгновенное
      ORLCalc = ORL_LVL_P; // присваиваем макс уровень
      if ((MedP2mW_Ind - PowClbr_mW)>0)
      {
        //ORLCalc = CoeffPM.BaseRet[indx_LW_ORL]-(10*log10(MedP2mW_Ind - PowClbr_mW)); // мгновенное
        ORLCalc = BaseRetCurrORL[indx_LW_ORL]-(10*log10(MedP2mW_Ind - PowClbr_mW)); // мгновенное
        // если вдруг посчиталось как то не так и получили более 60 снова зарубим до 60
        if(ORLCalc >= ORL_LVL_P) 
          ORLCalc = ORL_LVL_P; // присваиваем макс уровень
      }
        //ORLCalcCorr = CoeffPM.BaseRet[indx_LW_ORL]-(10*log10(MedP2mW_corr)); // среднее скоррект
        ORLCalcCorr = BaseRetCurrORL[indx_LW_ORL]-(10*log10(MedP2mW_corr)); // среднее скоррект

}
//void InitScreenPM (void) // инициализация структуры индикатора измерителя
//{
//  for (int i =0;i<LSPLACENUM;++i)
//  {
//    g_ScreenPM[i].iStateCH = S_FREE; // признак что не надо выводить на табло
//    g_ScreenPM[i].iLW_CH = 0;        // индицируемая длина волны
//    g_ScreenPM[i].fRef_CH = 0.;      // опора из разных источников в зависимости от режима для данной длины волны
//    g_ScreenPM[i].fRef_CH_FA = 0.;      // опора из разных источников в зависимости от режима для данной длины волны jn bcnjxybrf
//    g_ScreenPM[i].fValuePM_CH = 0.;  // значение мощности в миливатах, для расчета в дБ
//    g_ScreenPM[i].fValueDBM_CH = -100.;  // значение в дБм
//    //
//    g_RecAutoPM[i].fValuePM_CH =0.; // значение мощности, для данной длины волны
//    g_RecAutoPM[i].iCountStateCH = TIME_MAXOFF; // долго не обновлялись надо сбросить длину волны (old 100)
//    g_RecAutoPM[i].iLW_CH = 0; // нет длины волны, снимаем признак в индикации соотв
//    g_RecAutoPM[i].fPowScFA_CH = 0.;  // значение мощности источника при приеме в режиме полный авто
//
//  }
//}
//
//void SetLasersAsMode(void)
//{
//  static uint8_t SetActivLS = 1;
//  static int16_t iIndxLSCurr;
//  static int16_t Oldg_iActivePlace = 0;
//  
//  if(GetMultipageMode() == MPAGE_TESTER_MODE)  // вкладка в режиме тестера
//  {
//    if(g_iActivePlace>=0)
//    {
//      if(SetActivLS == 0) // только включили лазер (надо перенастроить)
//      {
//        SetActivLS = 1;
//        SetModeLSDefault (1);
//        //g_CountRepetCWADD=0;
//        // здесь включим  тот лазер который нужен
//        SetOnLaserPlace (g_iActivePlace); //включение лазера от места
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
//        Oldg_iActivePlace = g_iActivePlace;
//      }
//      if(Oldg_iActivePlace != g_iActivePlace) // изменилась длина волны источника
//      {
//        SetOnLaserPlace (g_iActivePlace); //включение лазера от места
//        Oldg_iActivePlace = g_iActivePlace;
//      }
//      // проверим режимы и установим лазеры
//      // если есть режим CW* то необходимо изменить активный лазер
//      if(g_aPlaceMode[g_iActivePlace] == 2) //2=LS_MODE_CW_ADD
//      {
//        if(g_CountRepetCWADD>TIME_CHNG_SC) // 100- на период опроса в рефлекте , получается 3+ сек (100) Сделали чуть больше
//          // если в таймере 0.4*8сек = 2.8S = 0.15*19 
//        {
//          g_CountRepetCWADD = 0; // надо выбрать новую длинну волны и переключить лазер
//          for(iIndxLSCurr=g_iActivePlace+1; iIndxLSCurr < g_iActivePlace+5; ++iIndxLSCurr)
//          {
//            if(g_aPlaceMode[iIndxLSCurr&0x3]==2)//LS_MODE_CW_ADD
//            {
//              g_iActivePlace = iIndxLSCurr&0x3;
//              UpdateLabels();  // вызов перерисовки если изменили здесь     
//              break;
//            }
//          }
//          // здесь включим  тот лазер который нужен
//          SetOnLaserPlace (g_iActivePlace); //включение лазера от места
//          osDelay(100);
//        //}
//        //if(g_CountRepetCWADD==1) // Send optic data
//        //{
//          // Send LW to RS_Optics
//          //sprintf((char*)Tx_BuffOptic, "########SL%04d:%04d\r\xffUUUUU",g_ConfigDevice.PlaceLS[g_iActivePlace], 9999);
//          //sprintf((char*)Tx_BuffOptic, "########SL%04d %.2f\r\xffUUUUU",g_ConfigDevice.PlaceLS[g_iActivePlace], g_SetLS_InOutParam.fPW_outLS[g_iActivePlace]);
//          // попробуем как в маленьких приборах
//          sprintf((char*)Tx_BuffOptic, "UUUU####SL%04d %.2f\r\xffUUU",g_ConfigDevice.PlaceLS[g_iActivePlace], g_SetLS_InOutParam.fPW_outLS[g_iActivePlace]);
//          //CntRXByte = 0;
//          HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
//                   osDelay(20); // без этой задержки в буффере мог быть мусор
//          HAL_UART_Transmit_DMA(&huart6, Tx_BuffOptic, strlen((char*)Tx_BuffOptic)); // выдаем ответ установки
//          TransmitUOpt = DATA_TX;
//        }
//        
//      }
//      g_StatLSOut =  g_ConfigDevice.PlaceLS[g_iActivePlace] + 0x8000;
//    }
//    else // 
//    {
//      if(SetActivLS == 1) // только выключили лазер (надо перенастроить)
//      {
//        SetActivLS = 0;
//        SetModeLSDefault (1);
//        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_SET);
//      }
//      g_StatLSOut = g_ConfigDevice.PlaceLS[Oldg_iActivePlace];
//    }
//    //CntrlExpUnit (OFF, EN_CH1); // выключение laser ch1
//    //CntrlExpUnit (OFF, EN_CH2); // вЫключение laser ch2
//    //g_aPlaceMode[] // состояния лазеров
//    //  g_iActivePlace = -1; // если активный лазер
//  }
//  
//}
//
//void SetOnLaserPlace (int Mode) //включение лазера от места
//{
//  switch(Mode)
//  {
//  case 0:
//    CntrlExpUnit (ON, EN_CH1); // включение laser ch1
//    CntrlExpUnit (OFF, EN_CH2); // вЫключение laser ch2
//    CntrlExpUnit (OFF, EN_CH3); // вЫключение laser ch3
//    CntrlExpUnit (OFF, EN_CH4); // вЫключение laser ch4
//    break;
//  case 1:
//    CntrlExpUnit (OFF, EN_CH1); // вылючение laser ch1
//    CntrlExpUnit (ON, EN_CH2); // включение laser ch2
//    CntrlExpUnit (OFF, EN_CH3); // вЫключение laser ch3
//    CntrlExpUnit (OFF, EN_CH4); // вЫключение laser ch4
//    break;
//  case 2:
//    CntrlExpUnit (OFF, EN_CH1); // вылючение laser ch1
//    CntrlExpUnit (OFF, EN_CH2); // вЫключение laser ch2
//    CntrlExpUnit (ON, EN_CH3); // включение laser ch3
//    CntrlExpUnit (OFF, EN_CH4); // вЫключение laser ch4
//    break;
//  case 3:
//    CntrlExpUnit (OFF, EN_CH1); // выключение laser ch1
//    CntrlExpUnit (OFF, EN_CH2); // вЫключение laser ch2
//    CntrlExpUnit (OFF, EN_CH3); // вЫключение laser ch3
//    CntrlExpUnit (ON, EN_CH4); // включение laser ch4
//    break;
//  default:
//    CntrlExpUnit (OFF, EN_CH1); // выключение laser ch1
//    CntrlExpUnit (OFF, EN_CH2); // вЫключение laser ch2
//    CntrlExpUnit (OFF, EN_CH3); // вЫключение laser ch3
//    CntrlExpUnit (OFF, EN_CH4); // вЫключение laser ch4
//    break;
//  }
//  
//}
//
//uint16_t SetUartCmdLS (uint8_t Mode, uint8_t Channel)
//{
//  uint16_t Status = 0;
//  for (int i =0;i<LSPLACENUM;++i) // все выключаем
//  {
//    g_aPlaceMode[i] = 0;
//    g_iActivePlace = -1;
//    //g_aPlaceMode[g_iActivePlace]
//  }
//  if (Mode)
//  {
//    Status = g_ConfigDevice.PlaceLS[Channel-1];
//    if(Status) // есть ли лазер        
//    {
//      g_iActivePlace = Channel-1;
//      g_aPlaceMode[g_iActivePlace] = 1;
//    }
//    
//  }
//  SetMultipageMode(MPAGE_TESTER_MODE, 0); // вызываем после изменения
//  //UpdateLabels();  // вызов перерисовки если изменили здесь     
//  return Status;
//}