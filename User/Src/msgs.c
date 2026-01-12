#include "main.h"


//#define LANG_NUM 3
//#define MSG_NUM 83


//const char *MsgMass[MSG_NUM][LANG_NUM] = 
//#pragma location=0x70000
//__root const char *MsgMass[MSG_NUM][LANG_NUM] = 
const char *MsgMass[MSG_NUM][LANG_NUM] = 
//const char MsgMass[MSG_NUM][LANG_NUM][32] = 

{


// меню установок
{"Русский","English","Cesky","Spanish","Deutsch","Francias"},                //0
{"Дата/Время","Date/Time","Datum/cas","Fecha/H.","Datum/Uhrzeit","Date/Time"},       //1
{"Подсветка","Backlight","Podsviceni","ilumin.","Beleuchtung","Backlight"},       //2
{"Выкл.","OFF","Vyp","OFF","Off","Off"},                        //3
{"с","s","s","s","s","s"},                                //4
{"Калибровка","Calibrate","Kalibrace","Calibracion","Kalibrierung","Calibrate"},       //5

// Главное меню
{"ТОПАЗ-7","MOT-700","MOT-700","MOT-700","MOT-700","MOT-700"},              //6
{"РЕФЛЕКТОМЕТР","OTDR","OTDR","OTDR","OTDR","OTDR"},       //7
{"ТЕСТЕР","LS","LS","FL","LQ","LS"},                         //8
{"ПАМЯТЬ","Memory","Pamet","Memoria","Speicher","Memory"},                  //9
{"УСТАНОВКИ","Setting","Nastaveni","Config.","Einstellungen","Setting"},          //10
{" СЕТЬ"," PWR"," Sit'","Potencia"," Netz"," PWR"},                      //11

// Заставка
{"ООО НПК СвязьСервис","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM","WWW.OPTOKON.COM"}, //12
{"Сегодня","Today ","Dnes","Hoy","Heute ","Today "},                  //13
{"г","y"," "," "," ","y"},                                //14
{"Время","Time ","Cas","Hora","Zeit ","Time "},                      //15
{"Батарея","Battery","Baterie","Bateria","Batterie","Battery"},              //16

// меню установки рефлектометра
//{"Длина волны","Wavelength","Vlnova delka","L.Onda","Wellenlange","Wavelength"},  //17
{"Длина волны","Wavelength","Vlnova delka","L.Onda","Wellenlange","Wavelength"},  //17
{"нм","nm","nm","nm","nm","nm"},                             //18
{"Длина линии","Range","Vzdalenost","Rango","Reichweite","Range"},             //19
{"км","km","km","km","km","km"},                             //20
{" авто"," Auto"," Auto"," Auto"," Auto"," Auto"},                    //21
{"Зонд.Имп.","Pulse width","Sirka pulzu","Pulso","Pulsbreite","Pulse width"},           //22
{"нс","ns","ns","ns","ns","ns"},                             //23
{"Время Изм.","Average","Prumerovani","Media","Mittelung","Average"},       //24
{" непр."," Inf"," Inf","Inf"," Inf"," Inf"},                     //25
{"реальн.","Realtime","Real","En vivo","Echtzeit","Realtime"},                //26
{"Индекс","Index","Index","Indice","Index","Index"},            //27 // 29.04.2022 замена Коэфф.прелом.

// сообщение при измерении 
{"== Ж Д И Т Е ==","=== W A I T ===","=C E K E J T E=","= E S P E R E =","= W A R T E N =","=== W A I T ==="},//28
{"Зонд. импульс:","Pulse width:","Sirka pulzu:","Pulso:","Impulsbreite:","Pulse width:"},//29
{"Идет измерение...","Measuring... ","Mereni... ","Midiendo...","Mess ... ","Measuring... "},//30
{"Длина линии:","Range:","Vzdalenost:","Rango:","Reichweite:","Range:"},//31
{"Вх. разъем:","OTDR port:","OTDR vstup:","Puerto:","OTDR-Anschluss:","OTDR port:"},//32
{"тест","test","test","testeo","test","test"},//33
{"Изл. на входе:","Input Power:","Vstupni uroven","Pw Salida","Eingangsleistung:","Input Power:"},//34
{"НЕТ","NO","Ne","No","NO","NO"},//35
{"В Н И М А Н И Е"," W A R N I N G !","  P O Z O R !   "," C U I D A D O !","W A R N U N G!"," W A R N I N G !"},//36
{" Излучение на входе","  High Input Power","  Uroven na vstupu","Pw.Muy Alt","Hohe Eingangsleistung","  High Input Power"},//37
{"Продолжить ?","To continue","Pokracovat","Seguir","Fortfahren","To continue"},//38 fOrtfahren
{"нажмите <S>","Press key <S>"," Stisknout <S> ","Teclea <S>","Taste <S> ","Press key <S>"},//39   Taste drUcken

{"калибровка","Calibrating...","Kalibrace...","Calibrado..."," Kalibrieren ...","Calibrating..."},//40
{"       начала  ","  ","      Zacatek","  ","  ","  "},//41

// Работа с памятью (
{"Память :","Memory :","Pamet :","Memoria","Speicher:","Memory :"},//42
{"свободно","  free","  Volna","Libre","  Frei","  free"},//43
{"ОЧИСТКА","Clear","Vymazat","Limpiar","Loschen","Clear"},//44 LOschen
{"Очистка памяти :","Clear memory :","Vymazat pamet :","Vaciar m.","Speicher loschen:","Clear memory :"},//45 Speicher lOschen

{"ОТМЕНА","Cancel","Storno","Anular","loschen","Cancel"},//46 lOschen

// Индикация рефлектограмм
{"дБ","dB","dB","dB","dB","dB"},//47
{"дБм","dBm","dBm","dBm","dBm","dBm"},//48

//R_SelPow() отрисовка картинки выбора режима измерителя
{"РУЧНОЙ","OLT Manual","Rucni mereni","OLT Man.","OLT Man.","OLT Manual"},//49
{"АВТОМАТ.","OLT Automat","Auto mereni","OLT Auto","OLT Auto.","OLT Automat"},//50

// прорисовка памяти измерителя Draw_Save_Meter ()
{"Уровень,","Level  ,","Uroven ,","Nivel","Niveau","Level  ,"},//51
{"Потери","Loss  ","Utlum","Perdida","Dampfung","Loss  "},//52 DAmpfung
{"Дл.волны","Wavelen.","Vl.delka","L.Onda","Wellenl.","Wavelen."},//53

//  R_Edit_Comm()
{"Комментарии","Comments","Poznamka","Coment.","Kommentare","Comments"}, //54
{"Счет волокон","Count fiber","Pocet vlaken","Сuenta fibra","Anzahl Faser","Count fiber"},//55 Anzahl Faser
{"ручной","Manual","Rucni","Manual","Handbuch","Manual"},//56
{"авто","Auto","Auto","Auto","Auto","Auto"},//57
{"Сбр.счетчика","Reset count","Vynulovat","Reset ct.","Zahl loschen","Reset count"},//58 Zahl lOschen 
{" Запись невозможна","Write ERROR","Chyba zapisu","Error E.","Schreibfehler","Write ERROR"},//59
{"нет свободной памяти","Memory full","Plna pamet","M. Llena","Speicher voll","Memory full"},//60
{"№ волокна","Fiber"," C.vlakna","№ Fibra","Faser"," Fiber"},//61

// save traces
{"сохранено"," Saved"," Ulozeno","Guardado","Gespeichert"," Saved"},//62
{"НЕТ ПАМЯТИ","Memory full","Plna pamet","M. Llena","Speicher voll","Memory full"},//63

// Edit_DaTime(void)
{"Дата/Время изменены","Up-to-date","Zmena data a casu","Conf. F.","Aktuell","Up-to-date"},//64 Aktuell
{"Дата","Date","Datum","Fecha","Datum","Date"},//65
{"удаляем?","Delete?","Vymazat?","Borrar?","Loschen?","Delete?"},//66 LOschen?
{"удалено","Deleted","Vymazano","Borrado","Geloscht","Deleted"},//67 GelOscht


//R_Source()
{"Источник","Source","Zdroj","Fuente","Quelle","Source"},//68
{"Вкл.","ON","Zap","ON","Auf","On"},//69

{"Событие","Event","Udalost","Evento","Ereignis","Event"},//70

//void BB(void)
{"Батарея разряжена","Low battery","Vybita baterie","B. Baja","Leere Batterie","Low battery"},//71 Leere Batterie
{"Выключите прибор","Turn off device","Vypnout pristroj","Apag equip.","Gerat ausschalten","Turn off device"},//72
{"зарядите батарею","Charge battery","Nabit baterii","Carg.Bat.","Batterie laden ","Charge battery"},//73 Batterie laden 
// дополнительная строка для индикации измерителя ( Msg008)
{"ТЕСТЕР","OLT","Tester","OLT","OLT","OLT"},//74
{"ООО НПК СвязьСервис","WWW.COMM-SERV.RU","WWW.COMM-SERV.RU","WWW.COMM-SERV.RU","WWW.OPTOKON.COM","WWW.COMM-SERV.RU"}, //75
{"ТЕСТЕР","Tester","Tester","Equipo","Tester","Tester"},                         //76
{"Контраст.","Contrast","Kontrast","Contrasto", "Kontrast","Contrast"},                         //77
{"м","m","m","m","m","m"},                             //78
// таблица событий
{"Таблица событий","Table of events","Tabulka udalosti","Tabla Ev.","Ereignistabelle","Table of events"},                         //79 Ereignistabelle
{"270Гц","270Hz","270Hz","270Hz","270Hz","270Hz"},                         //80
{"2кГц","2kHz","2kHz","2kHz","2kHz","2kHz"},                         //81
{"1Гц","1Hz","1Hz","1Hz","1Hz","1Hz"},                         //82
// строки заполнения экрана событий
{"СОБЫТИЕ","Event","Udalost","Evento","Ereignis","Event"},                         //83
{"Тип","Type","Typ","Tipo","Typ","Type"},                         //84
{"Положение","Position","Pozice","Posicion","Position","Position"},                         //85
{"Затухание","Loss","Utlum","Perdida","Dampfung","Loss"},                         //86 DAmpfung
{"Отражение","Reflectance","Odraz","Reflexion","Reflexionen","Reflectance"},                         //87 Reflexionen
// типы событий
{"Начало линии","Begin fiber","Zacatek trasy","Inicio FO","Begin Faser","Begin fiber"},                         //88
{"Без отражения","Non-Reflective","Ne-odraz","No-Reflex","Nicht reflektierend","Non-Reflective"},                         //89 Nicht reflektierend
{"Отражательное","Reflective","Odraz","Reflexion","Reflektierend","Reflective"},                         //90 Reflektierend
{"Перегрузка","Saturation","Saturace","Satur.","Sattigung","Saturation"},                         //91 SAttigung
{"Конец линии","End fiber","Konec trasy","Final FO","Faserende","End fiber"},                         //92 Faserende
{"Полные потери","Full loss","Uplna ztrata","Perd.Tot","Volle Dampfung","Full loss"},                         //93 Volle DAmpfung?
{"пВт","pW","pW","pW","pW","pW"},                             //94
{"нВт","nW","nW","nW","nW","nW"},                             //95
{"мкВт","uW","uW","uW","uW","uW"},                             //96
{"мВт","mW","mW","mW","mW","mW"},                             //97
// меню установки параметров рефл. измерений
{"Уст.BELCORE","Set analyze","Nast. analyzy","Config An","Analysieren","Set analyze"},//98 Analysieren
{"События","Events","Udalosti","Eventos","Ereignis","Events"},//99 Ereignis
{"Рефлектометр","OTDR Params","OTDR","OTDR Par.","OTDR Param.","OTDR Params"},       //100 OTDR Parameter
{"Анализ рефлектограммы","Analysis of traces","Analyza udalosti","A. Trazas","Trasse analysieren","Analysis of traces"},       //101
{"установка параметров:","Setting parameters:","Nastaveni parametru:","Config P.","Parameter einstellen:","Setting parameters:"},       //102
{"Обр.рассеяние","Backscatter","Rozptyl","Retrodisp.","Backscatter","Backscatter"},       //103 ?
{"рассеяния (BC)","Coefficient (BC)","Koeficient (BC)","Coef.(BC)","Koeffizient (BC)","Coefficient (BC)"},       //104
{"Потери стыка","Loss Threshold","Utlum - prah","Umbral perd.","Dampfunglimit","Loss Threshold"},       //105 DAmpfunglimit
{"соединении (LT)","of event (LT)","udalost (LT)","de Ev(LT)","von Ereignis (LT)","of event (LT)"},       //106
{"Коэфф.отраж.","Reflectance","Odraz","Reflexion","Reflexion","Reflectance"},       //107 Reflexion
{"отражения (RT)","of event (RT)","udalost (RT)","de Ev(RT)","von Ereignis (RT)","of event (RT)"},       //108
{"Конец линии","End of Fiber","Konec trasy","Final FO","Faserende","End of Fiber"},       //109 Faserende
{"конца линии (ET)","as fiber end (ET)","trasy (ET)","Final FO","als Faserende (ET)","as fiber end (ET)"},       //110
{"Запись событий","Save events","Zapnout analyzu","GuardarEv","Ereignis sichern","Save events"},    //111 (для подменю установок рефлектометра) Ereignis sichern
{"Параметры поиска","Event parameters","Param. udalosti","Param.Ev.","Ereignisparameter","Event parameters"}, // 112
{"Да","Yes","Ano","Si","Ya","Oui"}, //113
{"Нет","No","Ne","No","Nien","Pas"}, //114 Nein
{"Ошибка памяти","Memory error","Memory error","Error Mem","Speicherfehler","Memory error"}, //115
{"Фильтр","Filter","Filter","Filtro","Filter","Filter"}, //116
//{"Совместимость","Compatibility","Kompatibilita","Compatibilidad","Kompatibilitat","Compatibilite"} //117
{"Режим","Mode","Mode","Mode","Mode","Mode"}, //117
{"Наклон:","Slope:","Slope:","Slope:","Slope:","Slope:"}, //118
//{"Можно Write все что угодно","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF","Dont PWR OFF"},//119
//{"Обновление ПО LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD","Upload FW LCD"},//120
{"Прибор готов к","Device is ready","Device is ready","Device is ready","Device is ready","Device is ready"},//119
{"Обновлению ПО LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD","Upgrade FW LCD"},//120
{"Яркость","Brightness","Brightness","Brightness", "Brightness","Brightness"},                         //121 для нового индикатора
{"ДА","YES","Ano","Si","Ja","YES"},//122
{"tst","tst","tst","tst","tst","tst"},//123
{"мкм","um","um","um","um","um"},                             //124 микро метры
{"Длины волн ","WL AutoSave","Vlnova delka","L.Onda","Wellenlange","WL AutoSave"},  //125
// Для нового окна работы с памятью тестера
{"Память Тестера","Tester's Memory :","Pamet :","Memoria","Speicher:","Memory :"},//126
{"Просмотр","  free","  Volna","Libre","  Frei","  free"},//127 просмотреть
{"Сохранить","Save","Vymazat","Limpiar","Loschen","Save"},//128 сохранить в файл
{"Очистка памяти","Clear memory","Vymazat pamet","Vaciar m.","Speicher loschen","Clear memory"},//129 Очистить
{"Звук","Sound","Sound","Sound","Sound","Sound"},//130 ЗВУК
{"мкc","us","us","us","us","us"},                             //131
{"Переподключите","For read","For read","For read","For read","For read"},//132 переподключите
{"кабель USB","SD Card","SD Card","SD Card","SD Card","SD Card"},//133 кабель USB
{"для чтения","reconnect","reconnect","reconnect","reconnect","reconnect"},//134 для чтения
{"карты памяти","cable USB","cable USB","cable USB","cable USB","cable USB"}//135 карты памяти
};

