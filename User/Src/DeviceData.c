// ��������� ���������������� ���������
#include "system.h"

char NumVer[10];
//const char *NumVer = {
//  #include "version.h"
//}
//;
 const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};

const unsigned WAVE_LENGTHS[WAVE_LENGTHS_NUM] = {850,1310,1490,1550,1625};

const char *Ides[2] = {"OOO NPK SvyazServis,\0","OPTOKON Co. Ltd,\0"};
const char *VerFW[2] = {",SW_rev \0",",SW_rev \0"};
//const char *Device[2][LANG_NUM] = {{"�����-7\0","TOPAZ-7\0","TOPAZ-7\0"},{"MOT-700\0","MOT-700\0","MOT-700\0"}};
const char *Device[2][2][2] = {{{"�����-7\0","�����-9\0"},{"TOPAZ-7\0","TOPAZ-9\0"}},{{"MOT-700\0","MOT-950\0"},{"MOT-700\0","MOT-950\0"}}};

const char *PMset[6] = {"10\0","31\0","32\0","-PMH\0","-VFL\0","AR\0"};
const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// ����� ����� �� ������
const DWORD LengthLine[LENGTH_LINE_NUM]= {2,4,8,16,32,64,128};
const DWORD DelayPeriod[LENGTH_LINE_NUM]= {700,1200,4000,8000,10000,0,0};// �������� ������� � ����� CreatDelay()~ 83.33 ��
const WORD WidthPulse[2][WIDTH_PULSE_NUM]= {{20,40,150,500,1000,3000,10000,20000},{10,40,150,500,1000,3000,10000,20000}};
const DWORD TimeAver[TIME_AVR_NUM]= {15,30,60,180,3,600};
const unsigned TimeLight[TIME_LIGHT_NUM] = {0,15,30};
const char *IdnsBC[2]= {"SvyazServis   \0","OPTOKON Co.Ltd\0"};
const char *MfidBC[2][2]= {{"TOPAZ7\0","TOPAZ9\0"},{"MOT700\0","MOT950\0"}};
// ������� ������� �������� �� ������ ��� ���������� ����� � �������������� �����������
const float LSACoef[WAVE_LENGTHS_NUM][LENGTH_LINE_NUM] = {
{1.283367837, 2.566735673, 5.133471347, 10.26694269, 20.53388539, 30.80082808, 61.60165616},//850
{0.233984191, 0.467968382, 0.935936765, 1.87187353, 3.743747059, 5.615620589, 11.23124118},//1310
{0.148899031, 0.297798062, 0.595596123, 1.191192246, 2.382384492, 3.573576738, 7.147153477},//1490
{0.134718171, 0.269436341, 0.538872683, 1.077745366, 2.155490731, 3.233236097, 6.466472193},//1550
{0.148899031, 0.297798062, 0.595596123, 1.191192246, 2.382384492, 3.573576738, 7.147153477}//1625
};
const char HeaderBelcore[241] = {
#include "head1.h"
};

// ��������� ���������� �������������� ( �������� ���������)
TR_PARAM ReflParam;
// ��������� ���������� ��������� JDSU
JDSU_COM SetJDSU;
// ��������� ��������������� �����
DB_NAME_ENA NameDB;
// ��������� ������������ �����
TAB_SET CoeffPM;
// ��������� ��������� ������������� ( �������� ���������)
 REFL_SET SettingRefl;
//// ��������� ������������ �������
CFG_DEV ConfigDevice;
//// ��������� ��������������� ���������
CFG_USR UserSet;
//typedef struct
//{
//  WORD NumDevice;  // (999) � �������,
//  WORD PlaceLS[LSPLACENUM]; //  ����� 1, (1310) ������������ ������������� ������� �� ������
//  BYTE TypeDevice; // (5)  ������� � ���� ������� ������ ��� ����� (��������� �����)
//  BYTE CfgPM;       //	������� ���������� (0-���,1- �������(-70..+6), 2- ����������� (-55..+20)
//  BYTE CfgRE;       //	������� �������� ����� (0-���, 1-����)
//  BYTE ApdiSet;     //	������� ��������� ����� (0-���, 1 ����)
//  WORD InputCheckLvl;	//����� �������� ������ �� ����� (10- ������� ������) 
//  WORD EndCheckLvl; // 	����� �������� ����� ����� (8500)
//  BYTE ID_Device; // ������ 0-����� ��� 1-MOT_700
//  float BatStep; // ��� ��� ���������
//} CFG_DEV;

//char CommentsOTDR[20] = {"                   \0"}; //����������� ������������� ���������� �� modes.c
uint8_t StrTXOpt[64]; // ������ �������� ������ �� ������

uint8_t TxOptBusy=0; // ������� ��������� UatrOptic
uint8_t TimeSCWP=0; // ������� ������� ��� ������������ �������


static volatile BYTE ModeLS = 0; // ����� ���������
static volatile BYTE ModeRE = 0; // ����� VFL
static DWORD CurrentBegShiftZone = 0;


static volatile BYTE IndBkLight = 0; // ��������� �� ����� ������������ ���������
static volatile unsigned short LogarifmNoise = 65535; // ������� ���������������� ����, ���������

static volatile unsigned short CheckSum = 0;
// ������� ������ ������� ������
// ������ � ������� ����� ��� ���������� ��������, ������� �������� 
// ������ � ����������� ����������
unsigned long CountFiles;
unsigned long CurCountFiles; // ���������� ����� �������� ����� (������ �� ������)
// ���� ������ �� �������� (0xFFFF), �� ��� ���������� ����������� ���������� ����� �������� 
// ��� ����� ����������� ������ ��� ����� (��� ������)

BYTE g_SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index 
BYTE mg_SetModeLW; // 16.11.2022 ���������� ������������ ������������ �� ����� ����� Index for MEMORY

static volatile BYTE g_SeqCombLW[7]; //����������� ���������� ��������� �� ������ �������

// ��������� ��������� ������� � ������ ���������� ������� �� ������ ��������� �����
void SetIndxSeqLS (void)
{
  static volatile int h=0;
  
  // ���� ���������� ������ � ������� ���������� ����� ����� �����
      for(h=0; h<7; h++)
      {
        if(g_SeqCombLW[h] == (1<<SettingRefl.SW_LW))
        {
         g_SetModeLW = h;
         break;
        }
      }
}

unsigned StructSize(unsigned data)
{
  switch( data )
  {
  case CFG_DEVICE:
    return sizeof(CFG_DEV);
  case CFG_USER :
    return sizeof(CFG_USR);
  case COEF_PM: 
    return sizeof(TAB_SET);
  case EVEN_SET: 
    return sizeof(TR_PARAM);
  case JDSUSTRUCT:
    return sizeof(JDSU_COM);
  case DBNAMESTRUCT:
    return sizeof(DB_NAME_ENA);
  }
  return 0;
}

unsigned char * StructPtr(unsigned data)
{
  switch( data )
  {
  case CFG_DEVICE:
    return (unsigned char*)&ConfigDevice;
  case CFG_USER :
    return (unsigned char*)&UserSet;
  case COEF_PM: 
    return (unsigned char*)&CoeffPM;
  case EVEN_SET: 
    return (unsigned char*)&ReflParam;
  case JDSUSTRUCT:
    return (unsigned char*)&SetJDSU;
  case DBNAMESTRUCT:
    return (unsigned char*)&NameDB;
  }
  return 0;
}
// Check param DBNAME
unsigned InvalidDBNAME() //boolean actually
{
  unsigned res = NameDB.Ena_DB > 1;
  unsigned err_str = 1;
  for( unsigned i = 0; i < (sizeof(DB_NAME_ENA)-1);++i )
  {
    if(NameDB.AltName[i]==0) 
    {
      err_str = 0;
      break;
    }
  }
  res = (res<<1) | err_str;
  for( unsigned i = 0; i < 3;++i )
  {
    err_str = 0;
    if((NameDB.ph_A[i]<-0.02)||(NameDB.ph_A[i]>0.02))
    {
     err_str = 1;
    }
    res = (res<<1) | err_str;
  }
 
  return res;
}
void InitDBNAME(unsigned Err)
{
  if (Err && 0x10) NameDB.Ena_DB=1; // ������ ����� , ��������� ����� (Not Alternate)
  
  if (Err && 0x8) // ��� ����� ������
  {
  for( unsigned i = 0; i < (ARRAY_SIZE(NameDB.AltName)-2);++i )
  {
    NameDB.AltName[i]=' '; 
  }
  NameDB.AltName[(ARRAY_SIZE(NameDB.AltName)-1)]=0;
  }
  if (Err && 0x4) //������ ph_A[0]
  NameDB.ph_A[0]=0.0;
  if (Err && 0x2) //������ ph_A[1]
  NameDB.ph_A[1]=0.0;
  if (Err && 0x1) //������ ph_A[2]
  NameDB.ph_A[2]=0.0;
}



