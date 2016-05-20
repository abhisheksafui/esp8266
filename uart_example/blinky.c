#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "uart.h"

static volatile os_timer_t some_timer;
static volatile os_timer_t hello_timer;

void process_uart_data(char *data)
{
  os_printf("\nNeed to process UART0 DATA");
  tx_buff_enq(data,os_strlen(data));
}
void hello_timerfunc(void *arg)
{
  tx_buff_enq("HELLO",os_strlen("HELLO"));
}
void some_timerfunc(void *arg)
{
  //Call the rx_deq function to get uart data if any
#define RX_BUFF_LEN 100

  int ret = 0;
  char rx_buff[RX_BUFF_LEN];
  if(uart0_data_available()) 
  {
    ret = rx_buff_deq_line(rx_buff,RX_BUFF_LEN);
    if(ret == RX_BUFF_LEN)
    {
      os_printf("\nBuffer Overflow in rx_buff_deq_line");
    } 
    os_printf("\nMessage Received in UART0");
    os_printf("\nMessage : %s",rx_buff);

    process_uart_data(rx_buff); 
  }

}

void ICACHE_FLASH_ATTR user_init()
{
	UartBautRate uart1_baud = BIT_RATE_115200;
	UartBautRate uart0_baud = BIT_RATE_115200;
	uart_init(uart0_baud, uart1_baud);
  // setup timer (500ms, repeating)
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
  os_timer_arm(&some_timer, 10, 1);

  os_timer_setfn(&hello_timer, (os_timer_func_t *)hello_timerfunc, NULL);
  os_timer_arm(&some_timer, 2000, 1);

}
