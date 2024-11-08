
// ������� ������ � ������� 
// ��������������� � SSP

#include "system.h"
// ��������� ������������ �������

BYTE MemTable[MaxMemOTDR+1]; // ������� ������������� ������ ������ �������� � ������ �� MaxMemOTDR
BYTE MemTableExt[MaxMemOTDRExt+1]; // ������� ������������� ����������� ������ ������ �������� � ������ �� MaxMemOTDRExt
static unsigned short NumTrace = 0; // ����� ������ (����� �������) � ������ ������������

BYTE CheckMemID (void) // ��������� ����� ������ �����
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
// // ������ � ������������� ��������������� ����� �������
//  FlashReadPageNum (DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT),0);
//  unsigned ErrPramDBNAME = InvalidDBNAME();
//  if (ErrPramDBNAME)
//  {
//    //SPCTR_err=0x80;
//    InitDBNAME(ErrPramDBNAME); 
//    FlashErasePage(DBNAMESTRUCT); // ������ �������� �������� ������������ alternate name �������
//    FlashWritePageSM(DBNAMESTRUCT, StructPtr(DBNAMESTRUCT), StructSize(DBNAMESTRUCT), 0);
//  }
// // ������ � ������������� ������������ ������������� JDSU
//  FlashReadPageNum (JDSUSTRUCT, StructPtr(JDSUSTRUCT), StructSize(JDSUSTRUCT),0);
//  unsigned ErrPramJDSU = InvalidJDSU();
//  if (ErrPramJDSU)
//  {
//    //SPCTR_err=0x80;
//    InitJDSU(ErrPramJDSU); 
//    FlashErasePage(JDSUSTRUCT); // ������ �������� �������� ������������ �������
//    FlashWritePageSM(JDSUSTRUCT, StructPtr(JDSUSTRUCT), StructSize(JDSUSTRUCT), 0);
//  }
//  
//  // ������� �������� ������������ ������� (��������)
//  FlashReadPageNum (CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE),0);
//  unsigned ErrDev = InvalidDevice();
//  if (ErrDev)
//  {
//    SPCTR_err=0x80;
//    InitDevice(ErrDev); 
//    FlashErasePage(CFG_DEVICE); // ������ �������� �������� ������������ �������
//    FlashWritePageSM(CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE), 0);
//  }
//  // �������� ������������ �������������
//  FlashReadPageNum (COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
//  SPCTR_err = SPCTR_err + FindErrCoeff ();
//  FixErrCoeff (SPCTR_err); //  �������� ������� ������������� � ����������� ������� �������������
//  if (SPCTR_err)
//  {
//  FlashErasePage(COEF_PM); // ������ �������� �������� ������������ �����. �������
//  FlashWritePageSM(COEF_PM, StructPtr(COEF_PM), StructSize(COEF_PM), 0);
//  }
//  FlashReadPageNum (CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER),0);
//  SPCTR_err = SPCTR_err + CheckUserGonfig ();  // �������� ���������������� �������� 
//  if (SPCTR_err>=0x100)
//  {
//  FlashErasePage(CFG_USER); // ������ �������� ��������� ������������ �������
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
//  }
//  // �������� ����� ��������� �������� ��������������
//  FlashReadPageNum (EVEN_SET, StructPtr(EVEN_SET), StructSize(EVEN_SET),0);
//  DWORD Param_err =  CheckReflParam ();  // �������� ���������������� �������� 
//  if (Param_err>0)
//  {
//  FlashErasePage(EVEN_SET); // ������ �������� ��������� ������������ �������
//  FlashWritePageSM(EVEN_SET, StructPtr(EVEN_SET), StructSize(EVEN_SET), 0);
//  }
//  
  return SPCTR_err;
}

void SaveConfigDevice (void)
{
//    FlashErasePage(CFG_DEVICE); // ������ �������� �������� ������������ �������
//    FlashWritePageSM(CFG_DEVICE, StructPtr(CFG_DEVICE), StructSize(CFG_DEVICE), 0);
}