// Check param JDSU
unsigned InvalidJDSU() //boolean actually
{
  unsigned res = SetJDSU.CompMode > 1;
  for( unsigned i = 0; i < 6/*PLACE_LS_NUM*/;++i )
  {
  res = (res<<1) | (SetJDSU.LambdaNum[i] > 1650);
  res = (res<<1) | (SetJDSU.FreqLambda[i] > 10000);
  }
  return res;
}
void InitJDSU(unsigned Err)
{
  if (Err && 0x1000) SetJDSU.CompMode=0; // ������ ����� , ��������� �����
  
  if (Err && 0xAAA) 
  {
    SetJDSU.LambdaNum[0] = 850;
    SetJDSU.LambdaNum[1] = 1300;
    SetJDSU.LambdaNum[2] = 1310;
    SetJDSU.LambdaNum[3] = 1490;
    SetJDSU.LambdaNum[4] = 1550;
    SetJDSU.LambdaNum[5] = 1625;
  }
  if (Err && 0x400) SetJDSU.FreqLambda[0] = 0;
  if (Err && 0x100) SetJDSU.FreqLambda[1] = 0; 
  if (Err && 0x40) SetJDSU.FreqLambda[2] = 2000;
  if (Err && 0x10) SetJDSU.FreqLambda[3] = 0; 
  if (Err && 0x4) SetJDSU.FreqLambda[4] = 270;
  if (Err && 0x1) SetJDSU.FreqLambda[5] = 0; 
}


//
unsigned InvalidDevice() //boolean actually
{
  BYTE N_LS=0;
  unsigned res = ConfigDevice.NumDevice > 9999;
  for( unsigned i = 0; i < LSPLACENUM/*PLACE_LS_NUM*/;++i )
  {
    if (ConfigDevice.PlaceLS[i]!=0) N_LS++;
  res = (res<<1) | (ConfigDevice.PlaceLS[i] > 1650);
  }
  GetSetNumLS(N_LS); // �������� ������� ������� ������������
  res = (res<<1) | (ConfigDevice.TypeDevice > 9);
  res = (res<<1) | (ConfigDevice.CfgPM > 2);
  res = (res<<1) | (ConfigDevice.CfgRE > 1);
  res = (res<<1) | (ConfigDevice.ApdiSet > 1);
  res = (res<<1) | (ConfigDevice.InputCheckLvl > 10);
  res = (res<<1) | (ConfigDevice.EndCheckLvl < 8000);
  res = (res<<1) | (ConfigDevice.ID_Device >1);
  res = res<<1;
    if ((isnan(ConfigDevice.BatStep))||((ConfigDevice.BatStep<0.002)||(ConfigDevice.BatStep>0.003))) res++; 
  return res;
}
// ��������/��������� �������� �������� ������ �������������, ����� � 200 
unsigned long GetSetCntFiles (BYTE Dir)
{
  if (Dir)
  {
    if (CountFiles < 200) CountFiles = 200;
    else CountFiles++;
  }
  return CountFiles;
}
// ���������� �������� � ������ ������� �������������
void SetCurCntFiles (void)
{
   CountFiles = CurCountFiles ;
}

void InitDevice(unsigned Err)
{
  if (Err && 0x01)  
  {
  ConfigDevice.BatStep = 2.4414e-3; // ���� �������� ������, ��� ���������� 2.5�
  } 
  Err = Err>>1;
  if (Err && 0x01)  ConfigDevice.ID_Device = 0; // ����� 
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.InputCheckLvl = 10;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.EndCheckLvl = 9500; 
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.ApdiSet = 0;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.CfgRE = 0;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.CfgPM = 0;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.TypeDevice = 2;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.PlaceLS[2] = 0;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.PlaceLS[1] = 0;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.PlaceLS[0] = 1310;
  Err = Err>>1;
  if (Err && 0x01) ConfigDevice.NumDevice = 0;
}

float GetSetBatStep (float Dir) // ���������� ��� ������������� �������� ���� ��� ���������
{
  float DataOut = ConfigDevice.BatStep;
  if (Dir==0.0) DataOut = ConfigDevice.BatStep;
  else
  {
    if ((Dir>0.002)&&(Dir<0.003)) ConfigDevice.BatStep = Dir;
    else ConfigDevice.BatStep = 2.852e-3;
  }
    //if ((FIO3PIN & TEST_VER)) DataOut = ConfigDevice.BatStep;
    //else  DataOut = 2.4414e-3; // ���� �������� ������, ��� ����� 2.5�

  return DataOut;
}


void SetLang( BYTE lang ) // ������� �� �������
{
  if ( lang < MAX_LANG )//LANG_NUM-1
  {
    lang = 0;
  }
  if (lang==0)lang=ConfigDevice.ID_Device;
    UserSet.CurrLang = lang;
}

BYTE GetID_Dev(void) // ��������� �������������� ��� ���� ������
{
  return ConfigDevice.ID_Device;
}
BYTE SetID_Dev(BYTE Data) // ��������� �������������� ��� ���� ������
{
  if (Data>1) Data=1;
  UserSet.CurrLang = Eng; // ������ ��� ��������� �������������� ������������� Eng
  return ConfigDevice.ID_Device = Data;
}

BYTE GetLang(int Dir) // ��������� �������� �����, � ������������� �� �����
{
  BYTE Lang = UserSet.CurrLang;
  if (Dir) // ����������� �����
  {
    if (Dir>0)
    {
    Lang++;
    //Lang = GetID_Dev()*2 - Lang + 1;
    if (Lang > ((GetID_Dev())?(MAX_LANG):(1))) Lang = GetID_Dev(); //LANG_NUM-1
    }
    else
    {
      if ((Lang>1 && (GetID_Dev()))||(Lang>0 && !(GetID_Dev())))
        Lang--;
      else
        Lang=((GetID_Dev())?(MAX_LANG):(1));//LANG_NUM-1
    }
  }
  
    UserSet.CurrLang = Lang;
  
/*  Lang += Dir;
  if (Lang > MAX_LANG) 
  {
      if(ConfigDevice.ID_Device) UserSet.CurrLang = Eng;
      else UserSet.CurrLang = Rus;
  }
  else
  {
      if ( Lang < GetID_Dev() ) UserSet.CurrLang = MAX_LANG;
    else UserSet.CurrLang = Lang;
  }
  */
  return UserSet.CurrLang;
}
    

WORD GetNumDevice(void) // ��������� ������  �������
{
  return ConfigDevice.NumDevice;
}

WORD SetNumDevice(WORD Num) // ��������� ������  �������
{
  if (Num>9999) Num=9999;
  return ConfigDevice.NumDevice = Num;
}
// ��������� �������������� "������" , ��� � ��� ����������������
void GetDeviceHW( char* name ) // from Computer
{
  char Str[100];
  
  sprintf(Str,"STM32H743 DEVICE\r");
  strcat( name,Str);
}

void GetDeviceName( char* name ) // from Computer
{
  char c[10];
  char Str[8];
  strcpy( name, Ides[ConfigDevice.ID_Device] );//, strlen(Ides[device->ID_Device])
    strcat( name, Device[GetID_Dev()][1][1]);//, strlen(Idesl[device->ID_Device]) // ����� ������� �����������
   switch ( ConfigDevice.ID_Device)
   {
   case 0: // RUS-SvyazService
     strcpy(c,PMset[ConfigDevice.CfgPM]);
     //if(!(FIO1PIN & TABLET_95)) c[0] = '5';
     strcat( name, c);
     c[1]=0;
     c[0]= 0x30 + ConfigDevice.TypeDevice;
     strcat( name,c);
     c[0]= 'R';
     strcat( name,c);
     if (ConfigDevice.CfgRE) c[0]='+';
     else c[0]=' ';
     strcat( name,c);
     c[0]= 'A';
     strcat( name,c);
     //if (ConfigDevice.ApdiSet) c[0]='X';
     if ((ConfigDevice.ApdiSet)&&(NameDB.Ena_DB)) c[0]='X'; // ��� IDN ��� ��� �������!
     else c[0]=' ';
     strcat( name,c);
     sprintf(c,",%04d",ConfigDevice.NumDevice) ;
     strcat( name,c);
     break;
   case 1: // CZE-OPTOKON
     c[1]=0;
     if (ConfigDevice.ApdiSet) c[0]='D';
     else c[0]=0;
     strcat( name,c);
  //����� ���� �������
  for (int i=0; i<ARRAY_SIZE(ConfigDevice.PlaceLS); ++i)
  {
    if (ConfigDevice.PlaceLS[i] != 0)
    {
      sprintf (c,"%4d", ConfigDevice.PlaceLS[i]);
      c[0] = '-';
      c[3] = 0; // ��� ����� 
      strcat(name,c);
    }
  } 
     strcpy(c,PMset[3]); //-PMH
  
     switch (ConfigDevice.CfgPM)
     {
   case 0: // ��� ����������
     c[0]= 0;
     strcat( name,c);
     break;
   case 1: // ������� ����������
     c[3]= 0;
     strcat( name,c);
     break;
   case 2: // ���������� �� ������
     strcat( name,c);
     break;
     }
     strcpy(c,PMset[4]); //-VFL
     if (!(ConfigDevice.CfgRE)) c[0]=0;
     strcat( name,c);
     sprintf(c,",MT%04d",ConfigDevice.NumDevice) ;
     strcat( name,c);
     break;
   }
     strcat( name, VerFW[ConfigDevice.ID_Device]);  
     GetNumVer(Str);
     sprintf (c, "%s(%s)\r",Str,NumVer);
     strcat( name, c);   
  
}
// ��������� ������ ����� ��� ���� ���������, ��� �������!
unsigned short GetNoiseLvl (void)
{
   if (ConfigDevice.ApdiSet) return (25000);
   else return (18500);

}

