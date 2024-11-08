
// Функции работы с памятью 
// взаимодействует с SSP

#include "system.h"
// структура конфигурации прибора

BYTE MemTable[MaxMemOTDR+1]; // таблица рефлектограмм ячейки памяти меняется в памяти до MaxMemOTDR
BYTE MemTableExt[MaxMemOTDRExt+1]; // таблица рефлектограмм расширенной ячейки памяти меняется в памяти до MaxMemOTDRExt
static unsigned short NumTrace = 0; // номер трассы (число записей) в памяти рефлектограм

BYTE CheckMemID (void) // прочитаем какая флэшка стоит
{
  BYTE ID_Flash = 0;
//  BYTE Data_Flash_Devices[4];
//  FlashReadID ((unsigned char*)&Data_Flash_Devices);
//  ID_Flash = Data_Flash_Devices[1]&0x1f;
//  if (ID_Flash == 7) ID_Flash = 1;
//  else ID_Flash = 0;
  return ID_Flash;
}


DWORD CheckMemFlash (void)
{ 
  DWORD SPCTR_err=0;
// // чтение и инициализация альтернативного имени прибора
//  FlashReadPageNum (DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT),0);
//  unsigned ErrPramDBNAME = InvalidDBNAME();
//  if (ErrPramDBNAME)
//  {
//    //SPCTR_err=0x80;
//    InitDBNAME(ErrPramDBNAME); 
//    FlashErasePage(DBNAMESTRUCT); // чистим страницу хранящую конфигурацию alternate name прибора
//    FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
//  }
// // чтение и инициализация конфирурации совместимости JDSU
//  FlashReadPageNum (JDSUSTRUCT, StructPtr(JDSUSTRUCT), StructSize(JDSUSTRUCT),0);
//  unsigned ErrPramJDSU = InvalidJDSU();
//  if (ErrPramJDSU)
//  {
//    //SPCTR_err=0x80;
//    InitJDSU(ErrPramJDSU); 
//    FlashErasePage(JDSUSTRUCT); // чистим страницу хранящую конфигурацию прибора
//    FlashWritePageSM(JDSUSTRUCT, StructPtr(JDSUSTRUCT), StructSize(JDSUSTRUCT), 0);
//  }
//  
//  // функция контроля конфигурации прибора (проверка)
//  FlashReadPageNum (CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE),0);
//  unsigned ErrDev = InvalidDevice();
//  if (ErrDev)
//  {
//    SPCTR_err=0x80;
//    InitDevice(ErrDev); 
//    FlashErasePage(CFG_DEVICE); // чистим страницу хранящую конфигурацию прибора
//    FlashWritePageSM(CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE), 0);
//  }
//  // проверка спектральных коэффициентов
//  FlashReadPageNum (COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
//  SPCTR_err = SPCTR_err + FindErrCoeff ();
//  FixErrCoeff (SPCTR_err); //  проверка таблицы коэффициентов и исправление таблицы коэффициентов
//  if (SPCTR_err)
//  {
//  FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
//  FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
//  }
//  FlashReadPageNum (CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER),0);
//  SPCTR_err = SPCTR_err + CheckUserGonfig ();  // Проверка пользовательских настроек 
//  if (SPCTR_err>=0x100)
//  {
//  FlashErasePage(CFG_USER); // чистим страницу установок пользователя прибора
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
//  }
//  // проверка блока установок контроля рефлектограммы
//  FlashReadPageNum (EVEN_SET, StructPtr(EVEN_SET), StructSize(EVEN_SET),0);
//  DWORD Param_err =  CheckReflParam ();  // Проверка пользовательских настроек 
//  if (Param_err>0)
//  {
//  FlashErasePage(EVEN_SET); // чистим страницу установок пользователя прибора
//  FlashWritePageSM(EVEN_SET, StructPtr(EVEN_SET), StructSize(EVEN_SET), 0);
//  }
//  
  return SPCTR_err;
}

void SaveConfigDevice (void)
{
//    FlashErasePage(CFG_DEVICE); // чистим страницу хранящую конфигурацию прибора
//    FlashWritePageSM(CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE), 0);
}

