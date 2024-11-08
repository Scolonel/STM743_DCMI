// структура пользовательских установок
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
//const char *Device[2][LANG_NUM] = {{"ТОПАЗ-7\0","TOPAZ-7\0","TOPAZ-7\0"},{"MOT-700\0","MOT-700\0","MOT-700\0"}};
const char *Device[2][2][2] = {{{"ТОПАЗ-7\0","ТОПАЗ-9\0"},{"TOPAZ-7\0","TOPAZ-9\0"}},{{"MOT-700\0","MOT-950\0"},{"MOT-700\0","MOT-950\0"}}};

const char *PMset[6] = {"10\0","31\0","32\0","-PMH\0","-VFL\0","AR\0"};
const DWORD NumPointsPeriod[LENGTH_LINE_NUM]= {48,24,12,6,3,2,1};// число точек на период
const DWORD LengthLine[LENGTH_LINE_NUM]= {2,4,8,16,32,64,128};
const DWORD DelayPeriod[LENGTH_LINE_NUM]= {700,1200,4000,8000,10000,0,0};// задержка периода в тиках CreatDelay()~ 83.33 нс
const WORD WidthPulse[2][WIDTH_PULSE_NUM]= {{20,40,150,500,1000,3000,10000,20000},{10,40,150,500,1000,3000,10000,20000}};
const DWORD TimeAver[TIME_AVR_NUM]= {15,30,60,180,3,600};
const unsigned TimeLight[TIME_LIGHT_NUM] = {0,15,30};
const char *IdnsBC[2]= {"SvyazServis   \0","OPTOKON Co.Ltd\0"};
const char *MfidBC[2][2]= {{"TOPAZ7\0","TOPAZ9\0"},{"MOT700\0","MOT950\0"}};
// таблица расчета поправки на наклон для измеряемой линии с установленными параметрами
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

// структура параметров рефлектограммы ( текущего измерения)
TR_PARAM ReflParam;
// структура параметров поддержки JDSU
JDSU_COM SetJDSU;
// структура альтернативного имени
DB_NAME_ENA NameDB;
// структура спектральных коэфф
TAB_SET CoeffPM;
// структура настройки рефлектометра ( текущего измерения)
 REFL_SET SettingRefl;
//// структура конфигурации прибора
CFG_DEV ConfigDevice;
//// структура пользоваетьских установок
CFG_USR UserSet;
//typedef struct
//{
//  WORD NumDevice;  // (999) № прибора,
//  WORD PlaceLS[LSPLACENUM]; //  Место 1, (1310) Конфигурация установленных лазеров по местам
//  BYTE TypeDevice; // (5)  Признак в типе прибора только для ТОПАЗ (последняя цифра)
//  BYTE CfgPM;       //	Признак измерителя (0-нет,1- простой(-70..+6), 2- расширенный (-55..+20)
//  BYTE CfgRE;       //	Признак красного глаза (0-нет, 1-есть)
//  BYTE ApdiSet;     //	Признак лавинного диода (0-нет, 1 есть)
//  WORD InputCheckLvl;	//порог контроля уровня на входе (10- степень двойки) 
//  WORD EndCheckLvl; // 	порог контроля конца линии (8500)
//  BYTE ID_Device; // прибор 0-ТОПАЗ или 1-MOT_700
//  float BatStep; // шаг АЦП батарейки
//} CFG_DEV;

//char CommentsOTDR[20] = {"                   \0"}; //комментарии рефлектометра перенесено из modes.c
uint8_t StrTXOpt[64]; // буффер передачи данных по оптике

uint8_t TxOptBusy=0; // признак занятости UatrOptic
uint8_t TimeSCWP=0; // счетчик времени для переключения лазеров


static volatile BYTE ModeLS = 0; // режим источника
static volatile BYTE ModeRE = 0; // режим VFL
static DWORD CurrentBegShiftZone = 0;


static volatile BYTE IndBkLight = 0; // указатель на выбор длительности подсветки
static volatile unsigned short LogarifmNoise = 65535; // уровень логарифмического шума, установка

static volatile unsigned short CheckSum = 0;
// счетчик файлов пробная теория
// храним в нулевом файле при сохранении изменяем, текущее значение 
// держим в дублирующей переменной
unsigned long CountFiles;
unsigned long CurCountFiles; // порядковый номер текущего файла (читаем из памяти)
// если ячейка не записана (0xFFFF), то при считывании присваиваем порядковый номер хранения 
// для ранее сохраненных файлов без имени (без номера)

BYTE g_SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index 
BYTE mg_SetModeLW; // 16.11.2022 запоминаем установочную конфигурацию по длине волны Index for MEMORY

static volatile BYTE g_SeqCombLW[7]; //разрешенные комбинации измерения по местам лазеров

