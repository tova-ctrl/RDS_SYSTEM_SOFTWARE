/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32h7xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h>

/* Dumps the stacked exception frame and fault status registers, then hangs.
   Called from the inline-ASM trampoline below so R0 already holds the SP. */
static void fault_dump(uint32_t *sp, const char *label)
{
    printf("\r\n!!! %s PC=0x%08lX LR=0x%08lX\r\n", label, sp[6], sp[5]);
    printf("    R0=%08lX R1=%08lX R2=%08lX R3=%08lX\r\n", sp[0],sp[1],sp[2],sp[3]);
    printf("    HFSR=%08lX CFSR=%08lX\r\n", (unsigned long)SCB->HFSR, (unsigned long)SCB->CFSR);
    printf("    BFAR=%08lX MMFAR=%08lX\r\n",(unsigned long)SCB->BFAR,(unsigned long)SCB->MMFAR);

    // *** TEMPORARY — DEMO ONLY (2026-08-25), explicit user request under time
    // pressure ("אני צריכה להציג משהו - אז אם אפשר זמנית"). Does NOT fix or
    // undo whatever memory corruption caused this fault (see
    // [[ism-stm32-fault-handlers]] memory for the full safety discussion —
    // this is knowingly not a real fix for a safety-module firmware). Skips
    // the single faulting instruction and resumes instead of resetting:
    // whatever that instruction was trying to read/write silently does NOT
    // happen, and nothing downstream is told that. REMOVE THIS BLOCK and
    // restore a plain `NVIC_SystemReset();` (or, better, find and fix the
    // actual root cause via the PC printed above) once the demo is over —
    // do not leave a safety module running like this.
    //
    // Mechanism: decode the Thumb/Thumb-2 instruction at the faulting PC to
    // get its real length (2 or 4 bytes — a 32-bit Thumb-2 instruction's
    // first halfword always has bits[15:11] = 0b11101/11110/11111), advance
    // the STACKED pc (sp[6], not the live PC — this is the auto-saved
    // exception frame the hardware will restore FROM on return) past it, and
    // clear the fault-status bits (CFSR/HFSR are write-1-to-clear, so writing
    // back what was just read clears every bit that was set). Falling off the
    // end of this function relies on fault_dump() being an ordinary
    // (non-naked) C function reached via a plain `b` (not `bl`) from the
    // naked trampoline — LR still holds the original EXC_RETURN value from
    // exception entry, and a normal function's own prologue/epilogue
    // preserves LR around the printf() calls above by standard AAPCS
    // convention — so `bx lr` here correctly triggers a real hardware
    // exception return, resuming execution at the (advanced) stacked pc.
    {
        uint16_t *faultingInstr = (uint16_t *)(sp[6] & ~1u);
        uint16_t opword = *faultingInstr;
        uint32_t top5 = (opword >> 11) & 0x1Fu;
        uint32_t instrLen = (top5 == 0x1Du || top5 == 0x1Eu || top5 == 0x1Fu) ? 4u : 2u;
        sp[6] += instrLen;
        SCB->CFSR = SCB->CFSR;
        SCB->HFSR = SCB->HFSR;
        printf("    !!! DEMO MODE: skipped faulting instr (len=%lu), resuming PC=0x%08lX\r\n",
               (unsigned long)instrLen, sp[6]);
    }
}

// 2026-08-25: fault_dump() above was written but never actually reached — the
// real HardFault_Handler further down was still CubeMX's untouched empty
// `while(1){}` template, so a HardFault just silently hung the board forever
// (no UART output, no reset, indistinguishable from any other freeze). This
// is the trampoline fault_dump()'s own comment already said should exist:
// `tst lr,#4` reads bit 2 of the EXC_RETURN value in LR to tell whether the
// CPU was using MSP or PSP at the moment of the fault (the standard Cortex-M
// pattern — must be inline asm/naked, since by the time any ordinary C
// function prologue runs, the original SP is already gone), puts that
// pointer in R0 and the fault-type string's address in R1 (matching
// fault_dump(uint32_t *sp, const char *label)'s AAPCS argument registers),
// then branches (not calls — fault_dump never returns, it ends in
// NVIC_SystemReset()) straight into it.
static const char s_hardfault_label[] = "HardFault";
// 2026-08-25: same treatment extended to the other three configurable faults
// (enabled via SCB_SHCSR_MEMFAULTENA/BUSFAULTENA/USGFAULTENA in main.c's
// USER CODE Init block so they don't silently escalate to HardFault) — that
// enabling comment claimed these "now print their own diagnostic message",
// which wasn't actually true until now; all three were still CubeMX's empty
// while(1) template, same silent-forever-hang gap as HardFault was.
static const char s_memfault_label[]   = "MemManage";
static const char s_busfault_label[]   = "BusFault";
static const char s_usagefault_label[] = "UsageFault";
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */
  __asm volatile
  (
    " tst lr, #4                \n"
    " ite eq                    \n"
    " mrseq r0, msp              \n"
    " mrsne r0, psp              \n"
    " ldr r1, =s_hardfault_label \n"
    " b fault_dump               \n"
  );
  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */
  __asm volatile
  (
    " tst lr, #4                \n"
    " ite eq                    \n"
    " mrseq r0, msp              \n"
    " mrsne r0, psp              \n"
    " ldr r1, =s_memfault_label  \n"
    " b fault_dump               \n"
  );
  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */
  __asm volatile
  (
    " tst lr, #4                \n"
    " ite eq                    \n"
    " mrseq r0, msp              \n"
    " mrsne r0, psp              \n"
    " ldr r1, =s_busfault_label  \n"
    " b fault_dump               \n"
  );
  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */
  __asm volatile
  (
    " tst lr, #4                  \n"
    " ite eq                      \n"
    " mrseq r0, msp                \n"
    " mrsne r0, psp                \n"
    " ldr r1, =s_usagefault_label  \n"
    " b fault_dump                 \n"
  );
  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1_CH1 and DAC1_CH2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */
  /* Kick IWDG1 from the timer ISR (priority 0) every ~1ms.
     This survives printf/MX_LWIP_Process blocking the main loop. */
  IWDG1->KR = 0xAAAAU;
  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/* USER CODE END 1 */
