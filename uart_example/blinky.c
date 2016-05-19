#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

static volatile os_timer_t some_timer;


void some_timerfunc(void *arg)
{
  //Do blinky stuff
  if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & (1 << pin))
  {
    // set gpio low
    gpio_output_set(0, (1 << pin), 0, 0);
  }
  else
  {
    // set gpio high
    gpio_output_set((1 << pin), 0, 0, 0);
  }
  //Call the rx_deq function to get uart data if any

}

void ICACHE_FLASH_ATTR user_init()
{
	UartBautRate uart1_baud = BIT_RATE_115200;
	UartBautRate uart0_baud = BIT_RATE_9600;
	uart_init(uart0_baud, uart1_baud);
  // setup timer (500ms, repeating)
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);
  os_timer_arm(&some_timer, 500, 1);
}
