/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
	#include "stdio.h"
	#include "Protocol.h"
	#include "command.h"
	#include "dobot.h"
	extern "C"
	{
		#include "hmac_sha1.h"
		#include "emw3080.h"
	}
	#include "string.h"
//	#include "cJSON.h"
	#include "ArduinoJson.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define Motor_Start 		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)
#define Motor_Stop 			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)
#define Motor_Forward		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
//#define Motor_Backward	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int tim4uart1;
int tim4uart3;

int INred;
int INgreen;
int INchops;
int INid;

int supply_red;
int supply_green;
int supply_chops;

int stop_flag = 0;

bool ack_flag;
bool cam_flag;

bool end_service_flag;
bool pause_service_flag;

int idle_count;

char service_json_buffer[MAX_LEN];
bool service_reply_pending_flag; 

char status_json_buffer[MAX_LEN];

State run_state;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void initial(void);
void service_reply(void);
void camera_request(int id);
bool json_status(int p_id,int p_status,int p_supply_red,int p_supply_green,int p_supply_chops,int p_cam_red,int p_cam_green,int p_cam_chops);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_UART4_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	initial();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	  
		if(service_reply_pending_flag) service_reply();

		switch (run_state){
			case IDLE :{
				printf("[INFO][IDLE]\r\n");
				end_service_flag = false;
				pause_service_flag = false;
				json_status(INid,1,supply_red,supply_green,supply_chops,-1,-1,-1);
				if(status_pub_set()) pub_data(status_json_buffer);
				
				for(idle_count = 0;idle_count < 30;idle_count++) 
				{
					if(service_reply_pending_flag) break;
					HAL_Delay(1000);
				}				
				
				break;
			};
			
			case CARRY_IN :{
				printf("[INFO][CARRY_IN]\r\n");
				INid++;
				json_status(INid,2,supply_red,supply_green,supply_chops,-1,-1,-1);
				if(status_pub_set()) pub_data(status_json_buffer);
						
				stop_flag = -1;
				Motor_Start;
				while(stop_flag != 0) HAL_Delay(100);
				
				run_state = CAMERA;
				break;
			};
			
			case CAMERA :{
				printf("[INFO][CAMERA]\r\n");
				open_cam_receive;
				json_status(INid,3,supply_red,supply_green,supply_chops,-1,-1,-1);
				if(status_pub_set()) pub_data(status_json_buffer);
				
				cam_flag = false;
				ack_flag = false;
				while((!ack_flag)&&(!cam_flag))
				{					
					printf("[INFO]Send Camera Request\r\n"); 
					
					camera_request(INid);
										HAL_UART_Receive_IT(&huart1,&uart1rbuffer,1);
					HAL_Delay(3000);
				}
			
				while(!cam_flag)
				{
					printf("[INFO][CAM]Waiting For Camera Result\r\n");
					HAL_Delay(3000);
				}
				json_status(INid,3,supply_red,supply_green,supply_chops,INred,INgreen,INchops);
				if(status_pub_set()) pub_data(status_json_buffer);
				
				run_state = DOBOT;
				if((INred == 0)&&(supply_red == 0))run_state = WAIT_SUPPLY;
				if((INgreen == 0)&&(supply_green == 0))run_state = WAIT_SUPPLY;
				if((INchops == 0)&&(supply_chops == 0))run_state = WAIT_SUPPLY;
				break;
			};
			
			case WAIT_SUPPLY :{
				printf("[INFO][WAIT_SUPPLY]\r\n");
				json_status(INid,4,supply_red,supply_green,supply_chops,INred,INgreen,INchops);
				if(status_pub_set()) pub_data(status_json_buffer);		
				HAL_Delay(10000);
				break;
			};
			
			case DOBOT :{
				printf("[INFO][DOBOT]\r\n");
				json_status(INid,5,supply_red,supply_green,supply_chops,INred,INgreen,INchops);
				if(status_pub_set()) pub_data(status_json_buffer);	
				
				stop_flag = -1;
				Motor_Start;
				while(stop_flag != 0)HAL_Delay(100);
				
				/*if(INchops == 0)
				{
					grip_chops(supply_chops);
					supply_chops--;
				}*/
				if(INred == 0)
				{
					grip_RS(supply_red);
					supply_red--;
				}
				if(INgreen == 0)
				{
					grip_GS(supply_green);
					supply_green--;
				}
				json_status(INid,5,supply_red,supply_green,supply_chops,INred,INgreen,INchops);
				if(status_pub_set()) pub_data(status_json_buffer);
				run_state = CARRY_OUT;
				break;
			};
			
			case CARRY_OUT :{
				printf("[INFO][CARRY_OUT]\r\n");
				json_status(INid,6,supply_red,supply_green,supply_chops,INred,INgreen,INchops);
				if(status_pub_set()) pub_data(status_json_buffer);
				if(end_service_flag)
				{
					INid = 0;
					run_state = IDLE;
					stop_flag = 1500;
					Motor_Start;
					HAL_Delay(1500);
				}
				else if(pause_service_flag)
				{
					run_state = IDLE;
					stop_flag = 1500;
					Motor_Start;
					HAL_Delay(1500);
				}
				else run_state = CARRY_IN;
				break;
			};
			default:{	
				printf("[ERROR]Unknown Run_State\r\n");
				run_state = IDLE;
				break;
			};
		}
		  
  }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_UART4;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{	
	if(GPIO_Pin == GPIO_PIN_0) stop_flag = 100;
}
//TODO:���봫�ʹ���ʱ
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	//10ms??????��?
{
	int i;
	if(htim == &htim4)
	{
		if(stop_flag == 0) Motor_Stop; 
		if(stop_flag > 0) stop_flag--;
			
		if(tim4uart1 < 300)
		{
			if(tim4uart1 > 0) tim4uart1--;
			else
			{
				close_cam_receive;
				printf("[INFO]receive msg from cam:\r\n%s\r\n",uart1_receive_buffer);
				if(strstr(uart1_receive_buffer,"id") != NULL)
				{
					StaticJsonBuffer<200> jsonBuffer_uart1;
					JsonObject& root_uart1 = jsonBuffer_uart1.parseObject(uart1_receive_buffer);
					if((root_uart1.containsKey("id")) && (root_uart1["id"] == INid))
					{
						if (root_uart1["ack"] == 0)
						{
							root_uart1["ack"] = 1;
							for(i = 0;i < MAX_LEN;i++)uart1_transmit_buffer[i] = '\0';
							root_uart1.printTo(uart1_transmit_buffer,MAX_LEN);
							HAL_UART_Transmit(&huart1,(uint8_t*)uart1_transmit_buffer,strlen(uart1_transmit_buffer),10);
							INred = root_uart1["red"];
							INgreen = root_uart1["green"];
							INchops = root_uart1["chops"];
							printf("[INFO][CAM]Red:%d Green:%d Chops:%d\r\n",INred,INgreen,INchops);
							cam_flag = true;
						}
						else 
						{
							ack_flag = true;
						}
					}
					jsonBuffer_uart1.clear();
				}
				open_cam_receive;
			}
		}
		if(tim4uart3 < 300)
		{
			if(tim4uart3 > 0)
			{
				tim4uart3--;
			}
			else
			{
				close_service_receive;
				printf("[INFO]receive msg from wifi:\r\n%s\r\n",wifi_receive_buffer);
				char *json_start = NULL;
				json_start = strchr(wifi_receive_buffer,'{');
				if((json_start != NULL)&&(! service_reply_pending_flag))
				{
					for(i = 1;i < MAX_LEN;i++)service_json_buffer[i] = '\0';
					strcpy(service_json_buffer,json_start);
					service_reply_pending_flag = true;
				}
			}
		}
	}
}


