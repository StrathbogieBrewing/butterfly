//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

void USART_Init(unsigned int baudrate);

void USART_Tx(char);
char USART_Rx(void);
void USART_EOL(char eol);

// send one char as readable text
void USART_Tx_Hex(char data);
void USART_Tx_Byte(char data);
void USART_Tx_Bool(char data);


#ifdef EXTENDED_USART
void USART_sendmsg (const char *s);
void USART_sendADC(char channel,char invert);
void USART_setTheClock(char target);
char USART_getValue(char *value, char lowerbound, char upperbound);
void USART_changeInterval(void);
void USART_Tx_Int(unsigned int data);

// comment out lines below to remove 
// commands from the usart interface
// and reduce total code size
#define USART_CMD_ALARM
#define USART_CMD_BDUMP
#define USART_CMD_CLOCK
#define USART_CMD_DUMP
#define USART_CMD_EOL
#define USART_CMD_ROLLOVER
#define USART_CMD_WINDINTERVAL
#define USART_CMD_INTERVAL
#define USART_CMD_LIGHT
#define USART_CMD_LOGNOW
#define USART_CMD_LOGSTATUS
#define USART_CMD_PWRSAVE
#define USART_CMD_DS18X20
#define USART_CMD_RESET
#define USART_CMD_SLEEP
#define USART_CMD_TEMP
#define USART_CMD_INTERVAL
#define USART_CMD_BATTERY
#define USART_CMD_WINDSPEED
#define USART_CMD_STATUS
#define USART_CMD_DISPLAY_TEXT

#endif

extern char gUART;

#define USART_ERRORMAX 3

// add support for multiple EOL chars
extern char gEOL;
#define	EOL_NONE	0
#define EOL_MAC		1	
#define EOL_UNIX	2
#define	EOL_WIN		(EOL_MAC | EOL_UNIX)
#define EOL_TERM	4

#define DEFAULT_EOL EOL_WIN

