/*
*2021.7.7 
*2018119145 李嘉雯
*/
//二值信号量同步
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"

/* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"
#include "los_sem.h"

UINT32 g_TskHandle;

/* 定义任务句柄 */
UINT32 Read_Task_Handle;
UINT32 Write_Task_Handle;

/* 定义二值信号量的句柄 */
UINT32 BinarySem_Handle;

uint8_t ucValue [ 2 ] = { 0x00, 0x00 };

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
  * @ 函数名  ： Read_Task
  * @ 功能说明： Read_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Read_Task(void)
{
  /* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); //获取二值信号量 xSemaphore,没获取到则一直等待
		
		if ( ucValue [ 0 ] == ucValue [ 1 ] )
		{ 			
			printf ( "\r\nSuccessful\r\n" );
			LCD_ShowString(10, 50, 240, 24, 24, "Success");
			LCD_ShowString(10, 90, 240, 24, 24, "Open the port");
		}
		else
		{
			printf ( "\r\nFail\r\n" );			
		}
		
		LOS_SemPost( BinarySem_Handle );   //给出二值信号量 xSemaphore
		
  }
}
/******************************************************************
  * @ 函数名  ： Write_Task
  * @ 功能说明： Write_Task任务实现
  * @ 参数    ： NULL 
  * @ 返回值  ： NULL
  *****************************************************************/
static void Write_Task(void)
{
	/* 定义一个创建任务的返回类型，初始化为创建成功的返回值 */
	UINT32 uwRet = LOS_OK;				
	
	/* 任务都是一个无限循环，不能返回 */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  //获取二值信号量 BinarySem_Handle，没获取到则一直等待		
		ucValue [ 0 ] ++;		
		LOS_TaskDelay ( 1000 );        			/* 延时100ms */		
		ucValue [ 1 ] ++;		
		LOS_SemPost( BinarySem_Handle );    //给出二值信号量 xSemaphore		
		LOS_TaskYield();  									//放弃剩余时间片，进行一次任务切换	
	}
}

/******************************************************************
  * @ 函数名  ： Creat_Read_Task
  * @ 功能说明： 创建Read_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Read_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//定义一个用于创建任务的参数结构体
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Read_Task";/* 任务名 */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Read_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;		/* 堆栈大小 */

	uwRet = LOS_TaskCreate(&Read_Task_Handle, &task_init_param);/* 创建任务 */
	return uwRet;
}
/*******************************************************************
  * @ 函数名  ： Creat_Write_Task
  * @ 功能说明： 创建Write_Task任务
  * @ 参数    ：   
  * @ 返回值  ： 无
  ******************************************************************/
static UINT32 Creat_Write_Task()
{
	//定义一个返回类型变量，初始化为LOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* 任务优先级，数值越小，优先级越高 */
	task_init_param.pcName = "Write_Task";	/* 任务名*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Write_Task;/* 任务函数入口 */
	task_init_param.uwStackSize = 1024;	/* 堆栈大小 */
	
	uwRet = LOS_TaskCreate(&Write_Task_Handle, &task_init_param);/* 创建任务 */

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
			
		uwRet = Creat_Read_Task();
		if (uwRet != LOS_OK)
		{
			printf("Read_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Write_Task();
		if (uwRet != LOS_OK)
		{
			printf("Write_Task任务创建失败！失败代码0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
