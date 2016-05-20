#include "ets_sys.h"
#include "osapi.h"
#include "driver/uart.h"
#include "osapi.h"
#include "driver/uart_register.h"
#include "mem.h"
#include "os_type.h"

// UartDev is defined and initialized in rom code.
extern UartDevice    UartDev;

#define UART_BUFF_EN 1
LOCAL struct UartBuffer* pTxBuffer = NULL;
LOCAL struct UartBuffer* pRxBuffer = NULL;

/*uart demo with a system task, to output what uart receives*/
/*this is a example to process uart data from task,please change the priority to fit your application task if exists*/
/*it might conflict with your task, if so,please arrange the priority of different task,  or combine it to a different event in the same task. */
#define uart_recvTaskPrio        0
#define uart_recvTaskQueueLen    1
os_event_t    uart_recvTaskQueue[uart_recvTaskQueueLen];

LOCAL void ICACHE_FLASH_ATTR uart_recvTask(os_event *events)
{
  /*
   * This Function is posted from rx interrupt handler 
   */
  if(events->sig == 0)
  {
    /*
     * Copy the data from hardware FIFO to pRxBuffer
     */
    Uart_rx_buff_enq(); 
  }
  else 
  {

  }
}
uint16 ICACHE_FLASH_ATTR uart0_data_available(void)
{
  return (pRxBuffer->UartBuffSize - pRxBuffer->Space);
}
char ICACHE_FLASH_ATTR rx_buff_deq_char(char *databuff, uint16 dataBuffLen)
{
  char ch = 0; 
  if(pRxBuffer->UartBuffSize ==  pRxBuffer->Space) //Empty buffer
  {
    return RX_BUFF_EMPTY; 
  }
  else 
  {
    ch = *(pRxBuffer->pOutPos); 
    pRxBuffer->pOutPos = (pRxBuffer->pOutPos + 1 ) % pRxBuffer->UartBuffSize;
    return ch 
}

/*
 * copies a line  in the buffer provided
 * return the strlen 
 */
int16 ICACHE_FLASH_ATTR rx_buff_deq_line(char *databuff, uint16 dataBuffLen)
{
  uint16 count = 0; 
  uint16 buf_len =  (pRxBuffer->UartBuffSize- pRxBuffer->Space);

  char ch;
  while(count < dataBuffLen) 
  {
    ch =  rx_buff_deq_char()
    if(ch == RX_BUFF_EMPTY) //Loop until we find a \n or \r character or buffer exhausts 
    {
      continue; 
    }
    if( ch == '\n' || ch == '\r' || ch == 0) 
    {
      data[count] = 0;
      break;
    }
    databuff[count++] = ch; 
    pRxBuffer->pOutPos = (pRxBuffer->pOutPos + 1 ) % pRxBuffer->UartBuffSize;
  }


 return count;  
}
void ICACHE_FLASH_ATTR
uart_init(UartBautRate uart0_br, UartBautRate uart1_br)
{
    /*this is a example to process uart data from task,please change the priority to fit your application task if exists*/
    system_os_task(uart_recvTask, uart_recvTaskPrio, uart_recvTaskQueue, uart_recvTaskQueueLen);  //demo with a task to process the uart data
    
    UartDev.baut_rate = uart0_br;
    uart_config(UART0);
    UartDev.baut_rate = uart1_br;
    uart_config(UART1);
    ETS_UART_INTR_ENABLE();
    
    #if UART_BUFF_EN
    pTxBuffer = Uart_Buf_Init(UART_TX_BUFFER_SIZE);
    pRxBuffer = Uart_Buf_Init(UART_RX_BUFFER_SIZE);
    #endif


    /*option 1: use default print, output from uart0 , will wait some time if fifo is full */
    //do nothing...

    /*option 2: output from uart1,uart1 output will not wait , just for output debug info */
    /*os_printf output uart data via uart1(GPIO2)*/
    //os_install_putc1((void *)uart1_write_char);    //use this one to output debug information via uart1 //
    
    /*option 3: output from uart0 will skip current byte if fifo is full now... */
    /*see uart0_write_char_no_wait:you can output via a buffer or output directly */
    /*os_printf output uart data via uart0 or uart buffer*/
    //os_install_putc1((void *)uart0_write_char_no_wait);  //use this to print via uart0

    os_install_putc1((void *)uart1_write_char);  //use this to print via uart0
    
    #if UART_SELFTEST&UART_BUFF_EN
    os_timer_disarm(&buff_timer_t);
    os_timer_setfn(&buff_timer_t, uart_test_rx , NULL);   //a demo to process the data in uart rx buffer
    os_timer_arm(&buff_timer_t,10,1);
    #endif
}

