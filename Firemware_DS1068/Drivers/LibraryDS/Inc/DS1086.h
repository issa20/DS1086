/*
 * DS1086.h
 *
 *  Created on: Dec 23, 2022
 *      Author: elhomaini.i
 */

#ifndef LIBRARYDS_INC_DS1086_H_
#define LIBRARYDS_INC_DS1086_H_

#include "main.h"
#include "string.h"
#include "stdio.h"
#include "inttypes.h"
//#include "math.h"

#define PRESCALER 0X02
#define DACHIGH   0X08
#define DACLOW    0X09
#define OFFSET    0X0E
#define ADDR      0X0D
#define RANGE     0X37
#define WRITEEE   0X3F



//**************************************************************
// ***************** public function ***************************
//**************************************************************
void ConfigDS1086 (uint32_t frequence) ;
//uint8_t* Table(uint16_t data);
//void Scan_I2C () ;
uint32_t Conversion () ;
void commendHandler () ;
void DS_Delay(uint32_t Delay) ;

//***************************************************************
// **************************************************************




#endif /* LIBRARYDS_INC_DS1086_H_ */
