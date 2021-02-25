//
//Atmel AVR Design Contest 2006 Registration Number AT3221
//

// menu.h

// mt __flash typedef struct
typedef struct PROGMEM
{
    unsigned char state;
    unsigned char input;
    unsigned char nextstate;
} MENU_NEXTSTATE;


// mt __flash typedef struct
typedef struct PROGMEM
{
    unsigned char state;
    // char __flash *pText;
	PGM_P pText;
    char (*pFunc)(char input);
} MENU_STATE;

#if NO_MENUS
const char MT_AVRBF[] PROGMEM                    = "Butterfly Logger";
#else
#ifdef MINIMAL_MENU 

// Menu text
// mtA, these where all of the same structure as in the follow. line
// __flash char MT_AVRBF[]                         = "AVR Butterfly";

const char MT_AVRBF[] PROGMEM                     = "Butterfly Logger";
const char MT_TIME[]   PROGMEM                     = "Time";
const char MT_TIME_CLOCK[]   PROGMEM               = "Clock";
const char MT_TIME_CLOCK_ADJUST[]   PROGMEM        = "Adjust Clock";
//const char MT_TIME_CLOCKFORMAT_ADJUST[]  PROGMEM   = "Change Format";
const char MT_TIME_DATE[]  PROGMEM                 = "Date";
const char MT_TIME_DATE_ADJUST[]    PROGMEM        = "Adjust Date";
//const char MT_TIME_DATEFORMAT_ADJUST[]  PROGMEM    = "Change Format";/*
//const char MT_MUSIC[]   PROGMEM                    = "Music";
//const char MT_VCARD[]  PROGMEM                     = "Name";
//const char MT_ENTERNAME[]  PROGMEM                 = "Enter name";*/
const char MT_TEMPERATURE[]  PROGMEM               = "Temp";
const char MT_VOLTAGE[]  PROGMEM                   = "Battery";
const char MT_DIR[]  PROGMEM			 = "Direction";
const char MT_LIGHT[]  PROGMEM                     = "Light";
const char MT_SPEED[]	 PROGMEM 		= "Speed";
const char MT_OPTIONS[]  PROGMEM                   = "Options";
//const char MT_OPTIONS_DISPLAY[]  PROGMEM           = "Display";
//const char MT_OPTIONS_DISPLAY_CONTRAST[]  PROGMEM  = "Contrast";
//const char MT_OPTIONS_DISPLAY_SEG[]  PROGMEM       = "Browse segments";
//const char MT_VCARD_DOWNLOAD_NAME[]  PROGMEM       = "Download Name";
const char MT_OPTIONS_BOOT[]  PROGMEM              = "Bootloader";
const char MT_OPTIONS_RESET[]	 PROGMEM 		 = "Reset";
const char MT_OPTIONS_POWER_SAVE[]  PROGMEM        = "Power Save";
const char MT_OPTIONS_AUTO_POWER_SAVE[]  PROGMEM   = "Auto Power Save";
//const char MT_OPTIONS_TEST[]  PROGMEM              = "Test Mode";
const char MT_OPTIONS_UPLOAD[]	 PROGMEM 		= "Upload";
//const char MT_OPTIONS_START[]	 PROGMEM 		= "Logging";
const char MT_OPTIONS_LOGTIME[]	 PROGMEM 		= "Log Interval";
 

// mt MENU_NEXTSTATE menu_nextstate[] = { 
const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
//  STATE                       INPUT       NEXT STATE
    {ST_AVRBF,                  KEY_PLUS,   ST_OPTIONS},
    {ST_AVRBF,                  KEY_NEXT,   ST_AVRBF_REV},
    {ST_AVRBF,                  KEY_MINUS,  ST_TIME},

    {ST_AVRBF_REV,              KEY_PREV,   ST_AVRBF},


    {ST_TIME,                   KEY_PLUS,   ST_AVRBF},
    {ST_TIME,                   KEY_NEXT,   ST_TIME_CLOCK},
    {ST_TIME,                   KEY_PREV,   ST_AVRBF},
    {ST_TIME,                   KEY_MINUS,  ST_TEMPERATURE},
    
    {ST_TIME_CLOCK,             KEY_PLUS,   ST_TIME_DATE},
    {ST_TIME_CLOCK,             KEY_NEXT,   ST_TIME_CLOCK_FUNC},
    {ST_TIME_CLOCK,             KEY_PREV,   ST_TIME},
    {ST_TIME_CLOCK,             KEY_MINUS,  ST_TIME_DATE},

//    {ST_TIME_CLOCK_ADJUST,      KEY_PLUS,   ST_TIME_CLOCKFORMAT_ADJUST}, 
    {ST_TIME_CLOCK_ADJUST,      KEY_ENTER,  ST_TIME_CLOCK_ADJUST_FUNC},
    {ST_TIME_CLOCK_ADJUST,      KEY_PREV,   ST_TIME_CLOCK_FUNC},    
 //   {ST_TIME_CLOCK_ADJUST,      KEY_MINUS,  ST_TIME_CLOCKFORMAT_ADJUST}, 

 //   {ST_TIME_CLOCKFORMAT_ADJUST, KEY_PLUS,  ST_TIME_CLOCK_ADJUST},