void GetMFID( char* name ) // for Belcore
{
  char c[8];
  strcpy( name, MfidBC[ConfigDevice.ID_Device][1] );//, strlen(Ides[device->ID_Device])
   switch ( ConfigDevice.ID_Device)
   {
   case 0: // RUS-SvyazService
     strcpy(c,PMset[ConfigDevice.CfgPM]);
     //if(!(FIO1PIN & TABLET_95)) c[0] = '5';
     strcat( name, c);
     c[1]=0;
     c[0]= 0x30 + ConfigDevice.TypeDevice;
     strcat( name,c);
     c[0]= 'A';
     strcat( name,c);
     c[0]= 'R';
     strcat( name,c);
     //if (ConfigDevice.ApdiSet) c[0]='X';
     if ((ConfigDevice.ApdiSet)&&(NameDB.Ena_DB)) c[0]='X'; // ���� �������������� ���, �� ����� 'X'
     else c[0]=' ';
     strcat( name,c);
     break;
   case 1: // CZE-OPTOKON
     c[1]=0;
     if (ConfigDevice.ApdiSet) c[0]='D';
     else c[0]=' ';
     strcat( name,c);
     switch (ConfigDevice.CfgPM)
     {
   case 0: // ��� ����������
     strcat( name,"     ");
     break;
   case 1: // ������� ����������
     strcat( name,"-PM  ");
     break;
   case 2: // ���������� �� ������
     strcat( name,"-PMH ");
     break;
     }
   }
}


void GetTesterName( char* name ) // Tester
{
  char c[8];
  if (ConfigDevice.ID_Device)// CZE-OPTOKON
  {
    strcpy( name, MsgMass[8][UserSet.CurrLang] );//, strlen(Ides[device->ID_Device])
    strcpy(c,PMset[3]); //-PMH
    
    switch (ConfigDevice.CfgPM)
    {
    case 0: // ��� ����������
      c[0]= 0;
      strcat( name,c);
      break;
    case 1: // ������� ����������
      c[3]= 0;
      strcat( name,c);
      break;
    case 2: // ���������� �� ������
      strcat( name,c);
      break;
    }
    strcpy(c,PMset[4]); //-VFL
    if (!(ConfigDevice.CfgRE)) c[0]=0;
    strcat( name,c);
  }
  else// RUS-SvyazService
  {
    strcpy( name, MsgMass[76][UserSet.CurrLang] );//, strlen(Ides[device->ID_Device])
  }
}

void GetDeviceLabel( char* label, int Mode ) // Label on Main device page
{
    char c[18];
  //strncpy( name, Idesl[device->ID_Device], strlen(Idesl[device->ID_Device]) );
   strcpy(label, Device[GetID_Dev()][(UserSet.CurrLang)?(1):(0)][1]);//, strlen(Idesl[device->ID_Device]) [UserSet.CurrLang]
   switch ( ConfigDevice.ID_Device)
   {
   case 0: // RUS-SvyazService
     if(Mode) // ������� ���
     {
     strcpy(c,PMset[ConfigDevice.CfgPM]);
     //if(!(FIO1PIN & TABLET_95)) c[0] = '5';
     strcat( label,c);
     c[1]=0;
     c[0]= 0x30 + ConfigDevice.TypeDevice;
     strcat( label,c);
     strcpy(c,PMset[5]); //AR
     strcat( label,c);
     c[1]=0;
     if (ConfigDevice.ApdiSet) c[0]='X';
     //if ((ConfigDevice.ApdiSet)&&(!NameDB.Ena_DB)) c[0]='X'; // ��� IDN ��� ��� �������!
     else c[0]=0;
     strcat( label,c);
     if (ConfigDevice.CfgRE) c[0]='+';
     else c[0]=0;
     strcat( label,c);
     }
     else // alternate name
     {
       // ������ ����� ������
     //strncpy( label, NameDB.AltName, strlen(NameDB.AltName) );
     //label[strlen(NameDB.AltName)]=0;
       // �������� ����� ������������ �� ������
     //strncpy( c, NameDB.AltName, strlen(NameDB.AltName) );
     strncat( label, (void*)NameDB.AltName, strlen((void*)NameDB.AltName)+1);
     }
     break;
   case 1: // CZE-OPTOKON
     c[1]=0;
     if (ConfigDevice.ApdiSet) c[0]='D';
     else c[0]=0;
     strcat( label,c);
     //����� ���� �������
     for (int i=0; i<ARRAY_SIZE(ConfigDevice.PlaceLS); ++i)
     {
       if (ConfigDevice.PlaceLS[i] != 0)
       {
         sprintf (c,"%4d", ConfigDevice.PlaceLS[i]);
         c[0] = '-';
         c[3] = 0; // ��� ����� 
         strcat(label,c);
       }
     } 
   }
}


DWORD FindErrCoeff (void)
{
  DWORD Err = 0;
  DWORD NewFlash = 0;
    for (int i=0; i<ARRAY_SIZE(CoeffPM.ShZeroRng); ++i) 
    {
      if (CoeffPM.ShZeroRng[i]== -1) NewFlash++ ; // ������ �� ������
      if (CoeffPM.ShZeroRng[i]>300000) Err |=0x01; // �������� �� ����������
    }
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefStykRange); ++i)
    if (isnan(CoeffPM.CoefStykRange[i])||(CoeffPM.CoefStykRange[i]<=0.0)||(CoeffPM.CoefStykRange[i]>100.0)) Err |=0x40;// ������������ �������� ����������
  for (int i=0; i<ARRAY_SIZE(CoeffPM.PointKalib); ++i)
    if (CoeffPM.PointKalib[i]>2000) Err |=0x02;// ����� ���� ���������� ���� 0 �� �� ���
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefPointKlb); ++i)
    if (isnan(CoeffPM.CoefPointKlb[i])||(CoeffPM.CoefPointKlb[i]<=0.0)||(CoeffPM.CoefPointKlb[i]>400.0)) Err |=0x04;// ������������ �������� ���� ���� ����������
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i) 
    if (isnan(CoeffPM.CoefSpctrL[i])||(CoeffPM.CoefSpctrL[i]<=0.0)||(CoeffPM.CoefSpctrL[i]>(80000.0))) Err |=0x08;// ������������ ������������ ����. 800-900
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrM); ++i)
    if (isnan(CoeffPM.CoefSpctrM[i])||(CoeffPM.CoefSpctrM[i]<=0.0)||(CoeffPM.CoefSpctrM[i]>(80000.0))) Err |=0x10;// ������������ ������������ ����. 1210-1370
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrH); ++i)
    if (isnan(CoeffPM.CoefSpctrH[i])||(CoeffPM.CoefSpctrH[i]<=0.0)||(CoeffPM.CoefSpctrH[i]>(80000.0))) Err |=0x20;// ������������ ������������ ����. 1410-1650
  if(NewFlash == ARRAY_SIZE(CoeffPM.ShZeroRng)) Err |=0x7F;
  return Err;
}