// установка получение индекса в списке комбинации лазеров от номера лазерного места
void SetIndxSeqLS (void)
{
  static volatile int h=0;
  
  // надо установить индекс в таблице комбинаций соотв длины волны
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
  if (Err && 0x10) NameDB.Ena_DB=1; // плохой режим , установим ТОПАЗ (Not Alternate)
  
  if (Err && 0x8) // нет конца строки
  {
  for( unsigned i = 0; i < (ARRAY_SIZE(NameDB.AltName)-2);++i )
  {
    NameDB.AltName[i]=' '; 
  }
  NameDB.AltName[(ARRAY_SIZE(NameDB.AltName)-1)]=0;
  }
  if (Err && 0x4) //плохой ph_A[0]
  NameDB.ph_A[0]=0.0;
  if (Err && 0x2) //плохой ph_A[1]
  NameDB.ph_A[1]=0.0;
  if (Err && 0x1) //плохой ph_A[2]
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
  if (Err && 0x1000) SetJDSU.CompMode=0; // плохой режим , установим ТОПАЗ
  
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
  GetSetNumLS(N_LS); // запомним сколько лазеров установленно
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
// получить/инкремент значение счетчика файлов рефлектограмм, старт с 200 
unsigned long GetSetCntFiles (BYTE Dir)
{
  if (Dir)
  {
    if (CountFiles < 200) CountFiles = 200;
    else CountFiles++;
  }
  return CountFiles;
}
// установить принятый в начале счетчик рефлектограмм
void SetCurCntFiles (void)
{
   CountFiles = CurCountFiles ;
}

void InitDevice(unsigned Err)
{
  if (Err && 0x01)  
  {
  ConfigDevice.BatStep = 2.4414e-3; // тест контроля версий, для светодиода 2.5В
  } 
  Err = Err>>1;
  if (Err && 0x01)  ConfigDevice.ID_Device = 0; // ТОПАЗ 
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

float GetSetBatStep (float Dir) // возвращает или устанавливает значение шага АЦП батарейки
{
  float DataOut = ConfigDevice.BatStep;
  if (Dir==0.0) DataOut = ConfigDevice.BatStep;
  else
  {
    if ((Dir>0.002)&&(Dir<0.003)) ConfigDevice.BatStep = Dir;
    else ConfigDevice.BatStep = 2.852e-3;
  }
    //if ((FIO3PIN & TEST_VER)) DataOut = ConfigDevice.BatStep;
    //else  DataOut = 2.4414e-3; // тест контроля версий, для опоры 2.5В

  return DataOut;
}


void SetLang( BYTE lang ) // функция не рабочая
{
  if ( lang < MAX_LANG )//LANG_NUM-1
  {
    lang = 0;
  }
  if (lang==0)lang=ConfigDevice.ID_Device;
    UserSet.CurrLang = lang;
}

BYTE GetID_Dev(void) // получение идентификатора для кого прибор
{
  return ConfigDevice.ID_Device;
}
BYTE SetID_Dev(BYTE Data) // установка идентификатора для кого прибор
{
  if (Data>1) Data=1;
  UserSet.CurrLang = Eng; // всегда при установке принадлежности устанавливаем Eng
  return ConfigDevice.ID_Device = Data;
}

BYTE GetLang(int Dir) // получение текущего языка, и переустановка по циклу
{
  BYTE Lang = UserSet.CurrLang;
  if (Dir) // переключаем языки
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
    

WORD GetNumDevice(void) // получение номера  прибора
{
  return ConfigDevice.NumDevice;
}

WORD SetNumDevice(WORD Num) // установка номера  прибора
{
  if (Num>9999) Num=9999;
  return ConfigDevice.NumDevice = Num;
}
// получение идентификатора "железа" , что и как сконфигурировано
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
    strcat( name, Device[GetID_Dev()][1][1]);//, strlen(Idesl[device->ID_Device]) // чтобы отвечал английскими
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
     if ((ConfigDevice.ApdiSet)&&(NameDB.Ena_DB)) c[0]='X'; // для IDN там так напишем!
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
  //длины волн лазеров
  for (int i=0; i<ARRAY_SIZE(ConfigDevice.PlaceLS); ++i)
  {
    if (ConfigDevice.PlaceLS[i] != 0)
    {
      sprintf (c,"%4d", ConfigDevice.PlaceLS[i]);
      c[0] = '-';
      c[3] = 0; // два знака 
      strcat(name,c);
    }
  } 
     strcpy(c,PMset[3]); //-PMH
  
     switch (ConfigDevice.CfgPM)
     {
   case 0: // нет измерителя
     c[0]= 0;
     strcat( name,c);
     break;
   case 1: // простой измеритель
     c[3]= 0;
     strcat( name,c);
     break;
   case 2: // измеритель со сферой
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
// получение порога шумов для типа лавинника, для событий!
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
     if ((ConfigDevice.ApdiSet)&&(NameDB.Ena_DB)) c[0]='X'; // если альтернативное имя, не пишем 'X'
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
   case 0: // нет измерителя
     strcat( name,"     ");
     break;
   case 1: // простой измеритель
     strcat( name,"-PM  ");
     break;
   case 2: // измеритель со сферой
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
    case 0: // нет измерителя
      c[0]= 0;
      strcat( name,c);
      break;
    case 1: // простой измеритель
      c[3]= 0;
      strcat( name,c);
      break;
    case 2: // измеритель со сферой
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
     if(Mode) // главное имя
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
     //if ((ConfigDevice.ApdiSet)&&(!NameDB.Ena_DB)) c[0]='X'; // для IDN там так напишем!
     else c[0]=0;
     strcat( label,c);
     if (ConfigDevice.CfgRE) c[0]='+';
     else c[0]=0;
     strcat( label,c);
     }
     else // alternate name
     {
       // просто новая строка
     //strncpy( label, NameDB.AltName, strlen(NameDB.AltName) );
     //label[strlen(NameDB.AltName)]=0;
       // изменяем хвост дописыванием из строки
     //strncpy( c, NameDB.AltName, strlen(NameDB.AltName) );
     strncat( label, (void*)NameDB.AltName, strlen((void*)NameDB.AltName)+1);
     }
     break;
   case 1: // CZE-OPTOKON
     c[1]=0;
     if (ConfigDevice.ApdiSet) c[0]='D';
     else c[0]=0;
     strcat( label,c);
     //длины волн лазеров
     for (int i=0; i<ARRAY_SIZE(ConfigDevice.PlaceLS); ++i)
     {
       if (ConfigDevice.PlaceLS[i] != 0)
       {
         sprintf (c,"%4d", ConfigDevice.PlaceLS[i]);
         c[0] = '-';
         c[3] = 0; // два знака 
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
      if (CoeffPM.ShZeroRng[i]== -1) NewFlash++ ; // чистая ли память
      if (CoeffPM.ShZeroRng[i]>300000) Err |=0x01; // смещения на диапазонах
    }
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefStykRange); ++i)
    if (isnan(CoeffPM.CoefStykRange[i])||(CoeffPM.CoefStykRange[i]<=0.0)||(CoeffPM.CoefStykRange[i]>100.0)) Err |=0x40;// коэффициенты стыковки диапазонов
  for (int i=0; i<ARRAY_SIZE(CoeffPM.PointKalib); ++i)
    if (CoeffPM.PointKalib[i]>2000) Err |=0x02;// длины волн калибровки если 0 то ее нет
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefPointKlb); ++i)
    if (isnan(CoeffPM.CoefPointKlb[i])||(CoeffPM.CoefPointKlb[i]<=0.0)||(CoeffPM.CoefPointKlb[i]>400.0)) Err |=0x04;// коэффициенты привязки длин волн калибровки
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i) 
    if (isnan(CoeffPM.CoefSpctrL[i])||(CoeffPM.CoefSpctrL[i]<=0.0)||(CoeffPM.CoefSpctrL[i]>(80000.0))) Err |=0x08;// коэффициенты спектральной харр. 800-900
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrM); ++i)
    if (isnan(CoeffPM.CoefSpctrM[i])||(CoeffPM.CoefSpctrM[i]<=0.0)||(CoeffPM.CoefSpctrM[i]>(80000.0))) Err |=0x10;// коэффициенты спектральной харр. 1210-1370
  for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrH); ++i)
    if (isnan(CoeffPM.CoefSpctrH[i])||(CoeffPM.CoefSpctrH[i]<=0.0)||(CoeffPM.CoefSpctrH[i]>(80000.0))) Err |=0x20;// коэффициенты спектральной харр. 1410-1650
  if(NewFlash == ARRAY_SIZE(CoeffPM.ShZeroRng)) Err |=0x7F;
  return Err;
}