//    {ST_TIME_CLOCKFORMAT_ADJUST, KEY_ENTER, ST_TIME_CLOCKFORMAT_ADJUST_FUNC},
 //   {ST_TIME_CLOCKFORMAT_ADJUST, KEY_PREV,  ST_TIME_CLOCK_FUNC},
 //   {ST_TIME_CLOCKFORMAT_ADJUST, KEY_MINUS, ST_TIME_CLOCK_ADJUST},

    {ST_TIME_DATE,              KEY_PLUS,   ST_TIME_CLOCK},
    {ST_TIME_DATE,              KEY_NEXT,   ST_TIME_DATE_FUNC},
    {ST_TIME_DATE,              KEY_PREV,   ST_TIME},
    {ST_TIME_DATE,              KEY_MINUS,  ST_TIME_CLOCK},

//    {ST_TIME_DATE_ADJUST,       KEY_PLUS,   ST_TIME_DATEFORMAT_ADJUST},
    {ST_TIME_DATE_ADJUST,       KEY_ENTER,  ST_TIME_DATE_ADJUST_FUNC},
    {ST_TIME_DATE_ADJUST,       KEY_PREV,   ST_TIME_DATE_FUNC},
//    {ST_TIME_DATE_ADJUST,       KEY_MINUS,  ST_TIME_DATEFORMAT_ADJUST},
        
//    {ST_TIME_DATEFORMAT_ADJUST, KEY_PLUS,   ST_TIME_DATE_ADJUST},
//    {ST_TIME_DATEFORMAT_ADJUST, KEY_ENTER,  ST_TIME_DATEFORMAT_ADJUST_FUNC},
//   {ST_TIME_DATEFORMAT_ADJUST, KEY_PREV,   ST_TIME_DATE_FUNC},
//    {ST_TIME_DATEFORMAT_ADJUST, KEY_MINUS,  ST_TIME_DATE_ADJUST},

//  {ST_MUSIC,                  KEY_PLUS,   ST_TIME},
//  {ST_MUSIC,                  KEY_NEXT,   ST_MUSIC_SELECT},
//  {ST_MUSIC,                  KEY_PREV,   ST_AVRBF},
//  {ST_MUSIC,                  KEY_MINUS,  ST_VCARD},

//    {ST_SOUND_MUSIC,            KEY_NEXT,   ST_MUSIC_SELECT},
//    {ST_SOUND_MUSIC,            KEY_PREV,   ST_MUSIC},
/*
    {ST_VCARD,                  KEY_PLUS,   ST_TIME},
    {ST_VCARD,                  KEY_NEXT,   ST_VCARD_FUNC},
    {ST_VCARD,                  KEY_PREV,   ST_AVRBF},
    {ST_VCARD,                  KEY_MINUS,  ST_TEMPERATURE},
       
    {ST_VCARD_ENTER_NAME,       KEY_PLUS,     ST_VCARD_DOWNLOAD_NAME},
    {ST_VCARD_ENTER_NAME,       KEY_ENTER,    ST_VCARD_ENTER_NAME_FUNC},
    {ST_VCARD_ENTER_NAME,       KEY_PREV,     ST_VCARD_FUNC},    
    {ST_VCARD_ENTER_NAME,       KEY_MINUS,    ST_VCARD_DOWNLOAD_NAME},
    
    {ST_VCARD_DOWNLOAD_NAME,    KEY_PLUS,     ST_VCARD_ENTER_NAME},
    {ST_VCARD_DOWNLOAD_NAME,    KEY_ENTER,    ST_VCARD_DOWNLOAD_NAME_FUNC},
    {ST_VCARD_DOWNLOAD_NAME,    KEY_PREV,     ST_VCARD_FUNC},    
    {ST_VCARD_DOWNLOAD_NAME,    KEY_MINUS,    ST_VCARD_ENTER_NAME},    
  */  
    {ST_TEMPERATURE,            KEY_PLUS,   ST_TIME},
    {ST_TEMPERATURE,            KEY_NEXT,   ST_TEMPERATURE_FUNC},
    {ST_TEMPERATURE,            KEY_PREV,   ST_AVRBF},
    {ST_TEMPERATURE,            KEY_MINUS,  ST_VOLTAGE},
    
    {ST_VOLTAGE,                KEY_PLUS,   ST_TEMPERATURE},
    {ST_VOLTAGE,                KEY_NEXT,   ST_VOLTAGE_FUNC},
    {ST_VOLTAGE,                KEY_PREV,   ST_AVRBF},
    {ST_VOLTAGE,                KEY_MINUS,  ST_DIR},
	
    {ST_DIR,                	KEY_PLUS,   ST_VOLTAGE},
    {ST_DIR,                	KEY_NEXT,   ST_DIR_FUNC},
    {ST_DIR,                	KEY_PREV,   ST_AVRBF},
    {ST_DIR,                	KEY_MINUS,  ST_LIGHT},
	
    {ST_LIGHT,                  KEY_PLUS,   ST_DIR},
    {ST_LIGHT,                  KEY_NEXT,   ST_LIGHT_FUNC},
    {ST_LIGHT,                  KEY_PREV,   ST_AVRBF},
    {ST_LIGHT,                  KEY_MINUS,  ST_SPEED},
	
    {ST_SPEED,                  KEY_PLUS,   ST_LIGHT},
    {ST_SPEED,                  KEY_NEXT,   ST_SPEED_FUNC},
    {ST_SPEED,                  KEY_PREV,   ST_AVRBF},
    {ST_SPEED,                  KEY_MINUS,  ST_OPTIONS},
    
    {ST_OPTIONS,                KEY_PLUS,   ST_SPEED},
    {ST_OPTIONS,                KEY_NEXT,   ST_OPTIONS_BOOT},
    {ST_OPTIONS,                KEY_PREV,   ST_AVRBF},
    {ST_OPTIONS,                KEY_MINUS,  ST_AVRBF},
    