void FixErrCoeff (DWORD Err)
{

  if (Err != 0)
  {
    if (Err & 0x01) for (int i=0; i<ARRAY_SIZE(CoeffPM.ShZeroRng); ++i) CoeffPM.ShZeroRng[i]=0; // �������� ������
    if (Err & 0x02) 
    {
      for( unsigned i = 0; i < WAVE_LENGTHS_NUM; ++i )
        CoeffPM.PointKalib[i] =  WAVE_LENGTHS[i];
    }
    if (Err & 0x04) 
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefPointKlb); ++i) CoeffPM.CoefPointKlb[i]=1.0;
    if (Err & 0x08) 
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i) CoeffPM.CoefSpctrL[i]=1.0;
    if (Err & 0x10)
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrM); ++i) CoeffPM.CoefSpctrM[i]=1.0;
    if (Err & 0x20)
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrH); ++i) CoeffPM.CoefSpctrH[i]=1.0;
    if (Err & 0x40)
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefStykRange); ++i) CoeffPM.CoefStykRange[i]=1.0;
  }
}
// �������� ������� ��������� ����������
DWORD CheckErrCoeff (void)
{
    DWORD Err = 0;
    DWORD TstErr = 0;
    for (int i=0; i<ARRAY_SIZE(CoeffPM.ShZeroRng); ++i) // �������� ��������
    {
      //TstErr = TstErr<<1;
      if (CoeffPM.ShZeroRng[i]==0) TstErr++;
    }
    if (TstErr >2) Err |= 0x1; // ��� �������� 0 - ������
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i)
          {
            if (CoeffPM.CoefSpctrL[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrL)) Err |= 0x2; // ��� ������ 850 = 1.0 - ������
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrM); ++i)
          {
            if (CoeffPM.CoefSpctrM[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrM)) Err |= 0x4; // ��� ������ 1310 = 1.0 - ������
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrH); ++i)
          {
            if (CoeffPM.CoefSpctrH[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrH)) Err |= 0x8; // ��� ������ 1550 = 1.0 - ������
        TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i)
          {
            if (CoeffPM.CoefStykRange[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefStykRange)) Err |= 0x10; // ����������� = 1.0 - ������

     return Err;       

}

 
DWORD CheckUserGonfig (void)  // �������� ���������������� �������� 
{
  DWORD Err =0;
  BYTE EnaNumSC=0;
  if ((UserSet.CurrLang > MAX_LANG )||((ConfigDevice.ID_Device > 0)&&(UserSet.CurrLang == Rus))) //LANG_NUM-1
  {
    Err |=0x100;
    if ((ConfigDevice.ID_Device > 0)&&(UserSet.CurrLang == Rus)) UserSet.CurrLang = Eng;// ��� ����� ����� ����������
    else UserSet.CurrLang = Rus;
    
  }
  if (UserSet.SubModeRef>1)
  {
    Err |=0x200;
    UserSet.SubModeRef=AUTO;
  }
  if (UserSet.EnIncFiber>1)
  {
    Err |=0x400;
    UserSet.EnIncFiber=MANUAL;
  }
  if (UserSet.CellMem>MaxMemPM)
  {
    Err |=0x800;
    UserSet.CellMem=0;
  }
  if (isnan(UserSet.CurrLvldB) ||(UserSet.CurrLvldB < -100.0) || (UserSet.CurrLvldB > 100.0))
  {
    Err |=0x1000; // ������� �������� ��� �����
    UserSet.CurrLvldB=0.0;
  }
  for (int i=0; i<ARRAY_SIZE(UserSet.LevelRef); ++i)
  {
    if (isnan(UserSet.LevelRef[i])||(UserSet.LevelRef[i]<-100.0)||(UserSet.LevelRef[i]>100.0))
    {
      Err |=0x2000;// 
      UserSet.LevelRef[i]=0.0;
    }
  }
  for (int i=0; i<ARRAY_SIZE(UserSet.BegShiftZone); ++i)
  {
    if ((UserSet.BegShiftZone[i]<=0)||(UserSet.BegShiftZone[i]>600))
    {
      Err |=0x4000;// 
      // ������� �� ������������ �������� �� ������� ��
      if (i<=6) UserSet.BegShiftZone[i]=NumPointsPeriod[i]*8-1;
      // 
      else UserSet.BegShiftZone[i] = 140>>(i-7);
    if (i==10)UserSet.BegShiftZone[i] = 22;
    if (i==11)UserSet.BegShiftZone[i] = 11;
    if (i==12)UserSet.BegShiftZone[i] = 14;
        }
  }
  
  for (int i=0; i<ARRAY_SIZE(UserSet.EnaAutoSetSC); ++i) // �������� ��������� ����������� �������
  {
    if ((UserSet.EnaAutoSetSC[i]!=0)&&(UserSet.EnaAutoSetSC[i]!=1))
    {
      Err |=0x8000;// ��� ���� ����� ���������� !! 19.04.2013
      UserSet.EnaAutoSetSC[i]=1;
    }
    if ((UserSet.EnaAutoSetSC[i]==1) && ConfigDevice.PlaceLS[i])EnaNumSC++;// ���� ���� ����� � ����� ������� ����������
  }
  if (!EnaNumSC) // ��� ����������� ������� -> �������� ��� ���������
  {
  for (int i=0; i<ARRAY_SIZE(UserSet.EnaAutoSetSC); ++i) // �������� ��������� ����������� �������
  {
        if (ConfigDevice.PlaceLS[i])UserSet.EnaAutoSetSC[i]=1;// ���� ���� ����� �� ������ ������� ����������
  }
      Err |=0x8000;// ��� ���� ����� ���������� !! 19.04.2013
  }
  for (int i=0; i<ARRAY_SIZE(UserSet.K_pr_SC); ++i) // �������� ������������� ����������� ���� �� ���������� ������ �������
  {
    if (isnan(UserSet.K_pr_SC[i])||(UserSet.K_pr_SC[i]<1.4)||(UserSet.K_pr_SC[i]>1.6))
    {
      Err |=0x8000;// ��� ���� ����� ���������� !! 19.04.2013
      UserSet.K_pr_SC[i]=1.468;
    }
 
  }
  
  return Err; 
}

BYTE GetEnIncFiber (BYTE Dir)// ��������� - ������ ��������� �������� ���������� ���� ���������� �������� �������
{
  if (Dir)
  {
    if (UserSet.EnIncFiber) UserSet.EnIncFiber=0;
    else UserSet.EnIncFiber = 1;
  }
  return UserSet.EnIncFiber;
}

WORD GetCellMem (BYTE Dir)// ��������� ��������� �������� �������� �������� ���������� ���������
{
    if (Dir)  UserSet.CellMem++; 
    return UserSet.CellMem; 
}

WORD SetupWavePlaceLS (BYTE Place, WORD Data) // Setup num wave for place laser
{
  if (Place>2) Place = 0;
  if ((Data<800)||(Data>1700)) Data = 0;
  GetPlaceLS (NEXT);// ��������� ������� ����� ����� ����� �� ������ ������������� ������� 
  return ConfigDevice.PlaceLS[Place] = Data;
}
// ���������� ������� ��������� ���������� ������ � ����������� �� ������������� ������� � ���������� ������
// 1 ��� - 1 �����, 2��� - ������ �����, 3 ��� - 3 �����, ���������� ������� �� ���������� ���� ���� � �������� ������ �� ���� �� 0
BYTE SetSeqCombLW (void)
{
  const BYTE IniSeq[]={1,2,4,3,5,6,7};
  BYTE SetLSs = ((ConfigDevice.PlaceLS[0])?(1):(0))+((ConfigDevice.PlaceLS[1])?(2):(0))+((ConfigDevice.PlaceLS[2])?(4):(0));
  BYTE ret=0;
  for (int i=0;i<7;i++)
  {
    g_SeqCombLW[i]=0;
    if(IniSeq[i] == (IniSeq[i] & SetLSs)) g_SeqCombLW[ret++] = IniSeq[i];
  }
  return ret;
}
// ��������� ���������� ������� ������ �� �������
BYTE GetCombLS (BYTE Index)
{
  return g_SeqCombLW[Index];
}

BYTE SetTypeDevice (BYTE Data) // ��������� ���� ������� ��� �������
{
  if (Data >9) Data = 2;
  GetPlaceLS (NEXT);// ��������� ������� ����� ����� ����� �� ������ ������������� ������� 
  return ConfigDevice.TypeDevice= Data;
}

void SetPlaceLS (BYTE SetPlace) // ��������� ���������� ������ 
{
  SettingRefl.SW_LW = SetPlace;
}

BYTE GetPlaceLS (BYTE Dir)// ��������� ������� ����� ����� ����� �� ������ ������������� ������� CURRENT -�������, NEW- ����� (���������)
{
  BYTE SetPlace = SettingRefl.SW_LW;
  if (Dir)
  {
    if (SetPlace<LSPLACENUM-1) SetPlace++;
    else SetPlace = 0;
  }
    if (ConfigDevice.PlaceLS[SetPlace] == 0)
    {
      if (SetPlace<LSPLACENUM-1) SetPlace++;
      else SetPlace = 0;
    }
    if (ConfigDevice.PlaceLS[SetPlace] == 0)
    {
      if (SetPlace<LSPLACENUM-1) SetPlace++;
      else SetPlace = 0;
    }
  
  SettingRefl.SW_LW = SetPlace;
  // ��������� LSEL �� SetPlace
  LSEL0((SetPlace+1) & 0x01);
  LSEL1(((SetPlace+1) & 0x02)>>1);
  // ��������� ����� ����������� � ����������� �� ��������� ����� �����
  //SettingRefl.K_pr = UserSet.K_pr_SC[SetPlace];
  return SetPlace;
}