void FixErrCoeff (DWORD Err)
{

  if (Err != 0)
  {
    if (Err & 0x01) for (int i=0; i<ARRAY_SIZE(CoeffPM.ShZeroRng); ++i) CoeffPM.ShZeroRng[i]=0; // смещение плохое
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
// контроль таблицы настройки измерителя
DWORD CheckErrCoeff (void)
{
    DWORD Err = 0;
    DWORD TstErr = 0;
    for (int i=0; i<ARRAY_SIZE(CoeffPM.ShZeroRng); ++i) // проверка смещений
    {
      //TstErr = TstErr<<1;
      if (CoeffPM.ShZeroRng[i]==0) TstErr++;
    }
    if (TstErr >2) Err |= 0x1; // все смещения 0 - ошибка
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i)
          {
            if (CoeffPM.CoefSpctrL[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrL)) Err |= 0x2; // все спектр 850 = 1.0 - ошибка
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrM); ++i)
          {
            if (CoeffPM.CoefSpctrM[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrM)) Err |= 0x4; // все спектр 1310 = 1.0 - ошибка
    TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrH); ++i)
          {
            if (CoeffPM.CoefSpctrH[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefSpctrH)) Err |= 0x8; // все спектр 1550 = 1.0 - ошибка
        TstErr = 0;
      for (int i=0; i<ARRAY_SIZE(CoeffPM.CoefSpctrL); ++i)
          {
            if (CoeffPM.CoefStykRange[i] == 1.0)  TstErr++;
          }
    if (TstErr == ARRAY_SIZE(CoeffPM.CoefStykRange)) Err |= 0x10; // стыковочные = 1.0 - ошибка

     return Err;       

}

 
DWORD CheckUserGonfig (void)  // Проверка пользовательских настроек 
{
  DWORD Err =0;
  BYTE EnaNumSC=0;
  if ((UserSet.CurrLang > MAX_LANG )||((ConfigDevice.ID_Device > 0)&&(UserSet.CurrLang == Rus))) //LANG_NUM-1
  {
    Err |=0x100;
    if ((ConfigDevice.ID_Device > 0)&&(UserSet.CurrLang == Rus)) UserSet.CurrLang = Eng;// для Чехов чисто английский
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
    Err |=0x1000; // уровень привязки вне нормы
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
      // зависит от длительности импульса на больших км
      if (i<=6) UserSet.BegShiftZone[i]=NumPointsPeriod[i]*8-1;
      // 
      else UserSet.BegShiftZone[i] = 140>>(i-7);
    if (i==10)UserSet.BegShiftZone[i] = 22;
    if (i==11)UserSet.BegShiftZone[i] = 11;
    if (i==12)UserSet.BegShiftZone[i] = 14;
        }
  }
  
  for (int i=0; i<ARRAY_SIZE(UserSet.EnaAutoSetSC); ++i) // проверка установок разрешенных лазеров
  {
    if ((UserSet.EnaAutoSetSC[i]!=0)&&(UserSet.EnaAutoSetSC[i]!=1))
    {
      Err |=0x8000;// для того чтобы переписать !! 19.04.2013
      UserSet.EnaAutoSetSC[i]=1;
    }
    if ((UserSet.EnaAutoSetSC[i]==1) && ConfigDevice.PlaceLS[i])EnaNumSC++;// если есть лазер и стоит признак разрешения
  }
  if (!EnaNumSC) // нет разрешенных лазеров -> разрешим все имеющиеся
  {
  for (int i=0; i<ARRAY_SIZE(UserSet.EnaAutoSetSC); ++i) // проверка установок разрешенных лазеров
  {
        if (ConfigDevice.PlaceLS[i])UserSet.EnaAutoSetSC[i]=1;// если есть лазер то ставим признак разрешения
  }
      Err |=0x8000;// для того чтобы переписать !! 19.04.2013
  }
  for (int i=0; i<ARRAY_SIZE(UserSet.K_pr_SC); ++i) // проверка коэффициентов преломления волн на посадочных местах лазеров
  {
    if (isnan(UserSet.K_pr_SC[i])||(UserSet.K_pr_SC[i]<1.4)||(UserSet.K_pr_SC[i]>1.6))
    {
      Err |=0x8000;// для того чтобы переписать !! 19.04.2013
      UserSet.K_pr_SC[i]=1.468;
    }
 
  }
  
  return Err; 
}

BYTE GetEnIncFiber (BYTE Dir)// установка - запрос состояния признака разрешения авто инкремента счетчика волокна
{
  if (Dir)
  {
    if (UserSet.EnIncFiber) UserSet.EnIncFiber=0;
    else UserSet.EnIncFiber = 1;
  }
  return UserSet.EnIncFiber;
}

WORD GetCellMem (BYTE Dir)// установка получение текущего значения счетчика сохранения измерений
{
    if (Dir)  UserSet.CellMem++; 
    return UserSet.CellMem; 
}