/*    {ST_OPTIONS_DISPLAY,        KEY_PLUS,   ST_OPTIONS_LOGTIME},
    {ST_OPTIONS_DISPLAY,        KEY_NEXT,   ST_OPTIONS_DISPLAY_CONTRAST},
    {ST_OPTIONS_DISPLAY,        KEY_PREV,   ST_OPTIONS},
    {ST_OPTIONS_DISPLAY,        KEY_MINUS,  ST_OPTIONS_BOOT},
    
    {ST_OPTIONS_DISPLAY_CONTRAST, KEY_ENTER,    ST_OPTIONS_DISPLAY_CONTRAST_FUNC},
    {ST_OPTIONS_DISPLAY_CONTRAST, KEY_PREV,     ST_OPTIONS_DISPLAY},
*/
    {ST_OPTIONS_BOOT,             KEY_PLUS,     ST_OPTIONS_LOGTIME},
    {ST_OPTIONS_BOOT,             KEY_NEXT,     ST_OPTIONS_BOOT_FUNC},
    {ST_OPTIONS_BOOT,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_BOOT,             KEY_MINUS,    ST_OPTIONS_POWER_SAVE},

    {ST_OPTIONS_POWER_SAVE,       KEY_PLUS,     ST_OPTIONS_BOOT},
    {ST_OPTIONS_POWER_SAVE,       KEY_NEXT,     ST_OPTIONS_POWER_SAVE_FUNC},
    {ST_OPTIONS_POWER_SAVE,       KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_POWER_SAVE,       KEY_MINUS,    ST_OPTIONS_AUTO_POWER_SAVE},
        
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PLUS,     ST_OPTIONS_POWER_SAVE},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_NEXT,     ST_OPTIONS_AUTO_POWER_SAVE_FUNC},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_AUTO_POWER_SAVE,  KEY_MINUS,    ST_OPTIONS_UPLOAD},
/*	
	{ST_OPTIONS_START,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
    {ST_OPTIONS_START,             KEY_NEXT,     ST_OPTIONS_START_FUNC},
    {ST_OPTIONS_START,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_START,             KEY_MINUS,    ST_OPTIONS_LOGTIME},
*/   
	{ST_OPTIONS_UPLOAD,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
    {ST_OPTIONS_UPLOAD,             KEY_NEXT,     ST_OPTIONS_UPLOAD_FUNC},
    {ST_OPTIONS_UPLOAD,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_UPLOAD,             KEY_MINUS,    ST_OPTIONS_RESET},
	
	{ST_OPTIONS_RESET,             KEY_PLUS,     ST_OPTIONS_UPLOAD},
    {ST_OPTIONS_RESET,             KEY_NEXT,     ST_OPTIONS_RESET_FUNC},
    {ST_OPTIONS_RESET,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_RESET,             KEY_MINUS,    ST_OPTIONS_LOGTIME},

    {ST_OPTIONS_LOGTIME,             KEY_PLUS,     ST_OPTIONS_RESET},
    {ST_OPTIONS_LOGTIME,             KEY_NEXT,     ST_OPTIONS_LOGTIME_FUNC},
    {ST_OPTIONS_LOGTIME,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_LOGTIME,             KEY_MINUS,    ST_OPTIONS_BOOT},
	
  /*  {ST_OPTIONS_TEST,             KEY_MINUS,    ST_OPTIONS_POWER_SAVE},
    {ST_OPTIONS_TEST,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
    {ST_OPTIONS_TEST,             KEY_NEXT,     ST_OPTIONS_TEST_FUNC},
    {ST_OPTIONS_TEST,             KEY_PREV,     ST_OPTIONS},
    {ST_OPTIONS_TEST,             KEY_MINUS,    ST_OPTIONS_DISPLAY},    */
    {0,                         0,          0}
};
        

