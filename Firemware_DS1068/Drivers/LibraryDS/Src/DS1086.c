/*
 * DS1086.c
 *
 *  Created on: Dec 23, 2022
 *      Author: elhomaini.i
 */

#include "DS1086.h"

uint8_t data_read[10];
uint8_t data_send[50];
uint8_t DS_data[30]=" " ;
uint8_t DS_Wait_Receiption=0,DS_Wait_Transmission=0;
uint8_t ADD_Device=0XB0;
uint8_t Add_PRESCALER=0x02 ;
int8_t range = 6 , previousRange=0 ;
int8_t prescaler=8 ,previousPrescaler=0;
uint32_t Fmin[] = {30740,33300,35860,38420,40980,43540,46100,48660,51220,53780,56340,58900,61460} ;
uint32_t Sweep_Start=400,Sweep_End=1000,Sweep_Step=10 ;
uint32_t DS_TIM14_Count=0 ;

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc;
extern TIM_HandleTypeDef htim14;

// *************************************************************
// ***************** private function **************************
// *************************************************************

static void WriteByte (uint8_t Adresse);
static void ReadByte (uint8_t Adresse);
static void WriteTowByte (uint8_t Adresse);
//static void WriteEE_Command () ;
static uint32_t power(uint32_t x , uint32_t n ) ;
static void DS_UART_Transmission (uint8_t * data);
static void DS_UART_Reception () ;
static uint32_t DS_GetTick() ;
static void DS_init_TIM14 () ;


// *************************************************************
// ******************** Public function ************************
// *************************************************************

void ConfigDS1086 (uint32_t frequence){ // frequence en KHz
	uint32_t freq=0 ;
	for(int i=0; i < 9;  i++ ){
		freq = frequence * power(2,i);
		if( (30740 < freq) && (freq < 35860 )){
			prescaler = i ;
			range = - 6 ;
			break ;
		}
		if( (33300 < freq) && (freq < 38420 )){
			prescaler = i ;
			range = -5 ;
			break ;
		}
		if( (35860 < freq) && (freq < 40980 )){ //35.86 to 40.98
			prescaler = i ;
			range = -4 ;
			break ;
		}
		if( (38420 < freq) && (freq < 43540 )){ //38.42 to 43.54
				prescaler = i ;
				range = -3 ;
				break ;
		}
		if( (40980 < freq) && (freq < 46100 )){ //40.98 to 46.10
				prescaler = i ;
				range = -2 ;
				break ;
		}
		if( (43540 < freq) && (freq < 48660 )){ //43.54 to 48.66
				prescaler = i ;
				range = -1 ;
				break ;
		}
		if( (46100 < freq) && (freq < 51220 )){ //46.10 to 51.22
				prescaler = i ;
				range = 0 ;
				break ;
		}
		if( (48660 < freq) && (freq < 53780 )){ //48.66 to 53.78
				prescaler = i ;
				range = 1 ;
				break ;
		}
		if( (51220 < freq) && (freq < 56340 )){ //51.22 to 56.34
				prescaler = i ;
				range = 2 ;
				break ;
		}
		if( (53780 < freq) && (freq < 58900 )){ //53.78 to 58.90
				prescaler = i ;
				range = 3 ;
				break ;
		}
		if( (56340 < freq) && (freq < 61460 )){ //56.34 to 61.46
				prescaler = i ;
				range = 4 ;
				break ;
		}
		if( (58900 < freq) && (freq < 64020 )){ //58.90 to 64.02
				prescaler = i ;
				range = 5 ;
				break ;
		}
		if( (61460 < freq) && (freq < 66580 )){ //61.46 to 66.58
				prescaler = i ;
				range = 6 ;
				break ;
		}
	}

	if((previousRange!=range)||(previousPrescaler!=prescaler)){
	data_send[0]=  (0x0C & prescaler) >> 2;
	data_send[1]=  (0x03 & prescaler) << 6 ; //
	WriteTowByte (PRESCALER) ; //fMASTER OSCILLATOR = fDESIRED x prescaler = fDESIRED x 2^x
	//WriteEE_Command () ;
	ReadByte(RANGE);

	data_send[0]= ((data_read[0] & 0X1F) + range) & 0X1F ; // OFFSET = OS + range
	WriteByte(OFFSET) ;
	}

	int32_t DAC_Value = (int32_t )(((frequence * power(2,prescaler))  - Fmin[range+6])/5) ;
	DAC_Value = DAC_Value<<6 ;
	data_send[0]= (DAC_Value & 0xFF00)>>8 ; // F_OUTPUT= (Fmin + DAC * 5kHz )/PRESCALER
	data_send[1]= DAC_Value & 0x00C0 ;
	WriteTowByte (DACHIGH) ;

	previousRange = range ;
	previousPrescaler = prescaler ;
}