WORD GetLengthWaveLS (BYTE SetPlace) // ��������� ����� ����� �� ������� ������������� �����
{
    return ConfigDevice.PlaceLS[SetPlace];
}

BYTE GetIndexCurLengthWave (WORD LengthWave)// ��������� ������� ����� ����� � ����������� �� �������� ����� �����
{
  BYTE data= 0;
  switch (LengthWave)
  {
case 850:
  data = 0;
  break;
case 1310:
  data = 1;
  break;
case 1490:
  data = 2;
  break;
case 1550:
  data = 3;
  break;
case 1625:
  data = 4;
  break;
default:
  data = 1;
  break;
  
  }
  return data;
}

// ��������� ��������� �������� ��������������� ������ ����� ����� ���������
BYTE GetSetModeLW (int a)
{
  static volatile BYTE ModeLW; 
  if (a<0) ModeLW = 0; // ������� �����
  if (a>0) ModeLW = 1;
  return ModeLW;
}
// ��������� ��������� ������� ������� �����???
BYTE GetSetNumLS (BYTE a)
{
  static BYTE NumLS;
  if (a)
  {
    if (a>3) NumLS=1;
    else NumLS = a;
  }
  return NumLS;
}

float GetLSACoef (void) // ��������� �����. ������� ����� �� ������������� ����������
{
  return LSACoef[GetIndexCurLengthWave(GetLengthWaveLS(GetPlaceLS(CURRENT)))][GetIndexLN()];
}

BYTE GetSubModRefl (void) // ��������� ������ ������ �������������
{
  return UserSet.SubModeRef;
}
void SetSubModRefl (BYTE Mode) // ��������� ������ �������������
{
  if (Mode)
    UserSet.SubModeRef = AUTO;
  else
    UserSet.SubModeRef = MANUAL;
}
// ���������� ������ �����
BYTE GetIndexLN (void) // ��������� ������� ����� �����
{
  return SettingRefl.Index_Ln;
}

BYTE SetIndexLN (BYTE Index) // ��������� ������� ����� �����
{
  if (Index>LENGTH_LINE_NUM-1) Index=LENGTH_LINE_NUM-1;
  if (Index==0xFF) Index=0;
    SettingRefl.Index_Ln = Index;
    if ((Index == 5) && (GetIndexIM()>5)) SetIndexIM (5);
    if ((Index == 1) && (GetIndexIM()>4)) SetIndexIM (4);
          SetIndexIM(GetIndexIM()); // ����������������� ������� ���� � ����������� �� �������
return Index;
}

WORD GetLengthLine (BYTE Index) // ��������� ����� ����� �� �������
{
  return LengthLine [Index];
}
// ���������� ������ ��������
BYTE GetIndexIM (void) // ��������� ������� ����� Pulse
{
  return SettingRefl.Index_Im;
}

void SetIndexIM (BYTE Index) // ��������� ������� ����� Pulse
{
  if (Index==0xFF) Index=0;
  BYTE LineInx = GetIndexLN ();
  switch (LineInx) //����� �� ��������� ��������� ��������
  {
  case 0: //2km
  case 1: //4km
    if (Index>WIDTH_PULSE_NUM-4)Index = WIDTH_PULSE_NUM-4; //������� ����������� �� 1000�� (3) ��� 20000
    break;
  case 2: //8km
  case 3: //16km
  case 4: //32km
  case 5: //64km
    if (Index>WIDTH_PULSE_NUM-3)Index = WIDTH_PULSE_NUM-3; // ������ ����������� �� 3000�� (2) ��� 20000
    
    break;
  case 6: //128km
    if (Index>WIDTH_PULSE_NUM-1)Index = WIDTH_PULSE_NUM-1; // ������ ����������� �� ������������
    break;
  default:
    Index=0;
    break;
  }
  SW_FLTR(ON);
  if (Index <5)
  {
    SW_FLTR(OFF);
    CurrentBegShiftZone = UserSet.BegShiftZone[LineInx] ;
  }
  if (Index == 5) CurrentBegShiftZone = UserSet.BegShiftZone[5+LineInx] ;// 3000 ns
  if (Index == 6) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//10mks for 128km
  if (Index == 7) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//20mks for 128km
    
  SettingRefl.Index_Im = Index;
}

void SetIndexShadowIM (BYTE Index) // ��������� ������� ���� ��� �������� ��������
{
    BYTE LineInx = GetIndexLN ();
  SW_FLTR(ON);

  if (Index == 5) CurrentBegShiftZone = UserSet.BegShiftZone[5+LineInx] ;// 3000 ns
  if (Index == 6) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//10mks for 128km
  if (Index == 7) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//20mks for 128km

}

WORD GetWidthPulse (BYTE Index) // ��������� ����� Pulse �� �������
{
  return WidthPulse [ConfigDevice.ApdiSet][Index];
}
// ���������� �������� ���������
BYTE GetIndexVRM (void) // ��������� ������� ������ ����������
{
  return SettingRefl.Index_Vrm;
}

void SetIndexVRM (BYTE Index) // ��������� ������� ������� ����������
{
    if (Index==0xFF) Index=TIME_AVR_NUM-1;
  if (Index>TIME_AVR_NUM-1)Index=0;
    SettingRefl.Index_Vrm = Index;
}

WORD GetTimeAvrg (BYTE Index) // ��������� ������� ���������� �� �������
{
  return  TimeAver[Index];
}
// ���������� �����. �����������
float GetIndexWAV (void) // ��������� ������� �����������
{
  return SettingRefl.K_pr;
}
// ��������� ������� ����������� � ����������� �� ����� ������
float GetWAV_SC(BYTE Index)
{
  if (Index>2) Index = 0;
  return UserSet.K_pr_SC[Index];
  
}
// ���������� �������� ����� � ������������� ����� (��� ����������� ����������)
void ReSaveWAV_SC (void)
{
        if (SettingRefl.K_pr != UserSet.K_pr_SC[GetPlaceLS(CURRENT)])
        {
        UserSet.K_pr_SC[GetPlaceLS(CURRENT)] = SettingRefl.K_pr;
        //SSPInit_Any(MEM_FL1); // ������������� SSP ��� ������ FLASH 
        //SaveUserConfig();
        EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));

        }
}


float SetWAV_SC(float Data, BYTE Index)// ������ ����������� ����� � ���������
{
  if (Index>2) Index = 0;
  if (Data<1.4) Data = 1.4;
  if (Data>1.6) Data = 1.6;
    UserSet.K_pr_SC[Index] = Data;
  return Data;
  
}
void SetIndexWAV (float Data) // ��������� ������� �����������
{
  if (Data<1.4) Data = 1.4;
  if (Data>1.6) Data = 1.6;
    SettingRefl.K_pr = Data;
}
// ���������� �����������
BYTE GetCfgPM (void) // ��������� ��������� ����������
{
  return ConfigDevice.CfgPM;
}
// ���������� �����������
BYTE SetCfgPM (BYTE Data) // Setup ��������� ����������
{
  if (Data >2) Data = 0;
  return ConfigDevice.CfgPM = Data;
}
 
unsigned GetTimeLight (BYTE Index) // ��������� ������������ ��������� �� �������
{
  if (Index>TIME_LIGHT_NUM-1)
  {
  IndBkLight = 0;
  Index = 0;
  }
  return TimeLight[Index];
}

BYTE SetIndexLight (int Dir) // ���������(���������) ������� ������������ ��������� 0-�������, 1-����������, -1-����������
{
  if (Dir == 0) return IndBkLight;
  if (Dir < 0)
  {
    if (IndBkLight>0)IndBkLight--;
    else IndBkLight = TIME_LIGHT_NUM-1;
  }
  if (Dir > 0)
  {
    if (IndBkLight<TIME_LIGHT_NUM-1)IndBkLight++;
    else IndBkLight = 0;
  }
    return IndBkLight;
}

BYTE GetCfgRE (void)// ��������� �������� ��������� �������� �����
{
  return ConfigDevice.CfgRE;
}
BYTE SetCfgRE (BYTE Data)// Setup �������� ��������� �������� �����
{
  if (Data > 1) Data = 0;
  return ConfigDevice.CfgRE = Data;
}