// mt MENU_STATE menu_state[] = {
const MENU_STATE menu_state[] PROGMEM = {
//  STATE                               STATE TEXT                  STATE_FUNC
    {ST_AVRBF,                          MT_AVRBF,                   0},
    {ST_AVRBF_REV,                      0,                       Revision},
    {ST_TIME,                           MT_TIME,                    0},
    {ST_TIME_CLOCK,                     MT_TIME_CLOCK,              0},
    {ST_TIME_CLOCK_FUNC,                0,                       ShowClock},
    {ST_TIME_CLOCK_ADJUST,              MT_TIME_CLOCK_ADJUST,       0},
    {ST_TIME_CLOCK_ADJUST_FUNC,         0,                       SetClock},
 //   {ST_TIME_CLOCKFORMAT_ADJUST,        MT_TIME_CLOCKFORMAT_ADJUST, 0},
  //  {ST_TIME_CLOCKFORMAT_ADJUST_FUNC,   0,                       SetClockFormat},
    {ST_TIME_DATE,                      MT_TIME_DATE,               0},
    {ST_TIME_DATE_FUNC,                 0,                       ShowDate},
    {ST_TIME_DATE_ADJUST,               MT_TIME_DATE_ADJUST,        0},
    {ST_TIME_DATE_ADJUST_FUNC,          0,                       SetDate},    
  //  {ST_TIME_DATEFORMAT_ADJUST,         MT_TIME_DATEFORMAT_ADJUST,  0},
  //  {ST_TIME_DATEFORMAT_ADJUST_FUNC,    0,                       SetDateFormat},     
  //  {ST_MUSIC,                          MT_MUSIC,                   0},
  //  {ST_MUSIC_SELECT,                   0,                       SelectSound},
  //  {ST_MUSIC_PLAY,                     0,                       Sound},
  /*  {ST_VCARD,                          MT_VCARD,                   0},
    {ST_VCARD_FUNC,                     0,                       vCard},
    {ST_VCARD_ENTER_NAME,               MT_ENTERNAME,               0},
    {ST_VCARD_DOWNLOAD_NAME,            MT_VCARD_DOWNLOAD_NAME,     0},
    {ST_VCARD_ENTER_NAME_FUNC,          0,                       EnterName},
    {ST_VCARD_DOWNLOAD_NAME_FUNC,       0,                       RS232},
    */{ST_TEMPERATURE,                    MT_TEMPERATURE,             0},
    {ST_TEMPERATURE_FUNC,               0,                       TemperatureFunc},
    {ST_VOLTAGE,                        MT_VOLTAGE,                 0},
    {ST_VOLTAGE_FUNC,                   0,                       VoltageFunc},
	{ST_DIR,                        MT_DIR,                 0},
    {ST_DIR_FUNC,                   0,                       DirectionFunc},
    {ST_LIGHT,                          MT_LIGHT,                   0},
    {ST_LIGHT_FUNC,                     0,                       LightFunc},
    {ST_SPEED,                          MT_SPEED,                   0},
    {ST_SPEED_FUNC,                     0,                       SpeedFunc},
    {ST_OPTIONS,                        MT_OPTIONS,                 0},
   // {ST_OPTIONS_DISPLAY,                MT_OPTIONS_DISPLAY,         0},
  //  {ST_OPTIONS_DISPLAY_CONTRAST,       MT_OPTIONS_DISPLAY_CONTRAST,0},
   // {ST_OPTIONS_DISPLAY_CONTRAST_FUNC,  0,                       SetContrast},
    {ST_OPTIONS_BOOT,                   MT_OPTIONS_BOOT,            0},
    {ST_OPTIONS_BOOT_FUNC,              0,                       BootFunc},    
    {ST_OPTIONS_POWER_SAVE,             MT_OPTIONS_POWER_SAVE,      0},
    {ST_OPTIONS_POWER_SAVE_FUNC,        0,                       PowerSaveFunc},
    {ST_OPTIONS_AUTO_POWER_SAVE,        MT_OPTIONS_AUTO_POWER_SAVE, 0},
    {ST_OPTIONS_AUTO_POWER_SAVE_FUNC,   0,                       AutoPower},
	{ST_OPTIONS_RESET,        			MT_OPTIONS_RESET, 0},
    {ST_OPTIONS_RESET_FUNC,   				0,                       ResetFlash},
//    {ST_OPTIONS_START,		MT_OPTIONS_START,	0},
//    {ST_OPTIONS_START_FUNC,		0,                        ToggleLogging},	
    {ST_OPTIONS_UPLOAD,		MT_OPTIONS_UPLOAD,	0},
    {ST_OPTIONS_UPLOAD_FUNC,		0,                        UploadFunc},	
    {ST_OPTIONS_LOGTIME,		MT_OPTIONS_LOGTIME,	0},
    {ST_OPTIONS_LOGTIME_FUNC,		0,                        LogTiming},		
    //{ST_OPTIONS_TEST,                   MT_OPTIONS_TEST,            0},
    //{ST_OPTIONS_TEST_FUNC,              0,                       GoTest},
    {0,                                 0,                       0}
    
};

