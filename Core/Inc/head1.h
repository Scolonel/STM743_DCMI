0x64,0x00,                                          // 0-1 ������ Map-����� 1.00            ==2
0x52,0x00,0x00,0x00,                                // 2-5 ������ ����� � ������ 82 = 0x52  ==6
0x06,0x00,                                          // 6-7 ����� ������ � �����             ==8    
0x47,0x65,0x6E,0x50,0x61,0x72,0x61,0x6D,0x73,0x00,  // 8-17 ���� 'GenParams'                 ==
0x64,0x00,                                          // 18-19 ��� ������ 1.00
0x24,0x00,0x00,0x00,                                // 20-23 ��� ������ 36 = 0x24
0x53,0x75,0x70,0x50,0x61,0x72,0x61,0x6D,0x73,0x00,  // 24-33 ���� 'SupParams'
0x65,0x00,                                          // 34-35 ��� ������ 1.01
0x29,0x00,0x00,0x00,                                // 36-39 ��� ������ 40 = 0x28 // ��� 4-� ������� ������� 0x29
0x46,0x78,0x64,0x50,0x61,0x72,0x61,0x6D,0x73,0x00,  // 40-49���� 'FxdParams'
0x65,0x00,                                          // 50-51 ��� ������ 1.01
0x36,0x00,0x00,0x00,                                // 52-55 ��� ������ 54 = 0x36  
// c 56 ����� ����� ���� �������� ���� 'KeyEvents' �������� 16 ����
0x44,0x61,0x74,0x61,0x50,0x74,0x73,0x00,            // 56-63 ���� 'DataPts'           ----
0x65,0x00,                                          // 64-65 ��� ������ 1.01
//0x8C,0x3E,0x00,0x00,                                // ��� ������ 0x0C,0x20,  8192 = 0x1000 //4096 ���� + 12 ���� ���������� // stm2h743 8000*2+12=0x3E8C
//0x0C,0x20,0x00,0x00,                                // ��� ������ 0x0C,0x20,  8192 = 0x1000 //4096 ���� + 12 ���� ���������� // stm2h743 5200*2+12=0x28AC
0xAC,0x28,0x00,0x00,                                // 66-69��� ������  stm2h743 5200*2+12=0x28AC
0x43,0x6B,0x73,0x75,0x6D,0x00,                      // 70-75 ���� 'Cksum'
0x64,0x00,                                          // 76-77 ��� ������ 1.00
0x02,0x00,0x00,0x00,                                // 78-81 ��� ������.   (82 byte)
//--------    // ���� 'GenParams' == 36 ���� (82)
0x45,0x4E,                                          // 82-83 LC ���� EN
0x00,                                               // 84 CID Cable ID
0x00,                                               // 85 FID Fiber ID
0xFF,0xFF,                                          // 86-87 NW ����� ����� 1310 (87 ����)
0x00,                                               // 88 OL Originating Location
0x00,                                               // 89 TL Terminating Location
0x00,                                               // 90 CCD Cable Code
0x42,0x43,                                          // 91-92 CDF Current Data Flag (BC, RC, OT)
0x00,0x00,0x00,0x00,                                // 93-96 UO �������� ����������� ������������� � 100 ��
0x00,                                               // 97 OP Name or Operator Code 
0xFF,0xFF,0xFF,0xFF,0xFF,                      // 98-117 CMT ����������� �� ������ '�race ' begin 99
0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,                      
0xFF,0xFF,0xFF,0xFF,0x00,                                //###(105) CMT ����������� ����������� ����� ������ 3 ����� (i=)
//--------    // ���� 'SupParams' ==  40 ���� (118)  (
                                                                  // SN - ��� ��������    
