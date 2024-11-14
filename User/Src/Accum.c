//===============================
#include "system.h"

// переменные счетчика Т2, предыдущее и новое считает
volatile DWORD OldTimeAccum; // предыдущее значение таймера накопления
volatile DWORD CurrTimeAccum; // текущее значение
uint8_t EnaTimerAccum =0; //признак разрешения счета таймера накопления
int CntAccumulat=0; // счетчик накоплений



void RUN_SUM (DWORD* RawDataI);//


void SUMMER (DWORD* RawDataS)//
{
  //char Str[5];
  //static int CntAccumulat=0; // счетчик накоплений
  // DWORD* pRawData = RawData;
  // WORD* pDataADC = &DataADC;
   
  unsigned int i;
  unsigned int Step;
  //if (CntAccumulat==0)CntAccumulat++;// первое измерение не пропустить
  Step = PointsPerPeriod-1-PointInPeriod; // Считаем индекс первой записи массива для счета с прореживанием!!!!!!!!!!! 
  //ADC_DATA_EN(0); // Устанавливаем строб для работы АЦП               !!!!!!!!!!!
  //STARTPP(0); // устанавливаем в "0" START                            !!!!!!!!!!!  
   for (i=Step;i<RAWSIZE;i+=PointsPerPeriod)                                  // !!!!!!!!!!! 
    RawDataS[i] += 1;     // суммирование с шины данных непосредственно (есть тики для АЦП) !!!!!!!!!!!
  //STARTPP(1); // устанавливаем в "1" START                            !!!!!!!!!!!
  //ADC_DATA_EN(1);// снимаем строб                            !!!!!!!!!!!
  PointInPeriod ++;    // инкремент прореживания 
  
}


void Averaging (int NumAccum,unsigned DelayAfterAvrg, BYTE EnDrawGraph )// функция накопления с установкой задержки после накопления
// дополнительно устанавливаются: PointsPerPeriod - число точек на период, зависит от длины измерения
// NumAccumPerSec - число накоплений в данном фрэйме

{ 
  //    CntPointPick=0;

  //CurrTimeAccum = HAL_GetTick();
  OldTimeAccum = CurrTimeAccum;
  SetNumAccumPerSec (NumAccum);
  SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod=0)); // передача слова в Альтеру, первое поэтому чистим PointInPeriod
  if (CntAccumulat==0)CntAccumulat++;// первое измерение не пропустить
  //SetParamMeasure();
  while (CntAccumulat)
  {
    RUN_SUM(RawData);//34-67-130-260-510-770-1540~ max 34*48=1632 uS 1632/1540=
    CreatDelay (DelayAfterAvrg);
    SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod)); // (14 uS)передача слова в Альтеру
  }
  if (!RemoutCtrl)
  DrawPictureMeas (EnDrawGraph); // (28 mS) рисование картинки при измерении
}

void DrawPictureMeas (BYTE EnDraw) // рисование картинки при измерении
{ 
  char Str[32];
  Rect rct={0,0,128,56};
  GraphParams params = {GetVerticalSize(GetUserVerSize()),PosCursorMain (0),GetSetHorizontScale(0),0, GetUserTypeCentral ()};//PosCursorMain (0)
  
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
    
    MakeGraphNext( &rct, LogData, 4096, &params );// тут пересчитываем данные для графика (его вывод) в зависимости от параметров
    
    
    SendDrawNex(NexData,2,rct.right);
    HAL_Delay (1);
    
  }
  
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
  if (PointInPeriod > PointsPerPeriod-1) // проверка на конец цикла прореживания
  {
    PointInPeriod = 0;
    CntAccumulat ++;  // инкремент счетчика накоплений
    // совершено циклическое накопление, можно проверить время накопления и сделать выводы
    //CurrTimeAccum = HAL_GetTick(); // фиксируем время текущее при накоплении
  }
  // разрешаем запись в регистр данных ЦАП, вроде не важно....!!!
  //SetLvlDAC ((unsigned int)CntAccumulat);
  if ((CurrTimeAccum - OldTimeAccum) > 2500) // более 2.5 сек
  {
  }
  //if (CntAccumulat > GetNumAccumPerSec())//получение значения накоплений в данном режиме по числу накоплений
  if ((CntAccumulat > GetNumAccumPerSec())||((CurrTimeAccum/1000)>GetTimeAvrg(GetIndexVRM())))//получение значения накоплений в данном режиме по числу накоплений
    
  { // Закончили цикл накоплений можно по числу а можно и по времени!?
    OldTimeAccum = CurrTimeAccum;  
    //RawData[RAWSIZE-1]= GetTimer(2);
    DWORD Noise =0;
    DWORD NoiseAdd =0; // расчетные шумы по добавленым точкам в конце линии при 64 и 128 км
    int CntAddNoise= 0; // счетчик точек добавок
    DWORD MaxNoise =0;
    int j=GetCurrentBegShiftZone (); //получение текущего смещения по индексу
    DWORD LocalRaw;
    unsigned Avrgs = (GetCntNumAvrg()+CntAccumulat-1); // запись числа накоплений, на данный момент
    SetCntNumAvrg(Avrgs); // сохранение
    // filtr (WOW Super)
    //if (GetCntNumAvrg()>=GetFinAvrg())
    //processWindowMedian(RawData, RAWSIZE);
    //FiltrWOW(RawData, GetCntNumAvrg());
    //for (int i=0; i<j-1; ++i)
    for (int i=0; i<(2*PointsPerPeriod); ++i)
    {
      if (RawData[i]>MaxNoise) MaxNoise = RawData[i];
      Noise +=RawData[i];
    }
    Noise = (DWORD)1*(Noise/(2*PointsPerPeriod));
    if (GetIndexLN()>5)// длинные линии -> добавим точек по расчету шумов
    {
      for (int i=4090;i<4120;++i) // берем 30 точек в конце отображения окна
      {
        if (RawData[i] < (Noise + 100*Avrgs)) 
        {
          CntAddNoise++;
          NoiseAdd +=RawData[i];
        }
      }
      NoiseAdd += Noise;
      CntAddNoise++;
      Noise = (DWORD)1*(NoiseAdd/CntAddNoise);
    }
    //Noise = (DWORD)1*(Noise/(j-1));
    DWORD CurrentMaxLog =(DWORD)(5000.0*log10((double)Avrgs*4095)); // максимальный логарифм текщего накопления
    // расчет логарифмического шума (перед импульсом)
    SetLogNoise((unsigned short)(CurrentMaxLog - (DWORD)(5000.0*log10((double)(MaxNoise-Noise))))) ;
    
    // 
    //Noise += Avrgs/400; // добавка чуток смещения к шумам
    // сохраение текущих шумов
    //SaveNoise (Noise/2);
    for (int i=0; i<OUTSIZE; i++)
    { 
      if (PointsPerPeriod==48)   LocalRaw = (RawData[i+j-1]+RawData[i+j])>>1;// если самое мелкое то примитивный фильт на 2, иначе пила
      else  LocalRaw = RawData[i+j];
      //LocalRaw = RawData[i+j];
      // добавка перед скачком, то есть перед большим отражением в маленьких сигналах
      // пресловутые 12 метров
      LocalRaw += (DWORD)(RawData[i+j+(PointsPerPeriod/3)]*2.56e-5);
      
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

 
