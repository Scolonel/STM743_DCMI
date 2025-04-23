
#include "system.h"


volatile DWORD RecievOpt = STOP_UART;
BYTE RX_BufOpt[BUFSIZEUART1];
volatile DWORD CntRXOpt = 0;
BYTE RSOptYes = 0;


// Прием байта по UART1 OPTICS
void RS_com_Optic (BYTE temp)
{
  switch (RecievOpt)
  {
  case STOP_UART:                    //Состояние готовности к приёму новой команды
    if (temp=='#')
    {
      CntRXOpt=0;
      RX_BufOpt[CntRXOpt]=temp;
      RecievOpt=START_UART;
      CntRXOpt++;
    }
    break;
  case START_UART:                   //Состояние приёма текущей команды (исправлено V <-> W) 02/09/03
    {
      switch (temp)
      {
      case '#':                 //Ежели начало посылки во время приёма предыдущей - то ошибка, начинаем сначала.
        CntRXOpt=0;
        RX_BufOpt[CntRXOpt]=temp;
        CntRXOpt++;
        break;
      case 0xD:                 //Конец команды
        RX_BufOpt[CntRXOpt]=temp;
        RecievOpt=END_UART;
        RX_BufOpt[CntRXOpt+1]=0x0;
        RSOptYes = 1;            //Команда принята полностью, поэтому надо обработать установим признакRSDecode();
        break;
      default:                  //Продолжение приёма команды
        RX_BufOpt[CntRXOpt]=temp;
        CntRXOpt++;
        if (CntRXOpt > BUFSIZEUART1-1)
        {
          CntRXOpt=0;
          RecievOpt=STOP_UART;
        }
        break;
      }
    }
  case END_UART:                     //Состояние принятой полностью команды
    break;                      //Предыдущая команда не отработана - поэтому ничего не делать
  }
 // SCI1TXBUF = temp; // эхо приема для отладки
}

WORD GetNumWaveOpt (void) // возвращает длину волны или ноль если не принял
{
  WORD Temp = 0;
  if ((RecievOpt==END_UART)&&(GetModePowerMeter()==POWAUTO)) // получение текущего режима измерителя
                //Ежели приём команды закончен - обработка
  {
    for (int i=0; i <CntRXOpt; i++)// цикл преобразования регистра
    {
      if ((RX_BufOpt[i] >=0x61) && (RX_BufOpt[i]<=0x7A))RX_BufOpt[i] = RX_BufOpt[i] - 0x20;
    }
    
    if (!memcmp ((void*)RX_BufOpt, "#SL",3)) //
        {
        Temp=atoi((char*)(RX_BufOpt+3));   //Преобразование символьной информации в числовую
        if ((Temp<800)||(Temp>1650))Temp = 0;

        }
  }
  RecievOpt=STOP_UART;
  RSOptYes = 0;
          // test code
    memcpy(wrd ,(void*)RX_BufOpt,9);
    //sprintf(Str, "t13.txt=\"\"яяя");
    wrd[9]=0;

  return Temp;
}