void SaveUserConfig (void) // сохранение пользовательских настроек
{
//  FlashErasePage(CFG_USER); // чистим страницу установок пользователя прибора
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
}

// очистка рефлектограммы
void TraceERASE(unsigned int Trace)
{
//  int i;
//  //if (Trace <= ((CheckIDMEM)?(0x100+MaxMemOTDRExt):(MaxMemOTDR)))
//  if(CHECKNUMTRACE)
//  {
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      FlashErasePage(Trace*NUMPAGESFORREF+i+1);
//    }
//  }
}
;
// при записи рефлектограммы записываем 16 блоков по 528 байт из них 512 данные рефлектограммы 
// + 16 вспомогательные данные 0-блок = текущее время 1-2 блок = комментарии к файлу
// 15 блок - установочные параметры
void TraceWRITE(unsigned int Trace)
{
//  int i;
//  //extern unsigned char Index_set ; // индекс режима работы (0=выбор, 1=10км, 2=20км, 3=40км, 4=80км) 
//  //extern unsigned char Index_Ln ; // индекс значений длины линии в пользовательском режиме
//  //extern unsigned char Index_Im ; // индекс значений длительности импульса 
//  //extern unsigned char Index_Vrm ; // индекс значений времени работы 
//  //extern unsigned char ds1337 ; // индекс значения таймера реального времени  
//  //extern float K_pr; 
//  unsigned long DateFileName = GetSetCntFiles (0);
//  //if (Trace <= ((CheckIDMEM)?(0x100+MaxMemOTDRExt):(MaxMemOTDR)))
//      if(CHECKNUMTRACE)
//  {
//
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      //SPI1_write_page(Trace*16+i+1, (unsigned char*)&Dat[128*i],512,0);
//      // для 2 байтовой записи в тоже поле новый цикл записи по 2 байта 
//      FlashWritePageTrBuf(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&LogData[256*i]);
//      switch(i)
//      {
//      case 0: // запись текущего времени (первые 4 слова структуры времени)
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&TimeSaveOTDR,16,512);
//      break;
//      case 1:       // запись текущего времени (первые 4 слова структуры времени)
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&TimeSaveOTDR+16,16,512);
//      break;
//      case 2: // запись блока комментариев
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&CommentsOTDR,10,512);
//      break;
//      case 3:
//        
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&CommentsOTDR[10],10,512);
//      // сохраняем сквозной номер сохраненной рефлектограммы
//      break;
//      case 4:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&DateFileName,4,512);
//      // сохраняем ORL в случае новой прошивки и если делали измерение! А так пишем что-то
//      break;
//      case 5:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&g_VolORL,4,512);
//      break;
//      // 17.11.2022 сохраняем вариант установки выбора длин волн при измерении
//      case 6:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&g_SetModeLW,1,512);
//      break;
//      case 15:
//        //в последние значения пишем переменные измерения 
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&SettingRefl,16,512);
//        //Index_Ln  // индекс значений длины линии в пользовательском режиме
//        // Index_Im // индекс значений длительности импульса 
//        // Index_Vrm  // индекс значений времени работы 
//        // SW_LW  // признак длинны волны лазера 
//        // K_pr  // коэфф преломления 
//        // Index_Comm  // местоположения курсора в комментариях 
//        // SubModeRef; // рефлектометр авто или нет ( для востановления после включения)
//        // NumAverag; // число накоплений (реальные по данным)
//      break;
//      //
//      default:
//        break;
//      }
//    }
//  }
}
;
// аварийная перезапись нулевой рефлекторграммы
void AlarmReWriteTrace (void)
{
//  //TraceREAD(0);
//  //CheckSavedTrace();
//  //clear Buf 
//  memset( LogData, 43690, OUTSIZE * sizeof(unsigned short) ); // ZERO_ARRAY( array ) memset( array, 0, sizeof(array) )
//  // Set Comments
//  sprintf(CommentsOTDR,"Empty              ");
//  TimeSaveOTDR = RTCGetTime(); // сохраняем время сбора
//  TraceERASE(0);
//  TraceWRITE(0);
//  TraceREAD(0);
}
// при записи рефлектограммы записываем 16 блоков по 528 байт из них 512 данные рефлектограммы 
// запись 3 сек рефлектограммы при измерении "мощными" импульсами, измеряем на 1000нс, для склейки перегуженной зоны 
// пишем как бы в 250 рефлектограмму (4001 pg...4016)
void SpliceWRITE(unsigned int Trace) 
{
//  int i;
//  if (Trace == 250)
//  {
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      //SPI1_write_page(Trace*16+i+1, (unsigned char*)&Dat[128*i],512,0);
//      // для 2 байтовой записи в тоже поле новый цикл записи по 2 байта 
//      FlashWritePageTrBuf(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&LogData[256*i]);
//    }
//  }
}
;
// почитаем текущую точку Splice рефлекограммы
unsigned short FlashReadSpliceData (unsigned int NumData)
{
    unsigned short OUT_Data = 0;
//    unsigned long PgSplice = 4001;
//    PgSplice  = PgSplice + (NumData>>8);
//  unsigned long CmdSqn = PgSplice << 10; // адрес страницы 
//  CmdSqn = CmdSqn + ((NumData & 0xFF)<<1); // байт положение в странице
//  CmdSqn = CmdSqn + 0xD2000000; //команда 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   OUT_Data=SSP_FlashTransmit(1, 0)+\
//     SSP_FlashTransmit(1, 0)*256;
//  
// 
//  FL_CS(1,1); // CS_1
  return OUT_Data;
}


