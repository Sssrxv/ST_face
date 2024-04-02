#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
//#include "AI3100.h"
#include "sys/aiva_utils.h"
#include "sys/syslog.h"

#define configUSE_PREEMPTION					1
#define configUSE_IDLE_HOOK						1
#define configUSE_TICK_HOOK						0
#define configTICK_RATE_HZ				        ( ( portTickType ) 1000 )
#define configMINIMAL_STACK_SIZE		        ( ( unsigned portSHORT ) 1024) /* This can be made smaller if required. */
#define configTOTAL_HEAP_SIZE			        ( ( size_t ) ( __HEAP_SIZE ) )
#define configMAX_TASK_NAME_LEN			        ( 16 )

/* TODO: disable trace and run-time-status when release */
void     appConfigTimerForRuntimeStat(void);
uint32_t appGetRuntimeCounterValue(void);
#define configUSE_TRACE_FACILITY                1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configGENERATE_RUN_TIME_STATS           1
//#define configUSE_TRACE_FACILITY                0
//#define configUSE_STATS_FORMATTING_FUNCTIONS    0
//#define configGENERATE_RUN_TIME_STATS           0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS  appConfigTimerForRuntimeStat
#define portGET_RUN_TIME_COUNTER_VALUE          appGetRuntimeCounterValue

#define configUSE_16_BIT_TICKS      	        0
#define configIDLE_SHOULD_YIELD			        1
#define configUSE_MUTEXES				        1
#define configCHECK_FOR_STACK_OVERFLOW	        1 /* Do not use this option on the PC port. */
#define configUSE_RECURSIVE_MUTEXES		        1
#define configQUEUE_REGISTRY_SIZE				20
#define configUSE_MALLOC_FAILED_HOOK			1
#define configUSE_APPLICATION_TASK_TAG			1
#define configUSE_COUNTING_SEMAPHORES			1
#define configUSE_ALTERNATIVE_API				0
//#define configMAX_SYSCALL_INTERRUPT_PRIORITY	1

#define configUSE_QUEUE_SETS					1
#define configUSE_TASK_NOTIFICATIONS			1

/* Software timer related configuration options. */
#define configUSE_TIMERS						1
#define configTIMER_TASK_PRIORITY				( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH				20
#define configTIMER_TASK_STACK_DEPTH			( configMINIMAL_STACK_SIZE * 2 )
#define configMAX_PRIORITIES		            ( 10 )

//FIXME: wilzhang
#define configUNIQUE_INTERRUPT_PRIORITIES               32
//#define configINTERRUPT_VECTOR_ADDRESS	            0xFFFFF010UL
#define configINTERRUPT_CONTROLLER_BASE_ADDRESS         AI3100_GIC_BASE_ADDR
#define configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET 0x100
void vConfigureTickInterrupt( void );
#define configSETUP_TICK_INTERRUPT()                    vConfigureTickInterrupt()
void vClearTickInterrupt( void );
#define configCLEAR_TICK_INTERRUPT()                    vClearTickInterrupt()
#define configMAX_API_CALL_INTERRUPT_PRIORITY           18
#define configFPU_D32	                                0

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function.  In most cases the linker will remove unused
functions anyway. */
#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelay						1
#define INCLUDE_uxTaskGetStackHighWaterMark 	0 /* Do not use this option on the PC port. */
#define INCLUDE_xTaskGetSchedulerState			1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	1
#define INCLUDE_xTaskGetIdleTaskHandle			1
#define INCLUDE_pcTaskGetTaskName				1
#define INCLUDE_eTaskGetState					1
#define INCLUDE_xSemaphoreGetMutexHolder		1
#define INCLUDE_xTimerPendFunctionCall			1
#define INCLUDE_xTaskAbortDelay					1
#define INCLUDE_xTaskGetHandle					1

/* It is a good idea to define configASSERT() while developing.  configASSERT()
uses the same semantics as the standard C assert() macro. */
//extern void vAssertCalled( unsigned long ulLine, const char * const pcFileName );
//#define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __LINE__, __FILE__ )
//#define configASSERT( x )

#define configUSE_GCC_BUILTIN_ATOMICS           1
#define configUSE_POSIX_ERRNO                   1
#define configSUPPORT_STATIC_ALLOCATION         1

#define configUSE_TASK_FPU_SUPPORT              2 // create task with FPU context

int xPortIsInISR(void);
	
#endif /* FREERTOS_CONFIG_H */