BYTE PreSetModeLS (BYTE Index) // �������������� ��������� ������ ���������
{
  
  if (Index > MODELSNUM-1) ModeLS = 0;
  else ModeLS = Index;
  return ModeLS;
}



BYTE GetModeLS (void) // �������� ����� ���������
{
  return ModeLS;

}

BYTE SetModeLS (char* Str, int Dir, BYTE Lang) // ���������� ����� ������ ���������, � ����� ������ ��� ����������� �� ����������
{
  //char s[8];
    //if (Dir == 0) return ModeLS;
    if (Dir < 0)
    {
      if (ModeLS > 0) ModeLS--;
      else ModeLS = MODELSNUM-1;
    }
    if (Dir > 0)
    {
      if (ModeLS < MODELSNUM-1) ModeLS++;
      else ModeLS = 0;
    }
    switch (ModeLS)
    {
    case 0:
      sprintf (Str,"%s",MsgMass[3][Lang] ); // ��������
      break;
    case 1:
      sprintf (Str,"CW"); // CW
      break;
    case 2:
      sprintf (Str,"%s",MsgMass[80][Lang]); // 270Hz
      break;
    case 3:
      sprintf (Str,"%s",MsgMass[81][Lang]) ; // 2kHz
      break;
    case 4:
      sprintf (Str,"CW*"); // CW*
      break;
    }
    if (Dir) // ���� ��������� ��������� ����������
    {
      SetupSource (ModeLS); // ��������� ������ ������ ������
    }
      
return ModeLS;
}

BYTE SetModeRE (char* Str, int Dir, BYTE Lang) // ���������� ����� ������ VFL, � ����� ������ ��� ����������� �� ����������
{
  //char s[8];
 //   static volatile BYTE ModeRE = 0; // ����� ���������
    //if (Dir == 0) return ModeLS;
    if (Dir < 0)
    {
      if (ModeRE > 0) ModeRE--;
      else ModeRE = MODERENUM-1;
    }
    if (Dir > 0)
    {
      if (ModeRE < MODERENUM-1) ModeRE++;
      else ModeRE = 0;
    }
    switch (ModeRE)
    {
    case 0:
      sprintf (Str,"%s",MsgMass[3][Lang] ); // ��������
      break;
    case 1:
      sprintf (Str,"%s",MsgMass[69][Lang] ); // ���������
      break;
    case 2:
      sprintf (Str,"%s",MsgMass[82][Lang]); // 1Hz
      break;
    }
return ModeRE;
}

void CntrlRE (void) // ������� �������� ������� ������
{
  static DWORD CntRE = 0;
  static DWORD CntREOld = 0;
  static DWORD SostRed = 0;// ��������� �������� �����
  
    switch (ModeRE)
    {
    case 1:
      REDEYE(1); // �������
      CntRE = 0;
      break;
    case 2: // ����� �������� ������� ����
      CntRE = HAL_GetTick();
      if(CntRE>(CntREOld+TIMERE))
      //if((GetSysTime()%TIMERE) == 0)
      {
        if(SostRed) SostRed = 0;
        else  SostRed = 1;
        CntREOld = CntRE;
        REDEYE(SostRed);
      }

        
//      if (CntRE<TIMERE) CntRE++;
//      else CntRE = 0;
//      if (CntRE<TIMERE/2)
//        REDEYE(1);
//      else
//        REDEYE(0);
      break;
    default:
      REDEYE(0); // ��������
      CntRE = 0;
      break;
      
    }
  
  
}
BYTE GetModeRE(void) // ���������� ������� �������� ������ �������� �����
{
 return ModeRE;
 
}

BYTE OffModeRE(void) // ��������� ������� �����
{
  ModeRE =0;
 return ModeRE;
 
}


void InitReflSet (void) // ������������� ��������� �������������
{
  // ������������� ������ ���������� ��������� �������
  SetSeqCombLW();
  
  SettingRefl.Index_Ln=0; // ������ �������� ����� ����� � ���������������� ������
  SettingRefl.Index_Im=0; // ������ �������� ������������ �������� 
  SettingRefl.Index_Vrm=0; // ������ �������� ������� ������ 
  SettingRefl.Index_Comm=0;  // �������������� ������� � ������������ 
  SettingRefl.SW_LW=0;  // ������� ������ ����� ������ (����������� ����� ����� ������)
  SettingRefl.K_pr=1.4680; // ����� ����������� 
  SettingRefl.SubModeRef = AUTO; // ����� �������������
  SettingRefl.LogNoise = 65535; // ������� ���������������� ����,
  g_SetModeLW = 0; // ���������� �� ������ ��������� ������������, ��������� �� ��� ��������� �� ������������� �������
}

void SetLogNoise (unsigned short Data)// ������� ���������������� ����, ���������
{
  LogarifmNoise = Data; // ������� ���������������� ����,
}
void SaveLogNoise (void) // ��������� ������� ���������������� ���� ����� ���������
{
  SettingRefl.LogNoise = LogarifmNoise;
}


DWORD GetEndCheckLvl (void) // ������� ������ ��� �������� ����� �����
{
  return ConfigDevice.EndCheckLvl;
}

DWORD GetCurrentBegShiftZone (void) //��������� �������� �������� �� �������
{
return CurrentBegShiftZone;
}

DWORD SetBegShiftZone (BYTE Index, DWORD Data)// ������ ������� ���� �� �������
{
  
  if (Index<ARRAY_SIZE(UserSet.BegShiftZone)) // 
  {
  if (Data > 600) Data = 0;
  {
    UserSet.BegShiftZone[Index]=Data;
  }
  }
  else 
    Index = 0;
  return UserSet.BegShiftZone[Index];
}

DWORD GetBegShiftZone (BYTE Index)// ������ ������� ���� �� �������
{
    return UserSet.BegShiftZone[Index];
}

BYTE SetIndexCommOTDR (BYTE Index) // ��������� ������� ����������� � ��������� ��� ����������
{
  if (Index>19) Index=0;
  SettingRefl.Index_Comm = Index;
  return  SettingRefl.Index_Comm;
}

void CheckCommOTDR (void) // �������� � ������������� ������ ������������ OTDR
{
  // ��������������� ����!!! 
//  int Indx;
//    // ���������� ������ ����������� ��� ��������
//    IndexCommOTDR = 0;
//    CommentsOTDR[ARRAY_SIZE(CommentsOTDR)-1]=0; // ��������� ������� � ������� ����� 0
//    for (Indx =ARRAY_SIZE(CommentsOTDR)-2; Indx>=0; Indx--) // ���� � ����������
//    {
//      if (CommentsOTDR[Indx]<0x20) CommentsOTDR[Indx]=' '; //���� �����������, �� ������ �� "��������"
//      else if (CommentsOTDR[Indx]!=' ' && IndexCommOTDR == 0)IndexCommOTDR = Indx; // ��������� ����� ������ �� ������� ��"������"
//      //Index_Comm --;
//    }
//    if ((CommentsOTDR[IndexCommOTDR]!=' ')&&(IndexCommOTDR!=18))IndexCommOTDR ++;// ���� ��������� �� �� ������ � ��� �� ���������, �� ����������� ���������
//  
//  Indx = IndexCommOTDR;
//  
//  for (char i=0; i<Indx; i++)
//  {
//    if (CommentsOTDR[i] == 0xa7)
//    {
//      Indx=i;
//      break;
//    }
//  }
//  IndexCommOTDR = Indx;
//  SetIndexCommOTDR (Indx);
//  for(int i=IndexCommOTDR+1; i<ARRAY_SIZE(CommentsOTDR); i++) CommentsOTDR[i]=' ';
//  CommentsOTDR[ARRAY_SIZE(CommentsOTDR)-1]=0x0;
}

BYTE GetApdiSet (void) //��������� �������� ������� ��������� ���������
{
  return ConfigDevice.ApdiSet;
}
BYTE SetupApdiSet (BYTE Data) //Setup �������� ������� ��������� ���������
{
  if (Data>1) Data = 0;
  return ConfigDevice.ApdiSet = Data;
}


BYTE CheckSavedTrace(void) // ������� �������� ����������� �������������� 
{
  BYTE Err = 0;
  if (SettingRefl.Index_Ln > LENGTH_LINE_NUM -1) Err++;
  if (SettingRefl.Index_Im > WIDTH_PULSE_NUM -1) Err++;
  if (SettingRefl.Index_Vrm > TIME_AVR_NUM -1) Err++;
  if (SettingRefl.Index_Comm > ARRAY_SIZE(CommentsOTDR) -1) Err++;
  if (SettingRefl.SW_LW > 2) Err++;
  if (SettingRefl.SubModeRef > 1) Err++;
  if ((SettingRefl.K_pr > 1.6)||(SettingRefl.K_pr < 1.4)) Err++;
  if (mg_SetModeLW > 7) mg_SetModeLW=0; // �� ����� ���� ������������ ������ 7
  if (Err)
  {
    InitReflSet (); // ������������� ��������� �������������
    GetPlaceLS (NEXT); // ������������� �������� ������������ �����
  }
  return Err;
}

