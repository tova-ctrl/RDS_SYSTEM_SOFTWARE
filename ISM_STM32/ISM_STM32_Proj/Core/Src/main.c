/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "lwip/raw.h"
#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include <string.h>
#include "UartProtocol.h"
#include "IsmUartTransport.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

extern FDCAN_HandleTypeDef hfdcan1;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
extern struct netif gnetif;  /* defined in lwip.c */
extern volatile uint8_t g_eth_restart_needed;  /* set by low_level_output on TX error */

volatile uint8_t rx_buffer[2];  /* unused — kept for reference */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
extern void ism_init(void);
extern void ism_tick(void);
extern void ism_can_tick(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE BEGIN 0 */
#include "lwip/etharp.h"

// פונקציה שסורקת את טבלת ה-ARP הפנימית של lwIP ומדפיסה אותה ב-UART
void print_stm32_arp_table(void)
{
  printf("\r\n--- STM32 lwIP ARP Table ---\r\n");
  printf("IP Address      MAC Address       Status\r\n");
  printf("----------------------------------------\r\n");

  // לולאה הסורקת את כל הכניסות האפשריות בטבלת ה-ARP של lwIP
  for (int i = 0; i < ARP_TABLE_SIZE; i++) {
    ip4_addr_t *ipaddr;
    struct netif *netif;
    struct eth_addr *ethaddr;

    // שליפת הנתונים מתוך ליבת ה-lwIP
    if (etharp_get_entry(i, &ipaddr, &netif, &ethaddr)) {
      printf("%-15s %02X:%02X:%02X:%02X:%02X:%02X  ACTIVE\r\n",
             ip4addr_ntoa(ipaddr),
             ethaddr->addr[0], ethaddr->addr[1], ethaddr->addr[2],
             ethaddr->addr[3], ethaddr->addr[4], ethaddr->addr[5]);
    }
  }
  printf("----------------------------------------\r\n\r\n");
}

/* Actively ping the PC (192.168.55.5) FROM the ISM — the other direction from
   normal use (PC pinging us). lwIP's own icmp.c already auto-answers echo
   REQUESTS aimed at us; this is a separate raw-ICMP sender/listener so we can
   tell, from the STM32's own UART log, whether OUR transmit path actually
   gets a frame onto the wire and back — independent of whatever the PC side
   is doing with routes/ARP. Added 2026-08-03 while chasing the TX_EN/TXD0
   GPIO pin regression (see [[stm32-eth-gpio]]). */
static struct raw_pcb *s_ping_pcb = NULL;
static uint16_t s_ping_seq = 0;

static u8_t ping_recv_cb(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(pcb);
  if (p->tot_len >= PBUF_IP_HLEN + sizeof(struct icmp_echo_hdr)) {
    struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)((u8_t *)p->payload + PBUF_IP_HLEN);
    if (ICMPH_TYPE(iecho) == ICMP_ER) {
      printf("[PING] reply from %s seq=%u\r\n", ip4addr_ntoa(ip_2_ip4(addr)), lwip_ntohs(iecho->seqno));
      pbuf_free(p);
      return 1; /* eaten — don't let lwIP's core also try to process it */
    }
  }
  return 0;
}

static void ping_send_to_pc(void)
{
  const size_t data_len = 32;
  const size_t ping_size = sizeof(struct icmp_echo_hdr) + data_len;
  ip_addr_t target;
  IP4_ADDR(ip_2_ip4(&target), 192, 168, 55, 5);

  if (s_ping_pcb == NULL) {
    s_ping_pcb = raw_new(IP_PROTO_ICMP);
    if (s_ping_pcb == NULL) { printf("[PING] raw_new failed\r\n"); return; }
    raw_recv(s_ping_pcb, ping_recv_cb, NULL);
    raw_bind(s_ping_pcb, IP_ADDR_ANY);
  }

  struct pbuf *p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (p == NULL) { printf("[PING] pbuf_alloc failed\r\n"); return; }

  struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)p->payload;
  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id = PP_HTONS(0x4953); /* 'IS' — ISM */
  iecho->seqno = lwip_htons(++s_ping_seq);
  {
    char *data = (char *)iecho + sizeof(struct icmp_echo_hdr);
    for (size_t i = 0; i < data_len; i++) data[i] = (char)('a' + (i % 23));
  }
  iecho->chksum = inet_chksum(iecho, (u16_t)ping_size);

  printf("[PING] -> 192.168.55.5 seq=%u\r\n", s_ping_seq);
  raw_sendto(s_ping_pcb, p, &target);
  pbuf_free(p);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* Enable MemManage, BusFault, UsageFault so they don't silently escalate
     to HardFault — each now prints its own diagnostic message. */
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk
              | SCB_SHCSR_BUSFAULTENA_Msk
              | SCB_SHCSR_USGFAULTENA_Msk;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