// читаем страницу сохраненной рефлектограммы
void FlashReadPageTrace (unsigned int NumPg, unsigned char *adress, int Block)
{
//    unsigned char *c;
//    int i;
//    BYTE CheckSmb;
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000000; //команда 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   for (int i=0; i<512; ++i)
// {
//   *adress++=SSP_FlashTransmit(1, 0);
// }
//   FL_CS(1,1); // CS_0
//    CreatDelay (40000); // 3.3 мС
//   CmdSqn =(unsigned long)NumPg << 10;
//  CmdSqn = CmdSqn + 0xD2000200; //команда 
//   FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//
// if (Block==0)
// {
//   c = (unsigned char*)&TimeSaveOTDR;
//   for (i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
// if (Block==1)
// {
//   c = (unsigned char*)&TimeSaveOTDR+16;
//   for (i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
// if (Block==2)
// {
//   c = (unsigned char*)&CommentsOTDR;
//   for (i=0; i<10; i++)
//   {
//     CheckSmb=SSP_FlashTransmit(1, 0);
//     if (CheckSmb < 0x20) CheckSmb =0x20;
//     *c++ = CheckSmb;
////     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
// if (Block==3)
// {
//   c = (unsigned char*)&CommentsOTDR[10];
//   for (i=0; i<10; i++)
//   {
//     CheckSmb=SSP_FlashTransmit(1, 0);
//     if (CheckSmb < 0x20) CheckSmb =0x20;
//     *c++ = CheckSmb;
//     
//     //*c++=SSP_FlashTransmit(1, 0);
//   }
//   CommentsOTDR[19]=0; // принудительно зануляем строку
// }
// // порядковый номер текущего файла (читаем из памяти)
// if (Block==4)
// {
//   c = (unsigned char*)&CurCountFiles;
//   for (i=0; i<4; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
// if (Block==5) // чтение ORl (если записано) (может быть inf)
// {
//   c = (unsigned char*)&g_VolORL;
//   for (i=0; i<4; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//   // проверка на inf и диапазон
//       if (isnan(g_VolORL)||(g_VolORL<14.8)||(g_VolORL>70.0))
//         g_VolORL = 0.0;
// }
// if (Block==6) // чтение g_SetModeLW (BYTE) 17.11.2022
// {
//   c = (unsigned char*)&mg_SetModeLW;
//   for (i=0; i<1; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//   // проверка на inf и диапазон
//       if (mg_SetModeLW >7)
//         mg_SetModeLW = 0;
// }
// if (Block==15)
// {
//   c = (unsigned char*)&SettingRefl;
//   for (i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
//  FL_CS(1,1); // CS_1
}
// читаем комментарии сохраненной рефлектограммы по номеру
// читаем страницу сохраненной рефлектограммы
unsigned long FlashReadCommTrace (unsigned int Num, unsigned char *adress)
{
//    unsigned char *c;
//    BYTE CheckSmb;
  unsigned long NumWrSq = Num; // номер записи
//  unsigned long NumPg = GetNumTraceSaved(Num)*NUMPAGESFORREF+3; // сразу указатель на 2 блок где комментарии
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //команда чтения блока с 512 байта 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   for (int i=0; i<10; ++i)
// {
//     CheckSmb=SSP_FlashTransmit(1, 0);
//     if (CheckSmb < 0x20) CheckSmb =0x20;
//   *adress++= CheckSmb;
//   //*adress++=SSP_FlashTransmit(1, 0);
// }
//  FL_CS(1,1); // CS_1
//  NumPg++;
//  CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //команда чтения блока с 512 байта 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   for (int i=0; i<10; ++i)
// {
//     CheckSmb=SSP_FlashTransmit(1, 0);
//     if (CheckSmb < 0x20) CheckSmb =0x20;
//   *adress++= CheckSmb;
//   //*adress++=SSP_FlashTransmit(1, 0);
// }
// *adress--;
// *adress=0;
//  FL_CS(1,1); 
//  NumPg++;
//  CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //команда чтения блока с 512 байта 
//  FL_CS(1,0); // // CS_0 4 страница данные о номере записи
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//     c = (unsigned char*)&NumWrSq;
//   for (int i=0; i<4; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//  FL_CS(1,1); // CS_1
  if (NumWrSq > 0x100000) NumWrSq = (unsigned long)Num;

  return NumWrSq;
}

