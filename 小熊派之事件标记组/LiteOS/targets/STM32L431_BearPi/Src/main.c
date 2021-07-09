/*
*2021.7.7 
*2018119145 李嘉雯
*/
//事件标记组
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_sem.h"


#define KEY1_EVENT  (0x01 << 0)//设置事件掩码的位0
#define KEY2_EVENT  (0x01 << 1)//设置事件掩码的位1

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;
/* 定义任务句柄 */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

/* 定义事件标志组的控制块 */
static EVENT_CB_S EventGroup_CB;


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
	LCD_ShowString(10, 20, 240, 24, 24, "Open the port");
  LCD_ShowString(10, 50, 240, 16, 16, "Please press the Key");
	
//	LCD_ShowString(10, 50, 240, 24, 24, "Welcome to IoTCluB!");
//	LCD_ShowString(20, 90, 240, 16, 16, "BearPi IoT Develop Board");
//	LCD_ShowString(20, 130, 240, 16, 16, "Powerd by Huawei LiteOS!");
//	LCD_ShowString(10, 170, 240, 16, 16, "Please wait for system init");
}
//定义任务一
void task1(void)
{
	UINT32 uwRet;			
  /* 任务都是一个无限循环，不能返回 */

	while(1)
	{
		/* 等待事件标志组 */
		uwRet = LOS_EventRead(	&EventGroup_CB,        	//事件标志组对象
														KEY1_EVENT|KEY2_EVENT,  //等待任务感兴趣的事件
														LOS_WAITMODE_AND,     	//等待两位均被置位
														LOS_WAIT_FOREVER ); 		//无期限等待
					
    if(( uwRet & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT)) 
    {
			/* 如果接收完成并且正确 */
			printf ( "KEY1与KEY2都按下\t\r\n");
		//	LCD_ShowString(10, 30, 240, 24, 24, "File success!   ");
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);	
			LCD_ShowString(10, 130, 240, 24, 24, "Event OK   ");
			
			LOS_EventClear(&EventGroup_CB, ~KEY1_EVENT);	//清除事件标志
			LOS_EventClear(&EventGroup_CB, ~KEY2_EVENT);	//清除事件标志
		}
		
	}
}

//定义任务二
void task2(void)
{
	// 定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		/* KEY1 被按下 */
    if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )       
		{
			LCD_ShowString(10, 70, 240, 24, 24, "Event1 success!");
			//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      printf ( "KEY1被按下\t\r\n" );
			/* 触发一个事件1 */
			LOS_EventWrite(&EventGroup_CB,KEY1_EVENT);  		
		}
		LOS_TaskDelay(20); 
    /* KEY2 被按下 */
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON )      
		{
			LCD_ShowString(10, 100, 240, 24, 24, "Event2 success!");
			//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      printf ( "KEY2被按下\t\r\n" );	
			/* 触发一个事件2 */
			LOS_EventWrite(&EventGroup_CB,KEY2_EVENT); 				
		}
		LOS_TaskDelay(20);     //每20ms扫描一次		
	}
}

//创建任务一
UINT32 creat_task1()
{
	UINT32 uwRet = LOS_OK;
	TSK_INIT_PARAM_S task_init_param;
	task_init_param.usTaskPrio = 1;
	task_init_param.pcName = "task1";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
	task_init_param.uwStackSize = 0x200;
	uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
	if(LOS_OK != uwRet)
	{
		return uwRet;
	}
	return uwRet;
}

//创建任务二
UINT32 creat_task2()
{
	UINT32 uwRet = LOS_OK;
	TSK_INIT_PARAM_S task_init_param;
	task_init_param.usTaskPrio = 0;
	task_init_param.pcName = "task2";
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task2;
	task_init_param.uwStackSize = 0x400;
	uwRet = LOS_TaskCreate(&g_TskHandle, &task_init_param);
	if(LOS_OK != uwRet)
	{
		return uwRet;
	}
	return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	/* 创建一个事件标志组*/
	 uwRet = LOS_EventInit(&EventGroup_CB);
	
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }
//把创建的任务放在主函数中
//任务一
	uwRet = creat_task1();
    if (uwRet != LOS_OK)
    {
      return LOS_NOK;
    }
//任务二
	uwRet = creat_task2();
    if (uwRet != LOS_OK)
    {
      return LOS_NOK;
    }
	
    (void)LOS_Start();
    return 0;
}
