/*
*2021.7.7 
*2018119145 李嘉雯
*/
//任务管理
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
 /* LiteOS 头文件 */
#include "los_task.ph"
#include "los_sys.h"

UINT32 g_TskHandle;

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
//定义任务一
void task1(void)
{
	//int count = 1;
	while (1)
	{
		  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
			LOS_TaskDelay(500);	
	    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);	
		  LOS_TaskDelay(500);
			LCD_ShowString(10, 50, 240, 24, 24, "Task1 success");
			printf("Task1 success\t\r\n");
	}
}
//创建任务一
UINT32 creat_task1()
{
	UINT32 uwRet = LOS_OK;
	TSK_INIT_PARAM_S task_init_param;
	task_init_param.usTaskPrio = 0;
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

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
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
	uwRet = creat_task1();
    if (uwRet != LOS_OK)
    {
      return LOS_NOK;
    }
	
    (void)LOS_Start();
    return 0;
}