// чтение времени сохраненной рефлектограммы(поле минут)
unsigned long FlashReadTimeTrace (unsigned int Num)
{
  unsigned long NumPg =1; // сразу указатель на 2 блок где комментарии

//  unsigned char *c;
//  // время сохраненной рефлектограмм
//    RTCTime TimeReadOTDR;
//  unsigned long NumWrSq; // получение беззнакового целего поля минут ( для анализа писалось туда что нить или нет)
//  unsigned long NumPg = GetNumTraceSaved(Num)*NUMPAGESFORREF+1; // сразу указатель на 2 блок где комментарии
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //команда чтения блока с 512 байта 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   c = (unsigned char*)&TimeReadOTDR;
//   for (int i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//  FL_CS(1,1); // CS_1 прочитали перый блок
//  NumPg++;
//  CmdSqn =(unsigned long)NumPg << 10; // команда для передачи для AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //команда чтения блока с 512 байта 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   c = (unsigned char*)&TimeReadOTDR+16;
//   for (int i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//  FL_CS(1,1); // CS_1 прочитали перый блок
//  //
//  sprintf(NameReadFile,"%02d%02d%02d_%02d%02d%01d.sor",TimeReadOTDR.RTC_Year%100,
//          TimeReadOTDR.RTC_Mon,
//          TimeReadOTDR.RTC_Mday,
//          TimeReadOTDR.RTC_Hour,
//          TimeReadOTDR.RTC_Min,
//          TimeReadOTDR.RTC_Sec/10 );
  
  return NumPg;
}

// чтение рефлектограммы
void TraceREAD(unsigned int Trace)
{
//  int i;
//  //if (Trace <= MaxMemOTDR)
//    if(CHECKNUMTRACE)
//  {
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      FlashReadPageTrace(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&LogData[256*i], i);
//    }
//  }
//   //CheckCommOTDR (); // проверка и корректировка строки комментариев OTDR
//
//  if (CurCountFiles > 0x100000) // empty data
//  {
//    CurCountFiles = (unsigned long)Trace;
//  }
//  if (Trace == 0) // если читаем установочную рефлектограмму переустанавливаем режим рефлектометра
//  {
//    SetSubModRefl (SettingRefl.SubModeRef);
//    GetSetCntFiles(CurCountFiles);
//  }
}

