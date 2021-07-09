/*
*2021.7.7 
*2018119145 �����
*/
//��Ϣ����
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS ͷ�ļ� */
#include "los_sys.h"
#include "los_task.ph"
#include "los_queue.h"


#define KEY1_Pin GPIO_PIN_2
#define KEY1_GPIO_Port GPIOB

#define KEY2_Pin GPIO_PIN_3
#define KEY2_GPIO_Port GPIOB

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;

/* ���������� */
UINT32 Receive_Task_Handle;
UINT32 Send_Task_Handle;

 /* ������Ϣ���еľ�� */
UINT32 Test_Queue_Handle;
/* ������Ϣ���г��� */
#define  TEST_QUEUE_LEN  	10
#define  TEST_QUEUE_SIZE  10

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
  * @ ������  �� Send_Task
  * @ ����˵���� Send_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Send_Task(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;				
	
	//Ҫ���͵���Ϣ
	UINT32 send_data1 = 1;
	UINT32 send_data2 = 2;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		LCD_ShowString(10, 30, 240, 24, 24, "Open the port");
    LCD_ShowString(10, 60, 240, 16, 16, "Please press the Key");
		/* K1 ������ */
		if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )      
		{
			/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
															&send_data1,				/* д�루���ͣ������� */
															sizeof(send_data1),	/* ���ݵĳ��� */
															0);									/* �ȴ�ʱ��Ϊ 0  */
			if(LOS_OK != uwRet)
			{
				printf("���ݲ��ܷ��͵���Ϣ���У��������0x%X\n",uwRet);
			}
		}
		LOS_TaskDelay(20);  
		
		/* K2 ������ */
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON ) 
		{
			/* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
			uwRet = LOS_QueueWrite(	Test_Queue_Handle,	/* д�루���ͣ����е�ID(���) */
															&send_data2,				/* д�루���ͣ������� */
															sizeof(send_data2),	/* ���ݵĳ��� */
															0);									/* �ȴ�ʱ��Ϊ 0  */
			if(LOS_OK != uwRet)
			{
				printf("���ݲ��ܷ��͵���Ϣ���У��������0x%X\n",uwRet);
			}
		}
		/* 20msɨ��һ�� */
		LOS_TaskDelay(20);       
	}
}

/******************************************************************
  * @ ������  �� Receive_Task
  * @ ����˵���� Receive_Task����ʵ��
  * @ ����    �� NULL 
  * @ ����ֵ  �� NULL
  *****************************************************************/
static void Receive_Task(void)
{
	/* ����һ���������ͱ�������ʼ��ΪLOS_OK */
	UINT32 uwRet = LOS_OK;
	UINT32 *r_queue;
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
		uwRet = LOS_QueueRead(Test_Queue_Handle,	/* ��ȡ�����գ����е�ID(���) */
													&r_queue,			/* ��ȡ�����գ������ݱ���λ�� */
													TEST_QUEUE_SIZE,		/* ��ȡ�����գ������ݵĳ��� */
													LOS_WAIT_FOREVER); 	/* �ȴ�ʱ�䣺һֱ�� */
		if(LOS_OK == uwRet)
		{
			printf("���ν��յ���������%x\t\r\n",*r_queue);
		}
		else
		{
			printf("���ݽ��ճ���,�������0x%X\n",uwRet);
		}
	}
}

/******************************************************************
  * @ ������  �� Creat_Receive_Task
  * @ ����˵���� ����Receive_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Receive_Task()
{
	//����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;			
	
	//����һ�����ڴ�������Ĳ����ṹ��
	TSK_INIT_PARAM_S task_init_param;	

	task_init_param.usTaskPrio = 5;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Receive_Task";/* ������ */
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Receive_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;		/* ��ջ��С */

	uwRet = LOS_TaskCreate(&Receive_Task_Handle, &task_init_param);/* �������� */
	return uwRet;
}


/*******************************************************************
  * @ ������  �� Creat_Send_Task
  * @ ����˵���� ����Send_Task����
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ******************************************************************/
static UINT32 Creat_Send_Task()
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	TSK_INIT_PARAM_S task_init_param;

	task_init_param.usTaskPrio = 4;	/* �������ȼ�����ֵԽС�����ȼ�Խ�� */
	task_init_param.pcName = "Send_Task";	/* ������*/
	task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)Send_Task;/* ��������� */
	task_init_param.uwStackSize = 1024;	/* ��ջ��С */
	
	uwRet = LOS_TaskCreate(&Send_Task_Handle, &task_init_param);/* �������� */

	return uwRet;
}


int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
	
    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
		/* ����һ�����Զ���*/
		uwRet = LOS_QueueCreate("Test_Queue",				/* ���е����� */
														TEST_QUEUE_LEN,			/* ���еĳ��� */
														&Test_Queue_Handle,	/* ���е�ID(���) */
														0,						/* ����ģʽ���ٷ���ʱδʹ�� */
														TEST_QUEUE_SIZE);	/* �ڵ��С����λΪ�ֽ� */
		if (uwRet != LOS_OK)
		{
			printf("Test_Queue���д���ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
			
		uwRet = Creat_Receive_Task();
		if (uwRet != LOS_OK)
		{
			printf("Receive_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}

		uwRet = Creat_Send_Task();
		if (uwRet != LOS_OK)
		{
			printf("Send_Task���񴴽�ʧ�ܣ�ʧ�ܴ���0x%X\n",uwRet);
			return uwRet;
		}
		(void)LOS_Start();
		return 0;
}