WORD SetupWavePlaceLS (BYTE Place, WORD Data) // Setup num wave for place laser
{
  if (Place>2) Place = 0;
  if ((Data<800)||(Data>1700)) Data = 0;
  GetPlaceLS (NEXT);// установка индекса новой длины волны из списка установленных лазеров 
  return ConfigDevice.PlaceLS[Place] = Data;
}
// заполнение таблицы возможных комбинаций лазера в зависимости от установленных лазерах в посадочных местах
// 1 бит - 1 место, 2бит - второе место, 3 бит - 3 место, комбинации возмжны из заявленных длин волн в значащих местах то есть не 0
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
// получение комбинации лазеров исходя из индекса
BYTE GetCombLS (BYTE Index)
{
  return g_SeqCombLW[Index];
}

BYTE SetTypeDevice (BYTE Data) // установка типа прибора для ТОПАЗОВ
{
  if (Data >9) Data = 2;
  GetPlaceLS (NEXT);// установка индекса новой длины волны из списка установленных лазеров 
  return ConfigDevice.TypeDevice= Data;
}

void SetPlaceLS (BYTE SetPlace) // установка требуемого лазера 
{
  SettingRefl.SW_LW = SetPlace;
}

BYTE GetPlaceLS (BYTE Dir)// установка индекса новой длины волны из списка установленных лазеров CURRENT -текущий, NEW- новый (следующий)
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
  // установка LSEL по SetPlace
  LSEL0((SetPlace+1) & 0x01);
  LSEL1(((SetPlace+1) & 0x02)>>1);
  // установка коэфф преломления в зависимости от выбранной длины волны
  //SettingRefl.K_pr = UserSet.K_pr_SC[SetPlace];
  return SetPlace;
}

WORD GetLengthWaveLS (BYTE SetPlace) // получение длины волны от индекса установочного места
{
    return ConfigDevice.PlaceLS[SetPlace];
}

BYTE GetIndexCurLengthWave (WORD LengthWave)// получение индекса длины волны в зависимости от значения длины волны
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

// получение установка признака автоматического выбора длины волны измерения
BYTE GetSetModeLW (int a)
{
  static volatile BYTE ModeLW; 
  if (a<0) ModeLW = 0; // обычный режим
  if (a>0) ModeLW = 1;
  return ModeLW;
}
// установка получение сколько лазеров стоит???
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

float GetLSACoef (void) // получение коэфф. наклона линии по установленным параметрам
{
  return LSACoef[GetIndexCurLengthWave(GetLengthWaveLS(GetPlaceLS(CURRENT)))][GetIndexLN()];
}

BYTE GetSubModRefl (void) // получение режима работы рефлектометра
{
  return UserSet.SubModeRef;
}
void SetSubModRefl (BYTE Mode) // установка режима рефлектометра
{
  if (Mode)
    UserSet.SubModeRef = AUTO;
  else
    UserSet.SubModeRef = MANUAL;
}
// Управление длиной линии
BYTE GetIndexLN (void) // получение индекса длины линии
{
  return SettingRefl.Index_Ln;
}

BYTE SetIndexLN (BYTE Index) // установка индекса длины линии
{
  if (Index>LENGTH_LINE_NUM-1) Index=LENGTH_LINE_NUM-1;
  if (Index==0xFF) Index=0;
    SettingRefl.Index_Ln = Index;
    if ((Index == 5) && (GetIndexIM()>5)) SetIndexIM (5);
    if ((Index == 1) && (GetIndexIM()>4)) SetIndexIM (4);
          SetIndexIM(GetIndexIM()); // переустанавливаем мертвые зоны в зависимости от индекса
return Index;
}

WORD GetLengthLine (BYTE Index) // получение длины линии по индексу
{
  return LengthLine [Index];
}
// Управление длиной импульса
BYTE GetIndexIM (void) // получение индекса длины Pulse
{
  return SettingRefl.Index_Im;
}

void SetIndexIM (BYTE Index) // установка индекса длины Pulse
{
  if (Index==0xFF) Index=0;
  BYTE LineInx = GetIndexLN ();
  switch (LineInx) //здесь же установим начальное смещение
  {
  case 0: //2km
  case 1: //4km
    if (Index>WIDTH_PULSE_NUM-4)Index = WIDTH_PULSE_NUM-4; //деелаем ограничение на 1000нС (3) без 20000
    break;
  case 2: //8km
  case 3: //16km
  case 4: //32km
  case 5: //64km
    if (Index>WIDTH_PULSE_NUM-3)Index = WIDTH_PULSE_NUM-3; // делаем ограничение на 3000нС (2) без 20000
    
    break;
  case 6: //128km
    if (Index>WIDTH_PULSE_NUM-1)Index = WIDTH_PULSE_NUM-1; // делаем ограничение по максимальной
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

void SetIndexShadowIM (BYTE Index) // установка мертвой зоны для теневого импульса
{
    BYTE LineInx = GetIndexLN ();
  SW_FLTR(ON);

  if (Index == 5) CurrentBegShiftZone = UserSet.BegShiftZone[5+LineInx] ;// 3000 ns
  if (Index == 6) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//10mks for 128km
  if (Index == 7) CurrentBegShiftZone = UserSet.BegShiftZone[6+LineInx] ;//20mks for 128km

}

WORD GetWidthPulse (BYTE Index) // получение длины Pulse по индексу
{
  return WidthPulse [ConfigDevice.ApdiSet][Index];
}
// Управление временем измерения
BYTE GetIndexVRM (void) // получение индекса врмени накопления
{
  return SettingRefl.Index_Vrm;
}

void SetIndexVRM (BYTE Index) // установка индекса времени накопления
{
    if (Index==0xFF) Index=TIME_AVR_NUM-1;
  if (Index>TIME_AVR_NUM-1)Index=0;
    SettingRefl.Index_Vrm = Index;
}

WORD GetTimeAvrg (BYTE Index) // получение времени накопления по индексу
{
  return  TimeAver[Index];
}
// Управление коэфф. преломления
float GetIndexWAV (void) // получение индекса преломления
{
  return SettingRefl.K_pr;
}
// получение индекса преломления в зависимости от места лазера
float GetWAV_SC(BYTE Index)
{
  if (Index>2) Index = 0;
  return UserSet.K_pr_SC[Index];
  
}
// сохранение текущего коэфф в установленном месте (для динамичного сохранения)
void ReSaveWAV_SC (void)
{
        if (SettingRefl.K_pr != UserSet.K_pr_SC[GetPlaceLS(CURRENT)])
        {
        UserSet.K_pr_SC[GetPlaceLS(CURRENT)] = SettingRefl.K_pr;
        //SSPInit_Any(MEM_FL1); // Инициализация SSP для записи FLASH 
        //SaveUserConfig();
        EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));

        }
}


