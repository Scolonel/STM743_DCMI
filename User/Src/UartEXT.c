
/*****************************************************************************
 *   uartEXT.c:  
 *
 *   
 *   
 *
 *   History
 *   2024.11.06  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/

#include "system.h"

//// таблица для расчета контрольной суммы по полиному 0x1021
//static const short int table[] =
//{
//    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
//    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
//    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
//    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
//    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
//    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
//    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
//    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
//    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
//    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
//    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
//    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
//    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
//    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
//    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
//    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
//    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
//    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
//    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
//    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
//    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
//    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
//    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
//    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
//    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
//    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
//    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
//    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
//    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
//    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
//    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
//    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
//};

volatile DWORD UART0Status;
volatile BYTE UART0TxEmpty = 1;
volatile BYTE UART0Buffer[BUFSIZE];
volatile DWORD UART0Count = 0;
volatile DWORD Reciev = STOP_UART;
volatile BYTE RX_Buf[BUFSIZEUART1];
volatile DWORD CntRX = 0;
volatile uint32_t RSDecYes = 0;
char NameReadFile[32]; // глобальная имя файла при чтении в сохранении
char BufString[225];
char Strn[16]; // to Nextion


void SendBelcoreSet (void); // посылает установки белкора
//#pragma optimize=size  


//      if(ProgFW_LCD) // пробуем транслировать байты в UART2(NEX)
//     {
////    while ( !(UART2TxEmpty & 0x01) );
////    Tmp = U0RBR;
////    U2THR = Tmp;
////    UART2TxEmpty = 0; /* not empty in the THR until it shifts out */
////    UART2Count = 0;
//    Tmp = U0RBR;
//    U2THR = Tmp;
//    // ЭТО ПЕРЕДАЕМ В ИНДИКАТОР, ИЗ ВНЕ! 
//    UART0Buffer[UART0Count] = Tmp;
//    UART0Count++;
//    if ( UART0Count == BUFSIZE )
//    {
//      UART0Count = 0; /* buffer overflow */
//    }
//    // вдруг это команда сборса...
//          if ((UART0Count>=7)&&(!memcmp ((void*)&UART0Buffer[UART0Count-7], "restяяя",7)))
//          {
//      ProgFW_LCD = 2;
//          }
//     }
  

//    if(ProgFW_LCD) // перекинем что приняли из индикатора во внешний мир
//    {
//    Dummy = U2RBR;
//    U0THR = Dummy;
//
//    }

// Прием байта по UART 
void RS_com (BYTE temp)
{
  switch (Reciev)
  {
  case STOP_UART:                    //Состояние готовности к приёму новой команды
    if ((temp=='*')||(temp==';'))
    {
      CntRX=0;
      RX_Buf[CntRX]=temp;
      Reciev=START_UART;
      CntRX++;
    }
    break;
  case START_UART:                   //Состояние приёма текущей команды (исправлено V <-> W) 02/09/03
    {
      switch (temp)
      {
      case '*':                 //Ежели начало посылки во время приёма предыдущей - то ошибка, начинаем сначала.
        CntRX=0;
        RX_Buf[CntRX]=temp;
        CntRX++;
        break;
      case ';':                 //Ежели начало посылки во время приёма предыдущей - то ошибка, начинаем сначала.
        CntRX=0;
        RX_Buf[CntRX]=temp;
        CntRX++;
        break;
      case 0xD:                 //Конец команды
        RX_Buf[CntRX]=temp;
        Reciev=END_UART;
        RX_Buf[CntRX+1]=0x0;
        RSDecYes = 1;            //Команда принята полностью, поэтому надо обработать установим признакRSDecode();
        break;
      case 0xA:                 //Конец команды
        RX_Buf[CntRX]=temp;
        Reciev=END_UART;
        RX_Buf[CntRX+1]=0x0;
        RSDecYes = 1;            //Команда принята полностью, поэтому надо обработать установим признакRSDecode();
        break;
      default:                  //Продолжение приёма команды
        RX_Buf[CntRX]=temp;
        CntRX++;
        if (CntRX > BUFSIZEUART1) // если сторка больше приемного буфера
        {
          CntRX=0;
          Reciev=STOP_UART;
        }
        break;
      }
    }
  case END_UART:                     //Состояние принятой полностью команды
    break;                      //Предыдущая команда не отработана - поэтому ничего не делать
  }
 // SCI1TXBUF = temp; // эхо приема для отладки
}