void initial(void)
{
	printf("\r\n======Enetr demo application======\r\n");
	
	Motor_Stop;
	Motor_Forward;
	end_service_flag = false;
	pause_service_flag = false;
	stop_flag = 0;
	service_reply_pending_flag = false;
	
	supply_red = 2;
	supply_green = 2;
	supply_chops = 2;
	
	run_state = IDLE;
	INid = 0;
	close_cam_receive;
	close_service_receive;
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_UART_Receive_IT(&huart4,&uart4rbuffer,1);
	HAL_UART_Receive_IT(&huart1,&uart1rbuffer,1);
	HAL_UART_Receive_IT(&huart3,&uart3rbuffer,1);
	dobot_init();
	if(emw3080_init()) Ali_connect();
}

void service_reply(void)
{
	int i;
	close_service_receive;
	StaticJsonBuffer<200> jsonBuffer_uart3;
	JsonObject& root_uart3 = jsonBuffer_uart3.parseObject(service_json_buffer);
	char service_method[30];
	strcpy(service_method,root_uart3["method"]); 
	if(strstr(service_method,"Start") != NULL)
	{
		printf("[INFO][SERVICE]Start\r\n");
		if(run_state == IDLE)run_state = CARRY_IN;
		service_pub_set("Start");
	}
					
	if(strstr(service_method,"Pause") != NULL)
	{
		printf("[INFO][SERVICE]Pause\r\n");
		pause_service_flag = true;
		service_pub_set("Pause");
	}
					
	if(strstr(service_method,"End") != NULL)
	{
		printf("[INFO][SERVICE]End\r\n");
		end_service_flag = true;
		service_pub_set("End");
	}
	
	if(strstr(service_method,"SuppliesRefresh") != NULL)
	{
		printf("[INFO][SERVICE]Supplies Refresh\r\n");
		if(run_state == WAIT_SUPPLY)
		{
			run_state = DOBOT;
			supply_red = 2;
			supply_green = 2;
			supply_chops = 2;
		}
		service_pub_set("SuppliesRefresh");
	}
	
	root_uart3.remove("method");
	root_uart3.remove("version");
	root_uart3.remove("params");
	root_uart3["code"] = 200;
	root_uart3.createNestedObject("data");
	char service_reply[MAX_LEN];
	for(i = 0;i < MAX_LEN;i++)service_reply[i] = '\0';
	root_uart3.printTo(service_reply,MAX_LEN);
	strcat(service_reply,"\r");
	pub_data(service_reply);
	jsonBuffer_uart3.clear();
	service_reply_pending_flag = false;
	open_service_receive;
}