void SetNumAverag (unsigned Data) // ������ ����� ���������� � ������ �������������
{
  SettingRefl.NumAvrag = Data;
}
unsigned GetNumAverag (void) // ��������� ����� ���������� ��������������
{
  return SettingRefl.NumAvrag;
}
// ��������� ������� ����� ����� ����������, � ����������� �� ������� ����� ����� ( � �������� ��� 1300 � 1650)
int GetIndxPON (int Lambda)
{
  int i=0;
    // ����� �� ������� � 5 ������� ���� ���������� 1300
  if(Lambda<=900)
    i = 0; 
  else if(Lambda<=1310)
    i = 1;
  else if(Lambda<=1490)
    i = 2;
  else if(Lambda<=1550)
    i = 3;
  else 
    i = 4;
  return i;
}
//���� �.�.
float GetCurrLvldB(int Lambda)          // ���������� ������� ������� �������� ���
                                        // ��������  ������� ������� �������� � ����������� �� ����� �����
{ 
  BYTE i=0;     // ������
  
  if((Lambda<800) || ((Lambda>900)&&(Lambda<1210)) || (Lambda>1650)) // ���� ������� �������� ����� ����� 
    return UserSet.CurrLvldB;
  
  for(i=0;i<WAVE_LENGTHS_NUM;i++)
    if(Lambda==CoeffPM.PointKalib[i])
    {      
      UserSet.CurrLvldB=UserSet.LevelRef[i];
      SetPMWavelenght(Lambda); // ������������� ��� �������
      return UserSet.CurrLvldB;
    }
  // ����� �� ������� � 5 ������� ���� ���������� 1300
  UserSet.CurrLvldB=UserSet.LevelRef[GetIndxPON(Lambda)];
  SetPMWavelenght(Lambda); // ������������� ��� �������
  return UserSet.CurrLvldB;
 
}

float SetCurrLvldB(int Lambda, float LvldBm)  // ������������� ������� LvldBm � �������� ��������
{ 
  BYTE i=0;     // ������
  
  if((Lambda<800) || ((Lambda>900)&&(Lambda<1210)) || (Lambda>1650)) // ���� ������� �������� ����� ����� 
    return UserSet.CurrLvldB;
  
  UserSet.CurrLvldB   = LvldBm;
  for(i=0;i<WAVE_LENGTHS_NUM;i++)
    if(Lambda==CoeffPM.PointKalib[i])
      UserSet.LevelRef[i] = UserSet.CurrLvldB;
  // ����� �� ������� � 5 ������� ���� ���������� 1300
  UserSet.LevelRef[GetIndxPON(Lambda)] = UserSet.CurrLvldB;
  
  EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));
  
  return UserSet.CurrLvldB;
  
}

// ���������� ����� ����� � �������� �� ���������� ���������
// ��������� 10 � (100��) ��� ��������� ������������ ��������
// � ���������� ���������

unsigned short PointsInImpulse (int Real)
{
  unsigned short Data;
  if (Real)
  Data = (unsigned short)(((GetWidthPulse(GetIndexIM()))*NumPointsPeriod[GetIndexLN()])/ADCPeriod +1);
  else
  {
  // ��������� 100 �� ����� ����� �������
  Data = (unsigned short)(((GetWidthPulse(GetIndexIM())+((GetIndexIM()>2)?(0):(50)))*NumPointsPeriod[GetIndexLN()])/ADCPeriod);
  if (Data<4) Data = 4;
  } 
  return Data;
}

// ���������� �������� ��� ��� ������������� ������������ ��������

float TauImp (void)
{
  WORD Tau;
  float Data;
    if ((GetIndexIM()>4)&&(ConfigDevice.ApdiSet)) Tau = TAUFLTR; // 820nc
    else 
    {
      if (ConfigDevice.ApdiSet)  Tau = TAUARX;  // 5ns
      else Tau = TAUARN; // 10ns ������
    }
    Data = ADCPeriod/NumPointsPeriod[GetIndexLN()];
    Data = -1.0*Data/(float)Tau;
    return Data;
}
// ������� ��������� ���������� ������� �������������
unsigned short GetSet_BC (float Dir)// ���������� ��������� ���������
{
  if (Dir!=0.0)
  {
     ReflParam.BC +=(int short)(Dir*10);
  if ((ReflParam.BC>850)||(ReflParam.BC<450)) { ReflParam.BC = 850;}
 
  }
  return  ReflParam.BC;
}

unsigned short GetSet_LT (float Dir)// ��������� ������� �������� �������
{
  if (Dir!=0.0)
  {
    if(Dir<0.0)
    {
      if(ReflParam.LT>=(unsigned short)(10.0-Dir*100))
           ReflParam.LT +=(unsigned short)(Dir*100);
      else
        ReflParam.LT = 5000; //5dB max
    }
    else // ������ ����
    {
      if(ReflParam.LT<=(unsigned short)(5000-Dir*100))
           ReflParam.LT +=(unsigned short)(Dir*100);
      else
        ReflParam.LT = 50; //0,05dB min
      
    }
  if (ReflParam.LT>ReflParam.ET) { ReflParam.LT = ReflParam.ET;}
   
  }
  return  ReflParam.LT;
}

unsigned short GetSet_ET (float Dir) // ����� ����� �� ����� 20 ��
{
  if (Dir!=0.0)
  {
    if(Dir<0.0)
    {
      if(ReflParam.ET>=(unsigned short)(100.0-Dir*1000))
           ReflParam.ET +=(unsigned short)(Dir*1000);
      else
        ReflParam.ET = 20000; //20dB
    }
    else // ������ ����
    {
      if(ReflParam.ET<=(unsigned short)(20000-Dir*1000))
           ReflParam.ET +=(unsigned short)(Dir*1000);
      else
        ReflParam.ET = 100; //0,1dB
      
    }
  }
  return  ReflParam.ET;
}

unsigned short GetSet_RT (float Dir) // ��������� �� ����� -70��
{
  volatile int short Delta = (int short)(Dir*1000);
  if (Delta!=0)
  {
    if(Delta<0)
    {
      if(ReflParam.RT>=(10000-Delta))
           ReflParam.RT +=Delta;
      else
        ReflParam.RT = 65000; //10dB
    }
    else // ������ ����
    {
      if(ReflParam.RT<=(65000-Delta))
           ReflParam.RT +=Delta;
      else
        ReflParam.RT = 10000; //70dB
      
    }
  }
  return  ReflParam.RT;
}

// ������ ����� ����� �������� ��� ��������� �������
unsigned short GetPointsShift (void)
{
  WORD Tau;
  unsigned short Data;
  if ((GetIndexIM()>4)&&(ConfigDevice.ApdiSet)) Tau = TAUFLTR; // 820nc
  else Tau = TAUWFLTR; //70��
  if (GetWidthPulse(GetIndexIM())<Tau) Tau = GetWidthPulse(GetIndexIM());
  Data = (unsigned short)((Tau*NumPointsPeriod[GetIndexLN()])/ADCPeriod +1);
  return Data;
}


