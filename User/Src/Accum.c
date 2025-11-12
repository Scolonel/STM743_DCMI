//===============================
#include "system.h"

// переменные счетчика Т2, предыдущее и новое считает
volatile DWORD OldTimeAccum; // предыдущее значение таймера накопления
volatile DWORD CurrTimeAccum; // текущее значение
uint8_t EnaTimerAccum =0; //признак разрешения счета таймера накопления
int CntAccumulat=0; // счетчик накоплений
static int WT_R = 0;



void RUN_SUM (DWORD* RawDataI);//


void SUMMER (DWORD* RawDataS)

{
  //надо запустить измерение с утановленными параметрами
      //memset(&BufADD, 0, sizeof(BufADD));
     LED_KTT(1); // начало одного суммир "МОДУЛЬ" 2 (~102мкС)
// блок задержки обнуления буффера накопления одного прохода
      memset(&BufNAK, 0, sizeof(BufNAK));
     LED_KTT(0);  // конец задержки "МОДУЛЬ" 2
     if(!DistBad)
     {
  TIM1->CNT = TIM1->CCR1 - 15;
  //TIM1->CNT = TIM1->CCR1 - (15 + (rand()%13)); // для "размазывания"
     }
  // запустим накопление 
  TIM1->CR1 |=TIM_CR1_CEN;

  // Это будет одно измерение с учетом прореживания
         // первое прерывание от TIM1_CCR1 пред начало счета ТИМ1
      while(Ena_AVRG)
      {
        //CountWait=0;
        //LED_START(1);
        while(Ena_CFG) // ждем установок для измерения
        {
        //  CountWait++;
        asm("NOP");
        }
       // LED_START(0);
       // BufNAK[0] = CountWait;
       // CountWait=0;
       // LED_START(1);
        while(Ena_SUMM) // вот тут идет накопление по DMA, ждем разрешения5 суммирования
        {
          // в ожидании "подергаемся" вычислениями
//         WT_R = rand()%13;
//        while(WT_R)
//        {
//          WT_R--;
//          asm("NOP");
//        }

       //   CountWait++;
        asm("NOP");
        }
       // LED_START(0);
       // BufNAK[1] = CountWait;
        // тут бы поизменять начало ссумирования
//        WT_R = rand()%25;
//        while(WT_R)
//        {
//          WT_R--;
//          asm("NOP");
//        }
             ContNextAvrg();
      }
    //StopAllTIM(1); // stop all timers
  // в буффере BufNAK лежит одно накопление 
     //LED_KT(1); // начало одного суммир
 // TST_KTA(1);

//   for (int i=0;i<RAWSIZE;i++)                                  // !!!!!!!!!!! 
//    RawDataS[i] += BufNAK[i];     // суммирование с шины данных непосредственно (есть тики для АЦП) !!!!!!!!!!!
    // другой способ копирования
    //    memcpy( RawDataS, BufNAK, RAWSIZE * sizeof(DWORD) );
    // LED_KT(0); // конец одног суммир
 // TST_KTA(0);

  //STARTPP(1); // устанавливаем в "1" START                            !!!!!!!!!!!
  //ADC_DATA_EN(1);// снимаем строб                            !!!!!!!!!!!
  //PointInPeriod ++;    // инкремент прореживания 
      Sm_Shift = 0; // текущее значение сдвига Зонд.Импульса
      CountDMA = 0; //clear count DMA

      Ena_AVRG = 1; // для цикла накоплений
      Ena_CFG = 1; // для установок цикла одного накопления
      Ena_SUMM = 1; // для ожидания начала суммирования накоплений

  asm("NOP");
}


