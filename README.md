# DS1086
After searching in google for software libriry for DS1086U+ component i dont find any helfull infromation, so i had decide to developp my own
libaray, that makes programming DS1086U+ esier.

Programming the Maxim DS1086U+ oscillator chip with STM32

This repository contains the explanation and STM32 code to program a Maxim DS1086 oscillator chip very quickly. 
i will find folder conatain tow files : 
DS1086.h and DS1086.c in which you find all this function :

# // ***************** public function **************************
void ConfigDS1086 (uint32_t frequence) ;//this function configure  registers of DS1086U+ to ordre to have frequence in output 
uint32_t Conversion () ;
void commendHandler () ;
void DS_Delay(uint32_t Delay) ;
# // ***************** private function **************************
static void WriteByte (uint8_t Adresse);
static void ReadByte (uint8_t Adresse);
static void WriteTowByte (uint8_t Adresse);
//static void WriteEE_Command () ;
static uint32_t power(uint32_t x , uint32_t n ) ;
static void DS_UART_Transmission (uint8_t * data);
static void DS_UART_Reception () ;
static uint32_t DS_GetTick() ;
static void DS_init_TIM14 () ;