/*void Scan_I2C (){

	uint8_t addDevice=0X00 ;
	for(int i = 0 ; i<=255; i++){

		if(HAL_OK==HAL_I2C_Master_Transmit(&hi2c1, addDevice, &Add_PRESCALER, 1, 200)){
			sprintf((char*)data_read,"%x%s",addDevice," ");
			HAL_UART_Transmit_IT(&huart2,data_read ,strlen((char*)data_read));
		}
		else {
			sprintf((char*)data_read,"%s","- ");
			HAL_UART_Transmit_IT(&huart2,data_read ,strlen((char*)data_read));
		}

		addDevice=addDevice + 0X1;
		HAL_Delay(100);
	}


	strcpy((char*)data_send," Done\n");
	HAL_UART_Transmit_IT(&huart2,data_send ,strlen((char*)data_send));
	memset(data_send,'\0',10*sizeof(char));


}*/

uint32_t Conversion (){
	uint32_t ValeurADC=0 ;
	uint8_t valeur[20]=" ";
	uint8_t data[20]=" ";

	HAL_GPIO_WritePin(GPIOB, DA_DISCH_Pin, GPIO_PIN_RESET);
	DS_Delay(500) ;
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc,HAL_MAX_DELAY);
	ValeurADC = HAL_ADC_GetValue(&hadc);
	DS_Delay(100) ;

	uint32_t Vreal= ValeurADC*806 ; // uV pow(2,12)  ADC 12BIT
	memset(valeur,'\0',20*sizeof(char));
	sprintf((char*)valeur,"%"PRIu32"",Vreal); //%"PRIu32"

	if(strlen((char*)valeur)==4){
		sprintf((char*)data,"%s%c%s","0,00",valeur[0]," \n\r");
		DS_UART_Transmission(data);
	}
	if(strlen((char*)valeur)==5){
		sprintf((char*)data,"%s%c%c%s","0,0",valeur[0],valeur[1]," \n\r");
		DS_UART_Transmission(data);
	}
	if(strlen((char*)valeur)==6){
		sprintf((char*)data,"%s%c%c%c%s","0,",valeur[0],valeur[1],valeur[2]," \n\r");
		DS_UART_Transmission(data);
	}
	if(strlen((char*)valeur)>6){
		sprintf((char*)data,"%c%c%c%c%c%s",valeur[0],',',valeur[1],valeur[2],valeur[3]," \n\r");
		DS_UART_Transmission(data);
	}
	if(strlen((char*)valeur)<=3) {
		sprintf((char*)data,"%s","0 \n\r");
		DS_UART_Transmission(data);
	}



	HAL_GPIO_WritePin(GPIOB, DA_DISCH_Pin, GPIO_PIN_SET);

	return ValeurADC ;
}

/*static void WriteEE_Command (){
	int8_t Data[]={0x3F} ;
	HAL_I2C_Master_Transmit(&hi2c1, ADD_Device ,Data , 1, HAL_MAX_DELAY);
	HAL_Delay(100);
}

uint8_t* Table(uint16_t data){
	data_send[0]= data&0X00FF;
	data_send[1]= (data&0XFF00)>>8;
	return data_send;
}*/


