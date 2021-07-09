/*
*2021.7.7 
*2018119145 李嘉雯
*/
//优先级翻转
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS 头文件 */
#include "los_sys.h"
#include "los_task.ph"
#include "los_sem.h"

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 HighPriority_Task_Handle;
UINT32 MidPriority_Task_Handle;
UINT32 LowPriority_Task_Handle;

/* 定义二值信号量的句柄 */
UINT32 BinarySem_Handle;

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

/******************************************************************
  * @ 函数名  ： HighPriority_Task
  * @ 功能说明： HighPriority_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void HighPriority_Task(void)
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;	
	
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		//获取二值信号量 xSemaphore,没获取到则一直等待
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); 
		if (uwRet == LOS_OK)
			printf("HighPriority_Task Runing\t\r\n");
		
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		LCD_ShowString(10, 50, 240, 24, 24, "HighPriority ");
		
		LOS_SemPost( BinarySem_Handle );   //给出二值信号量 xSemaphore
		LOS_TaskDelay ( 1000 );        			/* 延时100ms */	
  }
}
/******************************************************************
  * @ 函数名  ： MidPriority_Task
  * @ 功能说明： MidPriority_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void MidPriority_Task(void)
{	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		printf("MidPriority_Task Runing\t\r\n");
		LOS_TaskDelay ( 1000 );        			/* 延时100ms */		
	}
}

/******************************************************************
  * @ 函数名  ： LowPriority_Task
  * @ 功能说明： LowPriority_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void LowPriority_Task(void)
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;		
	
	static uint32_t i;
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		//获取二值信号量 BinarySem_Handle，没获取到则一直等待		
		uwRet = LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  
		if (uwRet == LOS_OK)
			printf("LowPriority_Task Runing\t\r\n");
		
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		LCD_ShowString(10, 50, 240, 24, 24, "LowPriority ");
		
    for(i=0;i<3000000;i++)//模拟低优先级任务占用信号量
		{
			//放弃剩余时间片，进行一次任务切换
			LOS_TaskYield();
		}
    printf("LowPriority_Task 释放信号量!\t\r\n");
		LOS_SemPost( BinarySem_Handle );    //给出二值信号量 xSemaphore		
		
		LOS_TaskDelay ( 1000 );        			/* 延时100ms */		
	}
}

/******************************************************************
  * @ 函数名  ： Creat_HighPriority_Task
  * @ 功能说明： 创建HighPriority_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_HighPriority_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 3;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "HighPriority_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)HighPriority_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&HighPriority_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_MidPriority_Task
  * @ 功能说明： 创建MidPriority_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_MidPriority_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "MidPriority_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)MidPriority_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&MidPriority_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}

/*******************************************************************
  * @ 函数名  ： Creat_MidPriority_Task
  * @ 功能说明： 创建MidPriority_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_LowPriority_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "LowPriority_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)LowPriority_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&LowPriority_Task_Handle, &task_init_param);/* 创建任务 */

	return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    /* 创建一个二值信号量*/
		uwRet = LOS_BinarySemCreate(1,&BinarySem_Handle);
		if (uwRet != LOS_OK)
		{
			printf("BinarySem创建失败！失败代码0x%X\n",uwRet);
		}
			
		uwRet = Creat_HighPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("HighPriority_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_MidPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("MidPriority_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_LowPriority_Task();
		if (uwRet != LOS_OK)
		{
			printf("LowPriority_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
