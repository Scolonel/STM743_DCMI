//===============================
#include "system.h"

// ���������� �������� �2, ���������� � ����� �������
volatile DWORD OldTimeAccum; // ���������� �������� ������� ����������
volatile DWORD CurrTimeAccum; // ������� ��������
uint8_t EnaTimerAccum =0; //������� ���������� ����� ������� ����������
int CntAccumulat=0; // ������� ����������



void RUN_SUM (DWORD* RawDataI);//


void SUMMER (DWORD* RawDataS)//
{
  //char Str[5];
  //static int CntAccumulat=0; // ������� ����������
  // DWORD* pRawData = RawData;
  // WORD* pDataADC = &DataADC;
   
  unsigned int i;
  unsigned int Step;
  //if (CntAccumulat==0)CntAccumulat++;// ������ ��������� �� ����������
  Step = PointsPerPeriod-1-PointInPeriod; // ������� ������ ������ ������ ������� ��� ����� � �������������!!!!!!!!!!! 
  //ADC_DATA_EN(0); // ������������� ����� ��� ������ ���               !!!!!!!!!!!
  //STARTPP(0); // ������������� � "0" START                            !!!!!!!!!!!  
   for (i=Step;i<RAWSIZE;i+=PointsPerPeriod)                                  // !!!!!!!!!!! 
    RawDataS[i] += 1;     // ������������ � ���� ������ ��������������� (���� ���� ��� ���) !!!!!!!!!!!
  //STARTPP(1); // ������������� � "1" START                            !!!!!!!!!!!
  //ADC_DATA_EN(1);// ������� �����                            !!!!!!!!!!!
  PointInPeriod ++;    // ��������� ������������ 
  
}


void Averaging (int NumAccum,unsigned DelayAfterAvrg, BYTE EnDrawGraph )// ������� ���������� � ���������� �������� ����� ����������
// ������������� ���������������: PointsPerPeriod - ����� ����� �� ������, ������� �� ����� ���������
// NumAccumPerSec - ����� ���������� � ������ ������

{ 
  //    CntPointPick=0;

  //CurrTimeAccum = HAL_GetTick();
  OldTimeAccum = CurrTimeAccum;
  SetNumAccumPerSec (NumAccum);
  SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod=0)); // �������� ����� � �������, ������ ������� ������ PointInPeriod
  if (CntAccumulat==0)CntAccumulat++;// ������ ��������� �� ����������
  //SetParamMeasure();
  while (CntAccumulat)
  {
    RUN_SUM(RawData);//34-67-130-260-510-770-1540~ max 34*48=1632 uS 1632/1540=
    CreatDelay (DelayAfterAvrg);
    SendSetALT (CalkSetALT(PointsPerPeriod, PointInPeriod)); // (14 uS)�������� ����� � �������
  }
  if (!RemoutCtrl)
  DrawPictureMeas (EnDrawGraph); // (28 mS) ��������� �������� ��� ���������
}

