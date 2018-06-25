


unsigned long num_cycle; // счетчик задержки
unsigned short Registr_;
unsigned alarm_speed;
char num_position;
unsigned short OLD_;
char txt[6];


void draw_alarm_speed(unsigned alarm_speed, unsigned int color); // отрисовка лимита скорости
unsigned long counter (unsigned long num);  // счетчик
void bt_on_(); // включить блютуз
void bt_off_(); // выключить блютуз