#include "resources.h"

sbit bt_on at GPIOB_ODR.B8; // ВТ реле 1
sbit bt_off at GPIOC_ODR.B11; // ВТ реле 2



void draw_alarm_speed(unsigned alarm_speed, unsigned int color){ // отрисовка лимита скорости
  char txt[6];
  TFT_Set_Font(Arial_Narrow17x43_Bold, color, FO_HORIZONTAL);
  WordTostr(alarm_speed, txt);
  TFT_Write_Text(txt, 233, 187);
}

unsigned long counter (unsigned long num) {  // счетчик
  num = num + 1;
  return num;
}

void bt_on_(){
 bt_off = 0;
 bt_on = 1;
 delay_ms(300);
 bt_on = 0;
}

void bt_off_(){
 bt_on = 0;
 bt_off = 1;
 delay_ms(300);
 bt_off = 0;
}