void SaveUserConfig (void) // ���������� ���������������� ��������
{
//  FlashErasePage(CFG_USER); // ������ �������� ��������� ������������ �������
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
}

// ������� ��������������
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
// ��� ������ �������������� ���������� 16 ������ �� 528 ���� �� ��� 512 ������ �������������� 
// + 16 ��������������� ������ 0-���� = ������� ����� 1-2 ���� = ����������� � �����
// 15 ���� - ������������ ���������
void TraceWRITE(unsigned int Trace)
{
//  int i;
//  //extern unsigned char Index_set ; // ������ ������ ������ (0=�����, 1=10��, 2=20��, 3=40��, 4=80��) 
//  //extern unsigned char Index_Ln ; // ������ �������� ����� ����� � ���������������� ������
//  //extern unsigned char Index_Im ; // ������ �������� ������������ �������� 
//  //extern unsigned char Index_Vrm ; // ������ �������� ������� ������ 
//  //extern unsigned char ds1337 ; // ������ �������� ������� ��������� �������  
//  //extern float K_pr; 
//  unsigned long DateFileName = GetSetCntFiles (0);
//  //if (Trace <= ((CheckIDMEM)?(0x100+MaxMemOTDRExt):(MaxMemOTDR)))
//      if(CHECKNUMTRACE)
//  {
//
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      //SPI1_write_page(Trace*16+i+1, (unsigned char*)&Dat[128*i],512,0);
//      // ��� 2 �������� ������ � ���� ���� ����� ���� ������ �� 2 ����� 
//      FlashWritePageTrBuf(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&LogData[256*i]);
//      switch(i)
//      {
//      case 0: // ������ �������� ������� (������ 4 ����� ��������� �������)
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&TimeSaveOTDR,16,512);
//      break;
//      case 1:       // ������ �������� ������� (������ 4 ����� ��������� �������)
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&TimeSaveOTDR+16,16,512);
//      break;
//      case 2: // ������ ����� ������������
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&CommentsOTDR,10,512);
//      break;
//      case 3:
//        
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&CommentsOTDR[10],10,512);
//      // ��������� �������� ����� ����������� ��������������
//      break;
//      case 4:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&DateFileName,4,512);
//      // ��������� ORL � ������ ����� �������� � ���� ������ ���������! � ��� ����� ���-��
//      break;
//      case 5:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&g_VolORL,4,512);
//      break;
//      // 17.11.2022 ��������� ������� ��������� ������ ���� ���� ��� ���������
//      case 6:
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&g_SetModeLW,1,512);
//      break;
//      case 15:
//        //� ��������� �������� ����� ���������� ��������� 
//        FlashWritePageSM(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&SettingRefl,16,512);
//        //Index_Ln  // ������ �������� ����� ����� � ���������������� ������
//        // Index_Im // ������ �������� ������������ �������� 
//        // Index_Vrm  // ������ �������� ������� ������ 
//        // SW_LW  // ������� ������ ����� ������ 
//        // K_pr  // ����� ����������� 
//        // Index_Comm  // �������������� ������� � ������������ 
//        // SubModeRef; // ������������ ���� ��� ��� ( ��� ������������� ����� ���������)
//        // NumAverag; // ����� ���������� (�������� �� ������)
//      break;
//      //
//      default:
//        break;
//      }
//    }
//  }
}
;
// ��������� ���������� ������� ���������������
void AlarmReWriteTrace (void)
{
//  //TraceREAD(0);
//  //CheckSavedTrace();
//  //clear Buf 
//  memset( LogData, 43690, OUTSIZE * sizeof(unsigned short) ); // ZERO_ARRAY( array ) memset( array, 0, sizeof(array) )
//  // Set Comments
//  sprintf(CommentsOTDR,"Empty              ");
//  TimeSaveOTDR = RTCGetTime(); // ��������� ����� �����
//  TraceERASE(0);
//  TraceWRITE(0);
//  TraceREAD(0);
}
// ��� ������ �������������� ���������� 16 ������ �� 528 ���� �� ��� 512 ������ �������������� 
// ������ 3 ��� �������������� ��� ��������� "�������" ����������, �������� �� 1000��, ��� ������� ������������ ���� 
// ����� ��� �� � 250 �������������� (4001 pg...4016)
void SpliceWRITE(unsigned int Trace) 
{
//  int i;
//  if (Trace == 250)
//  {
//    for (i=0; i<NUMPAGESFORREF; i++)
//    {
//      //SPI1_write_page(Trace*16+i+1, (unsigned char*)&Dat[128*i],512,0);
//      // ��� 2 �������� ������ � ���� ���� ����� ���� ������ �� 2 ����� 
//      FlashWritePageTrBuf(Trace*NUMPAGESFORREF+i+1, (unsigned char*)&LogData[256*i]);
//    }
//  }
}
;
// �������� ������� ����� Splice �������������
unsigned short FlashReadSpliceData (unsigned int NumData)
{
    unsigned short OUT_Data = 0;
//    unsigned long PgSplice = 4001;
//    PgSplice  = PgSplice + (NumData>>8);
//  unsigned long CmdSqn = PgSplice << 10; // ����� �������� 
//  CmdSqn = CmdSqn + ((NumData & 0xFF)<<1); // ���� ��������� � ��������
//  CmdSqn = CmdSqn + 0xD2000000; //������� 
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


// ������ �������� ����������� ��������������
void FlashReadPageTrace (unsigned int NumPg, unsigned char *adress, int Block)
{
//    unsigned char *c;
//    int i;
//    BYTE CheckSmb;
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000000; //������� 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   for (int i=0; i<512; ++i)
// {
//   *adress++=SSP_FlashTransmit(1, 0);
// }
//   FL_CS(1,1); // CS_0
//    CreatDelay (40000); // 3.3 ��
//   CmdSqn =(unsigned long)NumPg << 10;
//  CmdSqn = CmdSqn + 0xD2000200; //������� 
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
//   CommentsOTDR[19]=0; // ������������� �������� ������
// }
// // ���������� ����� �������� ����� (������ �� ������)
// if (Block==4)
// {
//   c = (unsigned char*)&CurCountFiles;
//   for (i=0; i<4; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
// }
// if (Block==5) // ������ ORl (���� ��������) (����� ���� inf)
// {
//   c = (unsigned char*)&g_VolORL;
//   for (i=0; i<4; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//   // �������� �� inf � ��������
//       if (isnan(g_VolORL)||(g_VolORL<14.8)||(g_VolORL>70.0))
//         g_VolORL = 0.0;
// }
// if (Block==6) // ������ g_SetModeLW (BYTE) 17.11.2022
// {
//   c = (unsigned char*)&mg_SetModeLW;
//   for (i=0; i<1; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//   // �������� �� inf � ��������
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
// ������ ����������� ����������� �������������� �� ������
// ������ �������� ����������� ��������������
unsigned long FlashReadCommTrace (unsigned int Num, unsigned char *adress)
{
//    unsigned char *c;
//    BYTE CheckSmb;
  unsigned long NumWrSq = Num; // ����� ������
//  unsigned long NumPg = GetNumTraceSaved(Num)*NUMPAGESFORREF+3; // ����� ��������� �� 2 ���� ��� �����������
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //������� ������ ����� � 512 ����� 
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
//  CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //������� ������ ����� � 512 ����� 
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
//  CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //������� ������ ����� � 512 ����� 
//  FL_CS(1,0); // // CS_0 4 �������� ������ � ������ ������
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

// ������ ������� ����������� ��������������(���� �����)
unsigned long FlashReadTimeTrace (unsigned int Num)
{
  unsigned long NumPg =1; // ����� ��������� �� 2 ���� ��� �����������

//  unsigned char *c;
//  // ����� ����������� �������������
//    RTCTime TimeReadOTDR;
//  unsigned long NumWrSq; // ��������� ������������ ������ ���� ����� ( ��� ������� �������� ���� ��� ���� ��� ���)
//  unsigned long NumPg = GetNumTraceSaved(Num)*NUMPAGESFORREF+1; // ����� ��������� �� 2 ���� ��� �����������
//  unsigned long CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //������� ������ ����� � 512 ����� 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   c = (unsigned char*)&TimeReadOTDR;
//   for (int i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//  FL_CS(1,1); // CS_1 ��������� ����� ����
//  NumPg++;
//  CmdSqn =(unsigned long)NumPg << 10; // ������� ��� �������� ��� AT45DB161
//  CmdSqn = CmdSqn + 0xD2000200; //������� ������ ����� � 512 ����� 
//  FL_CS(1,0); // CS_0
//  SSP_FlashWriteLong( CmdSqn );
//  SSP_FlashWriteLong( 0 );
//   c = (unsigned char*)&TimeReadOTDR+16;
//   for (int i=0; i<16; i++)
//   {
//     *c++=SSP_FlashTransmit(1, 0);
//   }
//  FL_CS(1,1); // CS_1 ��������� ����� ����
//  //
//  sprintf(NameReadFile,"%02d%02d%02d_%02d%02d%01d.sor",TimeReadOTDR.RTC_Year%100,
//          TimeReadOTDR.RTC_Mon,
//          TimeReadOTDR.RTC_Mday,
//          TimeReadOTDR.RTC_Hour,
//          TimeReadOTDR.RTC_Min,
//          TimeReadOTDR.RTC_Sec/10 );
  
  return NumPg;
}

// ������ ��������������
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
//   //CheckCommOTDR (); // �������� � ������������� ������ ������������ OTDR
//
//  if (CurCountFiles > 0x100000) // empty data
//  {
//    CurCountFiles = (unsigned long)Trace;
//  }
//  if (Trace == 0) // ���� ������ ������������ �������������� ����������������� ����� �������������
//  {
//    SetSubModRefl (SettingRefl.SubModeRef);
//    GetSetCntFiles(CurCountFiles);
//  }
}

unsigned short SetNumTrace (unsigned short Num) // ��������� ������ ������
{
//  //������ ���������� ����� ������, ���� ��������������� � ����������� �� ���������� ������
//  // �������� �� ��� ��� ���� ���� � ����������� �� ������������
//  if (CheckIDMEM) // ���� ����������� ������
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

unsigned short GetNumTrace (void) // ��������� ������ ������ 
{
  return NumTrace;
}
// �������� "�������� �����" �������� ��������������
unsigned short GetNumTraceSaved (unsigned short Index) // ������ �������� ������� ������������ �� ������� (��������� �����)
{
  unsigned short RealNum = 0;
//  if (CheckIDMEM) // ���� ����������� ������
//  {
//    if (Index > (MaxMemOTDRExt + MaxMemOTDR) )
//    {
//      Index = 0;
//    }
//    else
//    {
//      if (Index>MemTable[0]) // ����� ������ ��� �������� � 1 �����
//      {
//        RealNum = (unsigned short)(0x100 + MemTableExt[Index - MemTable[0]]); 
//      }
//      else
//      {
//        RealNum = (unsigned short)MemTable[Index];
//      }
//    }
//    if (Index == 0)      RealNum = (unsigned short)MemTable[Index]+(unsigned short)MemTableExt[Index]; // ������� ����� ���������
//
//  }
//  else // ��� ����������� ������
//  {
//    if (Index > MaxMemOTDR)
//    {
//      Index = 0;
//    }
//      RealNum = (unsigned short)MemTable[Index];
//  }
  return  RealNum;

}

void CheckTableMemOTDR (BYTE MemErr) // �������� ������� ���������� ������������ 0 - ��������� 1 - ���������
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
////  AllTrace =  (unsigned short)MemTable[0] +  (unsigned short)MemTableExt[0]; // ������� ����� ������������� ��������
////  // ��������� ������������ ����������� ������������� �� ���� ����� � ����� ���������� ���� ����� ������� ��� ��������������
////  for (int i = AllTrace; i > 0 ; i--) 
////  {
////    GetNumTraceSaved
////    
////  }
  
  
}

unsigned short DeletingTrace (unsigned short Trace) // "��������" ��������������, ������ ��������� 
{
//  unsigned short IndexMem;
//      if (CheckIDMEM) // ��������� ����� ������ �����
//      {
//        // ���������� � ����� ����� ����� ��������� ������
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
//        else // ������ �� ������ �����
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
//      else  // ��� ������� ������ ��-�������
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

unsigned short ReanimateTrace (unsigned short ReanT) // "����������" ��������� ����� �����
{ 
  unsigned short Trace=0; // ���������� ����� ��������������� �����
//  if (CheckIDMEM) // ��������� ����� ������ �����, ���� ���� �������, �� ������ � ��� ������� �����
//  {
//    if(ReanT>200)
//    {
//      // ����� ������������� ������ � ������� �����
//      MemTableExt[0] = ReanT - 200;
//    }
//    else
//      MemTableExt[0] = 0;
//    //Trace = (unsigned short)MemTable[0] + (unsigned short)MemTableExt[0] ; // ������� ����� ������ � ������� �����
//    Trace =  (unsigned short)MemTableExt[0] ; // ������� ����� ����������� � ������� �����
//    // ��������� ������� ����������
//    FlashErasePage(TABLEMEMOTDREXT);
//    FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//  }
//  // ���� ���� ������ ����� ����� ����������� ������ �� ����� 200 
//  //      else
//  //      {
//  if(ReanT>200)
//    MemTable[0] = 200;
//  else
//    MemTable[0] = ReanT;
//  Trace = Trace + (unsigned short)MemTable[0];
//  SetNumTrace (0); // ��������� ������ ������
//  FlashErasePage(TABLEMEMOTDR);
//  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//  //      }
  return Trace; 
}

unsigned short DeletingAllTrace (void) // "��������"���� ��������������, ������ �������� ������� ����������
{ 
  unsigned short Trace=0; // ���������� ����� ��������� �����
//      if (CheckIDMEM) // ��������� ����� ������ �����, ���� ���� �������, �� ������ � ��� ������� �����
//      {
//  //Trace = (unsigned short)MemTable[0] + (unsigned short)MemTableExt[0] ; // ������� ����� ������ � ������� �����
//  Trace =  (unsigned short)MemTableExt[0] ; // ������� ����� ������ � ������� �����
//  
//  //SetNumTrace (0); // ��������� ������ ������
//  MemTable[0] = 0;
//  MemTableExt[0] = 0;
//  // ��������������� ����
//    for (int i = 0; i <= MaxMemOTDRExt; ++i) MemTableExt[i] = i;
////  
////  FlashErasePage(TABLEMEMOTDR);
////  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//        // ��������� ������� ����������
//        FlashErasePage(TABLEMEMOTDREXT);
//        FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//     }
//     // ���� ���� ��������������� ������!!!
////      else
////      {
//  Trace = Trace + (unsigned short)MemTable[0];
//  SetNumTrace (0); // ��������� ������ ������
//  MemTable[0] = 0;
//  // ��������������� ����
//    for (int i = 0; i <= MaxMemOTDR; ++i) MemTable[i] = i;
//  FlashErasePage(TABLEMEMOTDR);
//  FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
////      }
  return Trace; 
}

// �������� ������ - ������ �������� ������ �������������
void DrawMemoryRefl(unsigned short Trace, BYTE CurrLang, BYTE Param)
{
//  char Str[32];
//  char StrI[32];
//  char s_orl[6];
//  //BYTE CurrLang=GetLang(CURRENT);
//  if(g_FirstScr) // ���� ������ ��� ������ ������ ������
//  {
//    if (Trace)
//      TraceREAD(GetNumTraceSaved(Trace)); // ������ ������ �� ������ �� ������ �������������� � ������
//    else TraceREAD(0); // ���� 0 �� �� � ������
//  }
//      CheckCommOTDR (); // �������� � ������������� ������ ������������ OTDR ����� �������...
//
//  // �������� ��������� ������
//  if(CheckSavedTrace()) // ������� �������� ����������� �������������� 
//  {
//    // �������������� ������� ������ (���� �������)
//    Trace = DeletingTrace (Trace);
//    SetNumTrace(Trace);
//    TraceREAD(0);
//  }
//  //���������� ����������� ��������� ��� ������� ����������, ��� �� ������������ ��� ������, �� ����
//  GraphParams params = {27000,0,48,0,MEMDRAW};//PosCursorMain (0) // ������� 48 ( ��� ����������� ��������)
//  Rect rct={43,18,128,63};
//  //��� ������ ����������(������ ������)
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
//    SendDrawNex(NexData,6,rct.right); // ID=6 ��� ������� � ���������
//    // �������� ������ ����, ��� ������ ���������� � ������� ����������, �� ������ ������� ������� �����
//  sprintf(Str,"%02d%02d%02d_%02d%02d%01d.sor(%03d)",TimeSaveOTDR.RTC_Year%100,
//          TimeSaveOTDR.RTC_Mon,
//          TimeSaveOTDR.RTC_Mday,
//          TimeSaveOTDR.RTC_Hour,
//          TimeSaveOTDR.RTC_Min,
//          TimeSaveOTDR.RTC_Sec/10,
//          Trace);
//    sprintf(StrI, "t0.txt=\"%s\"���", Str);
//    UARTSend2((BYTE*)StrI, strlen(StrI));    // Date/Time/Num ������
//    sprintf(Str, "t5.txt=\"%s\"���",CommentsOTDR); 
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 ������ �����������
//  if (Param)
//  {
//    // ���� �������
//    CalkEventsKeys (LogData, PointsInImpulse(0), 1);
//    if (GetIndexLN()) // �� 2 �� ( ������ � ��)
//      sprintf(Str,"t1.txt=\"%.1f%s\"���",fabs(GetPosLine(EndEvenBlk.ELMP[1])) ,MsgMass[20][CurrLang]);//��
//    else
//      sprintf(Str,"t1.txt=\"%.1f%s\"���",fabs((GetPosLine(EndEvenBlk.ELMP[1]))*1000.0) ,MsgMass[78][CurrLang]);//� - �����
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 ������ �����������
//    sprintf(Str,"t2.txt=\"%.2f%s\"���",(LogData[EndEvenBlk.ELMP[1]]-LogData[EndEvenBlk.ELMP[0]])/1000.0,MsgMass[47][CurrLang]);//��
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 ������ �����������
//    if(g_VolORL!=0)sprintf(s_orl,"%.1f",g_VolORL);
//    else sprintf(s_orl,"???");
//    sprintf(Str,"t3.txt=\"%s%s\"���",s_orl,MsgMass[47][CurrLang]);//ORL, ���� ����������!
//    UARTSend2((BYTE*)Str, strlen(Str));    // 1 ������ �����������
//    sprintf(Str,"t4.txt=\"\"���");// Empty
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t6.txt=\"%d\"���",mg_SetModeLW);// Empty
//    UARTSend2((BYTE*)Str, strlen(Str));    
//  }
//  else
//  {
//    sprintf(Str,"t1.txt=\"%d%s\"���", GetLengthLine(GetIndexLN()),MsgMass[20][CurrLang]);
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t2.txt=\"%d%s\"���", GetWidthPulse(GetIndexIM()),MsgMass[23][CurrLang]);//ns
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t3.txt=\"%d%s\"���",GetLengthWaveLS (GetPlaceLS(CURRENT)),MsgMass[18][CurrLang]); // nm
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    sprintf(Str,"t4.txt=\"%d%s\"���", GetTimeAvrg(GetIndexVRM()),MsgMass[4][CurrLang]);//c
//    UARTSend2((BYTE*)Str, strlen(Str));    
//    //sprintf(Str,"t6.txt=\"%c/%c\"���",(SettingRefl.SubModeRef)?('A'):('M'),(GetSubModRefl())?('A'):('M'));// Empty
//    sprintf(Str,"t6.txt=\"%.4f\"���",GetWAV_SC(GetPlaceLS(CURRENT)));// ����� ����������� ��� ������ ����� �����
//    UARTSend2((BYTE*)Str, strlen(Str));    
//  }
//  }
//  // 
//  //sprintf(Str,"%d",  PointsInImpulse ());//ns
//  //putString(102,19,Str,1,0);//��
  
}