unsigned short SetNumTrace (unsigned short Num) // установка номера трассы
{
//  //просто порядковый номер трассы, надо модифициролвать в зависимости от заполнения памяти
//  // указание на той или иной банк в зависимости от конфигурации
//  if (CheckIDMEM) // ести расширенная память
//  {
//    if (Num > (MaxMemOTDRExt + MaxMemOTDR) )
//    {
//      Num = 0;
//    }
//  }
//  else
//  {
//    if (Num > MaxMemOTDR) Num = 0;
//  }
  NumTrace = Num;
  return NumTrace;
}

unsigned short GetNumTrace (void) // получение номера трассы 
{
  return NumTrace;
}
// получаем "реальный адрес" хранения рефлектограммы
unsigned short GetNumTraceSaved (unsigned short Index) // чтение значения таблицы рефлектограм по индексу (основного блока)
{
  unsigned short RealNum = 0;
//  if (CheckIDMEM) // есть расширенная память
//  {
//    if (Index > (MaxMemOTDRExt + MaxMemOTDR) )
//    {
//      Index = 0;
//    }
//    else
//    {
//      if (Index>MemTable[0]) // число больше чем записано в 1 банке
//      {
//        RealNum = (unsigned short)(0x100 + MemTableExt[Index - MemTable[0]]); 
//      }
//      else
//      {
//        RealNum = (unsigned short)MemTable[Index];
//      }
//    }
//    if (Index == 0)      RealNum = (unsigned short)MemTable[Index]+(unsigned short)MemTableExt[Index]; // сколько ячеек заполнено
//
//  }
//  else // нет расширенной памяти
//  {
//    if (Index > MaxMemOTDR)
//    {
//      Index = 0;
//    }
//      RealNum = (unsigned short)MemTable[Index];
//  }
  return  RealNum;

}

void CheckTableMemOTDR (BYTE MemErr) // проверка таблицы сохранения рефлктограмм 0 - проверяем 1 - обновляем
{
//  //BYTE MemErr=0;
////  unsigned short AllTrace, RTrace;
//  if (!MemErr)
//  {
//    FlashReadPageNum (TABLEMEMOTDR, MemTable , ARRAY_SIZE(MemTable),0);
//  for (int i = 0; i <= MaxMemOTDR; i++)  if (MemTable[i] > MaxMemOTDR)MemErr = 1;
//    FlashReadPageNum (TABLEMEMOTDREXT, MemTableExt , ARRAY_SIZE(MemTableExt),0);
//  for (int i = 0; i <= MaxMemOTDRExt; i++)  if (MemTableExt[i] > MaxMemOTDRExt)MemErr |= 2;
//  
//  
//  }
//  if ((MemErr & 0x01) == 1)
//  {
//    for (int i = 0; i <= MaxMemOTDR; ++i) MemTable[i] = i;
//    FlashErasePage(TABLEMEMOTDR);
//    FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//  }
//  if ((MemErr & 0x02) == 2)
//  {
//    for (int i = 0; i <= MaxMemOTDRExt; ++i) MemTableExt[i] = i;
//    FlashErasePage(TABLEMEMOTDREXT);
//    FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//  }
////  AllTrace =  (unsigned short)MemTable[0] +  (unsigned short)MemTableExt[0]; // сколько всего рефлектограмм записано
////  // проверяем правильность сохраненных рефлектограмм по полю минут в файле сохренения если плохо удаляем эту рефлектограмму
////  for (int i = AllTrace; i > 0 ; i--) 
////  {
////    GetNumTraceSaved
////    
////  }
  
  
}