#else

const char MT_AVRBF[] PROGMEM                    = "Butterfly Logger";
const char MT_TIME[]   PROGMEM                     = "Time";
const char MT_TIME_CLOCK[]   PROGMEM               = "Clock";
const char MT_TIME_CLOCK_ADJUST[]   PROGMEM        = "Adjust Clock";
const char MT_TIME_CLOCKFORMAT_ADJUST[]  PROGMEM   = "Change Format";
const char MT_TIME_DATE[]  PROGMEM                 = "Date";
const char MT_TIME_DATE_ADJUST[]    PROGMEM        = "Adjust Date";
const char MT_TIME_DATEFORMAT_ADJUST[]  PROGMEM    = "Change Format";/*
 const char MT_MUSIC[]   PROGMEM                    = "Music";
 const char MT_VCARD[]  PROGMEM                     = "Name";
 const char MT_ENTERNAME[]  PROGMEM                 = "Enter name";*/
const char MT_TEMPERATURE[]  PROGMEM               = "Temp";
const char MT_VOLTAGE[]  PROGMEM                   = "Battery";
const char MT_DIR[]  PROGMEM			 = "Direction";
const char MT_LIGHT[]  PROGMEM                     = "Light";
const char MT_SPEED[]	 PROGMEM 		= "Speed";
const char MT_OPTIONS[]  PROGMEM                   = "Options";
const char MT_OPTIONS_DISPLAY[]  PROGMEM           = "Display";
const char MT_OPTIONS_DISPLAY_CONTRAST[]  PROGMEM  = "Contrast";
//const char MT_OPTIONS_DISPLAY_SEG[]  PROGMEM       = "Browse segments";
//const char MT_VCARD_DOWNLOAD_NAME[]  PROGMEM       = "Download Name";
const char MT_OPTIONS_BOOT[]  PROGMEM              = "Bootloader";
const char MT_OPTIONS_RESET[]	 PROGMEM 		 = "Reset";
const char MT_OPTIONS_POWER_SAVE[]  PROGMEM        = "Power Save";
const char MT_OPTIONS_AUTO_POWER_SAVE[]  PROGMEM   = "Auto Power Save";
//const char MT_OPTIONS_TEST[]  PROGMEM              = "Test Mode";
const char MT_OPTIONS_UPLOAD[]	 PROGMEM 		= "Upload";
//const char MT_OPTIONS_START[]	 PROGMEM 		= "Logging";
const char MT_OPTIONS_LOGTIME[]	 PROGMEM 		= "Log Interval";


