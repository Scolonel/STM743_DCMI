
#include "system.h"


volatile DWORD RecievOpt = STOP_UART;
BYTE RX_BufOpt[BUFSIZEUART1];
volatile DWORD CntRXOpt = 0;
BYTE RSOptYes = 0;


// ����� ����� �� UART1 OPTICS
void RS_com_Optic (BYTE temp)
{
  switch (RecievOpt)
  {
  case STOP_UART:                    //��������� ���������� � ����� ����� �������
    if (temp=='#')
    {
      CntRXOpt=0;
      RX_BufOpt[CntRXOpt]=temp;
      RecievOpt=START_UART;
      CntRXOpt++;
    }
    break;
  case START_UART:                   //��������� ����� ������� ������� (���������� V <-> W) 02/09/03
    {
      switch (temp)
      {
      case '#':                 //����� ������ ������� �� ����� ����� ���������� - �� ������, �������� �������.
        CntRXOpt=0;
        RX_BufOpt[CntRXOpt]=temp;
        CntRXOpt++;
        break;
      case 0xD:                 //����� �������
        RX_BufOpt[CntRXOpt]=temp;
        RecievOpt=END_UART;
        RX_BufOpt[CntRXOpt+1]=0x0;
        RSOptYes = 1;            //������� ������� ���������, ������� ���� ���������� ��������� �������RSDecode();
        break;
      default:                  //����������� ����� �������
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
  case END_UART:                     //��������� �������� ��������� �������
    break;                      //���������� ������� �� ���������� - ������� ������ �� ������
  }
 // SCI1TXBUF = temp; // ��� ������ ��� �������
}

WORD GetNumWaveOpt (void) // ���������� ����� ����� ��� ���� ���� �� ������
{
  WORD Temp = 0;
  if ((RecievOpt==END_UART)&&(GetModePowerMeter()==POWAUTO)) // ��������� �������� ������ ����������
                //����� ���� ������� �������� - ���������
  {
    for (int i=0; i <CntRXOpt; i++)// ���� �������������� ��������
    {
      if ((RX_BufOpt[i] >=0x61) && (RX_BufOpt[i]<=0x7A))RX_BufOpt[i] = RX_BufOpt[i] - 0x20;
    }
    
    if (!memcmp ((void*)RX_BufOpt, "#SL",3)) //
        {
        Temp=atoi((char*)(RX_BufOpt+3));   //�������������� ���������� ���������� � ��������
        if ((Temp<800)||(Temp>1650))Temp = 0;

        }
  }
  RecievOpt=STOP_UART;
  RSOptYes = 0;
          // test code
    memcpy(wrd ,(void*)RX_BufOpt,9);
    //sprintf(Str, "t13.txt=\"\"���");
    wrd[9]=0;

  return Temp;
}
