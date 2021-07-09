/*
*2021.7.7 
*2018119145 �����
*/
//�����ʱ��
#include "main.h"
#include "sys_init.h"

#include "usart.h"
#include "gpio.h"
/* LiteOS ͷ�ļ� */
#include "los_task.ph"
#include "los_sys.h"
#include "los_swtmr.h"


UINT32 g_TskHandle;

/* ���嶨ʱ�������ID�� */
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

/*********************************************************************************
  * @ ������  �� Timer1_Callback
  * @ ����˵���� �����ʱ���ص�����1
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer1_Callback(UINT32 arg)
{
    UINT32 tick_num1;

    TmrCb_Count1++;						/* ÿ�ص�һ�μ�һ */
		LCD_ShowString(10, 50, 240, 24, 24, "Timer1 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    LOS_TaskDelay(300);
	  tick_num1 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
    printf("Timer_CallBack_Count1=%d\t\r\n", TmrCb_Count1);
    printf("tick_num1=%d\t\r\n", tick_num1);
}
/*********************************************************************************
  * @ ������  �� Timer2_Callback
  * @ ����˵���� �����ʱ���ص�����2
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer2_Callback(UINT32 arg)
{
    UINT32 tick_num2;
	
    TmrCb_Count2++;				/* ÿ�ص�һ�μ�һ */
		LCD_ShowString(10, 90, 240, 24, 24, "Timer2 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
	  
    tick_num2 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
    printf("Timer_CallBack_Count2=%d\t\r\n", TmrCb_Count2);
	
    printf("tick_num2=%d\t\r\n", tick_num2);
    
}
/*********************************************************************************
  * @ ������  �� Timer3_Callback
  * @ ����˵���� �����ʱ���ص�����2
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer3_Callback(UINT32 arg)
{
    UINT32 tick_num3;
	
    TmrCb_Count3++;				/* ÿ�ص�һ�μ�һ */
		LCD_ShowString(10, 120, 240, 24, 24, "Timer3 success!");
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	  LOS_TaskDelay(300);
    tick_num3 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
    printf("Timer_CallBack_Count3=%d\t\r\n", TmrCb_Count3);
	
    printf("tick_num3=%d\t\r\n", tick_num3);
    
}

/*********************************************************************************
  * @ ������  �� Timer4_Callback
  * @ ����˵���� �����ʱ���ص�����2
  * @ ����    �� ����1����������δʹ��  
  * @ ����ֵ  �� ��
  ********************************************************************************/
static void Timer4_Callback(UINT32 arg)
{
    UINT32 tick_num4;
	
    TmrCb_Count4++;				/* ÿ�ص�һ�μ�һ */
		LCD_ShowString(10, 50, 240, 24, 24, "                 ");
		LCD_ShowString(10, 90, 240, 24, 24, "                 ");
	  LCD_ShowString(10, 120, 240, 24, 24, "                 ");
	
	  LOS_TaskDelay(300);
    tick_num4 = (UINT32)LOS_TickCountGet();	/* ��ȡ�δ�ʱ���ļ���ֵ */
	
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
	
		/* ����һ�������ʱ����ʱ��*/
		uwRet = LOS_SwtmrCreate(1000, 					/* �����ʱ���Ķ�ʱʱ��*/	
														LOS_SWTMR_MODE_ONCE, 	/* �����ʱ��ģʽ һ��ģʽ */
														(SWTMR_PROC_FUNC)Timer1_Callback,		/* �����ʱ���Ļص����� */
														&Timer1_Handle,					/* �����ʱ����id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("�����ʱ��Timer1����ʧ�ܣ�\n");
			}
		uwRet = LOS_SwtmrCreate(3000, 					/* �����ʱ���Ķ�ʱʱ�䣨ms��*/	
														LOS_SWTMR_MODE_PERIOD, 	/* �����ʱ��ģʽ ����ģʽ */
														(SWTMR_PROC_FUNC)Timer2_Callback,		/* �����ʱ���Ļص����� */
														&Timer2_Handle,			/* �����ʱ����id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("�����ʱ��Timer2����ʧ�ܣ�\n");
			}
		uwRet = LOS_SwtmrCreate(5000, 					/* �����ʱ���Ķ�ʱʱ�䣨ms��*/	
														LOS_SWTMR_MODE_PERIOD, 	/* �����ʱ��ģʽ ����ģʽ */
														(SWTMR_PROC_FUNC)Timer3_Callback,		/* �����ʱ���Ļص����� */
														&Timer3_Handle,			/* �����ʱ����id */
														0,0,0);		
		if (uwRet != LOS_OK)
			{
			printf("�����ʱ��Timer3����ʧ�ܣ�\n");
			}
		uwRet = LOS_SwtmrCreate(7000, 					/* �����ʱ���Ķ�ʱʱ�䣨ms��*/	
														LOS_SWTMR_MODE_PERIOD, 	/* �����ʱ��ģʽ ����ģʽ */
														(SWTMR_PROC_FUNC)Timer4_Callback,		/* �����ʱ���Ļص����� */
														&Timer4_Handle,			/* �����ʱ����id */
														0,0,0);		
		
		if (uwRet != LOS_OK) 
		{
			printf("�����ʱ��Timer3����ʧ�ܣ�\n");
			return uwRet;
		}
		
		/* 1.����һ�������ʱ����ʱ��*/
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
		/* 2.����һ�������ʱ����ʱ��*/
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
		/* 3.����һ�������ʱ����ʱ��*/
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
		/* 4.����һ�������ʱ����ʱ��*/
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