void Averaging (int NumAccum,unsigned AddIndexLN, BYTE EnDrawGraph )// функция накопления с установкой задержки после накопления
// дополнительно устанавливаются: PointsPerPeriod - число точек на период, зависит от длины измерения
// NumAccumPerSec - число накоплений в данном фрэйме
// AddIndexLN - индекс по которому устанавливается период сбора для особых случаев
// исходно 0 - устанвливаем период согласно выбранного диапазона
// не 0 - устанвливает период согластно индекса диапазона оценивается в StartRunFirst
{ 
  //    CntPointPick=0;
  //TST_KTA(1);
  //CurrTimeAccum = HAL_GetTick();
  DistBad = AddIndexLN;
  OldTimeAccum = CurrTimeAccum;
  SetNumAccumPerSec (NumAccum);// запись числа накоплений
  // тут устанавливаются параметры съема и генерации зондирующего импульса при установленных ранее
  // нам надо установить свои значения таймеров для измерения, внимание на установку делителя чаастоты для тиков АЦП
  // и запустить первое измерение ( то есть вызвать ДМА)
  // отсюда не уходить и повторять запуск пока не завершим требуемое число накоплений
  // у меня две функции 1- подготовка и запуск первого измерения, StartRunFirst();
  // там сбрасываются приемные и накопительные буффера, сброс основныз таймеров
  // устанавливаются число реальных (проходов)накоплений равное числу установленных накоплений умноженное на число прореживаний
  // !!! Важно правильно установить период повторения имульсов, через таймер ТИМ1 (с заявленым тиком 1 мкС)
  // по прерыванию окончания цикла ДМА DCMI , первый съем, устанавливается признак вызова запуска
  // это не надо SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod=0)); // передача слова в Альтеру, первое поэтому чистим PointInPeriod
  // установим параметры измерения  перед запуском
  // в основном это установка (конфигурация таймеров) для измерения
  //LED_KT(1); // начало одного суммир
  StartRunFirst(AddIndexLN);// установлены таймеры достаточно запустить таймер TIM1 - 
  //LED_KT(0); // начало одного суммир
  
  if (CntAccumulat==0)CntAccumulat++;// первое измерение не пропустить
  //SetParamMeasure();
  while (CntAccumulat)
  {
    // здесь циклимся пока не накопим нужное число накоплений или по времени???
    // RUN_SUM цикл в котором вызывается\ однократное ссумирование - ОДНО
    // SUMMER - если с прореживанием то повторяется несколько раз, когда заканчивается
    // цикл перебора прореживания, инкрмент счетчика накоплений
    //( У НАС можно сразу снять одно накопление с прореживанием)
    // всегда перенастаивается альтера - унас не надо!
    // надо заменить SUMMER на НАШ цикл одного измерения без очстки буффеов
    // из 
    // проверяется 
    
    RUN_SUM(RawData);//34-67-130-260-510-770-1540~ max 34*48=1632 uS 1632/1540=
    //CreatDelay (DelayAfterAvrg);
    //SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod)); // (14 uS)передача слова в Альтеру
  }
  MeasureNow =0; //выключаем ограничение на прорисовку режима
  
  if (!RemoutCtrl)
  {
    // приостановим таймер времени накопления
    EnaTimerAccum = 0;
    DrawPictureMeas (EnDrawGraph); // (28 mS) рисование картинки при измерении
    EnaTimerAccum = 1;
  }
 // TST_KTA(0);
}