float SetWAV_SC(float Data, BYTE Index)// запись измененного коэфф в структуру
{
  if (Index>2) Index = 0;
  if (Data<1.4) Data = 1.4;
  if (Data>1.6) Data = 1.6;
    UserSet.K_pr_SC[Index] = Data;
  return Data;
  
}
void SetIndexWAV (float Data) // установка индекса преломления
{
  if (Data<1.4) Data = 1.4;
  if (Data>1.6) Data = 1.6;
    SettingRefl.K_pr = Data;
}
// управление измерителем
BYTE GetCfgPM (void) // получение установки измерителя
{
  return ConfigDevice.CfgPM;
}
// управление измерителем
BYTE SetCfgPM (BYTE Data) // Setup установки измерителя
{
  if (Data >2) Data = 0;
  return ConfigDevice.CfgPM = Data;
}
 
unsigned GetTimeLight (BYTE Index) // получение длительности подсветки от индекса
{
  if (Index>TIME_LIGHT_NUM-1)
  {
  IndBkLight = 0;
  Index = 0;
  }
  return TimeLight[Index];
}

BYTE SetIndexLight (int Dir) // установка(получение) индекса длительности подсветки 0-текущий, 1-увеличение, -1-уменьшение
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

BYTE GetCfgRE (void)// получение признака установки красного глаза
{
  return ConfigDevice.CfgRE;
}
BYTE SetCfgRE (BYTE Data)// Setup признака установки красного глаза
{
  if (Data > 1) Data = 0;
  return ConfigDevice.CfgRE = Data;
}

BYTE PreSetModeLS (BYTE Index) // принудительная установка режима источника
{
  
  if (Index > MODELSNUM-1) ModeLS = 0;
  else ModeLS = Index;
  return ModeLS;
}



BYTE GetModeLS (void) // получить режим источника
{
  return ModeLS;

}

BYTE SetModeLS (char* Str, int Dir, BYTE Lang) // возвращает номер режима источника, и пишет строку для отображения на индикаторе
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
      sprintf (Str,"%s",MsgMass[3][Lang] ); // выключен
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
    if (Dir) // есть изменения управляем источником
    {
      SetupSource (ModeLS); // установка режима работы лазера
    }
      
return ModeLS;
}

BYTE SetModeRE (char* Str, int Dir, BYTE Lang) // возвращает номер режима VFL, и пишет строку для отображения на индикаторе
{
  //char s[8];
 //   static volatile BYTE ModeRE = 0; // режим источника
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
      sprintf (Str,"%s",MsgMass[3][Lang] ); // выключен
      break;
    case 1:
      sprintf (Str,"%s",MsgMass[69][Lang] ); // вклшючено
      break;
    case 2:
      sprintf (Str,"%s",MsgMass[82][Lang]); // 1Hz
      break;
    }
return ModeRE;
}

void CntrlRE (void) // функция контроля красным глазом
{
  static DWORD CntRE = 0;
  static DWORD CntREOld = 0;
  static DWORD SostRed = 0;// состояние красного глаза
  
    switch (ModeRE)
    {
    case 1:
      REDEYE(1); // включен
      CntRE = 0;
      break;
    case 2: // здесь подвисал красный глаз
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
      REDEYE(0); // выключен
      CntRE = 0;
      break;
      
    }
  
  
}
BYTE GetModeRE(void) // возвращает текущее значение режима красного глаза
{
 return ModeRE;
 
}

BYTE OffModeRE(void) // выключает красный гдлаз
{
  ModeRE =0;
 return ModeRE;
 
}


void InitReflSet (void) // инициализация установок рефлектометра
{
  // инициализация списка комбинаций установок лазеров
  SetSeqCombLW();
  
  SettingRefl.Index_Ln=0; // индекс значений длины линии в пользовательском режиме
  SettingRefl.Index_Im=0; // индекс значений длительности импульса 
  SettingRefl.Index_Vrm=0; // индекс значений времени работы 
  SettingRefl.Index_Comm=0;  // местоположения курсора в комментариях 
  SettingRefl.SW_LW=0;  // признак длинны волны лазера (переключает длину волны лазера)
  SettingRefl.K_pr=1.4680; // коэфф преломления 
  SettingRefl.SubModeRef = AUTO; // режим рефлектометра
  SettingRefl.LogNoise = 65535; // уровень логарифмического шума,
  g_SetModeLW = 0; // показываем на первую возможную конфигурацию, формируем ее при включении из установленных лазеров
}

void SetLogNoise (unsigned short Data)// уровень логарифмического шума, установка
{
  LogarifmNoise = Data; // уровень логарифмического шума,
}
void SaveLogNoise (void) // сохраняем уровень логарифмического шума перед импульсом
{
  SettingRefl.LogNoise = LogarifmNoise;
}


DWORD GetEndCheckLvl (void) // возврат порога при контроле конца линии
{
  return ConfigDevice.EndCheckLvl;
}

DWORD GetCurrentBegShiftZone (void) //получение текущего смещения по индексу
{
return CurrentBegShiftZone;
}

DWORD SetBegShiftZone (BYTE Index, DWORD Data)// запись мертвой зоны по индексу
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

DWORD GetBegShiftZone (BYTE Index)// чтение мертвой зоны по индексу
{
    return UserSet.BegShiftZone[Index];
}