unsigned short DeletingTrace (unsigned short Trace) // "Удаление" рефлектограммы, просто переносим 
{
//  unsigned short IndexMem;
//      if (CheckIDMEM) // прочитаем какая флэшка стоит
//      {
//        // определяем в какой части лежит указанная трасса
//        if (Trace <= MemTable[0]) // main memory
//        {
//          IndexMem = MemTable[Trace];
//          for (int i=Trace; i<MaxMemOTDR; i++)
//          {
//            MemTable[i] = MemTable[i+1];
//          }
//          MemTable[MaxMemOTDR] = IndexMem;
//          MemTable[0]--;
//          Trace--;
//    FlashErasePage(TABLEMEMOTDR);
//    FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//        }
//        else // трасса во втором банке
//        {
//          IndexMem = MemTableExt[Trace - MemTable[0]];
//          for (int i=(Trace - MemTable[0]); i<MaxMemOTDRExt; i++)
//          {
//            MemTableExt[i] = MemTableExt[i+1];
//          }
//          MemTableExt[MaxMemOTDRExt] = IndexMem;
//          MemTableExt[0]--;
//          Trace--;
//    FlashErasePage(TABLEMEMOTDREXT);
//    FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//          
//        }
//      }
//      else  // для простой памяти по-старому
//      {
//    IndexMem = MemTable[Trace];
//          for (int i=Trace; i<MaxMemOTDR; i++)
//          {
//            MemTable[i] = MemTable[i+1];
//          }
//          MemTable[MaxMemOTDR] = IndexMem;
//          MemTable[0]--;
//          Trace--;
//    FlashErasePage(TABLEMEMOTDR);
//    FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//      }
         return Trace; 
          

}

unsigned short ReanimateTrace (unsigned short ReanT) // "реанимация" заданного числа трасс
{ 
  unsigned short Trace=0; // возвращаем число реанимированных трасс
//  if (CheckIDMEM) // прочитаем какая флэшка стоит, если есть большая, то удалем в ней верхнюю часть
//  {
//    if(ReanT>200)
//    {
//      // можно реанимировать трассы в верхнем блоке
//      MemTableExt[0] = ReanT - 200;
//    }
//    else
//      MemTableExt[0] = 0;
//    //Trace = (unsigned short)MemTable[0] + (unsigned short)MemTableExt[0] ; // сколько трасс удалим в верхнем блоке
//    Trace =  (unsigned short)MemTableExt[0] ; // сколько трасс восстановим в верхнем блоке
//    // сохраняем таблицу сохранений
//    FlashErasePage(TABLEMEMOTDREXT);
//    FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//  }
//  // этот блок нижней части можем востановить только не более 200 
//  //      else
//  //      {
//  if(ReanT>200)
//    MemTable[0] = 200;
//  else
//    MemTable[0] = ReanT;
//  Trace = Trace + (unsigned short)MemTable[0];
//  SetNumTrace (0); // установка номера трассы
//  FlashErasePage(TABLEMEMOTDR);
//  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//  //      }
  return Trace; 
}

unsigned short DeletingAllTrace (void) // "Удаление"всех рефлектограммы, просто обнуляем счетчик сохранения
{ 
  unsigned short Trace=0; // возвращаем число удаленных трасс
//      if (CheckIDMEM) // прочитаем какая флэшка стоит, если есть большая, то удалем в ней верхнюю часть
//      {
//  //Trace = (unsigned short)MemTable[0] + (unsigned short)MemTableExt[0] ; // сколько трасс удалим в верхнем блоке
//  Trace =  (unsigned short)MemTableExt[0] ; // сколько трасс удалим в верхнем блоке
//  
//  //SetNumTrace (0); // установка номера трассы
//  MemTable[0] = 0;
//  MemTableExt[0] = 0;
//  // переиндексируем блок
//    for (int i = 0; i <= MaxMemOTDRExt; ++i) MemTableExt[i] = i;
////  
////  FlashErasePage(TABLEMEMOTDR);
////  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//        // сохраняем таблицу сохранений
//        FlashErasePage(TABLEMEMOTDREXT);
//        FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//     }
//     // этот блок переиндексируем всегда!!!
////      else
////      {
//  Trace = Trace + (unsigned short)MemTable[0];
//  SetNumTrace (0); // установка номера трассы
//  MemTable[0] = 0;
//  // переиндексируем блок
//    for (int i = 0; i <= MaxMemOTDR; ++i) MemTable[i] = i;
//  FlashErasePage(TABLEMEMOTDR);
//  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
////      }
  return Trace; 
}

