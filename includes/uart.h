extern int initUart();
extern float requestTemperatureToUart(int, int);
extern int requestKeyToUart(int, unsigned char);
extern void sendIntToUart(int, int, int);
extern void sendFloatToUart(int, int, float);
extern void sendByteToUart(int, int, char);
#define TEMP_INT 0xC1
#define TEMP_REF 0XC2
#define USER_COMM 0XC3
#define CONTROL_SIGNAL 0XD1
#define REFERENCE_SIGNAL 0XD2
#define SYSTEM_STATE 0XD3
#define CTR_TEMPERATURE 0XD4
#define WORKING_STATE 0XD5
#define TIMER 0XD6