BYTE SetIndexCommOTDR (BYTE Index) // установка индекса коммкнтария в структкру для сохранения
{
  if (Index>19) Index=0;
  SettingRefl.Index_Comm = Index;
  return  SettingRefl.Index_Comm;
}

void CheckCommOTDR (void) // проверка и корректировка строки комментариев OTDR
{
  // закоментировали пока!!! 
//  int Indx;
//    // подготовка строки комментария для редакции
//    IndexCommOTDR = 0;
//    CommentsOTDR[ARRAY_SIZE(CommentsOTDR)-1]=0; // последний элемент в массиве равен 0
//    for (Indx =ARRAY_SIZE(CommentsOTDR)-2; Indx>=0; Indx--) // идем с последнего
//    {
//      if (CommentsOTDR[Indx]<0x20) CommentsOTDR[Indx]=' '; //если управляющие, то делаем их "пробелом"
//      else if (CommentsOTDR[Indx]!=' ' && IndexCommOTDR == 0)IndexCommOTDR = Indx; // фиксируем длину строки до первого НЕ"прбела"
//      //Index_Comm --;
//    }
//    if ((CommentsOTDR[IndexCommOTDR]!=' ')&&(IndexCommOTDR!=18))IndexCommOTDR ++;// если указатель на не пробел и это не последний, то увеличиваем указатель
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

BYTE GetApdiSet (void) //получение признака наличия лавинного фотодиода
{
  return ConfigDevice.ApdiSet;
}
BYTE SetupApdiSet (BYTE Data) //Setup признака наличия лавинного фотодиода
{
  if (Data>1) Data = 0;
  return ConfigDevice.ApdiSet = Data;
}


BYTE CheckSavedTrace(void) // функция контроля сохраненной рефлектограммы 
{
  BYTE Err = 0;
  if (SettingRefl.Index_Ln > LENGTH_LINE_NUM -1) Err++;
  if (SettingRefl.Index_Im > WIDTH_PULSE_NUM -1) Err++;
  if (SettingRefl.Index_Vrm > TIME_AVR_NUM -1) Err++;
  if (SettingRefl.Index_Comm > ARRAY_SIZE(CommentsOTDR) -1) Err++;
  if (SettingRefl.SW_LW > 2) Err++;
  if (SettingRefl.SubModeRef > 1) Err++;
  if ((SettingRefl.K_pr > 1.6)||(SettingRefl.K_pr < 1.4)) Err++;
  if (mg_SetModeLW > 7) mg_SetModeLW=0; // не может быть конфигурации больше 7
  if (Err)
  {
    InitReflSet (); // инициализация установок рефлектометра
    GetPlaceLS (NEXT); // устанавливаем заведомо существующий лазер
  }
  return Err;
}

void SetNumAverag (unsigned Data) // Запись числа накоплений в память рефлектограмм
{
  SettingRefl.NumAvrag = Data;
}
unsigned GetNumAverag (void) // получение числа накоплений рефлектограммы
{
  return SettingRefl.NumAvrag;
}
// получение индекса длины волны калибровки, в зависимости от текущей длины волны ( в основном для 1300 и 1650)
int GetIndxPON (int Lambda)
{
  int i=0;
    // здесь не совпало с 5 длинами волн калибровки 1300
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
//Зуев И.А.
float GetCurrLvldB(int Lambda)          // Возвращает текущий уровень привязки или
                                        // изменяет  текущий уровень привязки в зависимости от длины волны
{ 
  BYTE i=0;     // Индекс
  
  if((Lambda<800) || ((Lambda>900)&&(Lambda<1210)) || (Lambda>1650)) // Если указана неверная длина волны 
    return UserSet.CurrLvldB;
  
  for(i=0;i<WAVE_LENGTHS_NUM;i++)
    if(Lambda==CoeffPM.PointKalib[i])
    {      
      UserSet.CurrLvldB=UserSet.LevelRef[i];
      SetPMWavelenght(Lambda); // устанавливаем как текущую
      return UserSet.CurrLvldB;
    }
  // здесь не совпало с 5 длинами волн калибровки 1300
  UserSet.CurrLvldB=UserSet.LevelRef[GetIndxPON(Lambda)];
  SetPMWavelenght(Lambda); // устанавливаем как текущую
  return UserSet.CurrLvldB;
 
}

float SetCurrLvldB(int Lambda, float LvldBm)  // устанавливает уровень LvldBm в качестве привязки
{ 
  BYTE i=0;     // Индекс
  
  if((Lambda<800) || ((Lambda>900)&&(Lambda<1210)) || (Lambda>1650)) // Если указана неверная длина волны 
    return UserSet.CurrLvldB;
  
  UserSet.CurrLvldB   = LvldBm;
  for(i=0;i<WAVE_LENGTHS_NUM;i++)
    if(Lambda==CoeffPM.PointKalib[i])
      UserSet.LevelRef[i] = UserSet.CurrLvldB;
  // здесь не совпало с 5 длинами волн калибровки 1300
  UserSet.LevelRef[GetIndxPON(Lambda)] = UserSet.CurrLvldB;
  
  EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));
  
  return UserSet.CurrLvldB;
  
}

// определяет число точек в импульсе по параметрам измерения
// добавляем 10 м (100нс) как результат размазывания импульса
// в аналоговом усилителе

unsigned short PointsInImpulse (int Real)
{
  unsigned short Data;
  if (Real)
  Data = (unsigned short)(((GetWidthPulse(GetIndexIM()))*NumPointsPeriod[GetIndexLN()])/ADCPeriod +1);
  else
  {
  // добавлено 100 нс Какбы фронт сигнала
  Data = (unsigned short)(((GetWidthPulse(GetIndexIM())+((GetIndexIM()>2)?(0):(50)))*NumPointsPeriod[GetIndexLN()])/ADCPeriod);
  if (Data<4) Data = 4;
  } 
  return Data;
}

// возвращает значение ТАУ для установленной длительности импульса