void commendHandler (){
	uint32_t Val_Cmd=0 ;
	uint8_t data[30] ;

	DS_UART_Reception();


	Val_Cmd = (DS_data[1]-48)*10000 +(DS_data[2]-48)*1000 + (DS_data[3]-48)*100 + (DS_data[4]-48)*10 + DS_data[5]-48 ;
	switch(DS_data[0]){
	case 'F':
				if(Val_Cmd <= 10000){
					ConfigDS1086 (Val_Cmd) ;
					DS_UART_Transmission (DS_data);
					DS_UART_Transmission ((uint8_t *)" : ");
					Conversion () ;
				}
				else {DS_UART_Transmission ((uint8_t *)"Maximum frequency 10MHz\n\r");}
				break ;
	case 'T':
				if(Val_Cmd==11111){
					HAL_GPIO_WritePin(GPIOB, DA_TEST_Pin, GPIO_PIN_SET);
					DS_UART_Transmission((uint8_t *)"Circuit_test_Enable\n\r");

				}
				else {
					HAL_GPIO_WritePin(GPIOB, DA_TEST_Pin, GPIO_PIN_RESET);
					DS_UART_Transmission((uint8_t *)"Circuit_test_Disable\n\r");
				}
				break ;
	case 'D':
				if(Val_Cmd==11111){
					HAL_GPIO_WritePin(GPIOB, DA_DISCH_Pin, GPIO_PIN_SET);
					DS_UART_Transmission((uint8_t *)"DA_DISCH_Enable\n\r");

				}
				else {
					HAL_GPIO_WritePin(GPIOB, DA_DISCH_Pin, GPIO_PIN_RESET);
					DS_UART_Transmission((uint8_t *)"DA_DISCH_Disable\n\r");
				}
				break ;
	case 'X':
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"  ");
				if(Val_Cmd>=400) {
					Sweep_Start=Val_Cmd ;
					DS_UART_Transmission ((uint8_t *)"Start_Scan ");
					DS_UART_Transmission (DS_data+1);
					DS_UART_Transmission ((uint8_t *)"\n\r");
				}
				else {Sweep_Start=400 ;DS_UART_Transmission ((uint8_t *)"Minimum frequency 400KHz\n\r");}
				break ;
	case 'C':
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"  ");
				if(Val_Cmd<=10000 ) {
					Sweep_End=Val_Cmd ;
					DS_UART_Transmission ((uint8_t *)"End_Scan ");
					DS_UART_Transmission (DS_data+1);
					DS_UART_Transmission ((uint8_t *)"\n\r");

				}
				else {Sweep_End=10000 ;DS_UART_Transmission ((uint8_t *)"Maximum frequency 10MHz\n\r");}
				break ;
	case 'S':
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"  ");
				if(Val_Cmd>=10) {
					Sweep_Step=Val_Cmd ;
					DS_UART_Transmission ((uint8_t *)"Step_Scan ");
					DS_UART_Transmission (DS_data+1);
					DS_UART_Transmission ((uint8_t *)"\n\r");
				}
				else { Sweep_Step=10 ; DS_UART_Transmission ((uint8_t *)"Step min=10\n\r"); }
				break ;
	case 'B' :
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"\n\r");
				if(Sweep_Start < Sweep_End){
					for(uint32_t i= Sweep_Start; i <= Sweep_End; i+=Sweep_Step) {
						sprintf((char*)data,"%"PRIu32"%s",i," : ");
						DS_UART_Transmission(data);
						ConfigDS1086 (i) ;
						Conversion () ;
					}
			    }
				else {
					DS_UART_Transmission ((uint8_t *)"t'es con ou quoi Sweep_Start > Sweep_End\n\r");
					}
				break;
	case 'I':
				DS_UART_Transmission((uint8_t *)"Version 10/01/2023 Creator : EL-HOMAINI ISSA \n\r");
				break ;
	case 'H':
				DS_UART_Transmission((uint8_t *)"************************************************\n\r");
				DS_UART_Transmission((uint8_t *)"**************ALL AVAILABE COMMAND**************\n\r");
				DS_UART_Transmission((uint8_t *)"***+++++++++++++++++++++++++++++++++++++++++****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ All command have sex caracter         +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ Fx: Give value at x frequency         +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ T11111: Enable Cicuit Test            +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ T!: Disnable Cicuit Test              +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ D11111: DA_DISCH Enable               +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ D!: DA_DISCH Disnable                 +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ Xx: Start sweeping at x frequency     +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ Cx: End sweeping at x frequency       +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ Sx: Step sweeping x                   +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ I!: Version software                  +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ M!: Mesure consumption                +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ A!: Secure mode                       +****\n\r");
				DS_UART_Transmission((uint8_t *)"***+ !:any caracter x:number 5 digits      +****\n\r");
				DS_UART_Transmission((uint8_t *)"***++++++++++++++++++++++++++++++++++++++++*****\n\r");
				DS_UART_Transmission((uint8_t *)"************************************************\n\r");
				break ;
	case 'M':
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"\n\r");
				ConfigDS1086 (995) ;
				HAL_UART_Receive_IT(&huart2,DS_data,1);
				DS_Wait_Receiption=0;
				do{
					Conversion () ;
				}while(!DS_Wait_Receiption);
				break ;

	case 'A':
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission((uint8_t *)"\n\rPassword :");
				DS_UART_Reception ();
				//DS_UART_Transmission((uint8_t *)"\n\r");
				if(!strcmp((char*)DS_data,(char*)"ISSAAA")){
					DS_UART_Transmission((uint8_t *)"Correct \n\r");
					DS_UART_Transmission((uint8_t *)"you are in secure mode \n\r");
					DS_UART_Transmission((uint8_t *)"Now you can configure frequency intel 66MHz  \n\r");
					//UART_Transmission((uint8_t *)"Now you can configure frequence intel 66MHz  \n\r");
					DS_UART_Reception ();
					Val_Cmd = (DS_data[1]-48)*10000 +(DS_data[2]-48)*1000 + (DS_data[3]-48)*100 + (DS_data[4]-48)*10 + DS_data[5]-48 ;
					if(Val_Cmd <= 66000){
						ConfigDS1086 (Val_Cmd) ;
						DS_UART_Transmission (DS_data);
						DS_UART_Transmission ((uint8_t *)" : ");
						Conversion () ;
					}
					else {DS_UART_Transmission ((uint8_t *)"Maximum frequency 66MHz\n\r");}

				}
				else{
					DS_UART_Transmission((uint8_t *)"Eror\n\r");
				}

				break ;
	default :
				DS_UART_Transmission (DS_data);
				DS_UART_Transmission ((uint8_t *)"\n\r");
				DS_UART_Transmission ((uint8_t *)"fais attention a ce que tu m'ecris \n\r");
				break;

	}


}