//  SCB_DisableDCache();  //===TOVA TEST 4.7.26
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_USART3_UART_Init();
  MX_LWIP_Init();
  MX_FDCAN1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  //HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET); // הדלקת ה-PHY של הרשת (חובה בלוח Nucleo H7)
  //HAL_Delay(100); // המתנה קצרה להתעוררות הצ'יפ
  /* USER CODE END 2 */

  /* Infinite loop */
	ism_uart_rx_start();
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //ism_can_tick();
//	HAL_UART_Transmit(&huart2, (uint8_t*)my_data1, strlen(my_data1), 100); //===Tova 19.7.26
	//  HAL_UART_RxCpltCallback(&huart2);  //===Tova 19.7.26
  //    char tx_msg[] = "ID_HEARTBEAT_ACK 0x211\n";
   //   HAL_UART_Transmit(&huart2, (uint8_t*)tx_msg, strlen(tx_msg), 100);
	 /* השהייה של שנייה אחת (1000 מילישניות) בין שליחה לשליחה */
	/* MX_LWIP_Init()'s own generated comment says this must be called from
	   main's while(1) — it was never wired in. Without it, ethernetif_input()
	   never runs, so no received frame (ARP, ICMP, anything) ever actually
	   reaches lwIP, regardless of whether the DMA/PHY received it correctly
	   at the hardware level. This was the real reason the STM32's own ARP
	   table stayed empty even after the TX_EN/TXD0 GPIO fix (see
	   [[stm32-eth-gpio]]) — that GPIO bug was real too, but this is what
	   made RX (and therefore ANY two-way traffic) totally silent. */
	MX_LWIP_Process();
	HAL_Delay(1000);
    /* Ping the PC every 2s so we can watch, from the STM32's own UART log,
       whether the TX path actually works (see ping_send_to_pc above). */
   /* {
      static uint32_t s_ping_t = 0;
      if (HAL_GetTick() - s_ping_t >= 2000U) {
        s_ping_t = HAL_GetTick();
        ping_send_to_pc();
      }
    }*/
    /* הדפסת טבלת ה-ARP של הכרטיס בכל 5 שניות */
    {
      static uint32_t s_arp_print_t = 0;
      if (HAL_GetTick() - s_arp_print_t >= 10000U) {
        s_arp_print_t = HAL_GetTick();
        print_stm32_arp_table();
        /* ETH TX health — TPS (bit1) = TX DMA stopped (real failure).
           FBE (bit10) / TBU (bit2) are sticky accumulated bits that appear
           during normal operation and do NOT indicate current failure. */
        uint32_t dmacsr = heth.Instance->DMACSR;
        printf("[ETH] uptime=%lus DMACSR=0x%08lX%s\r\n",
               (unsigned long)(HAL_GetTick() / 1000U),
               (unsigned long)dmacsr,
               (dmacsr & (1UL<<1)) ? " TPS(TX_STOPPED!)" : "");
        /* Only restart on TPS — TX DMA actually stopped. */
        if (dmacsr & (1UL<<1)) {
            printf("[ETH] Restarting ETH (TPS set)\r\n");
            HAL_ETH_Stop_IT(&heth);
            HAL_ETH_Start_IT(&heth);
        }
      }
    }
  /* USER CODE END 3 */
  }
}
/* HAL_UART_RxCpltCallback moved to IsmUartTransport.c */



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 16;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 1;
  hfdcan1.Init.NominalTimeSeg2 = 1;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.RxFifo0ElmtsNbr = 0;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 0;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 239;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 999;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate =9600;  //115200;   //==Tova TST ==27.7.26 ==9600;  //==20.7.26==
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;  //9600;  //==Tova 25/8/26 == 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
