# DS1086
After searching in google for software libriry for DS1086U+ component i dont find anything, i had decide to developp my own
libaray, that makes programming DS1086U+ esier.

void ConfigDS1086 (uint32_t frequence) ;//this function configure  registers of DS1086U+ to ordre to have frequence in output 
uint32_t Conversion () ;
void commendHandler () ;
void DS_Delay(uint32_t Delay) ;
