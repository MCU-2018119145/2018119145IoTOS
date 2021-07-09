/*
*2021.7.7 
*2018119145 �����
*/
//��ֵ�ź���ͬ��
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"

/* LiteOS ͷ�ļ� */
#include "los_task.ph"
#include "los_sys.h"
#include "los_sem.h"

UINT32 g_TskHandle;

/* ���������� */
UINT32 Read_Task_Handle;
UINT32 Write_Task_Handle;

/* �����ֵ�ź����ľ�� */
UINT32 BinarySem_Handle;

uint8_t ucValue [ 2 ] = { 0x00, 0x00 };

VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();//GPIO��ʼ��
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
  * @ ������  �� Read_Task
  * @ ����˵���� Read_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Read_Task(void)
{
  /* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER ); //��ȡ��ֵ�ź��� xSemaphore,û��ȡ����һֱ�ȴ�
		
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
		
		LOS_SemPost( BinarySem_Handle );   //������ֵ�ź��� xSemaphore
		
  }
}
/******************************************************************
  * @ ������  �� Write_Task
  * @ ����˵���� Write_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Write_Task(void)
{
	/* ����һ����������ķ������ͣ���ʼ��Ϊ�����ɹ��ķ���ֵ */
	UINT32 uwRet = LOS_OK;				
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LOS_SemPend( BinarySem_Handle , LOS_WAIT_FOREVER );  //��ȡ��ֵ�ź��� BinarySem_Handle��û��ȡ����һֱ�ȴ�		
		ucValue [ 0 ] ++;		
		LOS_TaskDelay ( 1000 );        			/* ��ʱ100ms */		
		ucValue [ 1 ] ++;		
		LOS_SemPost( BinarySem_Handle );    //������ֵ�ź��� xSemaphore		
		LOS_TaskYield();  									//����ʣ��ʱ��Ƭ������һ�������л�	
	}
}

/******************************************************************
  * @ ������  �� Creat_Read_Task
  * @ ����˵���� ����Read_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Read_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Read_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Read_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Read_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}
/*******************************************************************
  * @ ������  �� Creat_Write_Task
  * @ ����˵���� ����Write_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Write_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Write_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Write_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Write_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}

int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    /* ����һ����ֵ�ź���*/
		uwRet = LOS_BinarySemCreate(1,&BinarySem_Handle);
		if (uwRet != LOS_OK)
		{
			printf("BinarySem����ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
		}
			
		uwRet = Creat_Read_Task();
		if (uwRet != LOS_OK)
		{
			printf("Read_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		
		uwRet = Creat_Write_Task();
		if (uwRet != LOS_OK)
		{
			printf("Write_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