void camera_request(int id)
{
	int i;
	StaticJsonBuffer<200> jsonBuffer_uart1;
	JsonObject& root_uart1 = jsonBuffer_uart1.createObject();
	root_uart1["id"] = id;
	root_uart1["ready"] = 1;
	root_uart1["ack"] = 0;
	char camera_request_buffer[MAX_LEN];
	for(i = 0;i < MAX_LEN;i++)camera_request_buffer[i] = '\0';
	root_uart1.printTo(camera_request_buffer,MAX_LEN);
	strcat(camera_request_buffer,"\r");
	HAL_UART_Transmit(&huart1,(uint8_t *)camera_request_buffer,strlen(camera_request_buffer),10);
	jsonBuffer_uart1.clear();
}

bool json_status(int p_id,int p_status,int p_supply_red,int p_supply_green,int p_supply_chops,int p_cam_red,int p_cam_green,int p_cam_chops)
{
	int i;

	StaticJsonBuffer<400> jsonBuffer_status;
	JsonObject& root_status = jsonBuffer_status.createObject();
	root_status["id"] = "100";
	root_status["version"] = "1.0";
	root_status["method"] = "thing.event.property.post";
	root_status.createNestedObject("params");
	root_status["params"]["ID"] = p_id;
	root_status["params"]["Status"] = p_status;
	if(p_supply_red > -1)root_status["params"]["RedSauce"] = p_supply_red;
	if(p_supply_green > -1)root_status["params"]["GreenSauce"] = p_supply_green;
	if(p_supply_chops > -1)root_status["params"]["Chopsticks"] = p_supply_chops;
	if(p_cam_red > -1)root_status["params"]["RedSauceDetect"] = p_cam_red;
	if(p_cam_green > -1)root_status["params"]["GreenSauceDetect"] = p_cam_green;
	if(p_cam_chops > -1)root_status["params"]["ChopsticksDetect"] = p_cam_chops;
	
	for(i = 0;i < MAX_LEN;i++)status_json_buffer[i] = '\0';
	root_status.printTo(status_json_buffer,MAX_LEN);
	jsonBuffer_status.clear();
	strcat(status_json_buffer,"\r");
			//printf("[INFO][STATUS]Json generate Success:%s\n",status_json_buffer);
	return true;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/