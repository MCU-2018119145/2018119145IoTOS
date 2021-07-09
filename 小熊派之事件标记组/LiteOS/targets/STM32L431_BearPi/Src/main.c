/*
*2021.7.7 
*2018119145 �����
*/
//�¼������
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS ͷ�ļ� */
#include "los_task.ph"
#include "los_sys.h"
#include "los_sem.h"


#define KEY1_EVENT  (0x01 << 0)//�����¼������λ0
#define KEY2_EVENT  (0x01 << 1)//�����¼������λ1

#define KEY_ON	1
#define KEY_OFF	0

UINT32 g_TskHandle;
/* ���������� */
UINT32 LED_Task_Handle;
UINT32 Key_Task_Handle;

/* �����¼���־��Ŀ��ƿ� */
static EVENT_CB_S EventGroup_CB;


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
	LCD_ShowString(10, 20, 240, 24, 24, "Open the port");
  LCD_ShowString(10, 50, 240, 16, 16, "Please press the Key");
	
//	LCD_ShowString(10, 50, 240, 24, 24, "Welcome to IoTCluB!");
//	LCD_ShowString(20, 90, 240, 16, 16, "BearPi IoT Develop Board");
//	LCD_ShowString(20, 130, 240, 16, 16, "Powerd by Huawei LiteOS!");
//	LCD_ShowString(10, 170, 240, 16, 16, "Please wait for system init");
}
//��������һ
void task1(void)
{
	UINT32 uwRet;			
  /* ������һ������ѭ�������ܷ��� */

	while(1)
	{
		/* �ȴ��¼���־�� */
		uwRet = LOS_EventRead(	&EventGroup_CB,        	//�¼���־�����
														KEY1_EVENT|KEY2_EVENT,  //�ȴ��������Ȥ���¼�
														LOS_WAITMODE_AND,     	//�ȴ���λ������λ
														LOS_WAIT_FOREVER ); 		//�����޵ȴ�
					
    if(( uwRet & (KEY1_EVENT|KEY2_EVENT)) == (KEY1_EVENT|KEY2_EVENT)) 
    {
			/* ���������ɲ�����ȷ */
			printf ( "KEY1��KEY2������\t\r\n");
		//	LCD_ShowString(10, 30, 240, 24, 24, "File success!   ");
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);	
			LCD_ShowString(10, 130, 240, 24, 24, "Event OK   ");
			
			LOS_EventClear(&EventGroup_CB, ~KEY1_EVENT);	//����¼���־
			LOS_EventClear(&EventGroup_CB, ~KEY2_EVENT);	//����¼���־
		}
		
	}
}

//���������
void task2(void)
{
	// ����һ���������ͱ�������ʼ��ΪLOS_OK
	UINT32 uwRet = LOS_OK;				
	
	/* ������һ������ѭ�������ܷ��� */
	while(1)
	{
		/* KEY1 ������ */
    if( Key_Scan(GPIOB,KEY1_Pin) == KEY_ON )       
		{
			LCD_ShowString(10, 70, 240, 24, 24, "Event1 success!");
			//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
      printf ( "KEY1������\t\r\n" );
			/* ����һ���¼�1 */
			LOS_EventWrite(&EventGroup_CB,KEY1_EVENT);  		
		}
		LOS_TaskDelay(20); 
    /* KEY2 ������ */
		if( Key_Scan(GPIOB,KEY2_Pin) == KEY_ON )      
		{
			LCD_ShowString(10, 100, 240, 24, 24, "Event2 success!");
			//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
      printf ( "KEY2������\t\r\n" );	
			/* ����һ���¼�2 */
			LOS_EventWrite(&EventGroup_CB,KEY2_EVENT); 				
		}
		LOS_TaskDelay(20);     //ÿ20msɨ��һ��		
	}
}

//��������һ
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

//���������
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
	/* ����һ���¼���־��*/
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
//�Ѵ��������������������
//����һ
	uwRet = creat_task1();
    if (uwRet != LOS_OK)
    {
      return LOS_NOK;
    }
//�����
	uwRet = creat_task2();
    if (uwRet != LOS_OK)
    {
      return LOS_NOK;
    }
	
    (void)LOS_Start();
    return 0;
}