// mt MENU_NEXTSTATE menu_nextstate[] = { 
const MENU_NEXTSTATE menu_nextstate[] PROGMEM = {
	//  STATE                       INPUT       NEXT STATE
	{ST_AVRBF,                  KEY_PLUS,   ST_OPTIONS},
	{ST_AVRBF,                  KEY_NEXT,   ST_AVRBF_REV},
	{ST_AVRBF,                  KEY_MINUS,  ST_TIME},
	
	{ST_AVRBF_REV,              KEY_PREV,   ST_AVRBF},
	
	
	{ST_TIME,                   KEY_PLUS,   ST_AVRBF},
	{ST_TIME,                   KEY_NEXT,   ST_TIME_CLOCK},
	{ST_TIME,                   KEY_PREV,   ST_AVRBF},
	{ST_TIME,                   KEY_MINUS,  ST_TEMPERATURE},
	
	{ST_TIME_CLOCK,             KEY_PLUS,   ST_TIME_DATE},
	{ST_TIME_CLOCK,             KEY_NEXT,   ST_TIME_CLOCK_FUNC},
	{ST_TIME_CLOCK,             KEY_PREV,   ST_TIME},
	{ST_TIME_CLOCK,             KEY_MINUS,  ST_TIME_DATE},
	
	{ST_TIME_CLOCK_ADJUST,      KEY_PLUS,   ST_TIME_CLOCKFORMAT_ADJUST}, 
	{ST_TIME_CLOCK_ADJUST,      KEY_ENTER,  ST_TIME_CLOCK_ADJUST_FUNC},
	{ST_TIME_CLOCK_ADJUST,      KEY_PREV,   ST_TIME_CLOCK_FUNC},    
	{ST_TIME_CLOCK_ADJUST,      KEY_MINUS,  ST_TIME_CLOCKFORMAT_ADJUST}, 
	
	{ST_TIME_CLOCKFORMAT_ADJUST, KEY_PLUS,  ST_TIME_CLOCK_ADJUST},
	{ST_TIME_CLOCKFORMAT_ADJUST, KEY_ENTER, ST_TIME_CLOCKFORMAT_ADJUST_FUNC},
	{ST_TIME_CLOCKFORMAT_ADJUST, KEY_PREV,  ST_TIME_CLOCK_FUNC},
	{ST_TIME_CLOCKFORMAT_ADJUST, KEY_MINUS, ST_TIME_CLOCK_ADJUST},
	
	{ST_TIME_DATE,              KEY_PLUS,   ST_TIME_CLOCK},
	{ST_TIME_DATE,              KEY_NEXT,   ST_TIME_DATE_FUNC},
	{ST_TIME_DATE,              KEY_PREV,   ST_TIME},
	{ST_TIME_DATE,              KEY_MINUS,  ST_TIME_CLOCK},
	
	{ST_TIME_DATE_ADJUST,       KEY_PLUS,   ST_TIME_DATEFORMAT_ADJUST},
	{ST_TIME_DATE_ADJUST,       KEY_ENTER,  ST_TIME_DATE_ADJUST_FUNC},
	{ST_TIME_DATE_ADJUST,       KEY_PREV,   ST_TIME_DATE_FUNC},
	{ST_TIME_DATE_ADJUST,       KEY_MINUS,  ST_TIME_DATEFORMAT_ADJUST},
        
	{ST_TIME_DATEFORMAT_ADJUST, KEY_PLUS,   ST_TIME_DATE_ADJUST},
	{ST_TIME_DATEFORMAT_ADJUST, KEY_ENTER,  ST_TIME_DATEFORMAT_ADJUST_FUNC},
	{ST_TIME_DATEFORMAT_ADJUST, KEY_PREV,   ST_TIME_DATE_FUNC},
	{ST_TIME_DATEFORMAT_ADJUST, KEY_MINUS,  ST_TIME_DATE_ADJUST},
	
	//  {ST_MUSIC,                  KEY_PLUS,   ST_TIME},
	//  {ST_MUSIC,                  KEY_NEXT,   ST_MUSIC_SELECT},
	//  {ST_MUSIC,                  KEY_PREV,   ST_AVRBF},
	//  {ST_MUSIC,                  KEY_MINUS,  ST_VCARD},
	
	//    {ST_SOUND_MUSIC,            KEY_NEXT,   ST_MUSIC_SELECT},
	//    {ST_SOUND_MUSIC,            KEY_PREV,   ST_MUSIC},
	/*
	 {ST_VCARD,                  KEY_PLUS,   ST_TIME},
	 {ST_VCARD,                  KEY_NEXT,   ST_VCARD_FUNC},
	 {ST_VCARD,                  KEY_PREV,   ST_AVRBF},
	 {ST_VCARD,                  KEY_MINUS,  ST_TEMPERATURE},
	 
	 {ST_VCARD_ENTER_NAME,       KEY_PLUS,     ST_VCARD_DOWNLOAD_NAME},
	 {ST_VCARD_ENTER_NAME,       KEY_ENTER,    ST_VCARD_ENTER_NAME_FUNC},
	 {ST_VCARD_ENTER_NAME,       KEY_PREV,     ST_VCARD_FUNC},    
	 {ST_VCARD_ENTER_NAME,       KEY_MINUS,    ST_VCARD_DOWNLOAD_NAME},
	 
	 {ST_VCARD_DOWNLOAD_NAME,    KEY_PLUS,     ST_VCARD_ENTER_NAME},
	 {ST_VCARD_DOWNLOAD_NAME,    KEY_ENTER,    ST_VCARD_DOWNLOAD_NAME_FUNC},
	 {ST_VCARD_DOWNLOAD_NAME,    KEY_PREV,     ST_VCARD_FUNC},    
	 {ST_VCARD_DOWNLOAD_NAME,    KEY_MINUS,    ST_VCARD_ENTER_NAME},    
	 */  
	{ST_TEMPERATURE,            KEY_PLUS,   ST_TIME},
	{ST_TEMPERATURE,            KEY_NEXT,   ST_TEMPERATURE_FUNC},
	{ST_TEMPERATURE,            KEY_PREV,   ST_AVRBF},
	{ST_TEMPERATURE,            KEY_MINUS,  ST_VOLTAGE},
	
	{ST_VOLTAGE,                KEY_PLUS,   ST_TEMPERATURE},
	{ST_VOLTAGE,                KEY_NEXT,   ST_VOLTAGE_FUNC},
	{ST_VOLTAGE,                KEY_PREV,   ST_AVRBF},
	{ST_VOLTAGE,                KEY_MINUS,  ST_DIR},
	
	{ST_DIR,                	KEY_PLUS,   ST_VOLTAGE},
	{ST_DIR,                	KEY_NEXT,   ST_DIR_FUNC},
	{ST_DIR,                	KEY_PREV,   ST_AVRBF},
	{ST_DIR,                	KEY_MINUS,  ST_LIGHT},
	
	{ST_LIGHT,                  KEY_PLUS,   ST_DIR},
	{ST_LIGHT,                  KEY_NEXT,   ST_LIGHT_FUNC},
	{ST_LIGHT,                  KEY_PREV,   ST_AVRBF},
	{ST_LIGHT,                  KEY_MINUS,  ST_SPEED},
	
	{ST_SPEED,                  KEY_PLUS,   ST_LIGHT},
	{ST_SPEED,                  KEY_NEXT,   ST_SPEED_FUNC},
	{ST_SPEED,                  KEY_PREV,   ST_AVRBF},
	{ST_SPEED,                  KEY_MINUS,  ST_OPTIONS},
	
	{ST_OPTIONS,                KEY_PLUS,   ST_SPEED},
	{ST_OPTIONS,                KEY_NEXT,   ST_OPTIONS_DISPLAY},
	{ST_OPTIONS,                KEY_PREV,   ST_AVRBF},
	{ST_OPTIONS,                KEY_MINUS,  ST_AVRBF},
	
	{ST_OPTIONS_DISPLAY,        KEY_PLUS,   ST_OPTIONS_LOGTIME},
	{ST_OPTIONS_DISPLAY,        KEY_NEXT,   ST_OPTIONS_DISPLAY_CONTRAST},
	{ST_OPTIONS_DISPLAY,        KEY_PREV,   ST_OPTIONS},
	{ST_OPTIONS_DISPLAY,        KEY_MINUS,  ST_OPTIONS_BOOT},
	
	{ST_OPTIONS_DISPLAY_CONTRAST, KEY_ENTER,    ST_OPTIONS_DISPLAY_CONTRAST_FUNC},
	{ST_OPTIONS_DISPLAY_CONTRAST, KEY_PREV,     ST_OPTIONS_DISPLAY},
	
	{ST_OPTIONS_BOOT,             KEY_PLUS,     ST_OPTIONS_DISPLAY},
	{ST_OPTIONS_BOOT,             KEY_NEXT,     ST_OPTIONS_BOOT_FUNC},
	{ST_OPTIONS_BOOT,             KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_BOOT,             KEY_MINUS,    ST_OPTIONS_POWER_SAVE},
	
	{ST_OPTIONS_POWER_SAVE,       KEY_PLUS,     ST_OPTIONS_BOOT},
	{ST_OPTIONS_POWER_SAVE,       KEY_NEXT,     ST_OPTIONS_POWER_SAVE_FUNC},
	{ST_OPTIONS_POWER_SAVE,       KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_POWER_SAVE,       KEY_MINUS,    ST_OPTIONS_AUTO_POWER_SAVE},
        
	{ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PLUS,     ST_OPTIONS_POWER_SAVE},
	{ST_OPTIONS_AUTO_POWER_SAVE,  KEY_NEXT,     ST_OPTIONS_AUTO_POWER_SAVE_FUNC},
	{ST_OPTIONS_AUTO_POWER_SAVE,  KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_AUTO_POWER_SAVE,  KEY_MINUS,    ST_OPTIONS_UPLOAD},
	/*	
	{ST_OPTIONS_START,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
	 {ST_OPTIONS_START,             KEY_NEXT,     ST_OPTIONS_START_FUNC},
	 {ST_OPTIONS_START,             KEY_PREV,     ST_OPTIONS},
	 {ST_OPTIONS_START,             KEY_MINUS,    ST_OPTIONS_LOGTIME},
	 */   
	{ST_OPTIONS_UPLOAD,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
	{ST_OPTIONS_UPLOAD,             KEY_NEXT,     ST_OPTIONS_UPLOAD_FUNC},
	{ST_OPTIONS_UPLOAD,             KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_UPLOAD,             KEY_MINUS,    ST_OPTIONS_RESET},
	
	{ST_OPTIONS_RESET,             KEY_PLUS,     ST_OPTIONS_UPLOAD},
	{ST_OPTIONS_RESET,             KEY_NEXT,     ST_OPTIONS_RESET_FUNC},
	{ST_OPTIONS_RESET,             KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_RESET,             KEY_MINUS,    ST_OPTIONS_LOGTIME},
	
	{ST_OPTIONS_LOGTIME,             KEY_PLUS,     ST_OPTIONS_RESET},
	{ST_OPTIONS_LOGTIME,             KEY_NEXT,     ST_OPTIONS_LOGTIME_FUNC},
	{ST_OPTIONS_LOGTIME,             KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_LOGTIME,             KEY_MINUS,    ST_OPTIONS_DISPLAY},
	
	/*  {ST_OPTIONS_TEST,             KEY_MINUS,    ST_OPTIONS_POWER_SAVE},
	{ST_OPTIONS_TEST,             KEY_PLUS,     ST_OPTIONS_AUTO_POWER_SAVE},
	{ST_OPTIONS_TEST,             KEY_NEXT,     ST_OPTIONS_TEST_FUNC},
	{ST_OPTIONS_TEST,             KEY_PREV,     ST_OPTIONS},
	{ST_OPTIONS_TEST,             KEY_MINUS,    ST_OPTIONS_DISPLAY},    */
	{0,                         0,          0}
};


// mt MENU_STATE menu_state[] = {
const MENU_STATE menu_state[] PROGMEM = {
	//  STATE                               STATE TEXT                  STATE_FUNC
	{ST_AVRBF,                          MT_AVRBF,                   0},
	{ST_AVRBF_REV,                      0,                       Revision},
	{ST_TIME,                           MT_TIME,                    0},
	{ST_TIME_CLOCK,                     MT_TIME_CLOCK,              0},
	{ST_TIME_CLOCK_FUNC,                0,                       ShowClock},
	{ST_TIME_CLOCK_ADJUST,              MT_TIME_CLOCK_ADJUST,       0},
	{ST_TIME_CLOCK_ADJUST_FUNC,         0,                       SetClock},
	{ST_TIME_CLOCKFORMAT_ADJUST,        MT_TIME_CLOCKFORMAT_ADJUST, 0},
	{ST_TIME_CLOCKFORMAT_ADJUST_FUNC,   0,                       SetClockFormat},
	{ST_TIME_DATE,                      MT_TIME_DATE,               0},
	{ST_TIME_DATE_FUNC,                 0,                       ShowDate},
	{ST_TIME_DATE_ADJUST,               MT_TIME_DATE_ADJUST,        0},
	{ST_TIME_DATE_ADJUST_FUNC,          0,                       SetDate},    
	{ST_TIME_DATEFORMAT_ADJUST,         MT_TIME_DATEFORMAT_ADJUST,  0},
	{ST_TIME_DATEFORMAT_ADJUST_FUNC,    0,                       SetDateFormat},     
	//  {ST_MUSIC,                          MT_MUSIC,                   0},
	//  {ST_MUSIC_SELECT,                   0,                       SelectSound},
	//  {ST_MUSIC_PLAY,                     0,                       Sound},
	/*  {ST_VCARD,                          MT_VCARD,                   0},
	{ST_VCARD_FUNC,                     0,                       vCard},
	{ST_VCARD_ENTER_NAME,               MT_ENTERNAME,               0},
	{ST_VCARD_DOWNLOAD_NAME,            MT_VCARD_DOWNLOAD_NAME,     0},
	{ST_VCARD_ENTER_NAME_FUNC,          0,                       EnterName},
	{ST_VCARD_DOWNLOAD_NAME_FUNC,       0,                       RS232},
	*/{ST_TEMPERATURE,                    MT_TEMPERATURE,             0},
	{ST_TEMPERATURE_FUNC,               0,                       TemperatureFunc},
	{ST_VOLTAGE,                        MT_VOLTAGE,                 0},
	{ST_VOLTAGE_FUNC,                   0,                       VoltageFunc},
	{ST_DIR,                        MT_DIR,                 0},
	{ST_DIR_FUNC,                   0,                       DirectionFunc},
	{ST_LIGHT,                          MT_LIGHT,                   0},
	{ST_LIGHT_FUNC,                     0,                       LightFunc},
	{ST_SPEED,                          MT_SPEED,                   0},
	{ST_SPEED_FUNC,                     0,                       SpeedFunc},
	{ST_OPTIONS,                        MT_OPTIONS,                 0},
	{ST_OPTIONS_DISPLAY,                MT_OPTIONS_DISPLAY,         0},
	{ST_OPTIONS_DISPLAY_CONTRAST,       MT_OPTIONS_DISPLAY_CONTRAST,0},
	{ST_OPTIONS_DISPLAY_CONTRAST_FUNC,  0,                       SetContrast},
	{ST_OPTIONS_BOOT,                   MT_OPTIONS_BOOT,            0},
	{ST_OPTIONS_BOOT_FUNC,              0,                       BootFunc},    
	{ST_OPTIONS_POWER_SAVE,             MT_OPTIONS_POWER_SAVE,      0},
	{ST_OPTIONS_POWER_SAVE_FUNC,        0,                       PowerSaveFunc},
	{ST_OPTIONS_AUTO_POWER_SAVE,        MT_OPTIONS_AUTO_POWER_SAVE, 0},
	{ST_OPTIONS_AUTO_POWER_SAVE_FUNC,   0,                       AutoPower},
	{ST_OPTIONS_RESET,        			MT_OPTIONS_RESET, 0},
	{ST_OPTIONS_RESET_FUNC,   				0,                       ResetFlash},
	//    {ST_OPTIONS_START,		MT_OPTIONS_START,	0},
	//    {ST_OPTIONS_START_FUNC,		0,                        ToggleLogging},	
	{ST_OPTIONS_UPLOAD,		MT_OPTIONS_UPLOAD,	0},
	{ST_OPTIONS_UPLOAD_FUNC,		0,                        UploadFunc},	
	{ST_OPTIONS_LOGTIME,		MT_OPTIONS_LOGTIME,	0},
	{ST_OPTIONS_LOGTIME_FUNC,		0,                        LogTiming},		
	//{ST_OPTIONS_TEST,                   MT_OPTIONS_TEST,            0},
	//{ST_OPTIONS_TEST_FUNC,              0,                       GoTest},
	{0,                                 0,                       0}
	
};
#endif
#endif