//  очистка приема по UART0 что бы ждать начала
void ClearRS (void)
{
            CntRX=0;
          Reciev=STOP_UART;

}
// обработка принятой команды по RS
void DecodeCommandRS (void)
{
  unsigned char *c;
  unsigned char value;
  float TmpACI;
  //char Str[128];
  //char BufString[225];
  char StartStr[10]={"#48419\0"}; // 4-х значные номера!!!
  //VICINTENCLEAR  = 1 << UART0_INT; /* Disable Interrupt */
  char NeedTransmit; //
  // 0 - команда не обработана надо выслать Err
  // 1 - передаем буффер BufString
  // 2 - уже передали  (бинарный блок, ничего не передаем
  NeedTransmit = 0;
  if (Reciev==END_UART)                //Ежели приём команды закончен - обработка
  {
    for (int i=0; ((i<CntRX)&&(RX_Buf[i]!=0x20)); i++ )
    {
      if ((RX_Buf[i] >=0x61) && (RX_Buf[i]<=0x7A))RX_Buf[i] = RX_Buf[i] - 0x20;
    }
    switch (RX_Buf[0])
    {
    case '*':
      //123      if (GetModeDevice() != MODEMEASURE)
      if(1)
      {
        // приняли признак *IDN -  идентификатор
        //-----------------
        if (!memcmp ((void*)RX_Buf, "*IDN?",5))
          //if ((RX_Buf[1]=='I')&&(RX_Buf[2]=='D')&&(RX_Buf[3]=='N')&&(RX_Buf[4]=='?'))
        {
          GetDeviceName( BufString ); // запрос сторки идентификатора
          NeedTransmit = 1;
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
        }
        if (!memcmp ((void*)RX_Buf, "*IDHW",5)) // идентификатор "железной" реализации
          //if ((RX_Buf[1]=='I')&&(RX_Buf[2]=='D')&&(RX_Buf[3]=='N')&&(RX_Buf[4]=='?'))
        {
          BufString[0]= 0;
          GetDeviceHW( BufString ); // запрос сторки идентификатора
          NeedTransmit = 1;
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
        }
        if (!memcmp ((void*)RX_Buf, "*IDLCD",6)) // идентификатор ПО "LCD"
          //if ((RX_Buf[1]=='I')&&(RX_Buf[2]=='D')&&(RX_Buf[3]=='N')&&(RX_Buf[4]=='?'))
        {
          sprintf(BufString,"%s\r", VerFW_LCD);
          NeedTransmit = 1;
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
        }
        if (!memcmp ((void*)RX_Buf, "*ALARM",6)) // сброс памяти рефлектограмм при плохой флэшке!
        {
          InitReflSet (); // инициализация установок рефлектометра
          GetPlaceLS (NEXT); // устанавливаем заведомо существующий лазер
          //123          AlarmReWriteTrace();
          //123          SetNumTrace (0); 
          sprintf(BufString,"AlarmReset\r");
          NeedTransmit = 1;
          UARTSendExt ((BYTE*)"AlarmReset\r", 11);
        }
      }
      break;
    case ';':
      //      // выдать экран 
      //      if (!memcmp ((void*)RX_Buf, ";SCR?",5)) // возможно уже не нужна!
      //      {
      //        // приняли признак ;SCR? - 
      //        // ;scr?  - получить файл картинки
      //        // начинаем передачу картинки (Заголовок)
      //        sprintf (StartStr, "#41024");
      //        UARTSendExt ((BYTE*)StartStr, 6);
      //        UARTSendExt ((BYTE*)GetScreen(), 1024);
      //      }
      
      if (!memcmp ((void*)RX_Buf, ";CFGSAV",7))
      { // сохраняем конфигурацию установленную
        WriteNeedStruct (0x01);
        NeedTransmit = 1;
        UARTSendExt ((BYTE*)"OK\r", 3);
      }
      //  ;syst:uart:hi установка скорости UART 460800 ответ уже на большой скорости
      if (!memcmp ((void*)RX_Buf, ";SYST:UART:HI",13)) //
      {
        UARTSendExt ((BYTE*)"OK\r", 3);
        NeedTransmit = 1;
        // отсылаем на старой скорости 
        //while ( !(UART0TxEmpty & 0x01) ); // ждем конца передачи только после этого перестраиваемся
        huart3.Init.BaudRate = 460800;
        if (HAL_UART_Init(&huart3) != HAL_OK)
        {
          Error_Handler();
        }
        g_SpeedUart = 8; // 1 - LO(57600), 2-ME(115200), 8-HI(460800)

        
      }
      if (!memcmp ((void*)RX_Buf, ";SYST:UART:ME",13)) //115200
      {
        UARTSendExt ((BYTE*)"OK\r", 3);
        
        NeedTransmit = 1;
        // отсылаем на старой скорости
        //while ( !(UART0TxEmpty & 0x01) ); // ждем конца передачи только после этого перестраиваемся
        
        huart3.Init.BaudRate = 115200;
        if (HAL_UART_Init(&huart3) != HAL_OK)
        {
          Error_Handler();
        }
        g_SpeedUart = 2; // 1 - LO(57600), 2-ME(115200), 8-HI(460800)

        
      }
      //  ;syst:uart:lo установка скорости UART 57600 ответ уже на меньшей скорости
      if (!memcmp ((void*)RX_Buf, ";SYST:UART:LO",13)) //
      {
        UARTSendExt ((BYTE*)"OK\r", 3);
        // отсылаем на старой скорости
        NeedTransmit = 1;
        //while ( !(UART0TxEmpty & 0x01) ); // ждем конца передачи только после этого перестраиваемся
        
        huart3.Init.BaudRate = 57600;
        if (HAL_UART_Init(&huart3) != HAL_OK)
        {
          Error_Handler();
        }
        g_SpeedUart = 1; // 1 - LO(57600), 2-ME(115200), 8-HI(460800)

      }
      // 
      // ;MEMM:LOAD:FILE? xx
      // другая конфигурация хранения , возможно изменение
      //123
      // !!!!ACHTUNG!!!!
            if (!memcmp ((void*)RX_Buf, ";MMEM:LOAD:FILE? ",17)) //RX_Buf[17] - номер рефл
            {
      //        //        unsigned long NowEND;
      //        //        unsigned long NowBEG;
      //        unsigned short NumEventNow = GetNumEvents();
      //        unsigned short NumTr = (unsigned short)(atoi((char*)&RX_Buf[17]));
      //        if (NumTr > GetNumTraceSaved(0)) NumTr = 0; // заданная рефлектограмма не существует
      //        SetNumTrace (NumTr); // установка номера трассы
      //        SetModeDevice (MODEMEMR); // принудительная установка режима прибора 
      //        // надо прочитать указанную рефлектограмму
      //        // первое заполнение надо прочитать файл
      //        if (GetNumTrace()) // если не нулевая то читаем по таблице
      //          TraceREAD(GetNumTraceSaved(GetNumTrace()));//  читаем файл который надо передать// 27/01/2011 неадекватно считывала рефлектограмму
      //        else  TraceREAD(0);
      //        // ищем события в линии (25.05.2011 пока конец линии)
      //        InitEventsTable (); // инициализация структур событий
      //        // признак разрешения событий при передаче
      //        if (GetSetEnaEvents (0)) // проверяем признак разрешения событий
      //        {
      //          // ищем события и заполняем файл
      //          NumEventNow =  (CalkEventsKeys (LogData, PointsInImpulse(0), 1)); 
      //          // расчет погонного затухания если есть точка
      //          /**/
      //          if (NumEventNow)          //имеем события - 
      //          {
      //            if (EndEvenBlk.ELMP[0]!=EndEvenBlk.ELMP[1]) // есть линия! занесем параметры
      //            {
      //              TmpACI = GetPosLine(EvenTrace[0].EPT);
      //              //TmpACI = ;
      //              TmpACI = (LogData[EvenTrace[0].EPT]-LogData[0])/TmpACI;//GetPosLine(EvenTrace[0].EPT);
      //              EvenTrace[0].ACI = (short int)TmpACI;
      //              EndEvenBlk.ELMP[1] = CalkEPT (EndEvenBlk.ELMP[1]); // расчет значений ELMP для конца линии от положения курсора
      //              
      //            }
      //            // цикл заполнения событий 
      //            if (NumEventNow>1)
      //            {
      //              for (int i=1;i<NumEventNow;++i)
      //              {
      //                TmpACI = GetPosLine(EvenTrace[i].EPT-EvenTrace[i-1].EPT);
      //                TmpACI = (LogData[EvenTrace[i].EPT]-(LogData[EvenTrace[i-1].EPT]+EvenTrace[i-1].EL))/TmpACI;
      //                EvenTrace[i].ACI = (short int)TmpACI;
      //                //EvenTrace[i].ACI = (LogData[EvenTrace[i].EPT]-(LogData[EvenTrace[i-1].EPT]+EvenTrace[i-1].EL))/GetPosLine(EvenTrace[i].EPT-EvenTrace[i-1].EPT);
      //                
      //                //EvenTrace[i-1].EPT = CalkEPT (EvenTrace[i-1].EPT); // расчет значений EPT для событий от положения курсора
      //              }
      //            }
      //            // Заполнение события и конец линии
      //            
      //            for (int i=0;i<NumEventNow;++i)
      //            {
      //              EvenTrace[i].EPT = CalkEPT (EvenTrace[i].EPT); // расчет значений EPT для событий от положения курсора
      //            }
      //          }
      //          
      //        }
      //        // тест загрузка формирование событий
      //        //NumEventNow = 9;
      //        //TestLoadEvents (NumEventNow);
        //< < < < <  !!! В Н И М А Н И Е !!! > > > > >
              // попробуем записать файл
              unsigned short NumEventNow = 0; // пока без событий
              // начинаем передачу трассы (Заголовок)
              //sprintf (StartStr, "#4%4d",8419 + ((NumEventNow)?(NumEventNow*32+40):(0)));
              // для 5 значных размеров файла и числа точек,
              // было 8192(sizePoints*2)+225(head1.h)+2(checksum)? = 8419
              // стало 8192(OUTSIZE*2)+225(head1.h)+2(checksum)? = 10827
              sprintf (StartStr, "#5%05d",(OUTSIZE*2) + 227 + ((NumEventNow)?(NumEventNow*32+40):(0)));
              UARTSendExt ((BYTE*)StartStr, 7);
              // Мар страница белкора с учетом Таблицы событий (блок 0)
              // если есть таблица событий....
              GetHeaderBelcore (BufString, 0, NumEventNow); // заполняем шапку белкора первые 56 байт Block=0
              UARTSendExt ((BYTE*)BufString, 56+16*((NumEventNow)?(1):(0)));
              unsigned short old_crc = 0xffff; 
              unsigned short new_crc = 0xffff;
              c = (unsigned char*)&BufString;
              for (int i=0;i<56+16*((NumEventNow)?(1):(0));i++)
              {
                /* первый вариант подсчета контрольной суммы - табличный                                             */		
                value = *c;
                new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                old_crc = new_crc;
                c++;
              }
              // заполняем шапку белкора  62 байт Block=1 (продолжение Мар блока + GenParams)
              GetHeaderBelcore (BufString, 1, NumEventNow); 
              UARTSendExt ((BYTE*)BufString, 62);
              c = (unsigned char*)&BufString;
              for (int i=0;i<62;i++)
              {
                /* Считаем контрольную сумму переданного блока                                             */		
                value = *c;
                new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                old_crc = new_crc;
                c++;
              }
              // заполняем шапку белкора  94 байт Block=2 - (SupParams FxdParam)
              GetHeaderBelcore (BufString, 2, NumEventNow); 
              UARTSendExt ((BYTE*)BufString, 95);
              c = (unsigned char*)&BufString;
              for (int i=0;i<95;i++)
              {
                /* Считаем контрольную сумму переданного блока                                             */		
                value = *c;
                new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                old_crc = new_crc;
                c++;
              }
              // Проверяем и передаем блок событий если он есть (блок событий)
              if (NumEventNow) // если есть события 2 байта +
                // события в фиксированном размере для каждого 32 байта  +  22 байт общее для всего блока
              {
                // передаем  число событий  2 байта
                UARTSendExt ((BYTE*)&NumEventNow, 2);
                c = (unsigned char*)&NumEventNow;
                for (int i=0;i<2;i++)
                {
                  /* Считаем контрольную сумму переданного блока                                             */		
                  value = *c;
                  new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                  old_crc = new_crc;
                  c++;
                }
                // передаем информационные блоки событий  N*32
                for (int s=0; s<NumEventNow; s++)
                {
                  UARTSendExt ((BYTE*)&EvenTrace[s], 32);
                  c = (unsigned char*)&EvenTrace[s];
                  for (int i=0;i<32;i++)
                  {
                    /* Считаем контрольную сумму переданного блока                                             */		
                    value = *c;
                    new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                    old_crc = new_crc;
                    c++;
                  }
                  
                }
                // передаем конечный блок событий 22 байта
                UARTSendExt ((BYTE*)&EndEvenBlk, 22);
                c = (unsigned char*)&EndEvenBlk;
                for (int i=0;i<22;i++)
                {
                  /* Считаем контрольную сумму переданного блока                                             */		
                  value = *c;
                  new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                  old_crc = new_crc;
                  c++;
                }
              }
              
              // заполняем шапку белкора 12 байт Block=3 (DataPts)
              GetHeaderBelcore (BufString, 3, NumEventNow); 
              UARTSendExt ((BYTE*)BufString, 12);
              c = (unsigned char*)&BufString;
              for (int i=0;i<12;i++)
              {
                /* Считаем контрольную сумму переданного блока                                             */		
                value = *c;
                new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                old_crc = new_crc;
                c++;
              }
              // посчитаем контрольную сумму до передачи
              c = (unsigned char*)&LogData;
              for (int i=0;i<OUTSIZE*2;i++)
              {
                /* первый вариант подсчета контрольной суммы - табличный                                             */		
                value = *c;
                new_crc = (old_crc << 8) ^ table[((old_crc >> 8) ^ ((unsigned short int)value)) & 0xff];
                old_crc = new_crc;
                c++;
              }
              // запишем в LogData
              LogData[OUTSIZE] = new_crc;
              // блок данных 
              UARTSendExt ((BYTE*)LogData, OUTSIZE*2+2);
              
              //UARTSendExt ((BYTE*)&new_crc, 2);
        NeedTransmit = 1;
              
              
              //ClearScreen(screen);
              
            }
      //123      
      //      // ;MEMM:NAME? -  чтение комментариев сохраненных рефлектограмм
      //      if (!memcmp ((void*)RX_Buf, ";MMEM:NAME?",11)) //
      //      {
      //        char BufStringIn[24];
      //        for (int i = 1 ; i <= GetNumTraceSaved(0); i++)
      //        {
      //          unsigned long PorNom = FlashReadCommTrace (i, (unsigned char*)BufStringIn);
      //          memcpy( &BufStringIn[19], "\0", 1 ); // 
      //          sprintf((char*)BufString,"(%03d)%s\n",i, BufStringIn);
      //          UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //        }
      //        
      //        
      //        sprintf(BufString,"\r");
      //        UARTSendExt ((BYTE*)BufString, 1);
      //      }
      //123
      // другая организация хранения необходимо изменеие      
            // ;MEMM:NFIL? -  чтение имен файлов сохраненных рефлектограмм
            if (!memcmp ((void*)RX_Buf, ";MMEM:NFIL?",11)) //
            {
              //EnaPrintRes =1;
              //PressKey =1;
              SDMMC_SDCard_DIR();
              NeedTransmit = 1;
      //        for (int i = 1 ; i <= GetNumTraceSaved(0); i++)
      //        {
      //          unsigned long PorNom = FlashReadTimeTrace (i);
      //          // 
      //          sprintf((char*)BufString,"(%03d)%s\n", i, NameReadFile);
      //          UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //        }
      //        
      //        
      //        sprintf(BufString,"\r");
      //        UARTSendExt ((BYTE*)BufString, 1);
            }
      //123
      // контроль свободной памяти рефлектограмм
      // сейчас возможно не АКТУАЛЬНО
      //      // ;MEMM:FREE?
      //      if (!memcmp ((void*)RX_Buf, ";MMEM:FREE?",11)) //
      //      {
      //        //sprintf(BufString,"%d,%d\r", GetNumTraceSaved(0), ((CheckIDMEM)?(MaxMemOTDR+MaxMemOTDRExt):(MaxMemOTDR)));
      //        sprintf(BufString,"%d,%d\r", GetNumTraceSaved(0), MAXMEMALL);
      //        UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //      }
      //123      
      //      // ;MEMM:INIT -  удаление рефлектограмм
      //      
      //      if (!memcmp ((void*)RX_Buf, ";MMEM:INIT",10)) //
      //      {
      //        // зачистим таблицу , то есть полностью переиндекируем, для "грязных" флэшек
      //        SetNumTrace (0); // установка номера трассы
      //        SetModeDevice (MODEMEMR); // принудительная установка режима прибора       
      //        sprintf(BufString,"%d deleted\r", DeletingAllTrace ());
      //        UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //      }
      //      // ;MEMM:REAN -  попытка реанимации рефлектграмм
      //      if (!memcmp ((void*)RX_Buf, ";MMEM:REAN",10)) //
      //      {
      //        // примем число сколько можно востановить
      //        unsigned short NumFiles = (unsigned short)(atoi((char*)&RX_Buf[10]));
      //        if(NumFiles>450) NumFiles = 0;
      //        SetModeDevice (MODEMEMR); // принудительная установка режима прибора       
      //        sprintf(BufString,"%d reanimate\r", ReanimateTrace(NumFiles));
      //        UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //      }
      //            // ;MEMM:TABL -  чтение таблицы сохранения
      //      if (!memcmp ((void*)RX_Buf, ";MMEM:TABL",10)) //
      //      {
      //        for(int i=0;i<ARRAY_SIZE(MemTable);i++) 
      //        {
      //          sprintf(BufString,"%d-%d\r", i, MemTable[i]);
      //          UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //        }
      //        
      //          if(CheckIDMEM)
      //          {
      //            for(int i=0;i<ARRAY_SIZE(MemTableExt);i++) 
      //            {
      //              sprintf(BufString,"%d-%d\r", i + ARRAY_SIZE(MemTable), MemTableExt[i]);
      //              UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //            }
      //          }
      //      }
      //            // Установка и запрос режима разрешения корректировки мертвых зон
      //      //  ;syst:enmz?
      //      if (!memcmp ((void*)RX_Buf, ";SYST:ENMZ?",11)) //
      //      {
      //        sprintf(BufString,"%01d\r",GetEnaMZ());// получение текущего 
      //        UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //      }
      //      //  ;syst:enmz N
      //      if (!memcmp ((void*)RX_Buf, ";SYST:ENMZ ",11)) //
      //      {
      //        BYTE NumMod = (BYTE)(atoi((char*)&RX_Buf[11]));
      //        if(NumMod > 1) NumMod = 0;
      //        SetEnaMZ(NumMod); // режим разрешения корректировки мертвых зон
      //        sprintf(BufString,"%01d\r",GetEnaMZ());// получение текущего 
      //        UARTSendExt ((BYTE*)BufString, strlen (BufString));
      //        
      //      }
      
      // Установка и запрос режима работы прибота по индексу
      //  ;syst:mode?
      if (!memcmp ((void*)RX_Buf, ";SYST:MODE?",11)) //
      {
        sprintf(BufString,"%01d\r",GetCurrentModeDevice ());// получение текущего режима прибора
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      //  ;syst:mode N
      if (!memcmp ((void*)RX_Buf, ";SYST:MODE ",11)) //
      {
        BYTE NumMode = (BYTE)(atoi((char*)&RX_Buf[11]));
        if(NumMode > 6) NumMode = 0;
        SetModeDevice (NumMode); // принудительная установка режима прибора
        
        //sprintf(BufString,"%01d\r",GetCurrentModeDevice ());// получение текущего режима прибора
        //UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // запрос установленных лазеров 
      //  ;syst:ava?
      if (!memcmp ((void*)RX_Buf, ";SYST:AVA?",10)) //
      {
        
        sprintf(BufString,"%d,%d,%d\r",GetLengthWaveLS (0),GetLengthWaveLS (1),GetLengthWaveLS (2)); // получение длины волны от индекса установочного места
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // запрос конфигурации OLT (измерителя ) 
      //  ;syst:olt?
      if (!memcmp ((void*)RX_Buf, ";SYST:OLT?",10)) //
      {
        
        sprintf(BufString,"%d\r",GetCfgPM ()); // получение установки измерителя); // 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // запрос конфигурации VFL (красного глаза) 
      //  ;syst:vfl?
      if (!memcmp ((void*)RX_Buf, ";SYST:VFL?",10)) //
      {
        
        sprintf(BufString,"%d\r",GetCfgRE ()); // получение установки Redeye); // 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // запрос значений мертвых зон
      //  ;syst:mzn?
      if (!memcmp ((void*)RX_Buf, ";SYST:MZN?",10)) //
      {
        sprintf(BufString,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\r" // чтение мертвой зоны по индексу
                ,GetBegShiftZone(0)
                  ,GetBegShiftZone(1)
                    ,GetBegShiftZone(2)
                      ,GetBegShiftZone(3)
                        ,GetBegShiftZone(4)
                          ,GetBegShiftZone(5)
                            ,GetBegShiftZone(6)
                              ,GetBegShiftZone(7)
                                ,GetBegShiftZone(8)
                                  ,GetBegShiftZone(9)
                                    ,GetBegShiftZone(10)
                                      ,GetBegShiftZone(11)
                                        ,GetBegShiftZone(12));
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // ;syst:date dd,mm,yyyy - установка даты
      if (!memcmp ((void*)RX_Buf, ";SYST:DATE ",11)) //
      {
        int i=11;
        DWORD Num;
        RTCTime SetNewTime;
        SetNewTime = RTCGetTime(); // получаем текущее время
        Num = atoi((char*)&RX_Buf[i]); // день
        if ((Num>31)||(Num<1)) Num = 1;
        SetNewTime.RTC_Mday = Num;
        i++;
        if (RX_Buf[i] != ',') i++;
        i++;
        Num = atoi((char*)&RX_Buf[i]); // месяц
        if ((Num>12)||(Num<1)) Num = 1;
        SetNewTime.RTC_Mon = Num;
        i++;
        if (RX_Buf[i] != ',') i++;
        i++;
        Num = atoi((char*)&RX_Buf[i]); // год
        if ((Num>2050)||(Num<2024)) Num = 2024;
        SetNewTime.RTC_Year = Num;
        
        RTCSetTime( SetNewTime ); // запишем новую дату
        //New
        sprintf(BufString,"%d-%d-%d\r" // выдаем дату
                ,SetNewTime.RTC_Year
                  ,SetNewTime.RTC_Mon
                    ,SetNewTime.RTC_Mday);
        //UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
        
      }
      // ;syst:time hh,mm,ss - установка времени
      if (!memcmp ((void*)RX_Buf, ";SYST:TIME ",11)) //
      {
        int i=11;
        DWORD Num;
        RTCTime SetNewTime;
        SetNewTime = RTCGetTime(); // получаем текущее время
        Num = atoi((char*)&RX_Buf[i]); // часы
        if (Num>23) Num = 0;
        SetNewTime.RTC_Hour = Num;
        i++;
        if (RX_Buf[i] != ',') i++;
        i++;
        Num = atoi((char*)&RX_Buf[i]); // минуты
        if (Num>59) Num = 0;
        SetNewTime.RTC_Min = Num;
        i++;
        if (RX_Buf[i] != ',') i++;
        i++;
        Num = atoi((char*)&RX_Buf[i]); //секунды 
        if (Num>59) Num = 0;
        SetNewTime.RTC_Sec = Num;
        
        RTCSetTime( SetNewTime ); // запишем новое время
        sprintf(BufString,"%d:%d:%d\r" // выдаем дату
                ,SetNewTime.RTC_Hour
                  ,SetNewTime.RTC_Min
                    ,SetNewTime.RTC_Sec);
        //UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // ;syst:key ss -  имитация нажатия кнопки
      if (!memcmp ((void*)RX_Buf, ";SYST:KEY ",10)) //
      {
        int i=10;
        DWORD Num;
        unsigned char nBtn, nSts; 
        Num = atoi((char*)&RX_Buf[i]); // 
        nBtn = Num%10;
        nSts = (Num/10)%10;
        SetKeyMd (nBtn, nSts);
        sprintf(BufString,"%d\r",Num); // выдаем // 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // ;lcd:thup n -  просыпаемся от тача
      if (!memcmp ((void*)RX_Buf, ";LCD:THUP ",10)) //
      {
        int i=10;
        DWORD Num;
        Num = atoi((char*)&RX_Buf[i]); // 
        sprintf(Strn, "thup=%dяяя", Num); //!
        NEX_Transmit((void*)Strn);    // 
        sprintf(BufString,"%d\r",Num); // выдаем // 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      // ;lcd:slep n -  спать не спать
      if (!memcmp ((void*)RX_Buf, ";LCD:SLEP ",10)) //
      {
        int i=10;
        DWORD Num;
        Num = atoi((char*)&RX_Buf[i]); // 
        sprintf(Strn, "sleep=%dяяя", Num); //!
        NEX_Transmit((void*)Strn);    // 
        sprintf(BufString,"%d\r",Num); // выдаем // 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      
      //===== КОМАНДЫ РЕЖИМА РЕФЛЕКТОМЕТРА УСТАНОВОК и ПРОСМОТРА ======================    
      // ЗАПРОС ПОлного дампа накоплений
      if (!memcmp ((void*)RX_Buf, ";OTDR:DUMP",10))
      {
        int i=0, sm=0;
        for (i=0; i<8; i++)
        {
          if (Head_RAW.Head[i]== 0x20) sm++;
          else break;
        }
        //TST_KTA(1);
        UARTSendExt ((BYTE*)&Head_RAW.Head[sm], sizeof(Head_RAW)-sm);//
        //TST_KTA(0);
        
        // надо передать 2 блока заголовок и дамп
        // передадим блоками по 2048 байт то есть для проверки
        // 0x1200 = 4608 (9*512*4)
        //for(int j=0; j<9; j++)
        //    UARTSendExt ((BYTE*)&RawData[j*512], 2048);// 

        //TST_KTA(1);
        //UARTSendExt ((BYTE*)&RawData, sizeof(RawData));// 
        UARTSendExt ((BYTE*)&RawData, 0x4800);// 
        //TST_KTA(0);
        NeedTransmit = 1;
      }
      if (GetCurrentModeDevice()==MODEMEASURE)
      {
        if (!memcmp ((void*)RX_Buf, ";OTDR:STOP",10)) //остановка текущего измерения
          // попробуем сгенерить прерывание от кнопки
        {
          rawPressKeyS = 1;
          sprintf(BufString,"OK\r"); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          NeedTransmit = 1;
        }
      }
      if ((GetCurrentModeDevice()==MODESETREFL)||(GetCurrentModeDevice()==MODEREFL))
        // режим рефлектометра
      {
        //;SYST:SET? - запрос установок рефлектометра
        if (!memcmp ((void*)RX_Buf, ";SYST:SET?",10)) //
        {
          BYTE Data = GetIndexLN();
          memcpy( BufString, "#18", 3 );
          memcpy( &BufString[3], &Data, 1 ); // 
          Data = GetIndexIM();
          memcpy( &BufString[4], &Data, 1 );
          Data = GetIndexVRM();
          memcpy( &BufString[5], &Data, 1 );
          Data = GetPlaceLS(CURRENT);
          memcpy( &BufString[6], &Data, 1 );
          float IndexKpr = GetIndexWAV();
          memcpy( &BufString[7], &IndexKpr, 4 );
          memcpy( &BufString[11], "\r", 1 );
          
          UARTSendExt ((BYTE*)BufString, 12);
          NeedTransmit = 1;
        }
        //;SYST:SET #18 - установка рефлектометра по команде
        if (!memcmp ((void*)RX_Buf, ";SYST:SET #18",13)) //
        {
          float Data;
          SetIndexLN((BYTE)RX_Buf[13]); // индекс длины линии
          SetIndexIM((BYTE)RX_Buf[14]); // индекс длины линии
          SetIndexVRM((BYTE)RX_Buf[15]); // индекс длины линии
          SetPlaceLS ((BYTE)RX_Buf[16]); // установка требуемого лазера 
          GetPlaceLS (CURRENT);// установка индекса новой длины волны из списка установленных лазеров CURRENT -текущий, NEW- новый (следующий)
          unsigned char *c = (unsigned char*)&Data;
          for (int i=0;i<4;i++)
          {
            *c = RX_Buf[17+i]; // 16+... для мониторинга
            c++;
          }
          SetIndexWAV(Data);
          SetSubModRefl (MANUAL); // установка режима рефлектометра 
          SetModeDevice (MODESETREFL); // принудительная установка режима прибора -  установка рефлектометра
          SendCfgOTDR (BufString); // передача конфигурации рефлектометра (настройки)
          NeedTransmit = 1;
          //UARTSendExt ((BYTE*)BufString, 12);
        }
        // ;syst:set:kpr  - коэфф преломления
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:KPR ",14)) //
        {
          float Data;
          Data = atof((char*)&RX_Buf[14]);
          SetIndexWAV(Data);
          sprintf(BufString,"%.4f\r",Data); // получение установки Redeye); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
          
        }
        // ;syst:set:cfg  - установка ASCII коменда
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:CFG ",14)) //
        {
          SetIndexLN((BYTE)atoi((char*)&RX_Buf[14])); // индекс длины линии
          SetIndexIM((BYTE)atoi((char*)&RX_Buf[16])); // индекс длины линии
          SetIndexVRM((BYTE)atoi((char*)&RX_Buf[18])); // индекс длины линии
          SetPlaceLS ((BYTE)atoi((char*)&RX_Buf[20])); // установка требуемого лазера
          // индекс рабочего места 0,1,2
          // если стартуем перепишем выбранную длину волны в соответствии с памяти
          SetIndxSeqLS();
          
          //g_SetModeLW = SettingRefl.SW_LW;
          g_AutoSave = 0; // длина волны одиночная сброс авто сохранения/
          GetPlaceLS (CURRENT);// установка индекса новой длины волны из списка установленных лазеров CURRENT -текущий, NEW- новый (следующий)
          SetIndexWAV(atof((char*)&RX_Buf[22]));
          SetSubModRefl (MANUAL); // установка режима рефлектометра 
          //SetModeDevice (MODESETREFL); // принудительная установка режима прибора -  установка рефлектометра
          // надо может чуток потупить?
          SendCfgOTDR (BufString); // передача конфигурации рефлектометра (настройки)
          NeedTransmit = 1;
          
        }
        // ;syst:get:cfg?  - запрос установок рефлектометра ASCII коменда
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:CFG?",14)) //
        {
          SendCfgOTDR (BufString); // передача конфигурации рефлектометра (настройки)
          NeedTransmit = 1;
        }
        // ;INIT
        if (!memcmp ((void*)RX_Buf, ";INIT",5)) //
        { 
          if (RX_Buf[5]=='E')
          {
            // 15 с измерение с установленными параметрами, расчет событий и выдача их по окончании измерений
            SetIndexVRM (0); // принудительная установка индекса времени накопления на 15 сек
            if (!GetSetEnaEvents(0))GetSetEnaEvents(1); // устанавливаем признак разрешения событий 
            SetGetMonEna (1);
          }
          RemoutCtrl = 1;
          if (GetIndexVRM()>3) // устанавливаем минимальное время ( для дистанционного управления не подходит)
            SetIndexVRM (0); // установка индекса времени накопления на 15 сек
          sprintf(BufString,"%d\r", GetTimeAvrg(GetIndexVRM())+5);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          SetModeDevice (MODEMEASURE); // принудительная установка режима прибора -  запкск рефлектометрии с установленными параметрами
          NeedTransmit = 1;
          //else  sprintf(BufString,"Not stopрed\r");
        }
        // получение необработанных данных
        if (!memcmp ((void*)RX_Buf, ";GET:RAW:DATA",13)) //
        { 
          for (int i = 0; i<RAWSIZE;++i)
          {
            sprintf(BufString,"%u\n", RawData[i]);//c
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
          }
          sprintf(BufString,"\r");//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // получение логарифмических данных
        if (!memcmp ((void*)RX_Buf, ";GET:LOG:DATA",13)) //
        { 
          for (int i = 0; i<OUTSIZE;++i)
          {
            sprintf(BufString,"%d\n", LogData[i]);//c
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
          }
          sprintf(BufString,"\r");//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // 11.10.2011 - команда установки комментариев внешней программой
        // ;syst:set:comm text  - изменение коментариев сохранения рефлектограммы
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:COMM ",15)) //
        {
          int i;
          char c;
          for (i=0; i<ARRAY_SIZE(CommentsOTDR)-1; ++i)
          {
            c =(char)RX_Buf[15+i];
            if (c >= 0x20) CommentsOTDR[i] = c;
            else break;
          }
          while (i<ARRAY_SIZE(CommentsOTDR)-1)
          {
            CommentsOTDR[i] = 0x20;
            i++;
          }
          CommentsOTDR[i] = 0;
          UARTSendExt ((BYTE*)CommentsOTDR, strlen (CommentsOTDR));
          sprintf(BufString,"\r");//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // 11.10.2011 - команда принудительного сохранения рефлектограммы
        // ;save:trace  - сохранениe рефлектограммы 0
        if (!memcmp ((void*)RX_Buf, ";SAVE:TRACE",11)) //
        {
          // сохранение рефлектограммы по команде от UART)
          SaveNewOTDRTrace (1); // ответ в функции т.к. = 1
          NeedTransmit = 1;
          
        }
        // команды управления OTDR конфигурация 
        if (!memcmp ((void*)RX_Buf, ";OTDR:CFG:",10)) //
        {
          DWORD Data ;
          int Index;
          // установка длины линии 
          if (!memcmp ((void*)&RX_Buf[10], "LIN",3)) // 50000 в 100пС ~ 1km
          {
            Data = (DWORD)atoi((char*)&RX_Buf[13]);
            Data = (DWORD)(Data/50000);
            for (Index = 0; Index <LENGTH_LINE_NUM; ++Index)
            {
              if (GetLengthLine(Index)>= Data) break;
            }
            SetIndexLN(Index);
            SetSubModRefl (MANUAL); // переводим в ручной режим
            //Head_RAW.ValDS = (ADCPeriod*50000)/(NumPointsPeriod[GetIndexLN()]); //  устанавливаем значения DS для установленного режима измерения
            SetHeadFileRaw(0); // устанавливаем параметры съема в Заголовок файла необработанных данных
            
            Data  = GetLengthLine(GetIndexLN());
            sprintf(BufString,"%d, %d\r",Data,Head_RAW.ValDS); // 
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
            NeedTransmit = 1;
          }
          // установка зондирующего импульса
          if (!memcmp ((void*)&RX_Buf[10], "ZI",2)) //
          {
            Data = (DWORD)atoi((char*)&RX_Buf[12]);
            for (Index = 0; Index <WIDTH_PULSE_NUM; ++Index)
            {
              if (GetWidthPulse(Index)>= Data) break;
            }
            SetIndexIM(Index);
            SetSubModRefl (MANUAL); // переводим в ручной режим
            Data  = GetWidthPulse(GetIndexIM());
            SetHeadFileRaw(0); // устанавливаем параметры съема в Заголовок файла необработанных данных
            sprintf(BufString,"%d\r",Data); // 
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
            NeedTransmit = 1;
          }
        }
        
      } // режим рефлектометра
      // можно принять в любом режиме
      // установка проверка параметров измерения ORL
      if (!memcmp ((void*)RX_Buf, ";GET:ORL:PRM?",13)) //
      {
        sprintf(BufString,"ORL=%.1f\nUPdB=%.1f\nkLog=%.1f\nOffdB=%.1f\r", g_VolORL, g_UpGrdB, g_kLog, g_OffSetdB);//c
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      if (!memcmp ((void*)RX_Buf, ";SET:ORL:P",10)) //
      {
        float Datf;
        if (!memcmp ((void*)&RX_Buf[10], "UP ",3)) //
        {
          Datf = atof((char*)&RX_Buf[13]);
          g_UpGrdB = Datf;
        } 
        if (!memcmp ((void*)&RX_Buf[10], "LG ",3)) //
        {
          Datf = atof((char*)&RX_Buf[13]);
          g_kLog = Datf;
        } 
        if (!memcmp ((void*)&RX_Buf[10], "DB ",3)) //
        {
          Datf = atof((char*)&RX_Buf[13]);
          g_OffSetdB = Datf;
        } 
        sprintf(BufString,"ORL=%.2f\nUPdB=%.2f\nkLog=%.2f\nOffdB=%.2f\r", g_VolORL, g_UpGrdB, g_kLog, g_OffSetdB);//c
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        NeedTransmit = 1;
      }
      
      //===== КОМАНДЫ РЕЖИМА УСТАНОВОК ======================    
      if (GetCurrentModeDevice()==MODESETUP)
      {
        WORD Data;
        // ;set:num  - НОМЕР ПРИБОРА
        if (!memcmp ((void*)RX_Buf, ";SET:NUM ",9)) //
        {
          Data = (WORD)atoi((char*)&RX_Buf[9]);
          SetNumDevice(Data); // установка номера  прибора      
          sprintf(BufString,"%d\r",Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // ;set:smX XXXX // посадочные места лазеров - длины волн
        if (!memcmp ((void*)RX_Buf, ";SET:SM",7)) //
        {
          Data = (WORD)atoi((char*)&RX_Buf[9]);
          SetupWavePlaceLS((BYTE)(RX_Buf[7]-'1'),Data); // установка длин волн лазеров в посадочные места 
          sprintf(BufString,"%d %d\r",RX_Buf[7]-'1',Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
          
        }
        // ;set:LW  - число и тип источников
        if (!memcmp ((void*)RX_Buf, ";SET:LW ",8)) //
        {
          BYTE Data = (BYTE)atoi((char*)&RX_Buf[8]);
          SetTypeDevice (Data); // установка типа прибора для ТОПАЗОВ
          sprintf(BufString,"%d\r",Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // ;set:sf  - наличие или вид измерителя
        if (!memcmp ((void*)RX_Buf, ";SET:SF ",8)) //
        {
          BYTE Data =(BYTE)atoi((char*)&RX_Buf[8]);
          SetCfgPM  (Data); // установка наличия измерителя
          sprintf(BufString,"%d\r",Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // ;set:re  - Признак красного глаза
        if (!memcmp ((void*)RX_Buf, ";SET:RE ",8)) //
        {
          BYTE Data =(BYTE)atoi((char*)&RX_Buf[8]);
          SetCfgRE  (Data); // установка наличия измерителя
          sprintf(BufString,"%d\r",Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // ;set:ap  - установка признака лавинного фотдиода
        if (!memcmp ((void*)RX_Buf, ";SET:AP ",8)) //
        {
          BYTE Data =(BYTE)atoi((char*)&RX_Buf[8]);
          SetupApdiSet  (Data); // установка наличия измерителя
          sprintf(BufString,"%d\r",Data);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // пишем мертвые зоны    
        if (!memcmp ((void*)RX_Buf, ";SET:MZ",7)) //
        {
          //UserSet.BegShiftZone
          uint8_t Nans = 0;
          // ;set:MZS  - установка мертвых зон
          if (RX_Buf[7] == 'S') // запуск измерения мертвых зон
          {
            StartSettingBegShift (); // старт измерения мертвых зон
            Nans = 1;
          }
          // ;set:MZy 
          else
          {
            SetBegShiftZone ((BYTE)atoi((char*)&RX_Buf[7]), atoi((char*)&RX_Buf[9]));// запись мертвой зоны по индексу
            //123            SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
            //123            FlashErasePage(CFG_USER); // чистим страницу установок пользователя прибора
            //123            FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
            WriteNeedStruct(0x04);
            Nans = 1;
          }
          if(Nans)
          {
            for (int i = 0; i<NUMSHIFTZONE;++i)
            {
              sprintf(BufString,"%d\n", UserSet.BegShiftZone[i]);//c
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
            }
            sprintf(BufString,"\r");//c
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            NeedTransmit = 1;
          }
        }
        // ;set:db*  dbt & dbs
        if (!memcmp ((void*)RX_Buf, ";SET:DBS ",9)) //
        {
          int Data = atoi((char*)&RX_Buf[9]);
          sprintf(BufString,"Err param\r"); // 
          
          // ;set:DBS  - установка признака разрешения альтернативного имени
          if (Data <= 1 ) // запуск 
          {
            NameDB.Ena_DB = Data;  
            //123            SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
            //123            FlashErasePage(DBNAMESTRUCT); // чистим страницу установок alternate name прибора
            //123            FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
            WriteNeedStruct(0x10);
            sprintf(BufString,"OK %d\r",Data ); // 
            
          }
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает ответ на команду
          NeedTransmit = 1;
        }
        // ;set:DBT text  - изменение alernate name device
        if (!memcmp ((void*)RX_Buf, ";SET:DBT ",9)) //
        {
          int i;
          char c;
          for (i=0; i<ARRAY_SIZE(NameDB.AltName)-1; ++i)
          {
            c =(char)RX_Buf[9+i];
            if (c >= 0x20) NameDB.AltName[i] = c;
            else break;
          }
          NameDB.AltName[i] = 0;
          //123          SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
          //123          FlashErasePage(DBNAMESTRUCT); // чистим страницу установок alternate name прибора
          //123          FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
          WriteNeedStruct(0x10);
          UARTSendExt ((BYTE*)NameDB.AltName, strlen ((void*)NameDB.AltName));
          sprintf(BufString,"\r");//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        if (!memcmp ((void*)RX_Buf, ";SET:DB?",8)) //проверка конфигурации альтернативного имени
        {
          sprintf(BufString,"%d ",NameDB.Ena_DB);//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NameDB.AltName[ARRAY_SIZE(NameDB.AltName)-1] = 0; // на всякий случай!
          UARTSendExt ((BYTE*)NameDB.AltName, strlen((void*)NameDB.AltName));
          sprintf(BufString,"\r");//c
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        //        if (!memcmp ((void*)RX_Buf, ";SET:DB0 ",9)) //устанавливаем поправочный коэфф для длины волны (пока 850) на простых фотодиодах (носатые)
        //        {
        //          float Data = atof((char*)&RX_Buf[9]);
        //          sprintf(BufString,"Err param\r"); // 
        //
        //          // ;set:DB0  - 
        //          if ((Data < 0.02)&&(Data > -0.02)) // установим коэфф 
        //          {
        //            NameDB.ph_A[0] = Data;  
        //            SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
        //            FlashErasePage(DBNAMESTRUCT); // чистим страницу установок alternate name прибора
        //            FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
        //            sprintf(BufString,"OK %f\r",Data ); // 
        //            
        //          }
        //          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает ответ на команду
        //        }
        
        
        // ;set:ubat  -установка калибровка батареи
        //        if (!memcmp ((void*)RX_Buf, ";SET:UBAT",9)) //
        //        {
        //          int i =0;
        //          float Data=0.0;
        //          if (RX_Buf[9] == 'S') // установка принудительно коэффициента баттареи
        //          {
        //            float Data = (atof((char*)&RX_Buf[11])); // получение значения оэффициента баттареи
        //            if (Data == 0.0)           Data = GetSetBatStep (0);
        //            else
        //              Data = GetSetBatStep (Data); 
        //            sprintf(BufString,"%f %0.2f\r",Data, Data*1024); // Читает данные из АЦП измерителя/
        //            UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает текущий диапазон работы измерительного усилителя
        //          }
        //          else
        //          {
        //            if((FIO2PIN & EXT_POW)==0) // внешний источник подключен
        //            {
        //              for (i=0;i<10;i++)
        //              {
        //                Data +=(float)ADC0Read(BAT_ADC);
        //              }
        //              Data = Data/10.0;
        //              Data = (4.54/Data)/3; // якобы привнешнем питании на контроль попадает 4.54В питание 5В - Udiod(0.46)???
        //              Data = GetSetBatStep (Data);
        //              sprintf(BufString,"%f %0.2f\r",Data, Data*1024); // Читает данные из АЦП измерителя/
        //              UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает значение шага и уровень опоры
        //            }
        //            else
        //            {
        //              sprintf(BufString,"Not Set ExtPower\r"); 
        //              UARTSendExt ((BYTE*)BufString, strlen (BufString));
        //            }
        //          }
        //        }
        // ;set:tstscr  - проверка работы индикатора
        if (!memcmp ((void*)RX_Buf, ";SET:TSTSCR",11)) //
        {
          sprintf(BufString,"Test Begin\r"); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          //TestScreen (); // выполнение теста - зарисовка экрана попиксельно, черным затем белым
          sprintf(BufString,"Test End\r"); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает текущий диапазон работы измерительного усилителя
          NeedTransmit = 1;
        }
        // ;set:enevents  - установка признака разрешения выдачи событий
        if (!memcmp ((void*)RX_Buf, ";SET:ENEVENTS ",14)) //
        {
          unsigned Data = 0;
          unsigned short CurrSet = GetSetEnaEvents(0); // текущая установка признака
          if (RX_Buf[14] == '1') Data = 1;
          if (((CurrSet == 1) && (Data==0)) || ((CurrSet == 0) && (Data==1)))
            GetSetEnaEvents(1) ; //надо поменять состояние 
          if ( GetSetEnaEvents(0))
          {
            sprintf(BufString,"Events Enable\r"); // 
          }
          else
          {
            sprintf(BufString,"Events Disable\r"); // 
          }
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          NeedTransmit = 1;
        }
        // ;syst:set:blcr - проверка и установка параметров Bellcore
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:BLCR",14)) //
        {
          if (RX_Buf[14] == '?') // запрос структуры устанвоки параметров белкора
          {
            SendBelcoreSet (); // посылает установки белкора
          NeedTransmit = 1;
          }
          if (RX_Buf[14] == ':') //   установка  структуры параметров белкора
          {
            if (!memcmp ((void*)&RX_Buf[15], "BC ",3)) //
            {
              ReflParam.BC = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "FPO ",4)) //
            {
              ReflParam.FPO = ((unsigned)atoi((char*)&RX_Buf[18])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "NF ",3)) //
            {
              ReflParam.NF = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "NFSF ",5)) //
            {
              ReflParam.NFSF = ((unsigned short)atoi((char*)&RX_Buf[19])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "PO ",3)) //
            {
              ReflParam.PO = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "LT ",3)) //
            {
              ReflParam.LT = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "RT ",3)) //
            {
              ReflParam.RT = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            if (!memcmp ((void*)&RX_Buf[15], "ET ",3)) //
            {
              ReflParam.ET = ((unsigned short)atoi((char*)&RX_Buf[17])); // 
            }
            CheckReflParam ();  // Проверка пользовательских настроек 
            //123            FlashErasePage(EVEN_SET); // чистим страницу установок пользователя прибора
            //123            FlashWritePageSM(EVEN_SET, StructPtr(EVEN_SET), StructSize(EVEN_SET), 0);
            WriteNeedStruct(0x08);
            SendBelcoreSet (); // посылает установки белкора
            NeedTransmit = 1;
          }
          
        }
        
        // ;syst:set:manufactured  - установка производителя
        if (!memcmp ((void*)RX_Buf, ";SYST:SET:MANUFACTURED ",23)) //
        {
          SetID_Dev((BYTE)atoi((char*)&RX_Buf[23])); // установка идентификатора для кого прибор
          CurrLang = GetLang(CURRENT);
          GetDeviceName( BufString ); // запрос сторки идентификатора
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
        }
        WriteNeedStruct (0x01); // сохраняем изменения
        // ;set:L1  - добавка к смещению 
        
        // ;set:L2  - порог шумов при определении длины
        // установка и сброс признака программирования индикатора NExtion
        //        if (!memcmp ((void*)RX_Buf, ";SET:FWLCD?",11)) //
        //        {
        //          // замутим запрос в индикатор и подождем ответ
        //          RX_BufNEX[0] = 0;
        //          g_WtRdyNEX = 0;
        //          ReadyNEX = 0;
        //          ClearRS2();
        //          GetRstTMUart2(1); // сбросим таймер Uart2
        //          sprintf(Str, "connectяяя"); 
        //          UARTSend2((BYTE*)Str, strlen(Str));    // 1 строка комментарии
        //  
        //  //UARTSend2 ((BYTE*)CmdBuf, strlen (CmdBuf));//
        //          while(!(ReadyNEX==5));
        //          CreatDelay (100000); // 33 мС - пока без ответа (подтверждения)
        //  // надо ждать получения ответа
        //          UARTSendExt ((BYTE*)RX_BufNEX, 69);//
        //        }
        if (!memcmp ((void*)RX_Buf, ";SET:FWLCDON",12)) //
        {
          SetMode(UploadFW_Nextion);
          CmdInitPage(26); // переключаемся на режим индикации сообщения что в режиме программирования
          //ProgFW_LCD = 1; // перенесем в First обработку что бы "правильно" работало
          //123          //UART2Count = 0;
          //123          //UART0Count = 0;
          sprintf(BufString,"OK\r"); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));//
          NeedTransmit = 1;
        }
        if (!memcmp ((void*)RX_Buf, ";SET:FWLCDOFF",13)) //
        {
          sprintf(BufString,"OK\r"); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));//
          ProgFW_LCD = 2;
          NeedTransmit = 1;
        }
        
      }
      //=========== КОМАНДЫ С ИЗМЕРИТЕЛЕМ =================
      if (GetCfgPM ()  && (GetCurrentModeDevice()==MODETEST))   
      {
        // ;SRCT:WORK x
        if (!memcmp ((void*)RX_Buf, ";SRCT:WORK ",11)) //
        {
          BYTE Mode = ((BYTE)atoi((char*)&RX_Buf[11])); // установка режима работы измерителя
          SetSwitchPMMode(Mode);          // Устанавливает режим переключения 
          // коэффициентов усиления измерителя мощности (ручной или автоматический)
          sprintf(BufString,"%d",Mode); // 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));//
          NeedTransmit = 1;
        }
        // ;SRCT:ADC?
        if (!memcmp ((void*)RX_Buf, ";SRCT:ADC?",10)) //
        {
          sprintf(BufString,"%d %d\r",GetPMData(),GetRange()); // Читает данные из АЦП измерителя/
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает текущий диапазон работы измерительного усилителя
          NeedTransmit = 1;
          // запрос  режима работы измерителя
        }
        
        // ;set:DB0  - 
        
        if (!memcmp ((void*)RX_Buf, ";SET:DB",7)) //устанавливаем поправочный коэфф для длины волны (пока 850) на простых фотодиодах (носатые)
        {
          float Data;
          switch(RX_Buf[7])
          {
          case '0':
            Data = atof((char*)&RX_Buf[9]);
            break;
          case 'U':
            Data = NameDB.ph_A[0] + 0.0001;  
            break;
          case 'D':
            Data = NameDB.ph_A[0] - 0.0001;  
            break;
          default:
            Data = NameDB.ph_A[0];  
          }
          sprintf(BufString,"Err param\r"); // 
          if ((Data < 0.03)&&(Data > -0.03)) // установим коэфф 
          {
            NameDB.ph_A[0] = Data;  
            //123            SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
            //123            FlashErasePage(DBNAMESTRUCT); // чистим страницу установок alternate name прибора
            //123            FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x10);
            sprintf(BufString,"OK %f\r",Data ); // 
            

          }
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает ответ на команду
          NeedTransmit = 1;
        }
        // КОМАНДЫ ДЛЯ РУЧНОГО РЕЖИМА ИЗМЕРИТЕЛЯ
        if (1)
        {
          // ;SRCT:RNG x
          if (!memcmp ((void*)RX_Buf, ";SRCT:RNG ",10)) //
          {
            // установка диапазона измерения
            BYTE Mode = ((BYTE)atoi((char*)&RX_Buf[10])); // установка режима работы измерителя
            if (Mode > 3) Mode = 3; // самый грубый
            SetRange(Mode);    // Установка ключей согласно выбранного диапазона (для ручного режима)
            sprintf(BufString,"%d",Mode); // Запуск измерния
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает текущий диапазон работы измерительного усилителя
            NeedTransmit = 1;
          }
          // ;SRCT:ZERO
          if (!memcmp ((void*)RX_Buf, ";SRCT:ZERO",10)) //
          {
            sprintf(BufString,"Go "); // Запуск измерния
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает текущий диапазон работы измерительного усилителя
            // запуск режима измерения смещений на диапазонах
            AcquireShZeroRng();                 // Измерение уровней смещения
            // смещение диапазонов
            for (int x=0; x<4; ++x)
            {
              sprintf(BufString,"%d ",CoeffPM.ShZeroRng[x]); // начальные смещения диапазонов
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
            }

            // сохраняем полученные коэфф. 
            //123            SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
            
            //123            FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
            //123            FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x02);
            sprintf(BufString,"End\r"); // завершение измерения
            UARTSendExt ((BYTE*)BufString, strlen (BufString));//
            NeedTransmit = 1;
          }
          // ;SRCT:STYK x
          if (!memcmp ((void*)RX_Buf, ";SRCT:STYK ",11)) //
          {
            float PrevRng=0;                         // Значение кодов АЦП на предыдущем диапазоне
            float CurrRng=0;                         // Значение кодов АЦП на текущем диапазоне
            
            BYTE Mode = ((BYTE)atoi((char*)&RX_Buf[11])); // получение номера диапазона
            // запуск режима измерения стыковочных коэффициентов
            AcquireCoefStykRange(Mode+1, &PrevRng, &CurrRng); // Вычисляет стыковычный коэффициент текущего диапазона с предыдущим
            
            sprintf(BufString,"LO=%.0f HI=%.0f ",PrevRng,CurrRng); //  диапазонов
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
            sprintf(BufString,"K_St_Range[%d]=%f ",Mode,CoeffPM.CoefStykRange[Mode+1]); //  диапазонов
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            // сохраняем полученные коэфф. 
            //123            SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
            
            //123            FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
            //123            FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x02);
            sprintf(BufString,"End\r"); // завершение измерения
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
            NeedTransmit = 1;
          }
          // ;SRCT:KKx
          if (!memcmp ((void*)RX_Buf, ";SRCT:KK",8)) //
          {
            BYTE Mode = ((BYTE)atoi((char*)&RX_Buf[8])); // получение индекса длины волны калибровки
            float Data = (atof((char*)&RX_Buf[9])); // получение значения мощности
            if (Mode>1) Mode=3; // исключена точка привязки на 1490 , остались 850,1310, ,1550
            CoeffPM.CoefPointKlb[Mode] =1.0;
            // запуск режима измерения коэффициентов привязки длинн волн калибровки
            Data = GetCoefSpctrKlb(Mode, Data);     // Возвращает спектральный коэффициент для калибровочных длин волн
            //123            SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
            
            //123            FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
            //123            FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x02);
            sprintf(BufString,"%f\r",  Data);     // // завершение измерения Возвращает спектральный коэффициент для текущей длины волны
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
            NeedTransmit = 1;
          }
          // ;SRCT:KSx
          if (!memcmp ((void*)RX_Buf, ";SRCT:KS",8)) //
          {
            BYTE Mode = ((BYTE)atoi((char*)&RX_Buf[9])); // получение индекса длины волны калибровки
            float Data = (atof((char*)&RX_Buf[11])); // получение значения мощности
            
            WORD Lambda;
            switch (RX_Buf[8])
            {
            case 'L':
              // запуск режима измерения коэффициентов спектралки 800-900
              if (Mode>5) Mode = 0;
              Lambda = 800 + 20*Mode;
              CoeffPM.CoefPointKlb[0] =1.0;
              break;
            case 'M':
              // запуск режима измерения коэффициентов спектралки 1200-1390
              if (Mode>9) Mode = 0;
              Lambda = 1210 + 20*Mode;
              CoeffPM.CoefPointKlb[1] =1.0;
              break;
            case 'H':
              // запуск режима измерения коэффициентов спектралки 1410-1650
              if (Mode>12) Mode = 0;
              Lambda = 1410 + 20*Mode;
              CoeffPM.CoefPointKlb[2] =1.0;
              CoeffPM.CoefPointKlb[3] =1.0;
              CoeffPM.CoefPointKlb[4] =1.0;
              break;
              
            }
            Data = GetCoefSpctr( Lambda, Data);
            switch (RX_Buf[8])
            {
            case 'L':
              // запись коэффициента спектралки 800-900
              CoeffPM.CoefSpctrL[Mode] = Data;  
              break;
            case 'M':
              // запись коэффициента спектралки 1210-1390
              CoeffPM.CoefSpctrM[Mode] = Data;  
              break;
            case 'H':
              // запись коэффициента спектралки 1410-1650
              CoeffPM.CoefSpctrH[Mode] = Data;  
              break;
            }
            //123            SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
            //123            FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
            //123            FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x02);
            sprintf(BufString,"%f\r",  Data);     // // завершение измерения Возвращает спектральный коэффициент для текущей длины волны
            UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
            NeedTransmit = 1;
          }
          
        } // от комманд ручного режима
        // команды для измерителя 
        // ;SRCT:ALL?
        if (!memcmp ((void*)RX_Buf, ";SRCT:ALL?",10)) //
        {
          // смещение диапазонов
          for (int x=0; x<4; ++x)
          {
            sprintf(BufString,"%d\n",CoeffPM.ShZeroRng[x]); // начальные смещения диапазонов
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          }  
          for (int x=0; x<4; ++x)
          {
            sprintf(BufString,"%f\n",CoeffPM.CoefStykRange[x]); // коэфф. стыковки диапазонов
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          }  
          for (int x=0; x<WAVE_LENGTHS_NUM; ++x)
          {
            sprintf(BufString,"%f\n",CoeffPM.CoefPointKlb[x]); // коэффициенты привязки длин волн калибровки={}
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          }  
          for (int x=0; x<6; ++x)
          {
            sprintf(BufString,"%f\n",CoeffPM.CoefSpctrL[x]); // коэффициенты спектральной харр. 800-900={}
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          }  
          for (int x=0; x<10; ++x)
          {
            sprintf(BufString,"%f\n",CoeffPM.CoefSpctrM[x]); // коэффициенты спектральной харр. 1210-1390={}
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          }  
          
          for (int x=0; x<13; ++x)
          {
            sprintf(BufString,"%f\n",CoeffPM.CoefSpctrH[x]); // коэффициенты спектральной харр. 1410-1650={}
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            
          } 
          sprintf(BufString,"\r");    
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // -=-=-=-=-=- КОМАНДЫ УСТАНОВКИ частот совместимости JDSU В РУЧНУЮ
        // ;SLWV:
        if (!memcmp ((void*)RX_Buf, ";SLWV:",6)) //
        {
          
          // ;SLWV:FRQx
          if (!memcmp ((void*)&RX_Buf[6], "FRQ",3)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[9])); // получение индекса частоты длины волны
            int Data = (atoi((char*)&RX_Buf[10])); // получение значения частоты 
            if (Indx < 6)
            {
              SetJDSU.FreqLambda[Indx]= Data;
              myBeep(5);
              //123              SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
              //123              FlashErasePage(JDSUSTRUCT); // чистим страницу хранящую параметры ОВЫГ совместимости
              //123              FlashWritePageSM(JDSUSTRUCT, StructPtr(JDSUSTRUCT), StructSize(JDSUSTRUCT), 0);
              //123              SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
              WriteNeedStruct(0x20);
              sprintf(BufString,"Ok\r");    
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            
          }
        }
        
        // ;SLWV:ALL?
        if (!memcmp ((void*)RX_Buf, ";SLWV:ALL?",10)) //
        {
          for (int x=0; x<6; ++x)
          {
            sprintf(BufString,"%d %d\n",SetJDSU.LambdaNum[x],SetJDSU.FreqLambda[x]); // коэффициенты спектральной харр. 1410-1650={}
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
          } 
          sprintf(BufString,"\r");    
          UARTSendExt ((BYTE*)BufString, strlen (BufString));
          NeedTransmit = 1;
        }
        // -=-=-=-=-=- КОМАНДЫ УСТАНОВКИ КОЭФФИЦИЕНТОВ В РУЧНУЮ
        // ;SPCT:
        if (!memcmp ((void*)RX_Buf, ";SPCT:",6)) //
        {
          BYTE NeedSave = 0;
          // ;SPCT:SMRx
          if (!memcmp ((void*)&RX_Buf[6], "SMR",3)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[9])); // получение индекса коэфф смещения
            int Data = (atoi((char*)&RX_Buf[10])); // получение значения смещения
            if (Indx < 4)
            {
              CoeffPM.ShZeroRng[Indx]= Data;
              NeedSave = 1;
            }
          }
          // ;SPCT:STRx
          if (!memcmp ((void*)&RX_Buf[6], "STR",3)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[9])); // получение индекса коэфф стыковки
            float Data = (atof((char*)&RX_Buf[10])); // получение значения стыковки диапазонов
            if (Indx < 4)
            {
              CoeffPM.CoefStykRange[Indx]= Data;
              NeedSave = 1;
            }
          }
          // ;SPCT:KPx
          if (!memcmp ((void*)&RX_Buf[6], "KP",2)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[8])); // получение индекса коэфф дл.волны
            DWORD Data = ((DWORD)atoi((char*)&RX_Buf[9])); // получение значения длины волны калибровки
            if (Indx < 5)
            {
              CoeffPM.PointKalib[Indx]= Data;
              NeedSave = 1;
            }
          }
          // ;SPCT:KKx
          if (!memcmp ((void*)&RX_Buf[6], "KK",2)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[8])); // получение индекса коэфф дл.волны
            float Data = (atof((char*)&RX_Buf[9])); // получение значения длины волны калибровки
            if (Indx < 5)
            {
              CoeffPM.CoefPointKlb[Indx] =Data;
              NeedSave = 1;
            }
          }
          // ;SPCT:KSx y.yyyy - запись спектральных коэффициентов
          if (!memcmp ((void*)&RX_Buf[6], "KS",2)) //
          {
            BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[9])); // получение индекса коэфф 
            float Data = (atof((char*)&RX_Buf[11])); // получение значения спектрального коэффициента
            switch (RX_Buf[8])
            {
            case 'L':
              if (Indx < 6)
              {
                CoeffPM.CoefSpctrL[Indx]= Data;
                NeedSave = 1;
              }
              break;
            case 'M':
              if (Indx < 10)
              {
                CoeffPM.CoefSpctrM[Indx]= Data;
                NeedSave = 1;
              }
              break;
            case 'H':
              if (Indx < 13)
              {
                CoeffPM.CoefSpctrH[Indx]= Data;
                NeedSave = 1;
              }
              break;
            }
          }
          if (NeedSave) // надо сохранить изменения
          {
            myBeep(5);
            //123            SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
            //123            FlashErasePage(COEF_PM); // чистим страницу хранящую спектральные коэфф. прибора
            //123            FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
            //123            SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
            WriteNeedStruct(0x02);
            sprintf(BufString,"Ok\r");    
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            NeedTransmit = 1;
          }
        }
        //                if (!memcmp ((void*)RX_Buf, ";SPCT:DB0 ",10)) //устанавливаем поправочный коэфф для длины волны (пока 850) на простых фотодиодах (носатые)
        //        {
        //          float Data = atof((char*)&RX_Buf[10]);
        //          sprintf(BufString,"Err param\r"); // 
        //
        //          // ;set:DBS  - установка признака разрешения альтернативного имени
        //          if ((Data < 0.02)&&(Data > -0.02)) // установим коэфф 
        //          {
        //            NameDB.ph_A[0] = Data;  
        //            SSPInit_Any(MEM_FL1); // Востанавливаем Инициализацию SSP для управления внешней FLASH (порт 1 та что на плате отладочной)
        //            FlashErasePage(DBNAMESTRUCT); // чистим страницу установок alternate name прибора
        //            FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
        //            sprintf(BufString,"OK %f\r",Data ); // 
        //            
        //          }
        //          UARTSendExt ((BYTE*)BufString, strlen (BufString));// Возвращает ответ на команду
        //        }
        // установка коэфф вручную.
        // ;FOT:
        // FOT:MEM:LD? XX
        
        if (!memcmp ((void*)&RX_Buf, ";FOT:",5)) //
        {
          
          if (!memcmp ((void*)&RX_Buf[5], "PM:",3)) // управвление измерителем
          {
            char Str[15];
            if (!memcmp ((void*)&RX_Buf[8], "LW ",3)) //установка длины волны
            {
              unsigned int NumLW = (unsigned int)atoi((char*)&RX_Buf[11]);// 
              if (((NumLW<800)||(NumLW>1650))||((NumLW>900)&&(NumLW<1210))) NumLW=1310;
              sprintf(BufString,"%4d nm\r",SetPMWavelenght(NumLW));// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
              
            }
            if (!memcmp ((void*)&RX_Buf[8], "MD ",3)) //установка режима работы измерителя (dB,dBm, mW)
            {
              BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[11])); // получение индекса режима работы измерителя (dB,dBm, mW)
              if (Indx>2) Indx = 1;
              SetTypeRslt(Indx); 
              SetStringPM(Str,GetLastPower ());           // устанавливает строку данных измерения
              sprintf(BufString,"%s\r",Str);// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[8], "REFF",4)) //привязка на текущей длине волны
            {
              float LvldBm = 10*log10(GetLastPower ());
              SetCurrLvldB(GetPMWavelenght(0),LvldBm); // здесь записываем в память
              SetTypeRslt(1); // set dB
              sprintf(BufString,"%.2f dBm\r",GetCurrLvldB(0));// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[8], "REF ",4)) //принудительная установка привязки
            {
              float LvldBm = atof((char*)&RX_Buf[12]);
              if ((LvldBm<-90.0)||(LvldBm>+20.0)) LvldBm = 0.0;
              SetCurrLvldB(GetPMWavelenght(0),LvldBm); // здесь записываем в память
              SetTypeRslt(1); // set dB
              SetStringPM(Str,GetLastPower ());           // устанавливает строку данных измерения
              sprintf(BufString,"%s\r",Str);// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[8], "CUR",3)) //запрос измерителя в формате длина волны и мощность
            {
              //  float LvlmW = GetLastPower ();
              sprintf(BufString,"%4d, %.6e\r",GetPMWavelenght(0),GetLastPower()*0.001);// передаем длину волны и мощность в ватах
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            
          }
          if (!memcmp ((void*)&RX_Buf[5], "PM?",3)) //состояние измерителя измерителем
          {
            char Str[35];
            SetStringPM(Str,GetLastPower ());           // устанавливает строку данных измерения
            sprintf(BufString,"%4d nm, %s, %.2f dBm\r",GetPMWavelenght(0),Str,GetCurrLvldB(0));// передаем слово о начале измерения
            UARTSendExt ((BYTE*)BufString, strlen (BufString));
            NeedTransmit = 1;
          }
        }
      } // команды измерителя // только в режиме измерителя
      // сюда необходимо вынести команды работы с памятью
      // ;FOT:
      // FOT:MEM:LD? XX 
      if (GetCfgPM ()  && ((GetCurrentModeDevice()==MODETEST)||(GetCurrentModeDevice()==MODETESTMEM))   )// работаем когда есть измеритель, в режиме памяти и в режиме измерителя
      {
        if (!memcmp ((void*)&RX_Buf, ";FOT:",5)) //
        {
          // работа с памятью
          if (!memcmp ((void*)&RX_Buf[5], "MEM:",4)) //
          { 
            if (!memcmp ((void*)&RX_Buf[9], "WRN",3)) //
            { // запись следующей, только если установлен режим измерителя
              //if (GetStateADC() == FREEADC)
              //              WORD WatchDog = 0;
              //              while (GetStateADC() == BUSYADC)
              //              {
              //                WatchDog++;
              //                if (WatchDog>20000)
              //                {
              //                  SSP_FlashTransmit(1, 0);
              //                  SSP_FlashTransmit(1, 0);
              //                  SSP_FlashTransmit(1, 0);  
              //                  CreatDelay (10); // задержка 0,8 мкс чтобы сосчитать все
              //                  SetStateADC (FREEADC); // установка режима АЦП
              //                  PM_CS(1);
              //                  break;
              //                }
              //              }
              {
                //REDEYE(1);
                //NeedSaveOLT = 1; // установка признака необходимости сохранить данные
                //123                SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
                ReLoadCommOLT (); // перезагружаем комментарии для измерителя
                SavePowerMeter(GetLastPower()); // Получаем мощность в мВт)функция сохранения в памяти ИЗмерений
                WriteMemPow(); // запись в память непосредственно
                //123                SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
                sprintf(BufString,"%d,%d\r",GetCellMem(0),MaxMemPM); // сколько занято ячеек
                UARTSendExt ((BYTE*)BufString, strlen (BufString));
                NeedTransmit = 1;
                //REDEYE(0);
              }
              //else
              //{
              //sprintf(BufString,"BUSY ADC\r"); // занято АЦП
              //UARTSendExt ((BYTE*)BufString, strlen (BufString));
              //}
            }
            if (!memcmp ((void*)&RX_Buf[9], "CLR",3)) //
            { // удаляем ячейки
              
              //              P08_IRQ_EN(OFF); // запрещаем прерывание по готовности АЦП
              //              PM_CS(1); // рвем сбор данных если был
              DeletedAllCell (); // удаление всех записей измерителя   
              SetModeDevice(MODETESTMEM);
              sprintf(BufString,"%d,%d\r",GetCellMem(0),MaxMemPM); // сколько занято ячеек
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[9], "FR",2)) //
            { // читаем сколько ячеек занято
              sprintf(BufString,"%d,%d\r",GetCellMem(0),MaxMemPM); // сколько занято ячеек
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[9], "LD? ",4)) //
            { // читаем ячейку памяти
              unsigned int Num_f = (unsigned int)atoi((char*)&RX_Buf[13]);
              if ((Num_f <= GetCellMem(0))&&(Num_f>0))
              {
                Num_f= Num_f-1;
                //                P08_IRQ_EN(OFF); // запрещаем прерывание по готовности АЦП
                //                PM_CS(1); // рвем сбор данных если был
                //                SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
                SetNumCellIzm (Num_f);
                //123!!!                ReadCellIzm(Num_f,(unsigned char*)&PONI);//  читаем из памяти(flash) в PONI ячейку сохранения измерителя
                Sec2Date (PONI.TotalTimeCell, &TimeSaveOLT);
                SetModeDevice(MODETESTMEM);
                //123                SSPInit_Any(SPI_PM); // востановление SSP для управления PM (порт 1 та что на плате отладочной)
                // приняли признак ;LD? -  выдаем файл
                //#265
                // ЗАГОЛОВОК
                memcpy( BufString, "#265", 4 );
                memcpy( &BufString[4], &PONI, 64 ); // 
                memcpy( &BufString[68], "\r", 1 );
                UARTSendExt ((BYTE*)BufString, 69);
                NeedTransmit = 1;
              }
            }
          }
        }
      }
      // управление источником и красным глазом 
      if (GetCurrentModeDevice()==MODETEST)   
      {
        if (!memcmp ((void*)&RX_Buf, ";FOT:",5)) //
        {
          if (!memcmp ((void*)&RX_Buf[5], "SR",2)) //
          {
            char Str[15];
            if (!memcmp ((void*)&RX_Buf[7], "?",1)) //состояние источника
            {
              sprintf(BufString,"%4d %d\r",GetLengthWaveLS (GetPlaceLS(CURRENT)), SetModeLS(Str,CURRENT,CurrLang ));// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[7], ":MD ",4)) //
            {
              BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[11])); // получение индекса режима источника
              if (Indx>4) Indx = 0;
              
              while (Indx!=SetModeLS(Str,NEXT,CurrLang ))
                SetModeLS(Str,CURRENT,CurrLang );  
              sprintf(BufString,"%s\r",Str);// 
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
              
            }
            if (!memcmp ((void*)&RX_Buf[7], ":LW ",4)) //
            {
              BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[11])); // получение индекса места
              if (Indx>2) Indx = 0;
              GetPlaceLS(Indx-GetPlaceLS(CURRENT));
              sprintf(BufString,"%4d nm\r",GetLengthWaveLS (GetPlaceLS(CURRENT)));// передаем слово о начале измерения
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            
          }
        }
      }
      if (GetCfgRE()) // если есть красный глаз
      {
        if (!memcmp ((void*)&RX_Buf, ";FOT:",5)) //
        {
          if (!memcmp ((void*)&RX_Buf[5], "VL",2)) //
          {
            char Str[15];
            if (!memcmp ((void*)&RX_Buf[7], "?",1)) //какой режим красного глаза
            {
              sprintf(BufString,"%d\r",SetModeRE(Str,CURRENT,CurrLang ));// 
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
            if (!memcmp ((void*)&RX_Buf[7], ":MD ",4)) //установить режим красного глаза
            {
              BYTE Indx = ((BYTE)atoi((char*)&RX_Buf[11])); // получение индекса режима REDEYE
              if (Indx>2) Indx = 0;
              while (Indx!=SetModeRE(Str,NEXT,CurrLang ))
                SetModeRE(Str,CURRENT,CurrLang );  
              sprintf(BufString,"%s\r",Str);// 
              UARTSendExt ((BYTE*)BufString, strlen (BufString));
              NeedTransmit = 1;
            }
          }
        }
      }
      break; // от ;
    }
      if(NeedTransmit == 0)
      {
        sprintf(BufString,"Err\r");// 
        UARTSendExt ((BYTE*)BufString, strlen (BufString));
        
      }
    
  }
  RSDecYes = 0;
  Reciev = STOP_UART;
  TST_KTB(0); // индикация конца приема и обработки команды

  //VICINTENABLE = 1 << UART0_INT;  /* Enable Interrupt */
  
}



void SendCfgOTDR (char* BufStr) // передача конфигурации рефлектометра (настройки)
{
  
                    // ответ конфигурации
        sprintf(BufStr,"CFG,%dkm,%dns,%ds,%dnm(%d),%.4f\r" // 
                ,GetLengthLine(GetIndexLN())
                ,GetWidthPulse(GetIndexIM())
                ,GetTimeAvrg(GetIndexVRM())
                ,GetLengthWaveLS (GetPlaceLS(CURRENT))
                ,GetPlaceLS(CURRENT)
                ,GetIndexWAV ());
        UARTSendExt ((BYTE*)BufStr, strlen (BufStr));
}


void SendBelcoreSet (void) // посылает установки белкора
{
  char BufString[25];
          sprintf(BufString,"BC=%d\n",ReflParam.BC); // BC = 800 предел обратного отражения
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"FPO=%d\n",ReflParam.FPO); // FPO смещение физическое (отступ сбора данных) равен 0
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"NF=%d\n",ReflParam.NF); // NF нижний уровень шумов равен 65535
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"NFSF=%d\n",ReflParam.NFSF); //// NFSF масштабный коэфф  уровень шумов равен 1000
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"PO=%d\n",ReflParam.PO); //// PO смещение по мощности равен 0 в 0.001 дБ 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"LT=%d\n",ReflParam.LT); //// LT минимальное значение затухания для события 200 в 0.001 дБ 
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"RT=%d\n",ReflParam.RT); //// RT минимальное значение отражения для события 40000 в -0.001дБ
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"ET=%d\n",ReflParam.ET); //// ET минимальное затухание для конца линии 3000 в 0.001 дБ
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 
          sprintf(BufString,"EVNT=%d\n\r",GetSetEnaEvents(0)); //// Вывод информации о включенном разрешении событий
          UARTSendExt ((BYTE*)BufString, strlen (BufString));// 

}

// тестотвое заполнение таблицы событий
   void  TestLoadEvents (int Num)
   {
     for (int i=0; i<Num; ++i)
     {
                   EvenTrace[i].EPT = CalkEPT (i*300+300); // расчет значений EPT для событий от положения курсора
       switch (i)
       {
       case 1:
        EvenTrace[i].EC[0] = '1';//2
        EvenTrace[i].EC[1] = 'F';//E
        break;
       case 2:
        EvenTrace[i].EC[0] = '2';//2
        EvenTrace[i].EC[1] = 'F';//E
        break;
       case 3:
        EvenTrace[i].EC[0] = '0';//2
        EvenTrace[i].EC[1] = 'E';//E
        break;
       case 4:
        EvenTrace[i].EC[0] = '1';//2
        EvenTrace[i].EC[1] = 'E';//E
        break;
       case 5:
        EvenTrace[i].EC[0] = '2';//2
        EvenTrace[i].EC[1] = 'E';//E
        break;
       case 6:
        EvenTrace[i].EC[0] = '0';//2
        EvenTrace[i].EC[1] = 'O';//E
        break;
       case 7:
        EvenTrace[i].EC[0] = '1';//2
        EvenTrace[i].EC[1] = 'O';//E
        break;
       case 8:
        EvenTrace[i].EC[0] = '2';//2
        EvenTrace[i].EC[1] = 'O';//E
        break;
       default:
        EvenTrace[i].EC[0] = '0';//2
        EvenTrace[i].EC[1] = 'F';//E
        break;
         
       }

     }
   }

/******************************************************************************
**                            End Of File
******************************************************************************/
