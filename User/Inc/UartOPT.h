/*****************************************************************************
 *   uart.h:  Header file for NXP LPC23xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __UARTOPT_H 
#define __UARTOPT_H


// Прием байта по UART1 OPTICS
void RS_com_Optic (BYTE temp);
WORD GetNumWaveOpt (void); // возвращает длину волны или ноль если не принял


extern char wrd[10];
extern BYTE RX_BufOpt[BUFSIZEUART1];
extern BYTE RSOptYes;

#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