void DrawPictureMeas (BYTE EnDraw) // рисование картинки при измерении
{ 
  char Str[32];
  Rect rct={0,0,128,56};
  GraphParams params = {GetVerticalSize(GetUserVerSize()),PosCursorMain (0),GetSetHorizontScale(0),0, GetUserTypeCentral ()};//PosCursorMain (0)
   //  LED_KT(1); // начало графика
  
  if (EnDraw)
  {
    if(g_NeedScr)
    {
      CmdInitPage(17);// посылка команды переключения окна на DrawOTDR
      
      sprintf(Str,"t0.txt=\"%s\"яяя",MsgMass[30][CurrLang]); //Идет измерение: XXс 
      NEX_Transmit((void*)Str);//
      // можно прописать длину волны и диапазон измерения
      if ((GetIndexLN()==0)&&(IndexSmall==0))
        sprintf(Str,"t1.txt=\"0.5%s\"яяя",MsgMass[20][CurrLang]);//Длина линии: XXкм
      else
        sprintf(Str,"t1.txt=\"%d%s\"яяя",GetLengthLine(GetIndexLN()),MsgMass[20][CurrLang]);//Длина линии: XXкм
      //sprintf(Str,"t6.txt=\"%s\"яяя",MsgMass[35][CurrLang]); // 
      NEX_Transmit((void*)Str);//
      // первая строка длина волны
      // режим измерения с автосохранением от одной до 3 длин волн,
      // в режиме автосохранеия рисуем длину волны в цвете обозначающем автосохрение
      // окрашиваем поле вывода длины волны
      if(g_AutoSave)
        sprintf(Str,"t3.bco=YELLOWяяя"); // оранжевый
      else
        sprintf(Str,"t3.bco=WHITEяяя"); // белый
      
      NEX_Transmit((void*)Str);//
      //if (GetSetModeLW(0)) 
      //sprintf(Str,"t3.txt=\"%d%s\"яяя",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[21][CurrLang]); // auto
      //  sprintf(Str,"t3.txt=\"%s\"яяя",MsgMass[21][CurrLang]); // auto
      //else
      sprintf(Str,"t3.txt=\"%d%s\"яяя",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[18][CurrLang]); // nm
      NEX_Transmit((void*)Str);//
      
      g_FirstScr = 0;
      g_NeedScr = 0;
    }
    
    //сбросим картинку по UART2 для индикатора NEXTION
    // прорисовка(заполнение) данных для нового индикатора
    if(GetIndexVRM()==4) // реальное время
      sprintf(Str,"t2.txt=\"%s\"яяя",MsgMass[26][CurrLang]); //Идет измерение:  реальное
    else
      
      sprintf(Str,"t2.txt=\"%d%s\"яяя",(WORD)(((float)GetCntNumAvrg()/(float)GetFinAvrg())*((float)GetTimeAvrg(GetIndexVRM()))),MsgMass[4][CurrLang]); //Идет измерение: XXс 
    NEX_Transmit((void*)Str);// 
    if(TypeLCD)
    {
      rct.right=480;
      rct.bottom=255;// здесь надо сделать 255, а в индикаторе масштаб 270/255 1.05
      params.scale=(params.scale>=16)?(8):(params.scale);
    }
    else
    {
      rct.right=400;
      rct.bottom = 210;
      params.scale=(params.scale>=16)?(10):(params.scale);
    }
    rct.left = 0;
    rct.top = 0;
    //rct={0,0,390,210};
    
    MakeGraphNext( &rct, LogData, POINTSIZE, &params );// тут пересчитываем данные для графика (его вывод) в зависимости от параметров
    
    
    SendDrawNex(NexData,2,rct.right);
    HAL_Delay (1);
    
  }
    //   LED_KT(0); // конец графика

}