float TauImp (void)
{
  WORD Tau;
  float Data;
    if ((GetIndexIM()>4)&&(ConfigDevice.ApdiSet)) Tau = TAUFLTR; // 820nc
    else 
    {
      if (ConfigDevice.ApdiSet)  Tau = TAUARX;  // 5ns
      else Tau = TAUARN; // 10ns всегда
    }
    Data = ADCPeriod/NumPointsPeriod[GetIndexLN()];
    Data = -1.0*Data/(float)Tau;
    return Data;
}
// функции изменения параметров анализа рефлктограммы
unsigned short GetSet_BC (float Dir)// коэффциент обратного отражения
{
  if (Dir!=0.0)
  {
     ReflParam.BC +=(int short)(Dir*10);
  if ((ReflParam.BC>850)||(ReflParam.BC<450)) { ReflParam.BC = 850;}
 
  }
  return  ReflParam.BC;
}

unsigned short GetSet_LT (float Dir)// мнимльный перепад контроля события
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
    else // больше нуля
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

unsigned short GetSet_ET (float Dir) // конец линии не более 20 дБ
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
    else // больше нуля
    {
      if(ReflParam.ET<=(unsigned short)(20000-Dir*1000))
           ReflParam.ET +=(unsigned short)(Dir*1000);
      else
        ReflParam.ET = 100; //0,1dB
      
    }
  }
  return  ReflParam.ET;
}

unsigned short GetSet_RT (float Dir) // отражение не менее -70дБ
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
    else // больше нуля
    {
      if(ReflParam.RT<=(65000-Delta))
           ReflParam.RT +=Delta;
      else
        ReflParam.RT = 10000; //70dB
      
    }
  }
  return  ReflParam.RT;
}

// расчет числа точек смещения при измерении событий
unsigned short GetPointsShift (void)
{
  WORD Tau;
  unsigned short Data;
  if ((GetIndexIM()>4)&&(ConfigDevice.ApdiSet)) Tau = TAUFLTR; // 820nc
  else Tau = TAUWFLTR; //70нс
  if (GetWidthPulse(GetIndexIM())<Tau) Tau = GetWidthPulse(GetIndexIM());
  Data = (unsigned short)((Tau*NumPointsPeriod[GetIndexLN()])/ADCPeriod +1);
  return Data;
}


