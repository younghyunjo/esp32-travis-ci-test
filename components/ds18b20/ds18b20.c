/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
     along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "rom/ets_sys.h"

static int DS_GPIO;
static int init=0;
/// Sends one bit to bus
static void send(char bit){
  gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(DS_GPIO,0);
  ets_delay_us(5);
  if(bit==1)gpio_set_level(DS_GPIO,1);
  ets_delay_us(80);
  gpio_set_level(DS_GPIO,1);
}
// Reads one bit from bus
unsigned char read(void){
  unsigned char PRESENCE=0;
  gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(DS_GPIO,0);
  ets_delay_us(2);
  gpio_set_level(DS_GPIO,1);
  ets_delay_us(15);
  gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
  if(gpio_get_level(DS_GPIO)==1) PRESENCE=1; else PRESENCE=0;
  return(PRESENCE);
}
// Sends one byte to bus
static void send_byte(char data){
  unsigned char i;
  unsigned char x;
  for(i=0;i<8;i++){
    x = data>>i;
    x &= 0x01;
    send(x);
  }
  ets_delay_us(100);
}
// Reads one byte from bus
static unsigned char read_byte(void){
  unsigned char i;
  unsigned char data = 0;
  for (i=0;i<8;i++)
  {
    if(read()) data|=0x01<<i;
    ets_delay_us(15);
  }
  return(data);
}
// Sends reset pulse
static unsigned char RST_PULSE(void){
  unsigned char PRESENCE;
  gpio_set_direction(DS_GPIO, GPIO_MODE_OUTPUT);
  gpio_set_level(DS_GPIO,0);
  ets_delay_us(500);
  gpio_set_level(DS_GPIO,1);
  gpio_set_direction(DS_GPIO, GPIO_MODE_INPUT);
  ets_delay_us(30);
  if(gpio_get_level(DS_GPIO)==0) PRESENCE=1; else PRESENCE=0;
  ets_delay_us(470);
  if(gpio_get_level(DS_GPIO)==1) PRESENCE=1; else PRESENCE=0;
  return PRESENCE;
}
// Returns temperature from sensor
float DS_get_temp(void) {
  if(init==1){
    unsigned char check;
    char temp1=0, temp2=0;
      check=RST_PULSE();
      if(check==1)
      {
        send_byte(0xCC);
        send_byte(0x44);
        vTaskDelay(750 / portTICK_RATE_MS);
        check=RST_PULSE();
        send_byte(0xCC);
        send_byte(0xBE);
        temp1=read_byte();
        temp2=read_byte();
        check=RST_PULSE();
        float temp=0;
        temp=(float)(temp1+(temp2*256))/16;
        return temp;
      }
      else{return 0;}

  }
  else{return 0;}
}
void DS_init(int GPIO){
  DS_GPIO = GPIO;
  gpio_pad_select_gpio(DS_GPIO);
  init=1;
}