void DrawPictureMeas (BYTE EnDraw) // ��������� �������� ��� ���������
{ 
  char Str[32];
  Rect rct={0,0,128,56};
  GraphParams params = {GetVerticalSize(GetUserVerSize()),PosCursorMain (0),GetSetHorizontScale(0),0, GetUserTypeCentral ()};//PosCursorMain (0)
  
  if (EnDraw)
  {
    if(g_NeedScr)
    {
      CmdInitPage(17);// ������� ������� ������������ ���� �� DrawOTDR
      
      sprintf(Str,"t0.txt=\"%s\"���",MsgMass[30][CurrLang]); //���� ���������: XX� 
      NEX_Transmit((void*)Str);//
      // ����� ��������� ����� ����� � �������� ���������
      if ((GetIndexLN()==0)&&(IndexSmall==0))
        sprintf(Str,"t1.txt=\"0.5%s\"���",MsgMass[20][CurrLang]);//����� �����: XX��
      else
        sprintf(Str,"t1.txt=\"%d%s\"���",GetLengthLine(GetIndexLN()),MsgMass[20][CurrLang]);//����� �����: XX��
      //sprintf(Str,"t6.txt=\"%s\"���",MsgMass[35][CurrLang]); // 
      NEX_Transmit((void*)Str);//
      // ������ ������ ����� �����
      // ����� ��������� � ��������������� �� ����� �� 3 ���� ����,
      // � ������ ������������� ������ ����� ����� � ����� ������������ ������������
      // ���������� ���� ������ ����� �����
      if(g_AutoSave)
        sprintf(Str,"t3.bco=YELLOW���"); // ���������
      else
        sprintf(Str,"t3.bco=WHITE���"); // �����
      
      NEX_Transmit((void*)Str);//
      //if (GetSetModeLW(0)) 
      //sprintf(Str,"t3.txt=\"%d%s\"���",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[21][CurrLang]); // auto
      //  sprintf(Str,"t3.txt=\"%s\"���",MsgMass[21][CurrLang]); // auto
      //else
      sprintf(Str,"t3.txt=\"%d%s\"���",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[18][CurrLang]); // nm
      NEX_Transmit((void*)Str);//
      
      g_FirstScr = 0;
      g_NeedScr = 0;
    }
    
    //������� �������� �� UART2 ��� ���������� NEXTION
    // ����������(����������) ������ ��� ������ ����������
    if(GetIndexVRM()==4) // �������� �����
      sprintf(Str,"t2.txt=\"%s\"���",MsgMass[26][CurrLang]); //���� ���������:  ��������
    else
      
      sprintf(Str,"t2.txt=\"%d%s\"���",(WORD)(((float)GetCntNumAvrg()/(float)GetFinAvrg())*((float)GetTimeAvrg(GetIndexVRM()))),MsgMass[4][CurrLang]); //���� ���������: XX� 
    NEX_Transmit((void*)Str);// 
    if(TypeLCD)
    {
      rct.right=480;
      rct.bottom=255;// ����� ���� ������� 255, � � ���������� ������� 270/255 1.05
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
    
    MakeGraphNext( &rct, LogData, 4096, &params );// ��� ������������� ������ ��� ������� (��� �����) � ����������� �� ����������
    
    
    SendDrawNex(NexData,2,rct.right);
    HAL_Delay (1);
    
  }
  
}

void RUN_SUM (DWORD* RawDataI)//
{
  //char Str[5];
  //static int CntAccumulat=0; // ������� ����������
  // DWORD* pRawData = RawData;
  // WORD* pDataADC = &DataADC;
  //--U0IER &= ~IER_RBR ;// | IER_THRE | IER_RLS/* Disable UART0 interrupt */
  //  TST_P1 (1); // set LED
  SUMMER(RawDataI); // 
  
  //SUMMER(RawDataI,   PointsPerPeriod-1-PointInPeriod); // ������� ������ ������ ������ ������� ��� ����� � �������������!!!!!!!!!!! 
  //---U0IER |= IER_RBR ;// | IER_THRE | IER_RLS/* Enable UART0 interrupt */
  //  TST_P1 (0); // off LED
  
  //  unsigned int i;
  //  unsigned int Step;
  //  //if (CntAccumulat==0)CntAccumulat++;// ������ ��������� �� ����������
  //  Step = PointsPerPeriod-1-PointInPeriod; // ������� ������ ������ ������ ������� ��� ����� � �������������!!!!!!!!!!! 
  //  ADC_DATA_EN(0); // ������������� ����� ��� ������ ���               !!!!!!!!!!!
  //  STARTPP(0); // ������������� � "0" START                            !!!!!!!!!!!  
  //   for (i=Step;i<RAWSIZE;i+=PointsPerPeriod)                                  // !!!!!!!!!!! 
  //    RawData[i] += DataADC;     // ������������ � ���� ������ ��������������� (���� ���� ��� ���) !!!!!!!!!!!
  //  STARTPP(1); // ������������� � "1" START                            !!!!!!!!!!!
  //  ADC_DATA_EN(1);// ������� �����                            !!!!!!!!!!!
  //  PointInPeriod ++;     // ��������� ������������ 
  if (PointInPeriod > PointsPerPeriod-1) // �������� �� ����� ����� ������������
  {
    PointInPeriod = 0;
    CntAccumulat ++;  // ��������� �������� ����������
    // ��������� ����������� ����������, ����� ��������� ����� ���������� � ������� ������
    //CurrTimeAccum = HAL_GetTick(); // ��������� ����� ������� ��� ����������
  }
  // ��������� ������ � ������� ������ ���, ����� �� �����....!!!
  //SetLvlDAC ((unsigned int)CntAccumulat);
  if ((CurrTimeAccum - OldTimeAccum) > 2500) // ����� 2.5 ���
  {
  }
  //if (CntAccumulat > GetNumAccumPerSec())//��������� �������� ���������� � ������ ������ �� ����� ����������
  if ((CntAccumulat > GetNumAccumPerSec())||((CurrTimeAccum/1000)>GetTimeAvrg(GetIndexVRM())))//��������� �������� ���������� � ������ ������ �� ����� ����������
    
  { // ��������� ���� ���������� ����� �� ����� � ����� � �� �������!?
    OldTimeAccum = CurrTimeAccum;  
    //RawData[RAWSIZE-1]= GetTimer(2);
    DWORD Noise =0;
    DWORD NoiseAdd =0; // ��������� ���� �� ���������� ������ � ����� ����� ��� 64 � 128 ��
    int CntAddNoise= 0; // ������� ����� �������
    DWORD MaxNoise =0;
    int j=GetCurrentBegShiftZone (); //��������� �������� �������� �� �������
    DWORD LocalRaw;
    unsigned Avrgs = (GetCntNumAvrg()+CntAccumulat-1); // ������ ����� ����������, �� ������ ������
    SetCntNumAvrg(Avrgs); // ����������
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
    if (GetIndexLN()>5)// ������� ����� -> ������� ����� �� ������� �����
    {
      for (int i=4090;i<4120;++i) // ����� 30 ����� � ����� ����������� ����
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
    DWORD CurrentMaxLog =(DWORD)(5000.0*log10((double)Avrgs*4095)); // ������������ �������� ������� ����������
    // ������ ���������������� ���� (����� ���������)
    SetLogNoise((unsigned short)(CurrentMaxLog - (DWORD)(5000.0*log10((double)(MaxNoise-Noise))))) ;
    
    // 
    //Noise += Avrgs/400; // ������� ����� �������� � �����
    // ��������� ������� �����
    //SaveNoise (Noise/2);
    for (int i=0; i<OUTSIZE; i++)
    { 
      if (PointsPerPeriod==48)   LocalRaw = (RawData[i+j-1]+RawData[i+j])>>1;// ���� ����� ������ �� ����������� ����� �� 2, ����� ����
      else  LocalRaw = RawData[i+j];
      //LocalRaw = RawData[i+j];
      // ������� ����� �������, �� ���� ����� ������� ���������� � ��������� ��������
      // ����������� 12 ������
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
    SetHeadFileRaw (GetCntNumAvrg()); // ����� ��������� ����� ������ ������������� ������
    
  }
}

 
