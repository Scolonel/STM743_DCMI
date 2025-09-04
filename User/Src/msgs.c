#include "main.h"


//#define LANG_NUM 3
//#define MSG_NUM 83


//const char *MsgMass[MSG_NUM][LANG_NUM] = 
//#pragma location=0x70000
//__root const char *MsgMass[MSG_NUM][LANG_NUM] = 
const char *MsgMass[MSG_NUM][LANG_NUM] = 
//const char MsgMass[MSG_NUM][LANG_NUM][32] = 

{


// ���� ���������
{"�������","English","Cesky","Spanish","Deutsch","Francias"},                //0
{"����/�����","Date/Time","Datum/cas","Fecha/H.","Datum/Uhrzeit","Date/Time"},       //1
{"���������","Backlight","Podsviceni","ilumin.","Beleuchtung","Backlight"},       //2
{"����.","OFF","Vyp","OFF","Off","Off"},                        //3
{"�","s","s","s","s","s"},                                //4
{"����������","Calibrate","Kalibrace","Calibracion","Kalibrierung","Calibrate"},       //5

// ������� ����
{"�����-7","MOT-700","MOT-700","MOT-700","MOT-700","MOT-700"},              //6
{"������������","OTDR","OTDR","OTDR","OTDR","OTDR"},       //7
{"������","LS","LS","FL","LQ","LS"},                         //8
{"������","Memory","Pamet","Memoria","Speicher","Memory"},                  //9
{"���������","Setting","Nastaveni","Config.","Einstellungen","Setting"},          //10
{" ����"," PWR"," Sit'","Potencia"," Netz"," PWR"},                      //11

// ��������
{"��� ��� �����������","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM"}, //12
{"�������","Today ","Dnes","Hoy","Heute ","Today "},                  //13
{"�","y"," "," "," ","y"},                                //14
{"�����","Time ","Cas","Hora","Zeit ","Time "},                      //15
{"�������","Battery","Baterie","Bateria","Batterie","Battery"},              //16

// ���� ��������� �������������
//{"����� �����","Wavelength","Vlnova delka","L.Onda","Wellenlange","Wavelength"},  //17
{"����� �����","Wavelength","Vlnova delka","L.Onda","Wellenlange","Wavelength"},  //17
{"��","nm","nm","nm","nm","nm"},                             //18
{"����� �����","Range","Vzdalenost","Rango","Reichweite","Range"},             //19
{"��","km","km","km","km","km"},                             //20
{" ����"," Auto"," Auto"," Auto"," Auto"," Auto"},                    //21
{"����.���.","Pulse width","Sirka pulzu","Pulso","Pulsbreite","Pulse width"},           //22
{"��","ns","ns","ns","ns","ns"},                             //23
{"����� ���.","Average","Prumerovani","Media","Mittelung","Average"},       //24
{" ����."," Inf"," Inf","Inf"," Inf"," Inf"},                     //25
{"������.","Realtime","Real","En vivo","Echtzeit","Realtime"},                //26
{"������","Index","Index","Indice","Index","Index"},            //27 // 29.04.2022 ������ �����.������.

// ��������� ��� ��������� 
{"== � � � � � ==","=== W A I T ===","=C E K E J T E=","= E S P E R E =","= W A R T E N =","=== W A I T ==="},//28
{"����. �������:","Pulse width:","Sirka pulzu:","Pulso:","Impulsbreite:","Pulse width:"},//29
{"���� ���������...","Measuring... ","Mereni... ","Midiendo...","Mess ... ","Measuring... "},//30
{"����� �����:","Range:","Vzdalenost:","Rango:","Reichweite:","Range:"},//31
{"��. ������:","OTDR port:","OTDR vstup:","Puerto:","OTDR-Anschluss:","OTDR port:"},//32
{"����","test","test","testeo","test","test"},//33
{"���. �� �����:","Input Power:","Vstupni uroven","Pw Salida","Eingangsleistung:","Input Power:"},//34
{"���","NO","Ne","No","NO","NO"},//35
{"� � � � � � � �"," W A R N I N G !","  P O Z O R !   "," C U I D A D O !","W A R N U N G!"," W A R N I N G !"},//36
{" ��������� �� �����","  High Input Power","  Uroven na vstupu","Pw.Muy Alt","Hohe Eingangsleistung","  High Input Power"},//37
{"���������� ?","To continue","Pokracovat","Seguir","Fortfahren","To continue"},//38 fOrtfahren
{"������� <S>","Press key <S>"," Stisknout <S> ","Teclea <S>","Taste <S> ","Press key <S>"},//39   Taste drUcken

{"����������","Calibrating...","Kalibrace...","Calibrado..."," Kalibrieren ...","Calibrating..."},//40
{"       ������  ","  ","      Zacatek","  ","  ","  "},//41

// ������ � ������� (
{"������ :","Memory :","Pamet :","Memoria","Speicher:","Memory :"},//42
{"��������","  free","  Volna","Libre","  Frei","  free"},//43
{"�������","Clear","Vymazat","Limpiar","Loschen","Clear"},//44 LOschen
{"������� ������ :","Clear memory :","Vymazat pamet :","Vaciar m.","Speicher loschen:","Clear memory :"},//45 Speicher lOschen

{"������","Cancel","Storno","Anular","loschen","Cancel"},//46 lOschen

// ��������� �������������
{"��","dB","dB","dB","dB","dB"},//47
{"���","dBm","dBm","dBm","dBm","dBm"},//48

//R_SelPow() ��������� �������� ������ ������ ����������
{"������","OLT Manual","Rucni mereni","OLT Man.","OLT Man.","OLT Manual"},//49
{"�������.","OLT Automat","Auto mereni","OLT Auto","OLT Auto.","OLT Automat"},//50

// ���������� ������ ���������� Draw_Save_Meter ()
{"�������,","Level  ,","Uroven ,","Nivel","Niveau","Level  ,"},//51
{"������","Loss  ","Utlum","Perdida","Dampfung","Loss  "},//52 DAmpfung
{"��.�����","Wavelen.","Vl.delka","L.Onda","Wellenl.","Wavelen."},//53

//  R_Edit_Comm()
{"�����������","Comments","Poznamka","Coment.","Kommentare","Comments"}, //54
{"���� �������","Count fiber","Pocet vlaken","�uenta fibra","Anzahl Faser","Count fiber"},//55 Anzahl Faser
{"������","Manual","Rucni","Manual","Handbuch","Manual"},//56
{"����","Auto","Auto","Auto","Auto","Auto"},//57
{"���.��������","Reset count","Vynulovat","Reset ct.","Zahl loschen","Reset count"},//58 Zahl lOschen 
{" ������ ����������","Write ERROR","Chyba zapisu","Error E.","Schreibfehler","Write ERROR"},//59
{"��� ��������� ������","Memory full","Plna pamet","M. Llena","Speicher voll","Memory full"},//60
{"� �������","Fiber"," C.vlakna","� Fibra","Faser"," Fiber"},//61

// save traces
{"���������"," Saved"," Ulozeno","Guardado","Gespeichert"," Saved"},//62
{"��� ������","Memory full","Plna pamet","M. Llena","Speicher voll","Memory full"},//63

// Edit_DaTime(void)
{"����/����� ��������","Up-to-date","Zmena data a casu","Conf. F.","Aktuell","Up-to-date"},//64 Aktuell
{"����","Date","Datum","Fecha","Datum","Date"},//65
{"�������?","Delete?","Vymazat?","Borrar?","Loschen?","Delete?"},//66 LOschen?
{"�������","Deleted","Vymazano","Borrado","Geloscht","Deleted"},//67 GelOscht


//R_Source()
{"��������","Source","Zdroj","Fuente","Quelle","Source"},//68
{"���.","ON","Zap","ON","Auf","On"},//69

{"�������","Event","Udalost","Evento","Ereignis","Event"},//70

//void BB(void)
{"������� ���������","Low battery","Vybita baterie","B. Baja","Leere Batterie","Low battery"},//71 Leere Batterie
{"��������� ������","Turn off device","Vypnout pristroj","Apag equip.","Gerat ausschalten","Turn off device"},//72
{"�������� �������","Charge battery","Nabit baterii","Carg.Bat.","Batterie laden ","Charge battery"},//73 Batterie laden 
// �������������� ������ ��� ��������� ���������� ( Msg008)
{"������","OLT","Tester","OLT","OLT","OLT"},//74
{"��� ��� �����������","WWW.COMM-SERV.RU","WWW.COMM-SERV.RU","WWW.COMM-SERV.RU","WWW.OPTOKON.COM","WWW.COMM-SERV.RU"}, //75
{"������","Tester","Tester","Equipo","Tester","Tester"},                         //76
{"��������.","Contrast","Kontrast","Contrasto", "Kontrast","Contrast"},                         //77
{"�","m","m","m","m","m"},                             //78
// ������� �������
{"������� �������","Table of events","Tabulka udalosti","Tabla Ev.","Ereignistabelle","Table of events"},                         //79 Ereignistabelle
{"270��","270Hz","270Hz","270Hz","270Hz","270Hz"},                         //80
{"2���","2kHz","2kHz","2kHz","2kHz","2kHz"},                         //81
{"1��","1Hz","1Hz","1Hz","1Hz","1Hz"},                         //82
// ������ ���������� ������ �������
{"�������","Event","Udalost","Evento","Ereignis","Event"},                         //83
{"���","Type","Typ","Tipo","Typ","Type"},                         //84
{"���������","Position","Pozice","Posicion","Position","Position"},                         //85
{"���������","Loss","Utlum","Perdida","Dampfung","Loss"},                         //86 DAmpfung
{"���������","Reflectance","Odraz","Reflexion","Reflexionen","Reflectance"},                         //87 Reflexionen
// ���� �������
{"������ �����","Begin fiber","Zacatek trasy","Inicio FO","Begin Faser","Begin fiber"},                         //88
{"��� ���������","Non-Reflective","Ne-odraz","No-Reflex","Nicht reflektierend","Non-Reflective"},                         //89 Nicht reflektierend
{"�������������","Reflective","Odraz","Reflexion","Reflektierend","Reflective"},                         //90 Reflektierend
{"����������","Saturation","Saturace","Satur.","Sattigung","Saturation"},                         //91 SAttigung
{"����� �����","End fiber","Konec trasy","Final FO","Faserende","End fiber"},                         //92 Faserende
{"������ ������","Full loss","Uplna ztrata","Perd.Tot","Volle Dampfung","Full loss"},                         //93 Volle DAmpfung?
{"���","pW","pW","pW","pW","pW"},                             //94
{"���","nW","nW","nW","nW","nW"},                             //95
{"����","uW","uW","uW","uW","uW"},                             //96
{"���","mW","mW","mW","mW","mW"},                             //97
// ���� ��������� ���������� ����. ���������
{"���.BELCORE","Set analyze","Nast. analyzy","Config An","Analysieren","Set analyze"},//98 Analysieren
{"�������","Events","Udalosti","Eventos","Ereignis","Events"},//99 Ereignis
{"������������","OTDR Params","OTDR","OTDR Par.","OTDR Param.","OTDR Params"},       //100 OTDR Parameter
{"������ ��������������","Analysis of traces","Analyza udalosti","A. Trazas","Trasse analysieren","Analysis of traces"},       //101
{"��������� ����������:","Setting parameters:","Nastaveni parametru:","Config P.","Parameter einstellen:","Setting parameters:"},       //102
{"���.���������","Backscatter","Rozptyl","Retrodisp.","Backscatter","Backscatter"},       //103 ?
{"��������� (BC)","Coefficient (BC)","Koeficient (BC)","Coef.(BC)","Koeffizient (BC)","Coefficient (BC)"},       //104
{"������ �����","Loss Threshold","Utlum - prah","Umbral perd.","Dampfunglimit","Loss Threshold"},       //105 DAmpfunglimit
{"���������� (LT)","of event (LT)","udalost (LT)","de Ev(LT)","von Ereignis (LT)","of event (LT)"},       //106
{"�����.�����.","Reflectance","Odraz","Reflexion","Reflexion","Reflectance"},       //107 Reflexion
{"��������� (RT)","of event (RT)","udalost (RT)","de Ev(RT)","von Ereignis (RT)","of event (RT)"},       //108
{"����� �����","End of Fiber","Konec trasy","Final FO","Faserende","End of Fiber"},       //109 Faserende
{"����� ����� (ET)","as fiber end (ET)","trasy (ET)","Final FO","als Faserende (ET)","as fiber end (ET)"},       //110
{"������ �������","Save events","Zapnout analyzu","GuardarEv","Ereignis sichern","Save events"},    //111 (��� ������� ��������� �������������) Ereignis sichern
{"��������� ������","Event parameters","Param. udalosti","Param.Ev.","Ereignisparameter","Event parameters"}, // 112
{"��","Yes","Ano","Si","Ya","Oui"}, //113
{"���","No","Ne","No","Nien","Pas"}, //114 Nein
{"������ ������","Memory error","Memory error","Error Mem","Speicherfehler","Memory error"}, //115
{"������","Filter","Filter","Filtro","Filter","Filter"}, //116
//{"�������������","Compatibility","Kompatibilita","Compatibilidad","Kompatibilitat","Compatibilite"} //117
{"�����","Mode","Mode","Mode","Mode","Mode"}, //117
{"������:","Slope:","Slope:","Slope:","Slope:","Slope:"}, //118
//{"����� Write ��� ��� ������","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF"},//119
//{"���������� �� LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD"},//120
{"������ ����� �","Device is ready","Device is ready","Device is ready","Device is ready","Device is ready"},//119
{"���������� �� LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD"},//120
{"�������","Brightness","Brightness","Brightness", "Brightness","Brightness"},                         //121 ��� ������ ����������
{"��","YES","Ano","Si","Ja","YES"},//122
{"tst","tst","tst","tst","tst","tst"},//123
{"���","um","um","um","um","um"},                             //124 ����� �����
{"����� ���� ","WL AutoSave","Vlnova delka","L.Onda","Wellenlange","WL AutoSave"},  //125
// ��� ������ ���� ������ � ������� �������
{"������ �������","Tester's Memory :","Pamet :","Memoria","Speicher:","Memory :"},//126
{"��������","  free","  Volna","Libre","  Frei","  free"},//127 �����������
{"���������","Save","Vymazat","Limpiar","Loschen","Save"},//128 ��������� � ����
{"������� ������","Clear memory","Vymazat pamet","Vaciar m.","Speicher loschen","Clear memory"}//129 ��������

};