// КАРТИНКА ПАМЯТИ - рисуем картинку памяти рефлектограмм
void DrawMemoryRefl(unsigned short Trace, BYTE CurrLang, BYTE Param)
{
//  char Str[32];
//  char StrI[32];
//  char s_orl[6];
//  //BYTE CurrLang=GetLang(CURRENT);
//  if(g_FirstScr) // если первый раз пришли читаем трассу
//  {
//    if (Trace)
//      TraceREAD(GetNumTraceSaved(Trace)); // читаем данные из памяти по номеру котораяуказана в списке
//    else TraceREAD(0); // если 0 то ее и читаем
//  }
//      CheckCommOTDR (); // проверка и корректировка строки комментариев OTDR перед записью...
//
//  // проверка считанных данных
//  if(CheckSavedTrace()) // функция контроля сохраненной рефлектограммы 
//  {
//    // рефлектограмма текущая плохая (надо удалить)
//    Trace = DeletingTrace (Trace);
//    SetNumTrace(Trace);
//    TraceREAD(0);
//  }
//  //объявления графических установок для старого индикатора, они же используются для нового, НО Ниже
//  GraphParams params = {27000,0,48,0,MEMDRAW};//PosCursorMain (0) // масштаб 48 ( для уменьшенной картинки)
//  Rect rct={43,18,128,63};
//  //для нового индикатора(рисуем график)
//  if(g_FirstScr)
//  {
//    if(TypeLCD)
//    {
//    params.scale = 13;// for 3.5
//    rct.right=313;//
//    rct.bottom=216;//
//    }
//    else
//    {
//    params.scale = 16;//for 3.2
//    rct.right=256;//
//    rct.bottom=128;//
//    }
//   rct.left=0;
//    rct.top=0;
//    MakeGraphNext( &rct, LogData, 4096, &params );
//    SendDrawNex(NexData,6,rct.right); // ID=6 для графика в просмотре
//    // заполним нужные поля, для нового индикатора в функции прорисовки, не снимая признак первого входа
//  sprintf(Str,"%02d%02d%02d_%02d%02d%01d.sor(%03d)",TimeSaveOTDR.RTC_Year%100,
//          TimeSaveOTDR.RTC_Mon,
//          TimeSaveOTDR.RTC_Mday,
//          TimeSaveOTDR.RTC_Hour,
//          TimeSaveOTDR.RTC_Min,
//          TimeSaveOTDR.RTC_Sec/10,
//          Trace);
//    sprintf(StrI, "t0.txt=\"%s\"яяя", Str);
//    UARTSend2((BYTE*)StrI, strlen(StrI));    // Date/Time/Num записи
//    sprintf(Str, "t5.txt=\"%s\"яяя",CommentsOTDR); 
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 строка комментарии
//  if (Param)
//  {
//    // ищем события
//    CalkEventsKeys (LogData, PointsInImpulse(0), 1);
//    if (GetIndexLN()) // не 2 км ( значит в км)
//      sprintf(Str,"t1.txt=\"%.1f%s\"яяя",fabs(GetPosLine(EndEvenBlk.ELMP[1])) ,MsgMass[20][CurrLang]);//км
//    else
//      sprintf(Str,"t1.txt=\"%.1f%s\"яяя",fabs((GetPosLine(EndEvenBlk.ELMP[1]))*1000.0) ,MsgMass[78][CurrLang]);//м - метры
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 строка комментарии
//    sprintf(Str,"t2.txt=\"%.2f%s\"яяя",(LogData[EndEvenBlk.ELMP[1]]-LogData[EndEvenBlk.ELMP[0]])/1000.0,MsgMass[47][CurrLang]);//дБ
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 строка комментарии
//    if(g_VolORL!=0)sprintf(s_orl,"%.1f",g_VolORL);
//    else sprintf(s_orl,"???");
//    sprintf(Str,"t3.txt=\"%s%s\"яяя",s_orl,MsgMass[47][CurrLang]);//ORL, если измерялось!
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 строка комментарии
//    sprintf(Str,"t4.txt=\"\"яяя");// Empty
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t6.txt=\"%d\"яяя",mg_SetModeLW);// Empty
//    UARTSend2((BYTE*)Str, strlen(Str));    
//  }
//  else
//  {
//    sprintf(Str,"t1.txt=\"%d%s\"яяя", GetLengthLine(GetIndexLN()),MsgMass[20][CurrLang]);
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t2.txt=\"%d%s\"яяя", GetWidthPulse(GetIndexIM()),MsgMass[23][CurrLang]);//ns
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t3.txt=\"%d%s\"яяя",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[18][CurrLang]); // nm
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t4.txt=\"%d%s\"яяя", GetTimeAvrg(GetIndexVRM()),MsgMass[4][CurrLang]);//c
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    //sprintf(Str,"t6.txt=\"%c/%c\"яяя",(SettingRefl.SubModeRef)?('A'):('M'),(GetSubModRefl())?('A'):('M'));// Empty
//    sprintf(Str,"t6.txt=\"%.4f\"яяя",GetWAV_SC(GetPlaceLS(CURRENT)));// коэфф преломления для данной длины волны
//    UARTSend2((BYTE*)Str, strlen(Str));    
//  }
//  }
//  // 
//  //sprintf(Str,"%d",  PointsInImpulse ());//ns
//  //putString(102,19,Str,1,0);//нс
  
}