void RUN_SUM (DWORD* RawDataI)//
{
  //char Str[5];
  //static int CntAccumulat=0; // счетчик накоплений
  // DWORD* pRawData = RawData;
  // WORD* pDataADC = &DataADC;
  //--U0IER &= ~IER_RBR ;// | IER_THRE | IER_RLS/* Disable UART0 interrupt */
  //  TST_P1 (1); // set LED

  SUMMER(RawDataI); // 
  
  //SUMMER(RawDataI,   PointsPerPeriod-1-PointInPeriod); // Считаем индекс первой записи массива для счета с прореживанием!!!!!!!!!!! 
  //---U0IER |= IER_RBR ;// | IER_THRE | IER_RLS/* Enable UART0 interrupt */
  //  TST_P1 (0); // off LED
  
  //  unsigned int i;
  //  unsigned int Step;
  //  //if (CntAccumulat==0)CntAccumulat++;// первое измерение не пропустить
  //  Step = PointsPerPeriod-1-PointInPeriod; // Считаем индекс первой записи массива для счета с прореживанием!!!!!!!!!!! 
  //  ADC_DATA_EN(0); // Устанавливаем строб для работы АЦП               !!!!!!!!!!!
  //  STARTPP(0); // устанавливаем в "0" START                            !!!!!!!!!!!  
  //   for (i=Step;i<RAWSIZE;i+=PointsPerPeriod)                                  // !!!!!!!!!!! 
  //    RawData[i] += DataADC;     // суммирование с шины данных непосредственно (есть тики для АЦП) !!!!!!!!!!!
  //  STARTPP(1); // устанавливаем в "1" START                            !!!!!!!!!!!
  //  ADC_DATA_EN(1);// снимаем строб                            !!!!!!!!!!!
  //  PointInPeriod ++;     // инкремент прореживания 
  //  PointInPeriod = 0;
    CntAccumulat ++;  // инкремент счетчика накоплений
    
//  if ((CurrTimeAccum - OldTimeAccum) > 2500) // более 2.5 сек
//  {
//  }
  //if (CntAccumulat > GetNumAccumPerSec())//получение значения накоплений в данном режиме по числу накоплений
  if ((CntAccumulat > GetNumAccumPerSec())||((CurrTimeAccum/1000)>GetTimeAvrg(GetIndexVRM())))//получение значения накоплений в данном режиме по числу накоплений
    
  { // Закончили цикл накоплений можно по числу а можно и по времени!?
    StopAllTIM(1);
    // если ручное управление попробуем выдать время 
    if((RemoutCtrl)&&(Measuring() == AVERAGING))
    {
      char Str[8];
      g_TimeAvrg -=3;
      sprintf(Str,"t10.txt=\"%d%s\"яяя",g_TimeAvrg,MsgMass[4][CurrLang]); //Идет измерение: XXс 
      NEX_Transmit((void*)Str);// 
      
    }
    OldTimeAccum = CurrTimeAccum;  
    //RawData[RAWSIZE-1]= GetTimer(2);
    unsigned Avrgs = (GetCntNumAvrg()+CntAccumulat-1); // запись числа накоплений, на данный момент
    static DWORD Noise =0;
    DWORD NoiseSqr =0; // корень квадратный из шума(смещения)
    DWORD AvergSqr =0; // корень квадратный из накопления
    
    DWORD NoiseBegin =0; //0-49
    DWORD NoiseEnd =0;   //5530-5580
    DWORD NoiseAdd =0; // расчетные шумы по добавленым точкам в конце линии при 64 и 128 км
    int CntAddNoise= 0; // счетчик точек добавок
    DWORD MaxNoise =0;
    int j=GetCurrentBegShiftZone (); //получение текущего смещения по индексу
    //int j=0; //получение текущего смещения по индексу
    DWORD LocalRaw;
    SetCntNumAvrg(Avrgs); // сохранение
    // filtr (WOW Super)
    //if (GetCntNumAvrg()>=GetFinAvrg())
    //processWindowMedian(RawData, RAWSIZE);
    //FiltrWOW(RawData, GetCntNumAvrg());
    //for (int i=0; i<j-1; ++i)
    // у нас в начале всегда 63 точки
    // расчет уровня в начале
    for (int i=0; i<(50); ++i)
    {
      if (RawData[i]>MaxNoise) MaxNoise = RawData[i];
      Noise +=RawData[i];
    }
    Noise = (DWORD)1*(Noise/(50));
    //    NoiseBegin = Noise;
    //    // посчитаем в конце 
    //      for (int i=5530;i<5580;++i) // берем 30 точек в конце отображения окна
    //      {
    //          //CntAddNoise++;
    //          NoiseEnd +=RawData[i];
    //      }
    //    NoiseEnd = (DWORD)1*(NoiseEnd/(50));// смешение в конце
    //     if(NoiseEnd>NoiseBegin) 
    //       Noise = NoiseEnd;
    //     else
    //       Noise = NoiseBegin;
    //// а теперь просто увеличим смещение посчитанное в начале на половину накоплений
    //            Noise = NoiseBegin + Avrgs/4;
    
    
    //if (GetIndexLN()>5)// длинные линии -> добавим точек по расчету шумов
    if (1)// длинные линии -> добавим точек по расчету шумов
    {
      for (int i=5530;i<5580;++i) // берем 30 точек в конце снятых данных без превышения сигнала на 100 ед АЦП от уровня смещения
      {
        if (RawData[i] < (Noise + 20*Avrgs)) 
        {
          CntAddNoise++;
          NoiseAdd +=RawData[i];
        }
      }
      NoiseAdd += Noise;
      CntAddNoise++;
      Noise = (DWORD)1*(NoiseAdd/CntAddNoise);
    }
    g_Noise = Noise; 
    //Noise = (DWORD)1*(Noise/(j-1));
    DWORD CurrentMaxLog =(DWORD)(5000.0*log10((double)Avrgs*1023)); // максимальный логарифм текщего накопления
    // расчет логарифмического шума (перед импульсом)
    SetLogNoise((unsigned short)(CurrentMaxLog - (DWORD)(5000.0*log10((double)(MaxNoise-Noise))))) ;
    
    // 
    //Noise += Avrgs/400; // добавка чуток смещения к шумам
    // сохраение текущих шумов
    //SaveNoise (Noise/2);
    for (int i=0; i<OUTSIZE; i++)
    { 
      
      // усреднение на коротких линиях пока уберем 
      //if (PointsPerPeriod==48)   LocalRaw = (RawData[i+j-1]+RawData[i+j])>>1;// если самое мелкое то примитивный фильт на 2, иначе пила
      //else  LocalRaw = RawData[i+j];
      // попробуем фильтрануть, то есть если разница по модулю между предыдущей и последующей
      // меньше 1/8 от числа накоплений то ппоследующую берем как среднее с предыдущей,
      // если разница больше не меняем
          LocalRaw = RawData[i+j];
      if(0)
      {
        if((abs((RawData[i+j+1])-(RawData[i+j]))<(Avrgs/2))&&(abs((RawData[i+j-1])-(RawData[i+j]))<(Avrgs/2)))
        {
          LED_KTS(1);
          
          LocalRaw = (int)(RawData[i+j+1]+RawData[i+j]+RawData[i+j-1])/3;
          LED_KTS(0);
          
        }
      }
      if(0)
      {
        if((abs((int)(500*log10(RawData[i+j+1]))-(int)(500*log10(RawData[i+j])))<(15))&&(abs((int)(500*log10(RawData[i+j-1]))-(int)(500*log10(RawData[i+j])))<(15)))
        {
          //LED_KTS(1);
          
          LocalRaw = (int)(RawData[i+j+1]+RawData[i+j]+RawData[i+j-1])/3;
          //LED_KTS(0);
          
        }
        
      }
      if(0) // выкл мини фильтр (1)...по следующему (8)
      {
        if(abs((int)(RawData[i+j+1]-RawData[i+j]))<(Avrgs/8))
        {
          //RawData[i+j] = (RawData[i+j+1]+RawData[i+j])>>1;
          LocalRaw = (RawData[i+j+1]+RawData[i+j])>>1;
        }
        else
          LocalRaw = RawData[i+j];

      }
      if(0) // выкл мини фильтр (1)...по следующему
      {
        if(abs((int)(RawData[i+j+1]-RawData[i+j]))<(Avrgs/2))
        {
          RawData[i+j] = (RawData[i+j+1]+RawData[i+j])>>1;
        }
      }
      if(0) // выкл мини фильтр (2)...по предыдущему
      {
        if(abs((int)(RawData[i+j-1]-RawData[i+j]))<(Avrgs/2))
        {
          RawData[i+j] = (RawData[i+j-1]+RawData[i+j])>>1;
        }
      }
      //LocalRaw = RawData[i+j];
      // добавка перед скачком, то есть перед большим отражением в маленьких сигналах
      // пресловутые 12 метров
      if(0)
      {
        if(PointsPerPeriod>=4)
          //LocalRaw += (DWORD)(RawData[i+j+(PointsPerPeriod/4)]*2.8e-3);
          LocalRaw += (DWORD)(RawData[i+j+2]*2.8e-3);
      }
      if (LocalRaw<=Noise) LocalRaw=Noise+1;
      LocalRaw= LocalRaw-Noise;
      LogData[i] = (unsigned short)(CurrentMaxLog - (DWORD)(5000.0*log10((double)LocalRaw))) ;
      if (LogData[i] > CurrentMaxLog)
      { 
        //__no_operation();
        CurrentMaxLog+=1;
      }
      
    }
    CntAccumulat =  0;
    SetHeadFileRaw (GetCntNumAvrg()); // пишем заголовок файла выдачи необработаных данных
    
  }
}
// сумматор должен работать быстро
 void ContNextAvrg(void)
{
  // быстро суммируем и продолжаем, по таймеру Т1, запустится новый цикл сбора
  //StopAllTIM(0);// останавливаем таймеры которые считают
    //TIM1->CR1 &=~TIM_CR1_CEN;
  // CountDMA указывает на смещение в индексе куда суммируем текущий съем
  //Sm_Shift = (CountDMA)&(NumRepit-1);
  //Cnt2Timer[CountDMA%128]=CurrCnt2Timer; //(071)
  //uint32_t PointDMA = (CountDMA&(NumRepit-1));
  //uint32_t PointDMA = (CountDMA%(NumRepit));// (071) - точка которая суммируется для данного цикла DMA
  //uint32_t PointDMA = (g_CountDMA);// (071) - точка которая суммируется для данного цикла DMA
  //LED_START(1);
  //LED_START(0);
  TST_KTA(1); // начали суммирование одного прохода МОДУЛЬ3
  
  TIM5->CR1 &= ~TIM_CR1_CEN; // STop генератора TIM5 (время когда надо суммировать)

  for(int i=0;i<SizeBlockNak; i++)
  {
    //BufNAK[NumRepit*i+PointDMA] +=BufADC[i]; 
    // for ADC MS9280 buffer BufADD
    //BufNAK[NumRepit*i+PointDMA] +=BufADD[i]; 
    //BufNAK[NumRepit*i+PointDMA] +=BufADD[i]; //(071)
    //BufNAK[NumRepit*i+g_CountDMA] +=BufADD[i]; 
    RawData[NumRepit*i+g_CountDMA]+=BufADD[i];
   //BufNAK[NumRepit*i+PointDMA] +=(BufADC[i]+BufADD[i-1])/2; 
    // BufNAK[2*(NumRepit*i+PointDMA)+1] +=BufADC[i]; 
    //BufNAK[2*(NumRepit*i+PointDMA)] +=BufADD[i-1]; 
    //TIM1->CNT = TIM1->CCR1 - 5; // так как он остановлен
  }
  //LED_START(1);
    //TIM1->CR1 |=TIM_CR1_CEN;
  //LED_START(0);
  TST_KTA(0); // закончили накопление - суммирование можно сбросить линию "МОДУЛЬ" 3
    StopAllTIM(0);// останавливаем таймеры которые считают
    SumNumNak++;
  //if(++CountDMA<SumNumNak)
  //if((++CountDMA) >= SumNumNak)// паора заканчивать накопления
  if((++CountDMA) >= NumRepit)// паора заканчивать накопления
  {
    StopAllTIM(1);
    
    TIM1->CR1 &=~TIM_CR1_CEN;
    //  останавливаем основной таймер для прекращения накопления
    // установим признак окончания измерения для вывода результатов
    // можно тормознуть все остальные таймеры, и даже сбросить
    Ena_AVRG = 0;
    //EnaPrintRes = 1;
    
  }
  //еще не донакопили, надо понять как установить новый запуск
  // по таймеру Т1 который установлен
  //        if(SW_TIM1>2)
  //        {
  //          TIM1->CNT = TIM1->CCR1 - 5;
  //          // переустанвливаем основной таймер ближе к окончанию счета
  //          
  //        }
  //        // и продолжаем его для запуска следующего сбора
  //        //TIM1->CNT =990;
  //        TIM1->CR1 |=TIM_CR1_CEN;
  
  //EnaNextAvrg = 0;
    Ena_SUMM = 1;
    Ena_CFG = 1;
  
}