void DS_Delay(uint32_t Delay)// Daley en 1us
{
  DS_init_TIM14 ();
  HAL_TIM_Base_Start_IT(&htim14) ;

  uint32_t tickstart = DS_GetTick();
  uint32_t wait = Delay;

  /* Add a freq to guarantee minimum wait */
  if (wait < HAL_MAX_DELAY)
  {
    wait += (uint32_t)(uwTickFreq);
  }

  while((DS_GetTick() - tickstart) < wait)
  {
  }
  HAL_TIM_Base_Stop_IT(&htim14) ;
}
// *************************************************************
// ***************** private function **************************
// *************************************************************

static void WriteByte (uint8_t Adresse ){

	if(HAL_OK!=HAL_I2C_Mem_Write(&hi2c1, ADD_Device , Adresse,1, data_send, 1, HAL_MAX_DELAY)) {
		strcpy((char*)data_read,"Er");
		HAL_UART_Transmit_IT(&huart2,data_read ,strlen((char*)data_read));
		HAL_Delay(100);
	}

}

static void ReadByte (uint8_t Adresse){

	if(HAL_OK!=HAL_I2C_Mem_Read(&hi2c1,ADD_Device, Adresse ,1, data_read, 1, HAL_MAX_DELAY)) {
		strcpy((char*)data_read,"Er");
		HAL_UART_Transmit_IT(&huart2,data_read ,strlen((char*)data_read));
		HAL_Delay(100);
	}
}

static void WriteTowByte (uint8_t Adresse){

	if(HAL_OK!=HAL_I2C_Mem_Write(&hi2c1, ADD_Device , Adresse,1, data_send,2, HAL_MAX_DELAY)) {
		strcpy((char*)data_read,"Er");
		HAL_UART_Transmit_IT(&huart2,data_read ,strlen((char*)data_read));
		HAL_Delay(100);
	}

}

static uint32_t power(uint32_t x , uint32_t n ) {
	uint32_t p=1;
	for(int i = 0 ; i < n ; i++){
		p*=x;
	}
	return p;
}

static uint32_t DS_GetTick(){
	return DS_TIM14_Count ;
}
static void DS_init_TIM14 (){
	DS_TIM14_Count=0 ;
}
// *************************************************************
// ************************* Callback **************************
// *************************************************************

static void DS_UART_Transmission (uint8_t * data){
	memset(data_send,'\0',50*sizeof(char));
	sprintf((char*)data_send,"%s",data);

	DS_Wait_Transmission=0;
	HAL_UART_Transmit_IT(&huart2,data_send , strlen((char *)data_send));
	while(!DS_Wait_Transmission);
}

static void DS_UART_Reception (){
	DS_Wait_Receiption=0;
	memset(DS_data,'\0',10*sizeof(char));
	HAL_UART_Receive_IT(&huart2,DS_data,6);
	while(!DS_Wait_Receiption);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	DS_Wait_Receiption=1;
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	DS_Wait_Transmission=1;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if( htim==&htim14){
		DS_TIM14_Count+=10;
	}
}