//0x22,0x43,0x6F,0x6D,0x6D,0x53,0x65,0x72,0x76,0x69,0x63,0x65,0x20, // "CommService"
//0x22,0x00,                                              //
'O','P','T','O','K','O','N',' ','C','o','.','L','t','d',0x00, //118-132
//0x54,0x4F,0x50,0x41,0x5A,0x2D,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00, // MFID - ������ ������� TOPAZ-710XR  (133 byte) 
'M','O','T','7','0','0',0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00, //133-145
0xFF,0xFF,0xFF,0xFF,0x00,                          // 146-150 OTDR �������� ����� ������� s/n 000 (146 byte) //4-x �������!!!
0x00,                                               // 151 OMID
0x00,                                               // 152 OMSN
'4','.','0','a',0x00,                      // 153-157 SR ������ �� v4.0a (152)
0x00,                                               // 158 Other
//--------    // ���� 'FxdParams' == 54 ���� (158) 
0xFF,0xFF,0xFF,0xFF,                                //###(159-162) DTS ������� �����   (i=183,;
0x6B,0x6D,                                          // 163-164 UD ������� ��������� km  
0xFF,0xFF,                                          // (165-166)AW ����� ����� � 0.1 �� (1310�� = 13100) 0x332c 0x3c8c (164)
0x00,0x00,0x00,0x00,                                // 167-170 AO �������� ����� ������
0x01,0x00,                                          // 171-172 TPW ���������� ������ ��������� (����)
0xFF,0xFF,                                          //###(173-174) PWU ������ ������ �������� (ns) 150ns=0x96, 500ns=0x1f4, 1000ns=0x3e8, 3000ns=0xbb8
0xFF,0xFF,0xFF,0xFF,                                //###(175-178)  DS data Spacing ����� ����� ������� ��������� � 100��
                              // DS = (GI*L)/(C*10e-14) L - ���������� ���������� ����� ������� ��������� �- �������� �����
                              // GI - �����. �����������, �� ������� ������������ ����� ��������� (� ��/2)*100000 = DS
                              // 10�� = 0x27BC86, 20km=0x4F790D, 40km=9EF21A, 80km=13DE435
0x50,0x14,0x00,0x00,                                // 179-182 NPPW ���������� ����� (5200)//stm32h743 = 8000
0xFF,0xFF,0xFF,0xFF,                                // ###(183-186) GI ����� �����������  146583 (1.46583)                           
0x20,0x03,                                          // ###(187-188) BC = 800 ������ ��������� ���������
0xFF,0xFF,0xFF,0xFF,                                // ###(189-192) NAV ����� ���������� - ������� �� ������� ���������� � ����� ����� = sycl
0xFF,0xFF,0xFF,0xFF,                                // ###(193-196) AR  ����� ����������� ������� (����� ����� ��������� �� ���) DS*NPPW/10000
0x00,0x00,0x00,0x00,                                // ###(197-200) FPO �������� ���������� (������ ����� ������) ����� 0
0x00,0x00,                                          // ###(201-202) NF ������ ������� ����� ����� 0
0xE8,0x03,                                          // ###(203-204) NFSF ���������� �����  ������� ����� ����� 1000
0x00,0x00,                                          // ###(205-206) PO �������� �� �������� ����� 0 � 0.001 ��
0xC8,0x00,                                          // ###(207-208) LT ����������� �������� ��������� ��� ������� 200 � 0.001 ��
0x40,0x9C,                                          // ###(209-210) RT ����������� �������� ��������� ��� ������� 40000 � -0.001��
0x68,0x06,                                          // ###(211-212) ET ����������� ��������� ��� ����� ����� 3000 � 0.001 ��
// ���� ���� ������� �� ����� ��������� ���� �������
//--------    // ���� 'DataPts' == 12 ���� (212)
//0x00,0x10,0x00,0x00,                                // TNDP ���������� ����� (4096)// stm32h743 = 8000
0x50,0x14,0x00,0x00,                                // 213-216 TNDP ���������� ����� stm32h743 = 5200
0x01,0x00,                                          // 217-218 TSF ����� ����
//0x00,0x10,0x00,0x00,                                // TNS1 ���������� ����� �� ����� (4096)
0x50,0x14,0x00,0x00,                                // 219-222 TNS1 ���������� ����� �� ����� (5200)
0xE8,0x03,                                          // 223-224 SF1 ������� �����
// ���������� ����� ������� ����������� ����� 56 ����
'K','e','y','E','v','e','n','t','s',0x00,0x65,0x00,0x3E,0x00,0x00,0x00 // ������� ���� ������� 32+22 ����

// 224 ���� + 16 ���� ��� �������
// ����� ���� ������
// ������������ � ������� ����� �������
// � 212 ����� (��� ��������� � �������� - � ���������� � 56 ����� (����� �� ��������� 16 ����)
// ��� ������ ���� ���� ������� �� ��������� ����������� � ���� �� ��������� 16 ����
// 'K','e','y','E','v','e','n','t','s',0x00 (10+6) ����� � 228 
// ������ ����� ������������� �� ����� ������� 
// 2 ����� ������������ (����� ����� �������)
// ��������� ������� (32�����) �� ������ �������
// + 4(ELL) + 4(EPT1) + 4(EPT) + 2(ORL) + 8(RLMP) (������ ��� ����� long � ����� 0 (���� �� ����������))
// ����� N*32+22 +2 (����� �������)=