unsigned short SaveTrace(void) // ���������� ��������������
        // 
      // ���� ��������� ��� ����������
      
{ 
  //char Str[22];
  //ClearScreen(screen);
  
//  if (MemTable[0]<MaxMemOTDR)
//  {
//    MemTable[0]++;
//    CurCountFiles = GetSetCntFiles (1); // �������� ������� �������������
//    // ��������� ������� ����������
//    FlashErasePage(TABLEMEMOTDR);
//    FlashWritePageSM(TABLEMEMOTDR, MemTable, ARRAY_SIZE(MemTable),0);
//    
//    // ��� ������������� �������� ��� ��������� ������� 
//    // ������ ����� ����� � ������ - ������� ����� ���������� �������� ����� ������ � ���� ������
//    TraceERASE(MemTable[MemTable[0]]);
//    TraceWRITE(MemTable[MemTable[0]]); // ���������� �������������� � ����������� �����  � N ������
//    //Total_Time = Total_Sec(ds1337); // ������� �������
//    // ����� ���������� � ��������� � ��������� BellCore �����
//    //Set_Bel_core(Total_Time,(unsigned short int)(Im_set[Index_Im]) , (unsigned int)(0x07F281*DS_Code), (unsigned int)(K_pr*100000), sycl, (unsigned int)(0x01A0AA*DS_Code) );
//    //SPI1_read_page0(HEAD); // ��������� ����� BELCORE � ������
//    
//    NumTrace = MemTable[0];   // ��������� ������ ������������� �� ����� �����������
//  }
//  else// ������ ��������� (�������� ����� �� ������ ���� �� ����������� ������ � �� ���������)
//  {
//    if (CheckIDMEM) // ��������� ����� ������ �����
//      // ���� ����������� ������ - ����� � ����������� �������
//    {
//      if (MemTableExt[0]<MaxMemOTDRExt)// ���� ����� � ����������� �������
//      {
//        MemTableExt[0]++;
//        CurCountFiles = GetSetCntFiles (1); // �������� ������� �������������
//        // ��������� ������� ����������
//        FlashErasePage(TABLEMEMOTDREXT);
//        FlashWritePageSM(TABLEMEMOTDREXT, MemTableExt, ARRAY_SIZE(MemTableExt),0);
//        
//        // ��� ������������� �������� ��� ��������� ������� 
//        // ������ ����� ����� � ������ - ������� ����� ���������� �������� ����� ������ � ���� ������
//        TraceERASE(0x100 + MemTableExt[MemTableExt[0]]);
//        TraceWRITE(0x100 + MemTableExt[MemTableExt[0]]); // ���������� �������������� � ����������� �����  � N ������
//        NumTrace = MemTable[0] + MemTableExt[0];   // ������������ ����� �� �����������  ������ ������������� �� ����� �����������
//        
//      }
//      else
//      {
//        NumTrace = 0;  // ��������� ������ ������������� �� �������// ��� ����� � ����������� ������
//      }
//    }
//    else // ��� ������ ��������� 
//    {
//      NumTrace = 0;
//    }
//  }
//  TraceERASE(0);
//  TraceWRITE(0); // ���������� �������������� � ����������� �����  � 0 ������
  // ��� �� ��������� ����� �������� �����
  return NumTrace;
  //PaintLCD();
}

