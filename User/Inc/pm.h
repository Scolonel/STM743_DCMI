#ifndef __PM_H__
#define __PM_H__

// состояние работы АЦП - AD7782
#define FREEADC 0 // АЦП можно запускать
#define READYDATA 1 // данные готовы
#define BUSYADC 2 // АЦП занято
#define PM_CS(a)    ((a>0)?(PM_CS_GPIO_Port->BSRR = PM_CS_Pin):(PM_CS_GPIO_Port->BSRR = (uint32_t)PM_CS_Pin << 16))  /* set CS_PM PB.04 to high/low */
#define PM_CLK(a)    ((a>0)?(PM_SCLK_GPIO_Port->BSRR = PM_SCLK_Pin):(PM_SCLK_GPIO_Port->BSRR = (uint32_t)PM_SCLK_Pin << 16))  /* set CLK_PM PB.08 to high/low */
#define GET_PM_DATA    HAL_GPIO_ReadPin(PM_DOUT_GPIO_Port, PM_DOUT_Pin)  /* set DATA_PM PB.05 to high/low */
//#define ADC_STEP (double)(7.9261495712E-7)
#define ADC_STEP (double)(2.5/2^24)

//#define KEY1(a)       CtrlExpand (COMMCTRL, PM_K1, ((a>0)?(DSBL):(ENBL))) // активный 1
//#define KEY2(a)       CtrlExpand (COMMCTRL, PM_K2, ((a>0)?(DSBL):(ENBL))) // активный 1
//#define KEY3(a)       CtrlExpand (COMMCTRL, PM_K3, ((a>0)?(DSBL):(ENBL))) // активный 1
  /* set K1 to high/low */



// ногоДрыганное чтение данных из АЦП
uint32_t ReadDataADC7782 (void);

// 
uint32_t GetPMData(void);              // Читает данные из АЦП
//void SetRange(unsigned char Rn);                            // Установка ключей согласно выбранного диапазона
uint8_t GetRange(void);                        // Возвращает текущий диапазон работы измерительного усилителя
uint8_t GetModePM (void); // получить режим измерителя
uint8_t SetModePM( uint8_t Mode); // установить  режим измерителя
uint32_t GetPMDataNorm(void);   // получение кодов АЦП нормированные     

float GetCoeffSpctr(uint16_t Lambda, int8_t Mode);   // получение спектрального коэффициента
double GetPower(unsigned int Lambda);       // расчет мещности для заданной длины волны
double GetPMValue (void); //получение текущей мощности, без привязки к спектральному коэффициенту!
float GetPMValuedBm (double PowOpt); //получение текущей мощности в dBm
float GetPMValuedB (double PowOpt); //получение текущей мощности в dB
// Установка и запись нового базового уровня при привязке для P1


void SetNewBaseLevel (void);

// Настройка нулевого смещения
//void SettingZeroPoints (void);

void InitTableCoeff (void); // инициализация коэффициентов настройки измерителя

        // среднее значение и буффер для Р2
void CalkP2Mode (void);

//void InitScreenPM (void); // инициализация структуры индикатора измерителя
//void SetLasersAsMode(void);
//void SetOnLaserPlace (int Mode); //включение лазера от места
//uint16_t SetUartCmdLS (uint8_t Mode, uint8_t Channel); // установка лазеров по команде UART
// Variables
extern uint16_t PMWavelenght;  // Текущая длина волны
extern uint16_t P1aWavelenght;  // Текущая длина волны P1a

extern float CurrBaseLevel; // текущее значение базового уровня для данной длины волны
extern float OldBaseLevel; // старое значение базового уровня
extern double OptPow; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
extern float Opt_dB; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
extern float Opt_dBm; //Текущее значение мощности в зависимости от установленного диапазона с привязкой к длине волны, обновляется в ShowPM
extern float CurrSpectrKoeff; // текущий спектральный коэфф, расчитывается и устанавливается при изменении длины волны
extern float volatile RPON[WAVE_LENGTHS_NUM]; // ячейки значений в режиме автомат P1a PON 0-850,1-1300,2-1310,3-1490,4-1550,5-1625,6-1650  
extern const uint16_t CalibrLW[WAVE_LENGTHS_NUM];

extern float P2dBmValue;                 // Текущее значение мощности для Р2 в зависимости от установленного диапазона и выбранной длины волны 
extern float P2dBValue;                 // Текущее значение мощности Р2 зависимости от установленного диапазона и выбранной длины волны
extern float P2ORLValue; // текущее значение ORL по данным P2 (от кодов АЦП)