void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents) // ��������� ����� �������
{
  char Str[10];
  
  unsigned short DataShort ; // 
  unsigned long DataInt ;
  
      // ��� ������ ����������������� ��������� ������� (������� ������������� ����� ����� �� ������� ��������)
      switch (Block)
      {
      case 0: //0...55 byte
//        // ������ ���������� ���� ��������� ����
//      if (GetNumTrace()) // ���� �� ������� �� ������ �� �������
//        TraceREAD(GetNumTraceSaved(GetNumTrace()));//  ������ ���� ������� ���� ��������// 27/01/2011 ����������� ��������� ��������������
//      else  TraceREAD(0);
      memcpy( Name, HeaderBelcore, 56 );
        if (NumEvents)
        {
          //memcpy( &Name[5], 7, 1);
          Name[2]=0x62; // ������ ������ ������� Map ����� +16
          Name[6]=7; // ������ ������ ���������� ����� � ��� �����
          memcpy( &Name[56], &HeaderBelcore[225], 16 ); // �������� ���� ������� � ��� ����
          DataShort = (unsigned short)(NumEvents*32+24); // ��������� ������� ����� � ����������� �� ����� �������
          memcpy( &Name[68], &DataShort, 2);
          
        }
        break;
      case 1: //56...117 (72...133) 62 byte
      memcpy( Name, &HeaderBelcore[56], 62 );
        // ������������� ����� �����
      DataShort = (unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT)); // ��������� ����� ����� �� ������� ������������� �����
      // ���������� ������ ����� 1300 ��� �4 ��������!
      
      memcpy( &Name[86-56], &DataShort, 2);
      // ��������� Comments
      memcpy( &Name[98-56], CommentsOTDR, 19);
        break;
      case 2: //118...211 (134...227) 94 byte
      memcpy( Name, &HeaderBelcore[118], 94 );
      // ��������� �������������
      memcpy( &Name[118-118], IdnsBC[ConfigDevice.ID_Device], 14 );
      GetMFID( Str ); // for Belcore
      // Set MDIF
      memcpy( &Name[133-118], Str, 12 );
      // Number Device
      sprintf (Str,"%04d",ConfigDevice.NumDevice);
      memcpy( &Name[146-118], Str, 4 );
      // ����� ���������
      GetNumVer(Str);
      memcpy( &Name[153-118], Str, 4 );
      
      // ����� ���������� �������������� 
      // ������������� ���� ����������� ��������������
      DataInt = TotalSec (TimeSaveOTDR);
      memcpy( &Name[159-118], &DataInt, 4);
      // (164)AW ����� ����� � 0.1 �� (1310�� = 13100) 0x332c 0x3c8c (164)
      DataShort = (unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT))*10;
      memcpy( &Name[165-118], &DataShort, 2);
      // ����� ������������ �������� 
      DataShort = GetWidthPulse(GetIndexIM());
      memcpy( &Name[173-118], &DataShort, 2);
      // ������������� DS
      // ((10000points*10(in 1ns 100ps))/2 = 50000 , 333.333 ns - �������� ����� ����������
      DataInt = (unsigned long)((ADCPeriod*50000)/NumPointsPeriod[GetIndexLN()]); //  ������������� �������� DS ��� �������������� ������ ���������
      memcpy( &Name[175-118], &DataInt, 4);
      // ###(182) GI ����� �����������  146583 (1.46583)  
      DataInt = (unsigned long)( GetIndexWAV()*100000);
      memcpy( &Name[183-118], &DataInt, 4);
      // ###(186) BC = 800 ������ ��������� ���������
      DataInt =  ReflParam.BC;
      memcpy( &Name[187-118], &DataInt, 2);
      // ###(188) NAV ����� ���������� - ������� �� ������� ���������� � ����� ����� = FinAvrg
      DataInt = SettingRefl.NumAvrag;
      memcpy( &Name[189-118], &DataInt, 4);
      // ###(192) AR  ����� ����������� ������� (����� ����� ��������� �� ���) DS*NPPW/10000
      DataInt = (unsigned long)((ADCPeriod*5*4096)/NumPointsPeriod[GetIndexLN()]); //  ������������� �������� DS ��� �������������� ������ ���������
      memcpy( &Name[193-118], &DataInt, 4);
      // ###(200) NF ������ ������� ����� ����� 65535
      DataInt =  ReflParam.NF;
      memcpy( &Name[201-118], &DataInt, 2);
      // ###(206) LT ����������� �������� ��������� ��� ������� 200 � 0.001 ��
      DataInt =  ReflParam.LT;
      memcpy( &Name[207-118], &DataInt, 2);
      // ###(208) RT ����������� �������� ��������� ��� ������� 40000 � -0.001��
      DataInt =  ReflParam.RT;
      memcpy( &Name[209-118], &DataInt, 2);
      // ###(210) ET ����������� ��������� ��� ����� ����� 3000 � 0.001 ��
      DataInt =  ReflParam.ET;
      memcpy( &Name[211-118], &DataInt, 2);
        break;
      case 3: //212...223 (228...239) 12 byte
      memcpy( Name, &HeaderBelcore[213], 12 );
       break; 
      }

}
// ���������� �������� ���������� (�������� CW � FW)-
void  SetupSource (BYTE ModeLS) // ��������� ������ ������ ������
{
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_PE);
  
  //SWITCH_LW(); // ��������� ��������� ����� ����� ��������� (���������� �������)
//  const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};
  switch(ModeLS)
  {
  case 0: // ��������� �������� � ������������� ������ ��������
    PHLD(0);
    HAL_UART_DMAStop(&huart5);
    LED_START(OFF);
    break;
  case 1: // �������� �������� � ������������� ������ ��������
    PHLD(1);
    HAL_UART_DMAStop(&huart5);
    LED_START(ON); //
    break;
  case 2: // ��������� �������� � ������������� UART � ��������� ��������� ������� 275 Hz
    PHLD(0);
    huart5.Init.BaudRate = 550;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ���� �������� ������� ����� �� �������� ����
    //Dummy = huart2.Instance->DR ; // ������ ������ ������ 
    
    HAL_UART_Transmit_DMA(&huart5, (void*)TxGenOpt,strlen((void*)TxGenOpt)); // ������ 
    LED_START(ON);
    break;
  case 3: // ��������� �������� � � ������������� UART � ��������� ��������� ������� 2 kHz
    PHLD(0);
    huart5.Init.BaudRate = 4000;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ���� �������� ������� ����� �� �������� ����
    //Dummy = huart2.Instance->DR ; // ������ ������ ������ 
    
    HAL_UART_Transmit_DMA(&huart5, (void*)TxGenOpt,strlen((void*)TxGenOpt)); // ������ 
    LED_START(ON);
    break;
  case 4: // ��������� �������� � ������������� UART �� �������� 1200
    PHLD(0);
    huart5.Init.BaudRate = 1200;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ���� �������� ������� ����� �� �������� ����
    //Dummy = huart2.Instance->DR ; // ������ ������ ������ 
    //    HAL_UART_DMAStop(&huart2);
    TimeSCWP = 70; // 3.5sec
    LED_START(ON);
    break;
  default:
    PHLD(0);
    HAL_UART_DMAStop(&huart5);
    LED_START(OFF);
    break;
  }
}
//
// ������� ������������ ������� �� ����� � ���������� ���������� � ����
void Run_SCWP(void) // ������ 50 ��
{
  if(TimeSCWP++<70)
  {
    switch(TimeSCWP)
    {
    case 2:
      sprintf((char*)StrTXOpt, "UUUU####SL%4u %.2f\rUUU",GetLengthWaveLS(GetPlaceLS (CURRENT)),-30.0);//UUU ooo1300
      HAL_UART_Transmit_DMA(&huart5, (void*)StrTXOpt,strlen((void*)StrTXOpt)); // ������ 
      TxOptBusy = 1;
      //TsT(1);// ������ ����� ��� ��������
      //  HAL_GPIO_WritePin(test_GPIO_Port, test_Pin, GPIO_PIN_SET); 

      //HAL_Delay(200);
      break;
    case 10: // ����� 500 �� ����� ������ ��������... 
      PHLD(1); // on CW 
      break;
    default:
      break;
    }
  }
  else
  {
    TimeSCWP = 0; // ��� �������
    PHLD(0); // off CW
    // ����������� ������
      //  GetSetLW_SC (1, 0) ; // ��� ����� ��� �������
    GetPlaceLS (NEXT);
    //    GetSetLW_SC (1, 1) ; // � ��� ������ �� ����� ������


    //NeedReDraw = 1;
  }
}


unsigned short CalkCheckSum (void)// ������� ����������� ����� ������������ �������
{
  unsigned short Sum =0;
  unsigned char *adress = StructPtr(CFG_DEVICE);
  for (int i=0; i<StructSize(CFG_DEVICE);++i)
  {
    Sum = Sum + *adress++;
  }
  adress = StructPtr(COEF_PM);
  for (int i=0; i<StructSize(COEF_PM);++i)
  {
    Sum = Sum + *adress++;
  }
  
  Sum = 0x2A56;
  return Sum;
}


void GetNumVer (char* Str) // ��������� ������ ������
{
  char out[5];
  strcpy( out, NumVer );//, strlen(Ides[device->ID_Device])
  out[4] = 0;
  int Num = atoi (out);
  //Num=Num/5;
  sprintf (Str, "5%c%c%c", (BYTE)(0x30+(Num/260)), (BYTE)(0x30+(Num/26)), (BYTE)(0x61+(Num%26)));
}

void SetNewLeveldB (float RPON, int i) // ��������� ������ ������ �������� �� �������� ������
{
  UserSet.LevelRef[i] = RPON + UserSet.LevelRef[i] ;   
  //UserSet.CurrLvldB = UserSet.LevelRef[i] ;      
}

void DeletedAllCell (void) // �������� ���� ������� ����������
{
  
  UserSet.CellMem = 0;
//  SSPInit_Any(MEM_FL1); // ������������� SSP ��� ���������� FLASH (���� 1 �� ��� �� ����� ����������)
//  FlashErasePage(CFG_USER); // ������ �������� ��������� ������������ �������
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
  EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));
 
}
//