unsigned short SaveTrace(void) // сохранение рефлектограммы
        // 
      // пока сохраняем без разговоров
      
{ 
  //char Str[22];
  //ClearScreen(screen);
  
//  if (MemTable[0]<MaxMemOTDR)
//  {
//    MemTable[0]++;
//    CurCountFiles = GetSetCntFiles (1); // изменяем счетчик рефлектограмм
//    // сохраняем таблицу сохранений
//    FlashErasePage(TABLEMEMOTDR);
//    FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//    
//    // для востановления настроек при включении прибора 
//    // только здесь пишем в память - поэтому здесь необходимы проверки какая память и куда писать
//    TraceERASE(MemTable[MemTable[0]]);
//    TraceWRITE(MemTable[MemTable[0]]); // записываем рефлектограмму с параметрами сьема  в N ячейку
//    //Total_Time = Total_Sec(ds1337); // считаем секунды
//    // пишем информацию о настройке в заголовок BellCore файла
//    //Set_Bel_core(Total_Time,(unsigned short int)(Im_set[Index_Im]) , (unsigned int)(0x07F281*DS_Code), (unsigned int)(K_pr*100000), sycl, (unsigned int)(0x01A0AA*DS_Code) );
//    //SPI1_read_page0(HEAD); // заголовок файла BELCORE в память
//    
//    NumTrace = MemTable[0];   // указатель памяти рефлектограмм на вновь сохраненную
//  }
//  else// память заполнена (проверим можем ли писать есть ли расширенная память и ее состояние)
//  {
//    if (CheckIDMEM) // прочитаем какая флэшка стоит
//      // есть расширенная память - пишем в расширенную область
//    {
//      if (MemTableExt[0]<MaxMemOTDRExt)// есть место в расширенной области
//      {
//        MemTableExt[0]++;
//        CurCountFiles = GetSetCntFiles (1); // изменяем счетчик рефлектограмм
//        // сохраняем таблицу сохранений
//        FlashErasePage(TABLEMEMOTDREXT);
//        FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//        
//        // для востановления настроек при включении прибора 
//        // только здесь пишем в память - поэтому здесь необходимы проверки какая память и куда писать
//        TraceERASE(0x100 + MemTableExt[MemTableExt[0]]);
//        TraceWRITE(0x100 + MemTableExt[MemTableExt[0]]); // записываем рефлектограмму с параметрами сьема  в N ячейку
//        NumTrace = MemTable[0] + MemTableExt[0];   // устанвливаем номер на сохраненную  памяти рефлектограмм на вновь сохраненную
//        
//      }
//      else
//      {
//        NumTrace = 0;  // указатель памяти рефлектограмм на нулевую// нет места в расширенной памяти
//      }
//    }
//    else // вся память заполнена 
//    {
//      NumTrace = 0;
//    }
//  }
//  TraceERASE(0);
//  TraceWRITE(0); // записываем рефлектограмму с параметрами сьема  в 0 ячейку
  // там же сохраняем номер текущего файла
  return NumTrace;
  //PaintLCD();
}