extern uint8_t IndxP2LW; // индекс длины волны калибровки для переключения в Р2
extern uint32_t BuffP2[ADCSIZE]; // буффер данных для P2 (Fifo)
extern float MediumP2;  // среднее значение АЦП Р2
extern float MediumP2_Ind; // среднее значение АЦП Р2 для расчета индикации

extern float P2UP_Lim; // верхний порог смены индикации
extern float P2DN_Lim; // нижний порог смены индикации

extern float BuffP2mW[ADCSIZE]; // буффер данных для P2 mW(Fifo)
extern float MedP2mW;  // среднее Р2 в миливатах
extern float MedP2mW_Ind; // среднее Р2 в миливатах для расчета индикации
extern float P2UPmW_Lim; // верхний порог смены индикации при миливатах
extern float P2DNmW_Lim; // нижний порог смены индикации при миливатах
extern float MedP2mW_corr;  // среднее Р2 в миливатах с коррекцией

extern uint32_t P2SUM;
extern float P2SUMmW;
// ORL - считаем всегда по где P2 - 
extern float BaseRetCurrORL[WAVE_LENGTHS_NUM]; // текущий уровень привязки для расчета ORL вяжется на длину волны, нужен для оперативной
// корректировки в "верху" (13-17 дБ ORL)

extern float ORLCalc; // расчетное значение ORL
extern float ORLCalcFree; // расчетное значение ORL
extern float ORLCalcCorr; // расчетное значение ORL
extern double PowP2_mW; // мощность на входе P2 в мВт, мгновенное
extern float PowClbr_mW; // мощность калибровочная для малых значений ORL в мВт
extern uint16_t indx_LW_ORL; // индекс длины волны измерения ORL, 

// P1a
extern uint32_t CntWaitLW;// счетчик ожидания длины волны в режиме P1a
extern uint32_t YesLW; // счетчик готовности длины волны для управления измерителем и получения данных, если 0 не считаем
extern uint8_t EnaOpticUART; // признак разрешения UARt2 (Optic)

//extern unsigned int g_CountRepetCWADD; // счетчик повтора переключения в режиме CW_ADD
//float GetCoefPointKlb(unsigned int Lambda);              // Aica?auaao aiiieieoaeuiue eiyooeoeaio aey eaeea?iaaiiuo aeei aiei
//
//
//// "Aiaoiea" ooieoee
//void SetSwitchPMMode(BYTE SwMode);          // Onoaiaaeeaaao ?a?ei ia?aee??aiey 
//                                            // eiyooeoeaioia oneeaiey ecia?eoaey iiuiinoe (?o?iie eee aaoiiaoe?aneee)
//unsigned int GetPMWavelenght(signed char DeltaLambda); //Aica?auaao, eeai eciaiyao oaeouo? aeeio aiei
//unsigned int SetPMWavelenght (unsigned int CurrPMWavelenght); // i?eioaeoaeuiay onoaiiaea oaeouae aeeiu aieiu
//float Watt2dB(char *str, float mWatt,BYTE TypeRslt);    // I?aia?acoao iAo a ieAo (iAo), aA eee aAi
//int GetTypeRslt();                          // Aica?auaao oaeouaa cia?aiea oeia aa. eca?aiey
//int SetTypeRslt(BYTE type);                 // Onoaiaaeeaaao iiaia cia?aiea oeia aa.ecia?aiey
//int AcquireShZeroRng(void);                 // Ecia?aiea o?iaiae niauaiey
//int AcquireShZeroLowRng(void);                          // Ecia?aiea o?iaiae niauaiey ia ie?iai aeaiaciia (ia?aonoaiiaea ioey)
//int SetDataADC (int Data); // onoaiiaea aaiiuo AOI a i?a?uaaiee
//void SetStateADC (BYTE State); // onoaiiaea ?a?eia AOI
//BYTE GetStateADC (void); // iieo?aiea ?a?eia AOI
//int AcquireCoefStykRange(BYTE Rng, float* PrevRng, float* CurrRng);         // Au?eneyao noueiau?iue eiyooeoeaio oaeouaai aeaiaciia n i?aauaouei
//float GetCoefSpctr(WORD Lambda, float RealPower);     // Aica?auaao niaeo?aeuiue eiyooeoeaio aey oaeouae aeeiu aieiu
//float GetCoefSpctrKlb(WORD index, float RealPower);    // Aica?auaao niaeo?aeuiue eiyooeoeaio aey eaeea?iai?iuo aeei aiei
//// onoaiiaea aeeiiu aieiu anee aee??ai Wide e JDSU iiaaa??ea
//void SetAutoLamdaJDSU (DWORD Freq);
//// oioei iieo?eou aeeiio aieiu ii ?anoioa
//unsigned int GetLambdaJDSUFreq (unsigned int Freq);
//



#endif  /* __PM_H__ */
