/*
*2021.7.7 
*2018119145 李嘉雯
*/
//软件定时器
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_swtmr.h"


UINT32 g_TskHandle;

/* 定义定时器句柄（ID） */
UINT16 Timer1_Handle;
UINT16 Timer2_Handle;
UINT16 Timer3_Handle;
UINT16 Timer4_Handle;

static UINT32 TmrCb_Count1 = 0;
static UINT32 TmrCb_Count2 = 0;
static UINT32 TmrCb_Count3 = 0;
static UINT32 TmrCb_Count4 = 0;

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();//GPIO初始化
	MX_USART1_UART_Init();
	MX_SPI2_Init();
	dwt_delay_init(SystemCoreClock);
	LCD_Init();					
	LCD_Clear(WHITE);		   	
	POINT_COLOR = RED;			
//	LCD_ShowString(10, 50, 240, 24, 24, "Welcome to IoTCluB!");
//	LCD_ShowString(20, 90, 240, 16, 16, "BearPi IoT Develop Board");
//	LCD_ShowString(20, 130, 240, 16, 16, "Powerd by Huawei LiteOS!");
//	LCD_ShowString(10, 170, 240, 16, 16, "Please wait for system init");
}

/*********************************************************************************
  * @ 函数名  ： Timer1_Callback
  * @ 功能说明： 软件定时器回调函数1
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer1_Callback(UINT32 arg)
{
    UINT32 tick_num1;

    TmrCb_Count1++;						/* 每回调一次加一 */
		LCD_ShowString(10, 50, 240, 24, 24, "Timer1 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    LOS_TaskDelay(300);
	  tick_num1 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count1=%d\t\r\n", TmrCb_Count1);
    printf("tick_num1=%d\t\r\n", tick_num1);
}
/*********************************************************************************
  * @ 函数名  ： Timer2_Callback
  * @ 功能说明： 软件定时器回调函数2
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer2_Callback(UINT32 arg)
{
    UINT32 tick_num2;
	
    TmrCb_Count2++;				/* 每回调一次加一 */
		LCD_ShowString(10, 90, 240, 24, 24, "Timer2 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	  
    tick_num2 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count2=%d\t\r\n", TmrCb_Count2);
	
    printf("tick_num2=%d\t\r\n", tick_num2);
    
}
/*********************************************************************************
  * @ 函数名  ： Timer3_Callback
  * @ 功能说明： 软件定时器回调函数2
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer3_Callback(UINT32 arg)
{
    UINT32 tick_num3;
	
    TmrCb_Count3++;				/* 每回调一次加一 */
		LCD_ShowString(10, 120, 240, 24, 24, "Timer3 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  LOS_TaskDelay(300);
    tick_num3 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count3=%d\t\r\n", TmrCb_Count3);
	
    printf("tick_num3=%d\t\r\n", tick_num3);
    
}

/*********************************************************************************
  * @ 函数名  ： Timer4_Callback
  * @ 功能说明： 软件定时器回调函数2
  * @ 参数    ： 传入1个参数，但未使用  
  * @ 返回值  ： 无
  ********************************************************************************/
static void Timer4_Callback(UINT32 arg)
{
    UINT32 tick_num4;
	
    TmrCb_Count4++;				/* 每回调一次加一 */
		LCD_ShowString(10, 50, 240, 24, 24, "                 ");
		LCD_ShowString(10, 90, 240, 24, 24, "                 ");
	  LCD_ShowString(10, 120, 240, 24, 24, "                 ");
	
	  LOS_TaskDelay(300);
    tick_num4 = (UINT32)LOS_TickCountGet();	/* 获取滴答定时器的计数值 */
	
    printf("Timer_CallBack_Count4=%d\t\r\n", TmrCb_Count3);
	
    printf("tick_num4=%d\t\r\n", tick_num4);
    
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
	
		/* 创建一个软件定时器定时器*/
		uwRet = LOS_SwtmrCreate(1000, 					/* 软件定时器的定时时间*/	
														LOS_SWTMR_MODE_ONCE, 	/* 软件定时器模式 一次模式 */
														(SWTMR_PROC_FUNC)Timer1_Callback,		/* 软件定时器的回调函数 */
														&Timer1_Handle,					/* 软件定时器的id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("软件定时器Timer1创建失败！\n");
			}
		uwRet = LOS_SwtmrCreate(3000, 					/* 软件定时器的定时时间（ms）*/	
														LOS_SWTMR_MODE_PERIOD, 	/* 软件定时器模式 周期模式 */
														(SWTMR_PROC_FUNC)Timer2_Callback,		/* 软件定时器的回调函数 */
														&Timer2_Handle,			/* 软件定时器的id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("软件定时器Timer2创建失败！\n");
			}
		uwRet = LOS_SwtmrCreate(5000, 					/* 软件定时器的定时时间（ms）*/	
														LOS_SWTMR_MODE_PERIOD, 	/* 软件定时器模式 周期模式 */
														(SWTMR_PROC_FUNC)Timer3_Callback,		/* 软件定时器的回调函数 */
														&Timer3_Handle,			/* 软件定时器的id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("软件定时器Timer3创建失败！\n");
			}
		uwRet = LOS_SwtmrCreate(7000, 					/* 软件定时器的定时时间（ms）*/	
														LOS_SWTMR_MODE_PERIOD, 	/* 软件定时器模式 周期模式 */
														(SWTMR_PROC_FUNC)Timer4_Callback,		/* 软件定时器的回调函数 */
														&Timer4_Handle,			/* 软件定时器的id */
														0,0,0);		
		
		if (uwRet != LOS_OK) 
		{
			printf("软件定时器Timer3创建失败！\n");
			return uwRet;
		}
		
		/* 1.启动一个软件定时器定时器*/
		uwRet = LOS_SwtmrStart(Timer1_Handle);
		if (LOS_OK != uwRet)
		{
			printf("start Timer1 failed\n");
			return uwRet;
		}
		else
		{
			printf("start Timer1 sucess\n");
		}
		/* 2.启动一个软件定时器定时器*/
		uwRet = LOS_SwtmrStart(Timer2_Handle);
		if (LOS_OK != uwRet)
		{
			printf("start Timer2 failed\n");
			return uwRet;
		}
		else
		{
			printf("start Timer2 sucess\n");
		}
		/* 3.启动一个软件定时器定时器*/
		uwRet = LOS_SwtmrStart(Timer3_Handle);
		if (LOS_OK != uwRet)
		{
			printf("start Timer3 failed\n");
			return uwRet;
		}
		else
		{
			printf("start Timer3 sucess\n");
		}
		/* 4.启动一个软件定时器定时器*/
		uwRet = LOS_SwtmrStart(Timer4_Handle);
		if (LOS_OK != uwRet)
		{
			printf("start Timer4 failed\n");
			return uwRet;
		}
		else
		{
			printf("start Timer4 sucess\n");
		}
		(void)LOS_Start();
		return 0;
}
