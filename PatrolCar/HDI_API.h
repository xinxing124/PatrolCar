
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the HDI_API_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// HDI_API_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#define GPIO_HIGH			1
#define GPIO_LOW			0
#define GPIO_OUTPUT_CH0 	0
#define GPIO_OUTPUT_CH1 	1
#define GPIO_OUTPUT_CH2 	2
#define GPIO_OUTPUT_CH3 	3
#define GPIO_OUTPUT_CH4 	4
#define GPIO_OUTPUT_CH5 	5
#define GPIO_OUTPUT_CH6 	6
#define GPIO_OUTPUT_CH7 	7

#define GPIO_INPUT_CH0 		0
#define GPIO_INPUT_CH1 		1
#define GPIO_INPUT_CH2 		2
#define GPIO_INPUT_CH3 		3
#define GPIO_INPUT_CH4 		4
#define GPIO_INPUT_CH5 		5
#define GPIO_INPUT_CH6 		6
#define GPIO_INPUT_CH7 		7

#define ROTATION_0			0
#define ROTATION_90			1
#define ROTATION_180		2
#define ROTATION_270		3

#define FRAM				0
#define EEPROM				1

#define PWM_CH0				0
#define PWM_CH1				1
#define PWM_CH2				2
#define PWM_CH3				3

#define PWM_DIV1			1
#define PWM_DIV2			2
#define PWM_DIV4			4
#define PWM_DIV8			8
#define PWM_DIV16			16

#define ADC_CH0				0
#define ADC_CH1				1
#define ADC_CH2				2
#define ADC_CH3				3
#define ADC_CH4				4
#define ADC_CH5				5

#define FM24CL64_SIZE		(8*1024 - 64)
/**************************************************************************
函数名:API_GPIO_Output
功  能:设置IO口电平
参  数:ch,通道号,取值0,1,2,3
       level,1--高电平;0--低电平
返回值:TRUE--设置成功
       FALSE--设置失败
**************************************************************************/
extern "C" _declspec(dllexport)BOOL API_GPIO_Output(UINT8 ch, UINT8 level);

/**************************************************************************
函数名:API_GPIO_Input
功  能:读取IO口电平
参  数:ch,通道号,取值0,1,2,3
       pLevel,缓存
返回值:TRUE--读取成功
       FALSE--读取失败
**************************************************************************/
extern "C" _declspec(dllexport)BOOL API_GPIO_Input(UINT8 ch, UINT8* pLevel);

/**************************************************************************
函数名:API_BEEP_Delay
功  能:蜂鸣器
参  数:dwDelay,响多少毫秒
返回值:无
**************************************************************************/
extern "C" _declspec(dllexport)void API_BEEP_Delay(DWORD dwDelay);

/*****************************************************************************************
函数名:	API_WDT_Enable
功  能:启动看门狗
返回值:无
*****************************************************************************************/
extern "C" _declspec(dllexport)BOOL API_WDT_Enable(void);


/*****************************************************************************************
函数名:API_WDT_Disable
功  能:禁用看门狗
返回值:无
*****************************************************************************************/
extern "C" _declspec(dllexport)BOOL API_WDT_Disable(void);



/*****************************************************************************************
函数名:API_WDT_SetTimeOut
功  能:设置看门狗超时时间
参  数:sec,单位为秒,最大值为40秒
返回值:TRUE
       FALSE
*****************************************************************************************/
extern "C" _declspec(dllexport)BOOL API_WDT_SetTimeOut(UINT8 sec);

/*****************************************************************************************
函数名:API_WDT_Feed
功  能:喂狗
返回值:无
说  明:如果启动了看门狗，在API_WDT_SetTimeOut设置的Sec秒内不喂狗，则重新启动系统。
       正常情况下，如果启动了看门狗，必须每Sec秒内至少喂狗，否则会重新启动系统。
*****************************************************************************************/
extern "C" _declspec(dllexport)BOOL API_WDT_Feed(void);
/****************************************************************************************
关于看门狗的使用说明:
【1】在wince启动时，看门狗默认是禁用的
【2】开启看门狗程序流程如下
1.SetWdtTimeOut(sec) -- 如sec等于10,20等
2.EnableWdt() -- 启动看门狗
3.每sec秒内必须FeedWdt(),否则超会重启系统。
【4】如果看门狗已启动，DisableWdt()可以禁用看门狗功能
****************************************************************************************/
extern "C" _declspec(dllexport)BOOL API_PWM_SetDiv(UINT8 ch, UINT8 div);
extern "C" _declspec(dllexport)BOOL API_PWM_Start(UINT8 ch, UINT32 cntb, UINT32 cmpb);
extern "C" _declspec(dllexport)BOOL API_PWM_Stop(UINT8 ch);
extern "C" _declspec(dllexport)BOOL API_PWM_GetFrequency(UINT8 ch, DWORD* pFreq);

extern "C" _declspec(dllexport)BOOL API_ADC_Read(UINT8 ch, UINT16* pVal);

extern "C" _declspec(dllexport)BOOL API_BLK_SetLevel(UINT8 level);
extern "C" _declspec(dllexport)void API_BLK_GetLevel(UINT8* pLevel);

extern "C" _declspec(dllexport)BOOL API_GUI_Rotate(UINT8 rotation, BOOL bIsSaveReg);

extern "C" _declspec(dllexport)void API_HWWDT_Feed(void);
extern "C" _declspec(dllexport)void API_HW_Reboot(void);
extern "C" _declspec(dllexport)void API_HW_GetID(UINT8* pData);
extern "C" _declspec(dllexport)void API_HW_GetIMEI(UINT8* pData);
extern "C" _declspec(dllexport)void API_HW_GetOSBootCnt(DWORD* pCnt);
extern "C" _declspec(dllexport)void API_HW_GetOSLifeTime(DWORD* pCnt);
extern "C" _declspec(dllexport)void API_HW_GetOSCurrLifeTime(DWORD* pCnt);

extern "C" _declspec(dllexport)BOOL API_FRAM_Read(UINT16 Addr,UINT8 *pBuf,UINT32 Lenght);
extern "C" _declspec(dllexport)BOOL API_FRAM_Write(UINT16 Addr,UINT8 *pBuf,UINT32 Lenght);