void GetHeaderBelcore (char* Name, unsigned short Block, unsigned short NumEvents) // заполняем шапку белкора
{
  char Str[10];
  
  unsigned short DataShort ; // 
  unsigned long DataInt ;
  
      // при чтении переустанавливаем настройки прибора (поэтому перезаполняем длину волны на которой измеряли)
      switch (Block)
      {
      case 0: //0...55 byte
//        // первое заполнение надо прочитать файл
//      if (GetNumTrace()) // если не нулевая то читаем по таблице
//        TraceREAD(GetNumTraceSaved(GetNumTrace()));//  читаем файл который надо передать// 27/01/2011 неадекватно считывала рефлектограмму
//      else  TraceREAD(0);
      memcpy( Name, HeaderBelcore, 56 );
        if (NumEvents)
        {
          //memcpy( &Name[5], 7, 1);
          Name[2]=0x62; // запись нового размера Map блока +16
          Name[6]=7; // запись нового количества полей в Мар блоке
          memcpy( &Name[56], &HeaderBelcore[225], 16 ); // дозапись поля Событий в Мар блок
          DataShort = (unsigned short)(NumEvents*32+24); // получение размера блока в зависимости от числа событий
          memcpy( &Name[68], &DataShort, 2);
          
        }
        break;
      case 1: //56...117 (72...133) 62 byte
      memcpy( Name, &HeaderBelcore[56], 62 );
        // устанавливаем длину волны
      DataShort = (unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT)); // получение длины волны от индекса установочного места
      // перезапись длинны волны 1300 для х4 приборов!
      
      memcpy( &Name[86-56], &DataShort, 2);
      // заполняем Comments
      memcpy( &Name[98-56], CommentsOTDR, 19);
        break;
      case 2: //118...211 (134...227) 94 byte
      memcpy( Name, &HeaderBelcore[118], 94 );
      // заполняем производителя
      memcpy( &Name[118-118], IdnsBC[ConfigDevice.ID_Device], 14 );
      GetMFID( Str ); // for Belcore
      // Set MDIF
      memcpy( &Name[133-118], Str, 12 );
      // Number Device
      sprintf (Str,"%04d",ConfigDevice.NumDevice);
      memcpy( &Name[146-118], Str, 4 );
      // релиз программы
      GetNumVer(Str);
      memcpy( &Name[153-118], Str, 4 );
      
      // время сохранения рефлектограммы 
      // устанавливаем дату сохраненной рефлектограммы
      DataInt = TotalSec (TimeSaveOTDR);
      memcpy( &Name[159-118], &DataInt, 4);
      // (164)AW длина волны в 0.1 нм (1310нм = 13100) 0x332c 0x3c8c (164)
      DataShort = (unsigned short)GetLengthWaveLS (GetPlaceLS(CURRENT))*10;
      memcpy( &Name[165-118], &DataShort, 2);
      // длина зондирующего импульса 
      DataShort = GetWidthPulse(GetIndexIM());
      memcpy( &Name[173-118], &DataShort, 2);
      // устанавливаем DS
      // ((10000points*10(in 1ns 100ps))/2 = 50000 , 333.333 ns - интервал съема информации
      DataInt = (unsigned long)((ADCPeriod*50000)/NumPointsPeriod[GetIndexLN()]); //  устанавливаем значения DS для установленного режима измерения
      memcpy( &Name[175-118], &DataInt, 4);
      // ###(182) GI коэфф преломления  146583 (1.46583)  
      DataInt = (unsigned long)( GetIndexWAV()*100000);
      memcpy( &Name[183-118], &DataInt, 4);
      // ###(186) BC = 800 предел обратного отражения
      DataInt =  ReflParam.BC;
      memcpy( &Name[187-118], &DataInt, 2);
      // ###(188) NAV число накоплений - зависит от времени накопления и длины линии = FinAvrg
      DataInt = SettingRefl.NumAvrag;
      memcpy( &Name[189-118], &DataInt, 4);
      // ###(192) AR  длина измеряемого участка (грубо число измерений на шаг) DS*NPPW/10000
      DataInt = (unsigned long)((ADCPeriod*5*4096)/NumPointsPeriod[GetIndexLN()]); //  устанавливаем значения DS для установленного режима измерения
      memcpy( &Name[193-118], &DataInt, 4);
      // ###(200) NF нижний уровень шумов равен 65535
      DataInt =  ReflParam.NF;
      memcpy( &Name[201-118], &DataInt, 2);
      // ###(206) LT минимальное значение затухания для события 200 в 0.001 дБ
      DataInt =  ReflParam.LT;
      memcpy( &Name[207-118], &DataInt, 2);
      // ###(208) RT минимальное значение отражения для события 40000 в -0.001дБ
      DataInt =  ReflParam.RT;
      memcpy( &Name[209-118], &DataInt, 2);
      // ###(210) ET минимальное затухание для конца линии 3000 в 0.001 дБ
      DataInt =  ReflParam.ET;
      memcpy( &Name[211-118], &DataInt, 2);
        break;
      case 3: //212...223 (228...239) 12 byte
      memcpy( Name, &HeaderBelcore[213], 12 );
       break; 
      }

}
// управление лазерным источником (сигналом CW и FW)-
void  SetupSource (BYTE ModeLS) // установка режима работы лазера
{
  __HAL_UART_DISABLE_IT(&huart5, UART_IT_PE);
  
  //SWITCH_LW(); // установка требуемой длины волны источника (управление ключами)
//  const uint8_t* TxGenOpt={"UUUUUUUUUUUUUUUUU"};
  switch(ModeLS)
  {
  case 0: // выключаем источник и останавливаем всякие действия
    PHLD(0);
    HAL_UART_DMAStop(&huart5);
    LED_START(OFF);
    break;
  case 1: // включаем источник и останавливаем всякие действия
    PHLD(1);
    HAL_UART_DMAStop(&huart5);
    LED_START(ON); //
    break;
  case 2: // выключаем источник и перестраиваем UART и запускаем Генерацию меандра 275 Hz
    PHLD(0);
    huart5.Init.BaudRate = 550;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ждем принятия первого байта из внешнего мира
    //Dummy = huart2.Instance->DR ; // чистим буффер приема 
    
    HAL_UART_Transmit_DMA(&huart5, (void*)TxGenOpt,strlen((void*)TxGenOpt)); // выдаем 
    LED_START(ON);
    break;
  case 3: // выключаем источник и и перестраиваем UART и запускаем Генерацию меандра 2 kHz
    PHLD(0);
    huart5.Init.BaudRate = 4000;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ждем принятия первого байта из внешнего мира
    //Dummy = huart2.Instance->DR ; // чистим буффер приема 
    
    HAL_UART_Transmit_DMA(&huart5, (void*)TxGenOpt,strlen((void*)TxGenOpt)); // выдаем 
    LED_START(ON);
    break;
  case 4: // выключаем источник и перестраиваем UART на скорость 1200
    PHLD(0);
    huart5.Init.BaudRate = 1200;
    if (HAL_UART_Init(&huart5) != HAL_OK)
    {
      Error_Handler();
    }
    HAL_UART_DMAStop(&huart5);
    //HAL_UART_Receive_IT(&huart2, RxBufOpt,1); // ждем принятия первого байта из внешнего мира
    //Dummy = huart2.Instance->DR ; // чистим буффер приема 
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
// функция переключения лазеров по кругу с излучением информации о себе
void Run_SCWP(void) // каждые 50 мС
{
  if(TimeSCWP++<70)
  {
    switch(TimeSCWP)
    {
    case 2:
      sprintf((char*)StrTXOpt, "UUUU####SL%4u %.2f\rUUU",GetLengthWaveLS(GetPlaceLS (CURRENT)),-30.0);//UUU ooo1300
      HAL_UART_Transmit_DMA(&huart5, (void*)StrTXOpt,strlen((void*)StrTXOpt)); // выдаем 
      TxOptBusy = 1;
      //TsT(1);// дернем ногой для проверки
      //  HAL_GPIO_WritePin(test_GPIO_Port, test_Pin, GPIO_PIN_SET); 

      //HAL_Delay(200);
      break;
    case 10: // через 500 мС после старта передачи... 
      PHLD(1); // on CW 
      break;
    default:
      break;
    }
  }
  else
  {
    TimeSCWP = 0; // все сначала
    PHLD(0); // off CW
    // переключаем ЛАЗЕРЫ
      //  GetSetLW_SC (1, 0) ; // так будут все светить
    GetPlaceLS (NEXT);
    //    GetSetLW_SC (1, 1) ; // а так только на одном выходе


    //NeedReDraw = 1;
  }
}


unsigned short CalkCheckSum (void)// подсчет контрольной суммы конфигурации прибора
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


void GetNumVer (char* Str) // получение номера версии
{
  char out[5];
  strcpy( out, NumVer );//, strlen(Ides[device->ID_Device])
  out[4] = 0;
  int Num = atoi (out);
  //Num=Num/5;
  sprintf (Str, "5%c%c%c", (BYTE)(0x30+(Num/260)), (BYTE)(0x30+(Num/26)), (BYTE)(0x61+(Num%26)));
}

void SetNewLeveldB (float RPON, int i) // установка нового уроаня привязки по принятым волнам
{
  UserSet.LevelRef[i] = RPON + UserSet.LevelRef[i] ;   
  //UserSet.CurrLvldB = UserSet.LevelRef[i] ;      
}

void DeletedAllCell (void) // удаление всех записей измерителя
{
  
  UserSet.CellMem = 0;
//  SSPInit_Any(MEM_FL1); // Инициализация SSP для управления FLASH (порт 1 та что на плате отладочной)
//  FlashErasePage(CFG_USER); // чистим страницу установок пользователя прибора
//  FlashWritePageSM(CFG_USER, StructPtr(CFG_USER), StructSize(CFG_USER), 0);
  EEPROM_write(&UserSet, ADR_UserMeasConfig, sizeof(UserSet));
 
}
//


