
// хедеры
#include "tft_lib.h"
#include "sd_card.h"
#include "resources.h"
#include "si4432.h"
#include "buttons.h"
#include "sound.h"
#include "screens_graph.h"
#include "rutine_functions.h"
#include "built_in.h"


  
  // переменные для GPS
bit gps_ready;  // если 1 то gps данные валидные, если 0- то gps данные не валидные
bit gps_ready_old;  // если 1 то gps данные валидные, если 0- то gps данные не валидные
char txt_data[768];
char *string;
int i;
unsigned s;
unsigned long num_cycle_mute; // счетчик задержки отключения звука
bit mute_sound; // 0 - звук отключен, 1- звук включен
long latitude_long, longitude_long; //, oldlatitude, oldlongitude;
float latitude_float, longitude_float, lat_long_sum_float ; //, oldlatitude_float, oldlongitude_float;
//float latitude_marker_float, longitude_marker_float;
//float distance_float;
bit sound_off_full; // 0 - звук отключен, 1- звук включен


//переменные для массива сохраненных данных координат (копию хранить на SD card)
float latitude_arr[100];
float longitude_arr[100];
char free_arr[100];  // таблица номеров действующих сохраненок 1= действует, 0=удалена
char data_1;
char data_contents;
unsigned long  size;
char _txt[20];
float data_f;
unsigned long data_long;
unsigned long flash_count;
unsigned long* ptr;

unsigned long first_on; // признак первого включения
int lat_long_arr_max;  // номер последней сохраненки
int lat_long_arr_count;  // счетчик и номер сохраненки
int lat_long_arr_count_old; // счетчик и номер сохраненки
int loc_for_mute;  // номер точки которая mute
bit loc_alarm; // 1=признак того, что в текущее время предупреждение о близовти к сохраненной точке
float temp1_float;
float temp2_float;
float distance_float;
float distance_to_min_float;

float alarm_distance_m;
unsigned distance_mt;
unsigned distance_mt_old;
char speed, speed_old;
signed int speed_diff;
unsigned short ready;
short time_corretion_hh;
//signed char time_corretion_hh;
signed char time_hh;
char time_hh_str[2];
//char satellites_num;
//char time_mm;
//char time_ss;
char distanse_alarm_str[20];
char time_str[9];
char time_old_str[9];
char date_str[9];
char lat_Str[15]; // 9
char long_Str[15]; // 10
char speed_str[3];
char sat_str[3];
char distance_str[8];
char lat_long_count_str[8];

//char txt_1[80];
//char txt_2[80];
//char password_arr[4];
int password_code;   // хранить в памяти
int password_code_tmp; // временный код для проверки валидности
char password_str[4];
char rf_sens; // чувствительность - чем выше тем хуже


//RF =====================
short frq_number;
short rssi_tmp;
unsigned rssi_level;
unsigned rf_code_1[200];
unsigned time_us;
unsigned time_betw_us;
float frq_;
float frq_tmp;
float frq_tmp1;
char freq_char[6];
short rf_num;  // номер кода для отправки
//short rf_close_last_save; // номер последнего сохраненного кода закрытия на SD  - хранить во флэше и SD   ?
unsigned data_word;
short rf_num_save_freq;  // номер сохраненной частоты для передачи или приема из таблицы выбора - хранить в SD
short rf_code_relevant;  // признак того, что код валидный - 1, был удален и не нуждается в отправке - 0
short i_tmp;
short brut_system_num;   // номер системы для врута пр.CAME, NICE и т.д.

void interrupt() iv IVT_INT_USART3 ics ICS_AUTO {    // вектор

  USART3_CR1bits.RXNEIE = 0;       // запретить uart rx interrupt
  
  i = 0;
  ready = 1;
  

}

  float MinToDegr (float data_ ){
   float float1, float2, float3;

   float1 = data_ / 100.0;
   float2 = modf(float1, &float3);  //' res = modf(6.25, iptr)  ' res = 0.25, iptr = 6.00
   float2 = float2 * 100.0;
   float2 = float2 / 60.0;
   while (float2 > 1){
     float2 = float2 / 10.0;
   }
   float1 = float3 + float2 ;
   return float1;
 }

 float DegrToRadian (float data_ ) {
   return data_ * 0.017453292;
 }



 float distance_m(float Lat1_, float Long1_, float Lat2_, float Long2_ ){
    float Lat1, Long1, Lat2, Long2, cl1, cl2, sl1, sl2, delta, cdelta, sdelta, y, x, ad;
    // перевести координаты из минут в градусы в формат ГГ.ГГГГ
    Lat1 = MinToDegr(Lat1_);
    Long1 = MinToDegr(Long1_);
    Lat2 = MinToDegr(Lat2_);
    Long2 = MinToDegr(Long2_);

    //перевести координаты в радианы
    Lat1 = DegrToRadian(Lat1);
    Long1 = DegrToRadian(Long1);
    Lat2 = DegrToRadian(Lat2);
    Long2 = DegrToRadian(Long2);

    //косинусы и синусы широт и разницы долгот
    cl1 = cos(Lat1);
    cl2 = cos(Lat2);
    sl1 = sin(Lat1);
    sl2 = sin(Lat2);
    delta = Long2 - Long1;
    cdelta = cos(delta);
    sdelta = sin(delta);

    //вычисления длины большого круга
    y = sqrt(pow(cl2 * sdelta, 2) + pow(cl1 * sl2 - sl1 * cl2 * cdelta, 2));
    x = sl1 * sl2 + cl1 * cl2 * cdelta;

    ad = atan2(x, y);
    return ad * 6372795.0;

 }
 // GPS данные =======================================================================================================
 char get_gps_data(){   // получение и рассчет GPS данных (долгота, широта, время, скорость и т.д....)
      //   USART3_CR1bits.RXNEIE = 0;       // запретить uart rx interrupt
         ready = 0;                      // обнулить переменную готовности
         // $GPRMC,023345.00,A,2232.17673,N,11401.19536,E,0.108,151.55,100410,,,A*6B
         string = strstr(txt_data,"$GPRMC");  // ,  отрезать все что было до $GPRMC

        if(string[17] == 'A') {              // если данные валидные (A) то определяем координаты и скорость,
             gps_ready = 1;
            // Определяем координаты и преобразоввываем String данных в Float формат
                latitude_long = (string[19]-48)*1000 + (string[20]-48)*100 + (string[21]-48)*10 + (string[22]-48);
                latitude_float = latitude_long;
                latitude_long = (string[24]-48)*1000 + (string[25]-48)*100 + (string[26]-48)*10 + (string[27]-48);
                latitude_long = latitude_long * 10 + (string[28]-48);
                latitude_float = latitude_float + latitude_long / 100000.0;

                longitude_long = (string[32]-48)*100 + (string[33]-48)*100 + (string[34]-48)*10 + (string[35]-48);
                longitude_long = longitude_long * 10 + (string[36]-48);
                longitude_float = longitude_long;
                longitude_long = (string[38]-48)*1000 + (string[39]-48)*100 + (string[40]-48)*10 + (string[41]-48);
                longitude_long = longitude_long * 10 + (string[42]-48);
                longitude_float = longitude_float + longitude_long / 100000.0;

                if(string[30] == 'S') {            // if the latitude is in the South direction it has minus sign
                   latitude_float = 0 - latitude_float;
                  }
                 if(string[44] == 'W') {            // if the longitude is in the West direction it has minus sign
                   longitude_float = 0 - longitude_float;
                  }
                 lat_long_sum_float = fabs(latitude_float - longitude_float);

                // определить время (UTC)
                //сперва сохронить старое значение
                time_old_str[0] = time_str[0];  //h
                time_old_str[1] = time_str[1];  //h
                time_old_str[2] = ':'       ;  //:
                time_old_str[3] = time_str[3];  //m
                time_old_str[4] = time_str[4];  //m
                time_old_str[5] = ':'       ;  //:
                time_old_str[6] = time_str[6];  //s
                time_old_str[7] = time_str[7];  //s

                // затем, достать новое
                time_str[0] =  string[7];  //h
                time_str[1] =  string[8];  //h
                time_str[2] = ':'       ;  //:
                time_str[3] = string[9];  //m
                time_str[4] = string[10];  //m
                time_str[5] = ':'       ;  //:
                time_str[6] = string[11];  //s
                time_str[7] = string[12];  //s

                // коррекция времени UTC - time_corretion_hh (часовой пояс +3)
                time_hh = (time_str[0]-48)*10 + (time_str[1]-48) + time_corretion_hh;
                if (time_hh > 23) time_hh = time_hh - 24;
                if (time_hh < 0) time_hh = time_hh + 24;
                ByteToStr(time_hh,time_hh_str);
                time_str[0] = time_hh_str[1];
                time_str[1] = time_hh_str[2];

                // определить дату (UTC)
                if (string[52]!=',') {   // если мы в движении то
                  date_str[0] =  string[59];  //h
                  date_str[1] =  string[60];  //h
                  date_str[2] = '.'       ;  //:
                  date_str[3] = string[61];  //m
                  date_str[4] = string[62];  //m
                  date_str[5] = '.'       ;  //:
                  date_str[6] = string[63];  //s
                  date_str[7] = string[64];  //s
                }
                else {                  // а если стоим на месте и кус отсутсвует то
                  date_str[0] =  string[53];  //d
                  date_str[1] =  string[54];  //d
                  date_str[2] = '.'       ;  //:
                  date_str[3] = string[55];  //m
                  date_str[4] = string[56];  //m
                  date_str[5] = '.'       ;  //:
                  date_str[6] = string[57];  //y
                  date_str[7] = string[58];  //y

                }

             // $GPVTG,148.81,T,,M,0.13,N,0.24,K,A*3D
             // Определяем скорость движения
             //  string = strstr(txt_data,"GPRMC");  // ,  отрезать все что было до $GPVTG/// просто так - правильно показывает дату
               string = strstr(txt_data,"$GPVTG");  // ,  отрезать все что было до $GPVTG
               if (string[7]==',') {               // если курс отсутствует - значит скорость - 0
                   speed = 0;
                }
               else {
                   string = strstr(string,"N");  // ,  отрезать все что было до N
                   speed = (string[2]-48);
                   if (string[3]!='.'){
                       speed = speed * 10 + (string[3]-48);
                       if (string[4]!='.'){
                          speed = speed * 10 + (string[4]-48);
                           if (string[5]!='.'){
                              speed = speed * 10 + (string[5]-48);
                            }
                           else {
                              if (string[6]>='5') speed++;
                            }
                        }
                       else {
                          if (string[5]>='5') speed++;
                        }
                    }
                        else {
                          if (string[4]>='5') speed++;
                        }
                }

              return 1; //вернуть 1 если данные валидные
         }
         else {
           gps_ready = 0;
           return 0; //вернуть 0 если данные не валидные
         }
      //   USART3_CR1bits.RXNEIE = 1;       // разрешить uart rx interrupt
 }
 
 // функция записи данных в SD карту из памяти процессор (при сохранении новых точек)
 void sd_data_write(){
 
  //загрузка данных в SD карту из памяти процессор (при сохранении новых точек)
  Mmc_Fat_Assign("R2_D2.TXT", 0xA0);            // 0xA0  найти существующий файл или создать новый с именем "filename"
  Mmc_Fat_Rewrite();                     // Очистить файл и перезаписать данные
  
  /*TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
    TFT_Write_Text("SD  card  format", 90, 130);*/

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("Please wait...", 100, 130);

  TFT_Set_Pen(CL_WHITE, 1);
  TFT_H_Line(60, 260, 160);
  TFT_Set_Pen(CL_RED, 1);
  TFT_H_Line(60, 62, 160);
  
  i = 0;
  while (i < 100){
      // прочитать значение latitude из массива
      data_f = latitude_arr[i];
      // перевести значение latitude в long
      data_long = data_f * 100000.0;
      // записать значение latitude на SD карту
      data_1 = Lo(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Hi(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Higher(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Highest(data_long);
      Mmc_Fat_Write(&data_1, 1);

      // прочитать значение longitude из массива
      data_f = longitude_arr[i];
      // перевести значение longitude в long
      data_long = data_f * 100000.0;
      // записать значение longitude на SD карту
      data_1 = Lo(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Hi(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Higher(data_long);
      Mmc_Fat_Write(&data_1, 1);
      data_1 = Highest(data_long);
      Mmc_Fat_Write(&data_1, 1);

      // прочитать значение переменной free_arr - значение действующая сохраненка или нет (1- действует, 0- удалена)
      data_1 = free_arr[i];  // таблица номеров действующих сохраненок 1= действует, 0=удалена
      Mmc_Fat_Write(&data_1, 1);
      
      i++;
      TFT_H_Line(60, i*2+60, 160);

   }

 }
 

 
 void sd_data_read(){
    //загрузка данных из SD карты в память
  Mmc_Fat_Assign("R2_D2.TXT", 0xA0);            // 0xA0  найти существующий файл или создать новый с именем "filename"
  Mmc_Fat_Reset(&size);                      // прочитать его размер в байтах size

  i = 0;
  while (i < 100){
      // прочитать значение latitude
      Mmc_Fat_Read(&data_1);
      Lo(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Hi(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Higher(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Highest(data_long) = data_1;
      // перевести значение latitude в float
      data_f =  data_long / 100000.0;
      // записать значение latitude float в массив
      latitude_arr[i] = data_f;

      // прочитать значение longitude
      Mmc_Fat_Read(&data_1);
      Lo(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Hi(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Higher(data_long) = data_1;
      Mmc_Fat_Read(&data_1);
      Highest(data_long) = data_1;
      // перевести значение longitude в float
      data_f =  data_long / 100000.0;
      // записать значение longitude float в массив
      longitude_arr[i] = data_f;

      // прочитать значение переменной free_arr - значение действующая сохраненка или нет (1- действует, 0- удалена)
      Mmc_Fat_Read(&data_1);
      free_arr[i] = data_1;  // таблица номеров действующих сохраненок 1= действует, 0=удалена
      
      i++;
   }
 
 }
 
   // функция записи данных во FLASH из памяти процессора (при сохранении новых точек)
 void flash_data_write(){

  //загрузка данных
  flash_count = 0x08030000;   //   адрес начала записи
  FLASH_ErasePage(flash_count);
  i = 0;
  while (i < 100){
      // прочитать значение latitude из массива
      data_f = latitude_arr[i];
      // перевести значение latitude в long
      data_long = data_f * 100000.0;
      // записать значение latitude во FLASH
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;

      // прочитать значение longitude из массива
      data_f = longitude_arr[i];
      // перевести значение longitude в long
      data_long = data_f * 100000.0;
      // записать значение longitude во FLASH
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;

      // прочитать значение переменной free_arr - значение действующая сохраненка или нет (1- действует, 0- удалена)
      data_1 = free_arr[i];  // таблица номеров действующих сохраненок 1= действует, 0=удалена
      data_long = data_1;
      // записать значение longitude во FLASH
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;

       /*time_corretion_hh = 3;  // short - 1 byte// достать из памяти
       alarm_speed = 60;  // unsigned - 2 byte// дефолтное значение ограничения скорости после перезагрузки (сохранять в памяти и сделать настройки через меню)
       password_code = 0; //int - 2 byte */
       //alarm_distance_m = 100.0; // float - 4 byte  data_long дефолтное значение дистанции для предупреждения о приближении к точке
      i++;
   }
   
   //flash_count = 0x08030068;   //   адрес начала записи
      data_long = time_corretion_hh;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      
      data_long = alarm_speed;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      
      data_long = password_code;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      
      data_long = alarm_distance_m;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      
      data_long = first_on;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      
      data_long = sound_off_full;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;
      

      
/*data_long = rf_open_last_save;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;

      data_long = rf_close_last_save;
      FLASH_Write_Word(flash_count, data_long);
      flash_count = flash_count + 4;*/
      // сохранить

 }

 void flash_data_read(){
    //загрузка данных из FLASH в память
    
  flash_count = 0x08030000;   // адрес начала записи/чтения
  i = 0;
  while (i < 100){
      // прочитать значение latitude из FLASH
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      data_f = data_long / 100000.0;
      latitude_arr[i] = data_f;
      flash_count = flash_count + 4;

      // прочитать значение longitude из FLASH
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      data_f = data_long / 100000.0;
      longitude_arr[i] = data_f;
      flash_count = flash_count + 4;

      // прочитать значение переменной free_arr - значение действующая сохраненка или нет (1- действует, 0- удалена)
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      data_1 = data_long;
      free_arr[i] = data_1;  // таблица номеров действующих сохраненок 1= действует, 0=удалена
      flash_count = flash_count + 4;

      i++;
      
   }
   
   //flash_count = 0x08030068;   //   адрес начала чтения
      
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      time_corretion_hh = data_long;
      flash_count = flash_count + 4;

      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      alarm_speed = data_long;
      flash_count = flash_count + 4;

      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      password_code = data_long;
      flash_count = flash_count + 4;

      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      alarm_distance_m = data_long;
      flash_count = flash_count + 4;
      
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      first_on = data_long;
      flash_count = flash_count + 4;
      
      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      sound_off_full = data_long;
      flash_count = flash_count + 4;
      
      /*ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      rf_open_last_save = data_long;
      flash_count = flash_count + 4;

      ptr = (unsigned long*)flash_count;
      data_long = *ptr;
      rf_close_last_save = data_long;
      flash_count = flash_count + 4;*/

 }






void main() {

 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_8, _GPIO_CFG_DIGITAL_OUTPUT);  // порты для передергивания BT
 GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_11, _GPIO_CFG_DIGITAL_OUTPUT);

 TFT_Activate();
 TFT_BLED = 0;
 Sound_Init(&GPIOC_ODR, 10); // инициализация звука
 Sound_OFF();

 buttons_init();
 TFT_Fill_Screen(CL_BLACK);
 
 TFT_BLED = 0;
 
  // включить RF модуль
  frq_ = 315.00;    //418
  Si_Activate();
  si_4313_registrs_settings();
  Frequency_setting(frq_);   //   ставить на 60 больше чем надо    315.90     315.00
  ant_RX(); // подключить антенну к приемнику
  rx_mode(); // режим приемника

 // инициализация SD
  Init_Ext_Mem();

 
 // массив
 // записать в массив 0 при первом включении
  i = 0;
 while(i<100){
   free_arr[i] = 0;
   i++;
 }
 // прочитать данные из FLASH
 flash_data_read();
 
 // проверка первого включения
  if (first_on != 888) {  // если первое включение то ключ не равен 888
    // запишем дефолтные данные в память
    // дефолтные значения если во флэш памяти
     time_corretion_hh = 3;  // short // достать из памяти
     alarm_speed = 60;  // unsigned // дефолтное значение ограничения скорости после перезагрузки (сохранять в памяти и сделать настройки через меню)
     alarm_distance_m = 100.0; // float  дефолтное значение дистанции для предупреждения о приближении к точке
     password_code = 0; //int
     first_on = 888;
     sound_off_full = 1;
    // rf_open_last_save = 0;
    // rf_close_last_save = 0;
     
     sound_duble(sound_off_full);

     flash_data_write();
     delay_ms(50);
     flash_data_read();
  }
 
 // проверка пароля
 password_check:
 if (password_code != 0){
    TFT_Fill_Screen(CL_BLACK);
    TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
    TFT_Write_Text("INSERT  PASSWORD", 55, 100);
    TFT_BLED = 1;
    password_code_tmp = 0;
       while(1) {
           Registr_ = button_num(); // опрос кнопок
          if (Registr_ != 88){
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(password_str, 140, 135);
            password_code_tmp = Registr_;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            WordToStr(password_code_tmp, password_str);
            TFT_Write_Text(password_str, 140, 135);
            delay_ms(500);
              while(1){
                Registr_ = button_num(); // опрос кнопок
                 if (Registr_ != 88){
                   sound_press_but(sound_off_full);
                   TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                   TFT_Write_Text(password_str, 140, 135);
                   password_code_tmp = password_code_tmp * 10 + Registr_;
                   TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                   WordToStr(password_code_tmp, password_str);
                   TFT_Write_Text(password_str, 140, 135);
                   delay_ms(500);
                    while(1){
                      Registr_ = button_num(); // опрос кнопок
                      if (Registr_ != 88){
                         sound_press_but(sound_off_full);
                         TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                         TFT_Write_Text(password_str, 140, 135);
                         password_code_tmp = password_code_tmp * 10 + Registr_;
                         TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                         WordToStr(password_code_tmp, password_str);
                         TFT_Write_Text(password_str, 140, 135);
                         delay_ms(500);
                          while(1){
                            Registr_ = button_num(); // опрос кнопок
                            if (Registr_ != 88){
                               sound_press_but(sound_off_full);
                               TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                               TFT_Write_Text(password_str, 140, 135);
                               password_code_tmp = password_code_tmp * 10 + Registr_;
                               TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                               WordToStr(password_code_tmp, password_str);
                               TFT_Write_Text(password_str, 140, 135);
                               delay_ms(500);
                               
                               sound_duble(sound_off_full);
                               TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
                               TFT_Write_Text(password_str, 140, 135);
                               if (password_code != password_code_tmp){
                                   TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                                   TFT_Write_Text("PASSWORD INCORRECT", 30, 180);
                                   delay_ms(1000);
                                   TFT_BLED = 0;
                                   goto password_check;
                                }
                                TFT_BLED = 0;
                                TFT_Fill_Screen(CL_BLACK);
                                break;
                             }
                          }
                        break;
                       }
                    }
                  break;
                 }
              }
             break;
           }

        }

  }
 
 password_ok:
 
 loc_alarm = 0;  // если 1 то в это время идут предупреждения о близости к сохраненной точке
 loc_for_mute = 220;  // специальный признак, который говорит о том, что сохраненная точка временно стоит на mute.

 // GPS модуль ==================================================
  gps_ready_old = 1;
 // time_corretion_hh = 3;     // достать из памяти
 // TFT_BLED = 1;
  ready = 0;
  i = 0;
  UART3_Init_Advanced(9600, _UART_8_BIT_DATA, _UART_NOPARITY, _UART_ONE_STOPBIT, &_GPIO_MODULE_USART3_PB10_11);    //  9600
  delay_ms(100);
  
 // выключить все сообщения кроме RMC и VTG
 UART3_Write_Text("$PUBX,40,GLL,0,0,0,0*5C\r\n");
 UART3_Write_Text("$PUBX,40,ZDA,0,0,0,0*44\r\n");
 //UART3_Write_Text("$PUBX,40,VTG,0,0,0,0*5E\r\n");  //
 UART3_Write_Text("$PUBX,40,GSV,0,0,0,0*59\r\n");
 UART3_Write_Text("$PUBX,40,GSA,0,0,0,0*4E\r\n");
 //UART3_Write_Text("$PUBX,40,RMC,0,0,0,0*47\r\n");  //
 UART3_Write_Text("$PUBX,40,GGA,0,0,0,0*5A\r\n");
 
// можно изменить скорость на 19200// 9600
  UART3_Write_Text("$PUBX,41,1,0007,0003,19200,0*25\r\n");  // "$PUBX,41,1,0007,0003,4800,0*13\r\n"
  delay_ms(100);
  UART3_Init_Advanced(19200, _UART_8_BIT_DATA, _UART_NOPARITY, _UART_ONE_STOPBIT, &_GPIO_MODULE_USART3_PB10_11);    //  9600
  delay_ms(100);
  
  
  // Разрешить прерывания от UART:
 // USART3_CR1bits.RXNEIE = 1;       // enable uart rx interrupt
  NVIC_IntEnable(IVT_INT_USART3);  // enable interrupt vector
  Enableinterrupts();

  speed_old = 0;

 // =================================================================================================
 screen_1:        //  главный экран
 screen_main(sound_off_full);   // отрисовать главный экран
 sound_perform_off(sound_off_full);

 delay_ms(200);
 
 screen_1_1:
 Registr_ = 88; // обнулить кнопки
 num_position = 4;
 num_cycle = 0;
 num_cycle_mute = 0; // счетчик задержки отключения звука
 mute_sound = 1; // звук включен для предупрездения о скорости
 //speed_old = 0;
 TFT_Set_Font(Arial_Narrow39x99_Bold, CL_WHITE, FO_HORIZONTAL);
 ByteToStr(speed_old, speed_str);
 TFT_Write_Text(speed_str, 10, 150);

 gps_ready = 0;
 gps_ready_old = 0;
 TFT_Image_Jpeg(60, 2, gps_red_jpg);
 
 Draw_alarm_speed(alarm_speed, CL_RED);   // отрисовать скорость красным цветом
 
 TFT_BLED = 1;
 
 USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS

  while(1){    // основной цикл главного экрана
    
     //  проверка GPS данных
    if (ready == 1) {                     // Если прерывание было и данные пришли, то:

        USART3_CR1bits.RXNEIE = 0;       // запретить uart прерывание GPS
        ready = 0;
            i = 0;
            s = 0;
            while(s < 10){
              txt_data[i] = UART3_Read();
              if(txt_data[i] =='$') break;
              s++;
            }
            i++;
            s = 0;
            while(s < 100){
              txt_data[i] = UART3_Read();
              if(txt_data[i] =='$') break;
              i++;
              s++;
            }
            i++;
            s = 0;
            while(s < 100){
              txt_data[i] = UART3_Read();
              if(txt_data[i] =='*') break;
              i++;
              s++;
            }
       get_gps_data();  // обработать GPS данные
       // опредилить каким цветом отрисовывать значек GPS
          if (gps_ready != gps_ready_old){
                if (gps_ready == 0) {
                    TFT_Image_Jpeg(60, 2, gps_red_jpg);
                    gps_ready_old = gps_ready;
                 }
                 else {
                    TFT_Image_Jpeg(60, 2, gps_jpg);
                    gps_ready_old = gps_ready;
                 }
           }

        // отрисовать данные на экране ========================================================
        // отрисовать скорость
        if (speed_old != speed){
          ByteToStr(speed_old, speed_str);
          TFT_Set_Font(Arial_Narrow39x99_Bold, CL_BLACK, FO_HORIZONTAL);
          TFT_Write_Text(speed_str, 10, 150);
          ByteToStr(speed, speed_str);
          TFT_Set_Font(Arial_Narrow39x99_Bold, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text(speed_str, 10, 150);
          speed_old = speed;
        }
        // отрисовать время
        TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
        TFT_Write_Text(time_old_str, 218, 58);
        TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
        TFT_Write_Text(time_str, 218, 58);
        
        
        
        // расстояние до ближайшей сохраненной точки
        i = 0;
        lat_long_arr_count = 0;
        distance_to_min_float = distance_m(latitude_float, longitude_float, latitude_arr[i], longitude_arr[i]);
        i++;
        while(i < 100){
          if (free_arr[i] == 1){
            distance_float = distance_m(latitude_float, longitude_float, latitude_arr[i], longitude_arr[i]);
            if (distance_float < distance_to_min_float) {
             distance_to_min_float = distance_float;
             lat_long_arr_count = i;
            }
          }
         i++;
        }

        // 2 шаг отрисовать расстояние до ближайшей точки
       if (lat_long_arr_count != 0) {  // если ячейка с сохраненной точкой не нулевая, тогда оповещать о приближении и отрисовывать
           if (gps_ready == 1){  // если данные валидные, тогда тогда отрабатывать алгоритм оповещения о близости к точке
                if (distance_to_min_float < alarm_distance_m) {
                   loc_alarm = 1;
                   if (loc_for_mute != lat_long_arr_count){   // если звук включен то воспроизводить
                     sound_alarm_1(sound_off_full);
                    }
                 }
                 else {
                  loc_alarm = 0;
                  loc_for_mute = 220;
                 }
            }
           if (distance_to_min_float < 9000.0) {
              TFT_Set_Font(Arial21x24_Regular, CL_black, FO_HORIZONTAL);
              WordToStr(distance_mt_old, distance_str);
              TFT_Write_Text(distance_str, 45, 58);
              WordToStr(lat_long_arr_count_old, lat_long_count_str);
              TFT_Write_Text(lat_long_count_str, 40, 109);

              // если точка с mute то отрисовывать расстояние и номер точки красным цветом, если нет то белым
              if (loc_for_mute != lat_long_arr_count){
                TFT_Set_Font(Arial21x24_Regular, CL_white, FO_HORIZONTAL);
              }
              else {
                TFT_Set_Font(Arial21x24_Regular, CL_red, FO_HORIZONTAL);
              }
              WordToStr(distance_to_min_float, distance_str);
              TFT_Write_Text(distance_str, 45, 58);
              WordToStr(lat_long_arr_count, lat_long_count_str);
              TFT_Write_Text(lat_long_count_str, 40, 109);
              distance_mt_old = distance_to_min_float;
              lat_long_arr_count_old = lat_long_arr_count;
            }
        }
        // сравнение скорости и установленного лимита
       if (mute_sound == 1) {  // если звук включен, то воспроизводить звук
           speed_diff = speed - alarm_speed;
           if (speed_diff > 20) {
             sound_alarm_2(sound_off_full);
            }
            else {
                if (speed_diff > 10) {
                  sound_alarm_1(sound_off_full);
                 }
                 else {
                  if (speed_diff > 0) sound_press_but(sound_off_full);
                 }
            }

          USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
        }
        
        
        if (mute_sound == 0) {  // таймер для звука
          num_cycle_mute++;  // проверить, если звук отключен увеличить счетчик
          if (num_cycle_mute > 10){   // если счетчтк достиг 10 то
             mute_sound = 1;          // включить звук
            TFT_Image_Jpeg(170, 2, mute_off_jpg); // изменить иконку
            num_cycle_mute = 0;
          }
          USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
        }
        
        
    }
    else {
        /*if (gps_ready == gps_ready_old){
          TFT_Image_Jpeg(60, 2, gps_red_jpg);
          gps_ready = 0;
          gps_ready_old = 1;
        }*/
    }
    
    
    
    
      // счетчик для времени установки лимита скорости
      if (num_position < 4){
       // USART3_CR1bits.RXNEIE = 0;       // запретить uart прерывание GPS
        num_cycle = counter(num_cycle);
        if (num_cycle > 250000) {    //250000
          num_cycle = 0;
          num_position = 4;
          sound_perform(sound_off_full);
          USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
         }

       }







     Registr_ = button_num(); // опрос кнопок
     // 0 - 9 - согласно кнопкам
     // стрелка - 10
     // Плюс - 11 (обе кнопки)
     // М+ - 12
     // М- -13
     // по умолчанию - 88
     if (Registr_ != 88){
     
        USART3_CR1bits.RXNEIE = 0;       // запретить uart прерывание GPS
        if(ready == 1) ready = 0;
        num_cycle = 0;
        sound_press_but(sound_off_full);
        if (Registr_ < 10) {                       // если нажата одна из кнопок цифр - установка лимита скорости
           if (num_position > 3){                  //  если записано 3 цифры, то
                 draw_alarm_speed(alarm_speed, CL_BLACK);   // стереть скорость лимита (отрисовать скорость черным цветом)
                 alarm_speed = 0;
                 num_position = 1;                 //  установить в первую позицию
            }
           if (num_position == 1) alarm_speed = Registr_;
           if (num_position > 1){
                 alarm_speed = alarm_speed*10 + Registr_;
            }
           num_position = num_position + 1;
           if (alarm_speed != 0) {
               Draw_alarm_speed(alarm_speed, CL_RED);   // отрисовать скорость красным цветом
               if (alarm_speed > 200) {                 // усли установлен лимит больше 200 км.ч
                   Draw_alarm_speed(alarm_speed, CL_BLACK);   // удалить старую скорость
                   alarm_speed = 200;
                   Draw_alarm_speed(alarm_speed, CL_RED);   // установить 200 (мах)
                }
               if (num_position == 4){
                 delay_ms(300);
                 sound_perform(sound_off_full);
                 USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
                }
            }
           else num_position = 4;
          // delay_ms(200);
         }

         if (Registr_ == 11){                   // если нажат + (mute)
           if (loc_alarm == 1) {
            loc_for_mute = lat_long_arr_count;
           }
           else {
            mute_sound = 0; // звук выключен для предупрездения о скорости
            
             if (sound_off_full ==1) TFT_Image_Jpeg(170, 2, mute_jpg); // если общий мьют не включен тогда отрисовать

           }
            USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
         }
         
         // если нажата М- (удалить текущую точку)
         if (Registr_ == 13) { ;
            if (loc_alarm == 1) {
               free_arr[lat_long_arr_count] = 0;
               TFT_BLED = 0;
               TFT_Fill_Screen(CL_BLACK);
               TFT_Image_Jpeg(125, 40, point_red_jpg);
               TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
               TFT_Write_Text("DELETE   LOCATION   #", 30, 125);
               WordToStr(lat_long_arr_count, lat_long_count_str);
               TFT_Write_Text(lat_long_count_str, 245, 125);
               TFT_BLED = 1;
               delay_ms(2000);
               
               //перепишем все данные во FLASH
               flash_data_write();
               
               
               TFT_BLED = 0;
               goto screen_1;
            }
         }
         
         if (Registr_ == 10) goto screen_1_2;                  // если нажата стрелка
         if (Registr_ == 12) goto screen_3;                  // если нажата M+ перейти к меню сохранения точки GPS


       Registr_ = 88;
       delay_ms(200);
       //USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS
      }
    //  USART3_CR1bits.RXNEIE = 1;       // разрешить uart прерывание GPS

   // }
  }
  



   // ============= Сохранение локаций  ========================================================================
  screen_3:
  TFT_BLED = 0;
  screen_Save_location();  // экран сохранения GPS точки
  sound_perform(sound_off_full);
  TFT_BLED = 1;
  num_cycle = 0;
  delay_ms(200);
  Registr_ = 88;
  while(1){
      num_cycle = counter(num_cycle);
      /*if (num_cycle > 300000) {
          num_cycle = 0;
          screen_main_help_off(); // удалить подсказки
          sound_perform();
          goto screen_1_1; // если время вышло возвращаемся обратно
       } */
      Registr_ = button_num(); // опрос кнопок
     // 0 - 9 - согласно кнопкам
     // стрелка - 10
     // Плюс - 11 (обе кнопки)
     // М+ - 12
     // М- -13
     // по умолчанию - 88
     if (Registr_ != 88){
        sound_press_but(sound_off_full);
        if (Registr_ == 0) {    // нажата кнопка 0 = вернуться в главное меню
          TFT_BLED = 0;
          goto screen_1; //
         }
        if (Registr_ == 11) {   // нажата кнопка 1 = команда добавить точку
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
           TFT_Write_Text("  +   Add GPS location", 22, 79);
          //  код добавления сохраненной точки в память (массив)
          // найдем своюодную ячейку для записи (0)
           i=1;
           while (free_arr[i] == 1){
            i++;
           }
           // пометим ее как занятую
           free_arr[i] = 1;
           // запишем в нее данные
           latitude_arr[i] = latitude_float;
           longitude_arr[i] = longitude_float;
           loc_for_mute = i;  // выключим звуковое предупреждение для этой точки

           //перепишем все данные во FLASH
           flash_data_write();
           
          delay_ms(500);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
           TFT_Write_Text("  +   Add GPS location", 22, 79);
          sound_duble(sound_off_full);
          TFT_BLED = 0;
          goto screen_1;
         }

        /*if (Registr_ == 1) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("1.   Add GPS location & direction", 22, 119);
          //  вставить код
          delay_ms(500);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("1.   Add GPS location & direction", 22, 119);
          sound_duble();
         }*/

        /*if (Registr_ == 2) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("2.   Add speed limit", 22, 159);
          //  вставить код
          delay_ms(500);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("2.   Add speed limit", 22, 159);
          sound_duble();
         }*/

        Registr_ = 88;
        delay_ms(200);
      }

   }


   // =================================================================================================
  screen_1_2:
  screen_main_help_on();                     // отрисовать подсказки
  num_cycle = 0;
  delay_ms(500);
  Registr_ = 88;
  while(1){
      num_cycle = counter(num_cycle);
      if (num_cycle > 300000) {
          num_cycle = 0;
          screen_main_help_off(sound_off_full); // удалить подсказки
          sound_perform(sound_off_full);
          goto screen_1_1; // если время вышло возвращаемся обратно
       }
      Registr_ = button_num(); // опрос кнопок
     // 0 - 9 - согласно кнопкам
     // стрелка - 10
     // Плюс - 11 (обе кнопки)
     // М+ - 12
     // М- -13
     // по умолчанию - 88
     if (Registr_ != 88){
        sound_press_but(sound_off_full);
        if (Registr_ == 12) {   // если нажата кнопка М+
          screen_main_help_off(sound_off_full); // удалить подсказки
          //sound_perform();
          goto screen_3; //   перейти к сохранению GPS точки
         }
        if (Registr_ == 13) {   // если нажата кнопка М-
          screen_main_help_off(sound_off_full); // удалить подсказки
          sound_perform(sound_off_full);
          goto screen_1_1; //
         }
        if (Registr_ == 8) {    // если нажата кнопка 8 (BT)
          screen_main_help_off(sound_off_full); // удалить подсказки
          TFT_Image_Jpeg(115, 2, bt_red_jpg);
          sound_perform(sound_off_full);
          bt_off_();
          delay_ms(500);
          bt_on_();
          TFT_Image_Jpeg(115, 2, bt_jpg);
          sound_perform_off(sound_off_full);
          goto screen_1_1; //
         }
         
         
         if (Registr_ == 11) {    // если нажата кнопка + (выключить звук совсем)
          sound_off_full = !sound_off_full;
          screen_main_help_off(sound_off_full); // удалить подсказки
          //if (sound_off_full ==0) TFT_Image_Jpeg(170, 2, mute_jpg);
          //if (sound_off_full ==1) TFT_Image_Jpeg(170, 2, mute_off_jpg);
          flash_data_write();
          sound_perform(sound_off_full);
          goto screen_1_1; //
         }
         
         if (Registr_ == 0) {    // если нажата кнопка 0 (перейти в меню RF модуля)
          goto screen_2; //
         }
         if (Registr_ == 9) {    // если нажата кнопка 9 (перейти в настройки)
          goto screen_settings; //
         }
        
        Registr_ = 88;
        delay_ms(200);
      }
 
   }
   
   
   
  // =================================================================================================
 screen_settings:
 TFT_BLED = 0;
 screen_Settings(); //отрисовать экран настроек
 sound_perform(sound_off_full);
 TFT_BLED = 1;
 delay_ms(200);
 //time_corretion_hh=3
  //alarm_speed = 60;
  //alarm_distance_m = 100.0;
  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
 
  FloatToStr(alarm_distance_m, distanse_alarm_str);
  TFT_Write_Text(distanse_alarm_str, 240, 55);

  ByteToStr(alarm_speed, speed_str);
  TFT_Write_Text(speed_str, 240, 75);

  ShortToStr(time_corretion_hh, time_hh_str);
  TFT_Write_Text(time_hh_str, 240, 95);
  
  WordToStr(password_code, password_str);
  TFT_Write_Text(password_str, 240, 135);
 
 while(1){
     Registr_ = button_num(); // опрос кнопок
     if (Registr_ == 0) {    // если нажата кнопка 0 выйти
        TFT_BLED = 0;
        goto screen_1;  //
      }

     if (Registr_ == 1) {  // регулировка alarm расстояния до сохраненной точки
      sound_press_but(sound_off_full);
      TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
      TFT_Write_Text("9.   SAVE", 212, 199);
      FloatToStr(alarm_distance_m, distanse_alarm_str);
      TFT_Write_Text(distanse_alarm_str, 240, 55);
      TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
      TFT_Set_Pen(CL_WHITE, 1);
      TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);
      TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
      TFT_Write_Text("0.   BACK", 22, 199);
      TFT_Set_Pen(CL_BLACK, 1);
      TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
      
      delay_ms(200);
     
       while (1){
         Registr_ = button_num(); // опрос кнопок
         if (Registr_ == 12) {    // если нажата кнопка М+ увеличить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(distanse_alarm_str, 240, 55);
            alarm_distance_m = alarm_distance_m + 50.0;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            if (alarm_distance_m > 2000.0) alarm_distance_m = 2000.0;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            FloatToStr(alarm_distance_m, distanse_alarm_str);
            TFT_Write_Text(distanse_alarm_str, 240, 55);
            delay_ms(200);
          }

         if (Registr_ == 13) {    // если нажата кнопка М- уменьшить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(distanse_alarm_str, 240, 55);
            alarm_distance_m = alarm_distance_m - 50.0;
            if (alarm_distance_m < 100.0) alarm_distance_m = 100.0;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            FloatToStr(alarm_distance_m, distanse_alarm_str);
            TFT_Write_Text(distanse_alarm_str, 240, 55);
            delay_ms(200);
          }

         if (Registr_ == 9) {    // если нажата кнопка 9 сохранить и выйти
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            FloatToStr(alarm_distance_m, distanse_alarm_str);
            TFT_Write_Text(distanse_alarm_str, 240, 55);
            TFT_Write_Text("9.   SAVE", 212, 199);
            TFT_Write_Text("0.   BACK", 22, 199);
            TFT_Set_Pen(CL_WHITE, 1);
            TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
            sound_duble(sound_off_full);
            delay_ms(200);
            
            flash_data_write();   // записать во флэш
            break;  //
          }
       }

     }
     
     if (Registr_ == 2) {  // регулировка alarm скорости по умолчанию при включении
      sound_press_but(sound_off_full);
      TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
      TFT_Set_Pen(CL_WHITE, 1);
      TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);
      TFT_Write_Text("9.   SAVE", 212, 199);
      ByteToStr(alarm_speed, speed_str);
      TFT_Write_Text(speed_str, 240, 75);
      TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
      TFT_Write_Text("0.   BACK", 22, 199);
      TFT_Set_Pen(CL_BLACK, 1);
      TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
      delay_ms(200);

       while (1){
         Registr_ = button_num(); // опрос кнопок
         if (Registr_ == 12) {    // если нажата кнопка М+ увеличить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(speed_str, 240, 75);
            alarm_speed ++;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            if (alarm_speed > 200) alarm_speed = 200;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            ByteToStr(alarm_speed, speed_str);
            TFT_Write_Text(speed_str, 240, 75);
            delay_ms(200);
          }

         if (Registr_ == 13) {    // если нажата кнопка М- уменьшить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(speed_str, 240, 75);
            alarm_speed --;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            if (alarm_speed < 10) alarm_speed = 10;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            ByteToStr(alarm_speed, speed_str);
            TFT_Write_Text(speed_str, 240, 75);
            delay_ms(200);
          }

         if (Registr_ == 9) {    // если нажата кнопка 9 сохранить и выйти
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            ByteToStr(alarm_speed, speed_str);
            TFT_Write_Text(speed_str, 240, 75);
            TFT_Write_Text("9.   SAVE", 212, 199);
            TFT_Write_Text("0.   BACK", 22, 199);
            TFT_Set_Pen(CL_WHITE, 1);
            TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
            sound_duble(sound_off_full);
            delay_ms(200);
            
            flash_data_write();   // записать во флэш
            break;  //
          }
       }

     }

     if (Registr_ == 3) {  // регулировка TIME zone
      sound_press_but(sound_off_full);
      TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
      TFT_Set_Pen(CL_WHITE, 1);
      TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);
      TFT_Write_Text("9.   SAVE", 212, 199);
      ShortToStr(time_corretion_hh, time_hh_str);
      TFT_Write_Text(time_hh_str, 240, 95);
      TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
      TFT_Write_Text("0.   BACK", 22, 199);
      TFT_Set_Pen(CL_BLACK, 1);
      TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
      delay_ms(200);

       while (1){
         Registr_ = button_num(); // опрос кнопок
         if (Registr_ == 12) {    // если нажата кнопка М+ увеличить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(time_hh_str, 240, 95);
            time_corretion_hh ++;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            if (time_corretion_hh > 12) alarm_speed = 12;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            ShortToStr(time_corretion_hh, time_hh_str);
            TFT_Write_Text(time_hh_str, 240, 95);
            delay_ms(200);
          }

         if (Registr_ == 13) {    // если нажата кнопка М- уменьшить
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(time_hh_str, 240, 95);
            time_corretion_hh --;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            if (time_corretion_hh < -12) alarm_speed = -12;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            ShortToStr(time_corretion_hh, time_hh_str);
            TFT_Write_Text(time_hh_str, 240, 95);
            delay_ms(200);
          }

         if (Registr_ == 9) {    // если нажата кнопка 9 сохранить и выйти
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            ShortToStr(time_corretion_hh, time_hh_str);
            TFT_Write_Text(time_hh_str, 240, 95);
            TFT_Write_Text("9.   SAVE", 212, 199);
            TFT_Write_Text("0.   BACK", 22, 199);
            TFT_Set_Pen(CL_WHITE, 1);
            TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
            sound_duble(sound_off_full);
            delay_ms(200);
            
            flash_data_write();   // записать во флэш
            break;  //
          }
       }

     }
     
    if (Registr_ == 4) {  // стереть все сохраненки
      sound_press_but(sound_off_full);
      TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
      TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
      delay_ms(300);
      sound_alarm_3(sound_off_full);
      delay_ms(1000);
      
       while(1){
        TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
        TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
        delay_ms(300);
        TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
        TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
        sound_press_but(sound_off_full);
        delay_ms(300);

        Registr_ = button_num(); // опрос кнопок
         if (Registr_ == 0) {    // если нажата кнопка 0 выйти
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
            sound_duble(sound_off_full);
            delay_ms(200);
            break;  //
          }
          if (Registr_ == 4) {    // если нажата кнопка 4 удалить и выйти
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
            
             i = 0;
             while(i<100){
               free_arr[i] = 0;
               i++;
             }
             
            // перезаписать FLASH
            // дефолтные значения если во флэш памяти - 0
             time_corretion_hh = 3;  // short // достать из памяти
             alarm_speed = 60;  // unsigned // дефолтное значение ограничения скорости после перезагрузки (сохранять в памяти и сделать настройки через меню)
             alarm_distance_m = 100.0; // float  дефолтное значение дистанции для предупреждения о приближении к точке
             password_code = 0; //int
             first_on = 888;
            
             sound_duble(sound_off_full);
            
             flash_data_write();
             delay_ms(50);
             flash_data_read();
             delay_ms(200);
             
             goto screen_Settings;
             
            break;  //
          }
       }

     }
     
    
    if  (Registr_ == 5) {  // установить пароль на вход
        sound_press_but(sound_off_full);
        TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
        WordToStr(password_code, password_str);
        TFT_Write_Text(password_str, 240, 135);
        TFT_Set_Pen(CL_BLACK, 1);
        TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);
        TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
        TFT_Write_Text("0.   BACK", 22, 199);
        TFT_Write_Text("9.   SAVE", 212, 199);
        TFT_Set_Pen(CL_BLACK, 1);
        TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
        delay_ms(300);
        Registr_ = 88;
        
        while(1) {
           Registr_ = button_num(); // опрос кнопок
          if (Registr_ != 88){
            sound_press_but(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text(password_str, 240, 135);
            password_code = Registr_;
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            WordToStr(password_code, password_str);
            TFT_Write_Text(password_str, 240, 135);
            delay_ms(500);
              while(1){
                Registr_ = button_num(); // опрос кнопок
                 if (Registr_ != 88){
                   sound_press_but(sound_off_full);
                   TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                   TFT_Write_Text(password_str, 240, 135);
                   password_code = password_code * 10 + Registr_;
                   TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                   WordToStr(password_code, password_str);
                   TFT_Write_Text(password_str, 240, 135);
                   delay_ms(500);
                    while(1){
                      Registr_ = button_num(); // опрос кнопок
                      if (Registr_ != 88){
                         sound_press_but(sound_off_full);
                         TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                         TFT_Write_Text(password_str, 240, 135);
                         password_code = password_code * 10 + Registr_;
                         TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                         WordToStr(password_code, password_str);
                         TFT_Write_Text(password_str, 240, 135);
                         delay_ms(500);
                          while(1){
                            Registr_ = button_num(); // опрос кнопок
                            if (Registr_ != 88){
                               sound_press_but(sound_off_full);
                               TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                               TFT_Write_Text(password_str, 240, 135);
                               password_code = password_code * 10 + Registr_;
                               TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
                               WordToStr(password_code, password_str);
                               TFT_Write_Text(password_str, 240, 135);
                               delay_ms(500);
                               break;
                             }
                          }
                        break;
                       }
                    }
                  break;
                 }
              }

            sound_duble(sound_off_full);
            TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
            TFT_Write_Text(password_str, 240, 135);
            //TFT_Write_Text("9.   SAVE", 212, 199);
            TFT_Write_Text("0.   BACK", 22, 199);
            TFT_Set_Pen(CL_WHITE, 1);
            TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
            //TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);
            
             flash_data_write();   // записать во флэш
             break;  //
             
           }

        }


 
     }
     
     
     if (Registr_ == 6) {  // BackUP запись или восстановление данных
      sound_press_but(sound_off_full);
      TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
      TFT_Write_Text("6.   Data  BackUP", 5, 155);
      delay_ms(300);
      //sound_alarm_3();
      TFT_BLED = 0;
      TFT_Fill_Screen(CL_BLACK);

      TFT_Image_Jpeg(20, 10, setting_jpg);


      TFT_Set_Pen(CL_WHITE, 1);
      TFT_Set_Brush(0, 0, 0, 0, 0, 0);

      TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
      TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
      TFT_Write_Text("SETTINGS", 63, 14);
      TFT_Write_Text("0.   BACK", 22, 199);

      TFT_Write_Text("1.   BackUP GPS data (SD card)", 5, 55);
      TFT_Write_Text("2.   GPS data restore (SD card)", 5, 75);
      TFT_Write_Text("3.   Format SD card", 5, 95);
      TFT_BLED = 1;
      
       while(1){
        Registr_ = button_num(); // опрос кнопок
         if (Registr_ == 0) {    // если нажата кнопка 0 выйти
            delay_ms(200);
            TFT_BLED = 0;
            goto screen_settings;  //
          }
          if (Registr_ == 1) {    // если нажата кнопка 1 сделать BackUP на SD карте
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            TFT_Write_Text("1.   BackUP GPS data (SD card)", 5, 55);
            
            // инициализация SD
            //Init_Ext_Mem();
            TFT_Set_Font(Arial21x24_Regular, CL_white, FO_HORIZONTAL);
           // TFT_Write_Text("SD  card  format", 90, 130);
            //Format_Init_Ext_Mem();
            delay_ms(200);
            
            sd_data_write();
            delay_ms(200);
            TFT_BLED = 0;
            break;  //
          }
          if (Registr_ == 2) {    // если нажата кнопка 2 восстановить BackUP с SD карты во FLASH
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            TFT_Write_Text("2.   GPS data restore (SD card)", 5, 75);
            // инициализация SD
            Init_Ext_Mem();
            delay_ms(200);
            
            sd_data_read();
            flash_data_write();
            delay_ms(200);
            TFT_BLED = 0;
            break;  //
          }
          if (Registr_ == 3) {    // если нажата кнопка 3 - форматировать карту
            TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
            TFT_Write_Text("3.   Format SD card", 5, 95);
            // инициализация SD
            Init_Ext_Mem();
            Format_Init_Ext_Mem();
            
            Mmc_Fat_Assign("open1.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open2.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open3.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open4.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open5.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open6.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open7.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open8.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open9.TXT", 0xA0);            // создать файл
            Mmc_Fat_Assign("open10.TXT", 0xA0);           // создать файл
            
            Mmc_Fat_Assign("close1.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close2.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close3.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close4.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close5.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close6.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close7.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close8.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close9.TXT", 0xA0);           // создать файл
            Mmc_Fat_Assign("close10.TXT", 0xA0);          // создать файл

            delay_ms(200);

            TFT_BLED = 0;
            break;  //
          }
       }
       
       goto screen_1;  //

     }
     

 }
 

   // ===============================================================================================================================================
   // ===============================================================================================================================================
   // ===============================================================================================================================================
  screen_2:
  TFT_BLED = 0;
  screen_RF_tool();  // экран работы с RF модулем
  sound_perform(sound_off_full);
  TFT_BLED = 1;
  num_cycle = 0;
  delay_ms(200);
  Registr_ = 88;
   // включить RF модуль
  //frq_ = 315.00;    //418
  Si_Activate();
  si_4313_registrs_settings();
  //Frequency_setting(frq_);   //
  ant_TX(); // подключить антенну к приемнику
  tx_mode(); // режим приемника
  n_SEL_Hi(); // для модуляции
  Si_SDI_(0);
  
  while(1){
      num_cycle = counter(num_cycle);

      Registr_ = button_num(); // опрос кнопок
     // 0 - 9 - согласно кнопкам
     // стрелка - 10
     // Плюс - 11 (обе кнопки)
     // М+ - 12
     // М- -13
     // по умолчанию - 88
     if (Registr_ != 88){
            sound_press_but(sound_off_full);
            if (Registr_ == 0) {    // нажата кнопка 0 = вернуться в главное меню
              TFT_BLED = 0;
              goto screen_1; //
             }
        
         if (Registr_ == 1) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          Registr_ = 88;
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("1. OPEN +", 195, 29);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("     SEND", 195, 29);
          delay_ms(300);
          
           sound_press_but(sound_off_full);
           rf_num = 0;
           Registr_ = 88;

           rf_num_save_freq = 0;
          //rf_num = 0;
          //  код упрравления RF - команда открыть
            
            for ( rf_num = 0; rf_num < 10; rf_num++){
            
                 //  прочитать нужный код из SD
                 switch (rf_num) {
                    case 0: Mmc_Fat_Assign("open1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("open2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("open3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("open4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("open5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("open6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("open7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("open8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("open9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("open10.TXT", 0xA0);  break;
                 }

                Mmc_Fat_Reset(&size);                      // прочитать его размер в байтах size

                i = 0;
                 while (i!=200){
                    Mmc_Fat_Read(&data_1);
                    Lo(data_word) = data_1;
                    Mmc_Fat_Read(&data_1);
                    Hi(data_word) = data_1;
                    rf_code_1[i] = data_word;
                    i++;
                 }

                // прочитать номер частоты для выбранного кода посылки
                Mmc_Fat_Read(&data_1);
                rf_num_save_freq = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                data_1 = 1;
                Mmc_Fat_Read(&data_1);
                rf_code_relevant = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                Mmc_Fat_Read(&data_1);
                i_tmp = data_1;

               if (rf_code_relevant == 9) {

                  // выбрать частоту для выбранного кода посылки
                   switch (rf_num_save_freq) {
                       case 0: frq_ = 315.00;  break;
                       case 1: frq_ = 418.00;  break;
                       case 2: frq_ = 433.92;  break;
                       case 3: frq_ = 868.00;  break;
                       case 4: frq_ = 915.00;  break;
                    }
                   Frequency_setting(frq_);   //  установить нужную частоту для выбранной посылки кода
                   delay_ms(10);
                   // проверить какая пауза должна быть между посылками
                   /*time_betw_us = 0;
                   i = 0;
                   while (i != 200){
                      if (rf_code_1[i] > time_betw_us) time_betw_us = rf_code_1[i];
                      i++;
                    }
                   time_betw_us = time_betw_us * 3;*/
                   Si_SDI_(0);

                   //  отправить код в эфир 10 раз
                    for (s=0; s<10; s++){
                      i = 0;
                      time_us = 0;
                      Si_SDI_(0);

                        while (i != 200){
                            Si_SDI_(1);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;

                            Si_SDI_(0);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;
                        }
                       /*while (time_betw_us != 0){
                          delay_us(10);
                          time_betw_us --;
                        }*/
                       delay_ms(10);
                    }
               }
               
            }
               
               

          delay_ms(100);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("     SEND", 195, 29);
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("1. OPEN +", 195, 29);
          sound_duble(sound_off_full);
         }
             
             
             
          if (Registr_ == 2) {   // нажата кнопка 2 = команда ЗАКРЫТЬ
          Registr_ = 88;
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("2.CLOSE +", 195, 69);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("     SEND", 195, 69);
          delay_ms(300);

           sound_press_but(sound_off_full);
           rf_num = 0;
           Registr_ = 88;

           rf_num_save_freq = 0;
          //rf_num = 0;
          //  код упрравления RF - команда открыть

            for ( rf_num = 0; rf_num < 10; rf_num++){

                 //  прочитать нужный код из SD
                 switch (rf_num) {
                    case 0: Mmc_Fat_Assign("close1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("close2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("close3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("close4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("close5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("close6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("close7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("close8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("close9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("close10.TXT", 0xA0);  break;
                 }

                Mmc_Fat_Reset(&size);                      // прочитать его размер в байтах size

                i = 0;
                 while (i!=200){
                    Mmc_Fat_Read(&data_1);
                    Lo(data_word) = data_1;
                    Mmc_Fat_Read(&data_1);
                    Hi(data_word) = data_1;
                    rf_code_1[i] = data_word;
                    i++;
                 }

                // прочитать номер частоты для выбранного кода посылки
                Mmc_Fat_Read(&data_1);
                rf_num_save_freq = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                data_1 = 1;
                Mmc_Fat_Read(&data_1);
                rf_code_relevant = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                Mmc_Fat_Read(&data_1);
                i_tmp = data_1;

               if (rf_code_relevant == 9) {

                  // выбрать частоту для выбранного кода посылки
                   switch (rf_num_save_freq) {
                       case 0: frq_ = 315.00;  break;
                       case 1: frq_ = 418.00;  break;
                       case 2: frq_ = 433.92;  break;
                       case 3: frq_ = 868.00;  break;
                       case 4: frq_ = 915.00;  break;
                    }
                   Frequency_setting(frq_);   //  установить нужную частоту для выбранной посылки кода
                   delay_ms(10);
                   // проверить какая пауза должна быть между посылками
                   /*time_betw_us = 0;
                   i = 0;
                   while (i != 200){
                      if (rf_code_1[i] > time_betw_us) time_betw_us = rf_code_1[i];
                      i++;
                    }
                   time_betw_us = time_betw_us * 3;*/
                   Si_SDI_(0);

                   //  отправить код в эфир 10 раз
                    for (s=0; s<10; s++){
                      i = 0;
                      time_us = 0;
                      Si_SDI_(0);

                        while (i != 200){
                            Si_SDI_(1);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;

                            Si_SDI_(0);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;
                        }
                       /*while (time_betw_us != 0){
                          delay_us(10);
                          time_betw_us --;
                        }*/
                       delay_ms(10);
                    }
               }

            }



          delay_ms(100);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("     SEND", 195, 69);
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("2.CLOSE +", 195, 69);
          sound_duble(sound_off_full);
         }


        if (Registr_ == 4) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          Registr_ = 88;
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("4.   #OPEN", 195, 149);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 195, 149);
          delay_ms(300);
          while (1) {
            Registr_ = button_num(); // опрос кнопок
            if (Registr_ < 10) break;
           }
           sound_press_but(sound_off_full);
           rf_num = Registr_;
           Registr_ = 88;

          rf_num_save_freq = 0;
          //rf_num = 0;
          //  код упрравления RF - команда открыть
                 //  прочитать нужный код из SD
                 switch (rf_num) {
                    case 0: Mmc_Fat_Assign("open1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("open2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("open3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("open4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("open5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("open6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("open7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("open8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("open9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("open10.TXT", 0xA0);  break;
                 }

                Mmc_Fat_Reset(&size);                      // прочитать его размер в байтах size

                i = 0;
                 while (i!=200){
                    Mmc_Fat_Read(&data_1);
                    Lo(data_word) = data_1;
                    Mmc_Fat_Read(&data_1);
                    Hi(data_word) = data_1;
                    rf_code_1[i] = data_word;
                    i++;
                 }

                // прочитать номер частоты для выбранного кода посылки
                Mmc_Fat_Read(&data_1);
                rf_num_save_freq = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                data_1 = 1;
                Mmc_Fat_Read(&data_1);
                rf_code_relevant = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                Mmc_Fat_Read(&data_1);
                i_tmp = data_1;

               if (rf_code_relevant == 9) {

                  // выбрать частоту для выбранного кода посылки
                   switch (rf_num_save_freq) {
                       case 0: frq_ = 315.00;  break;
                       case 1: frq_ = 418.00;  break;
                       case 2: frq_ = 433.92;  break;
                       case 3: frq_ = 868.00;  break;
                       case 4: frq_ = 915.00;  break;
                    }
                   Frequency_setting(frq_);   //  установить нужную частоту для выбранной посылки кода
                   delay_ms(10);
                   // проверить какая пауза должна быть между посылками
                   time_betw_us = 0;
                   i = 0;
                   while (i != 200){
                      if (rf_code_1[i] > time_betw_us) time_betw_us = rf_code_1[i];
                      i++;
                    }
                   time_betw_us = time_betw_us * 3;
                   Si_SDI_(0);

                   //  отправить код в эфир 10 раз
                    for (s=0; s<10; s++){
                      i = 0;
                      time_us = 0;
                      Si_SDI_(0);

                        while (i != 200){
                            Si_SDI_(1);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;

                            Si_SDI_(0);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;
                        }
                       /*while (time_betw_us != 0){
                          delay_us(10);
                          time_betw_us --;
                        }*/
                       delay_ms(10);
                    }
               }

          delay_ms(100);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 195, 149);
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("4.   #OPEN", 195, 149);
          sound_duble(sound_off_full);
         }
         

        if (Registr_ == 5) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          Registr_ = 88;
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("5. #CLOSE", 195, 189);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 195, 189);
          delay_ms(300);
           while (1) {
            Registr_ = button_num(); // опрос кнопок
            if (Registr_ < 10) break;
            }
           sound_press_but(sound_off_full);
           rf_num = Registr_;
           Registr_ = 88;

          rf_num_save_freq = 0;
          //rf_num = 0;
          //  код упрравления RF - команда открыть
                 //  прочитать нужный код из SD
                 switch (rf_num) {
                    case 0: Mmc_Fat_Assign("close1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("close2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("close3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("close4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("close5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("close6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("close7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("close8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("close9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("close10.TXT", 0xA0);  break;
                 }

                Mmc_Fat_Reset(&size);                      // прочитать его размер в байтах size

                i = 0;
                 while (i!=200){
                    Mmc_Fat_Read(&data_1);
                    Lo(data_word) = data_1;
                    Mmc_Fat_Read(&data_1);
                    Hi(data_word) = data_1;
                    rf_code_1[i] = data_word;
                    i++;
                 }

                // прочитать номер частоты для выбранного кода посылки
                Mmc_Fat_Read(&data_1);
                rf_num_save_freq = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                data_1 = 1;
                Mmc_Fat_Read(&data_1);
                rf_code_relevant = data_1;

                // прочитать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                Mmc_Fat_Read(&data_1);
                i_tmp = data_1;

               if (rf_code_relevant == 9) {

                  // выбрать частоту для выбранного кода посылки
                   switch (rf_num_save_freq) {
                       case 0: frq_ = 315.00;  break;
                       case 1: frq_ = 418.00;  break;
                       case 2: frq_ = 433.92;  break;
                       case 3: frq_ = 868.00;  break;
                       case 4: frq_ = 915.00;  break;
                    }
                   Frequency_setting(frq_);   //  установить нужную частоту для выбранной посылки кода
                   delay_ms(10);
                   // проверить какая пауза должна быть между посылками
                   time_betw_us = 0;
                   i = 0;
                   while (i != 200){
                      if (rf_code_1[i] > time_betw_us) time_betw_us = rf_code_1[i];
                      i++;
                    }
                   time_betw_us = time_betw_us * 3;
                   Si_SDI_(0);

                   //  отправить код в эфир 10 раз
                    for (s=0; s<10; s++){
                      i = 0;
                      time_us = 0;
                      Si_SDI_(0);

                        while (i != 200){
                            Si_SDI_(1);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;

                            Si_SDI_(0);
                            time_us = rf_code_1[i]*3;
                            while (time_us != 0){
                              delay_us(10);
                              time_us--;
                            }
                            i++;
                        }
                       /*while (time_betw_us != 0){
                          delay_us(10);
                          time_betw_us --;
                        }*/
                       delay_ms(10);
                    }
               }

          delay_ms(100);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 195, 189);
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("5. #CLOSE", 195, 189);
          sound_duble(sound_off_full);
         }


         if (Registr_ == 3) {   // нажата кнопка 3 = перейти в меню SCAN
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("3.     SCAN", 195, 109);
          delay_ms(100);
          goto screen_4;
         }
         
        /*if (Registr_ == 4) {   // нажата кнопка 3 = перейти в меню BREAK (brutforce)
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("4.   BREAK", 195, 149);
          delay_ms(100);
          goto screen_6;
         }*/
         
        /*if (Registr_ == 5) {   // нажата кнопка 3 = перейти в меню LIST
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("5.     LIST", 195, 189);
          delay_ms(100);
          goto screen_5;
         }*/

        Registr_ = 88;
        delay_ms(200);
      }

   }
 



 // =====================================================================================================================================
  screen_4:
  TFT_BLED = 0;
  screen_RF_scan();  // экран RF сканирования
  sound_perform(sound_off_full);
  TFT_BLED = 1;
  num_cycle = 0;
  delay_ms(200);
  Registr_ = 88;
  rf_sens = 20;
  
  // включить RF модуль
  frq_ = 315.00;    //418
  Si_Activate();
  si_4313_registrs_settings();
  Frequency_setting(frq_);   //   ставить на 60 больше чем надо    315.90     315.00
  ant_RX(); // подключить антенну к приемнику
  rx_mode(); // режим приемника
  rf_num_save_freq = 0;

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("MHz", 240, 30);
  TFT_Write_Text("315.00", 165, 30);
 // TFT_Write_Text("SENS:    ", 22, 179);
  ShortToStr(rf_sens, txt);
  TFT_Write_Text(txt, 80, 179);

  
  while(1){
      num_cycle = counter(num_cycle);
      i = 0;
      Registr_ = button_num(); // опрос кнопок
     // 0 - 9 - согласно кнопкам
     // стрелка - 10
     // Плюс - 11 (обе кнопки)
     // М+ - 12
     // М- -13
     // по умолчанию - 88
     if (Registr_ != 88){
        sound_press_but(sound_off_full);
        if (Registr_ == 0) {    // нажата кнопка 0 = вернуться в главное меню
          TFT_BLED = 0;
          goto screen_2; //
         }
         
         // изменить чувствительность при сканировании - трэшхолд при калибровке
         
         /*if (Registr_ ==  5) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("Sens:    ", 22, 179);*/


               Registr_ = button_num(); // опрос кнопок
               if (Registr_ == 12) {    // если нажата кнопка М+ увеличить
                  sound_press_but(sound_off_full);
                  TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                  ShortToStr(rf_sens, txt);
                  TFT_Write_Text(txt, 80, 179);
                  rf_sens ++;
                  TFT_Set_Font(Arial21x24_Regular, CL_White, FO_HORIZONTAL);
                  ShortToStr(rf_sens, txt);
                  TFT_Write_Text(txt, 80, 179);
                  delay_ms(200);
                }

               if (Registr_ == 13) {    // если нажата кнопка М- уменьшить
                  sound_press_but(sound_off_full);
                  TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
                  ShortToStr(rf_sens, txt);
                  TFT_Write_Text(txt, 80, 179);
                  rf_sens --;
                  TFT_Set_Font(Arial21x24_Regular, CL_White, FO_HORIZONTAL);
                  ShortToStr(rf_sens, txt);
                  TFT_Write_Text(txt, 80, 179);
                  delay_ms(200);
                }


         
        if (Registr_ ==  1) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("1.   START", 22, 79);
          // калибровка на шум в эфире
          i = 0;
          rssi_tmp = 0;
            while(i<100){
              Registr_ = Si_Read_Data(0x26);
              Registr_ = 130 - Registr_;
              if (rssi_tmp < Registr_) rssi_tmp = Registr_ + rf_sens;  // 10- защита от помех, чем больше тем чище сигнал.
              delay_ms(10);
              i++;
            }
          // обнулить временный массив
          /*i = 0;
            while (i!=200){
              rf_code_1[i] = 0;
              i++;
            }
          Registr_ = 0;
          i = 0;
          time_us = 0;*/
          


          
          scan_start:    // начало сканирования
          TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
          TFT_Write_Text("1.   START", 22, 79);
          TFT_Write_Text("1.REPEAT", 30, 79);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("RECEIVE", 22, 79);
          // обнулить временный массив
          i = 0;
            while (i!=200){
              rf_code_1[i] = 0;
              i++;
            }
          Registr_ = 0;
          i = 0;
          time_us = 0;
          
          // ждем пока сигнал появится в эфире
            while (Registr_ == 0){
              Registr_ = read_si_data(rssi_tmp);
              if (button_num() == 0) goto screen_4;      // опрос кнопок

            }
          // читаем эфир и записываем в массив
            while (time_us < 65000) {
              // ждем пока появится 0
              while (Registr_ == 1){
                delay_us(10);
                Registr_ = read_si_data(rssi_tmp);    // прочитать rssi
                time_us++;
                if (time_us > 65000) break;
              }
              if (time_us < 65000){
                rf_code_1[i] = time_us;
                i++;
                time_us = 0;
              }
              // ждем пока появится 1
              while (Registr_ == 0){
                delay_us(10);
                Registr_ = read_si_data(rssi_tmp);    // прочитать rssi
                time_us++;
                if (time_us > 65000) break;
              }

              if (time_us < 65000){
                rf_code_1[i] = time_us;
                i++;
                time_us = 0;
              }

              if (i==200) break;

            }
            
            TFT_Set_Font(Arial21x24_Regular, CL_BLACK, FO_HORIZONTAL);
            TFT_Write_Text("RECEIVE", 22, 79);
            if (time_us >65000) {
              // TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
              // TFT_Write_Text("1.REPEAT", 30, 79);
               goto scan_start;
               while(1){
                if (button_num() == 1) goto scan_start;
                if (button_num() == 0) goto screen_4;
               }
            }
            else {
              TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
              TFT_Write_Text("     DONE", 22, 79);
              i = 88;
            }

          delay_ms(500);
          sound_duble(sound_off_full);
         }
         
        if (Registr_ == 2) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("2.    FREQ", 202, 79);

          frq_number++;
          if (frq_number > 4 ) frq_number = 0;
              
              frq_tmp = frq_;
              TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
              TFT_Write_Text("315.00", 165, 30);
              TFT_Write_Text("418.00", 165, 30);
              TFT_Write_Text("433.92", 165, 30);
              TFT_Write_Text("868.00", 165, 30);
              TFT_Write_Text("915.00", 165, 30);

              TFT_Set_Font(Arial21x24_Regular, CL_White, FO_HORIZONTAL);
            switch (frq_number) {
               case 0: frq_ = 315.00; TFT_Write_Text("315.00", 165, 30); break;
               case 1: frq_ = 418.00; TFT_Write_Text("418.00", 165, 30); break;
               case 2: frq_ = 433.92; TFT_Write_Text("433.92", 165, 30); break;
               case 3: frq_ = 868.00; TFT_Write_Text("868.00", 165, 30); break;
               case 4: frq_ = 915.00; TFT_Write_Text("915.00", 165, 30); break;
               
            }
            rf_num_save_freq = frq_number;
            Frequency_setting(frq_);

          delay_ms(500);
          //
          TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
          TFT_Write_Text("2.    FREQ", 202, 79);
          sound_duble(sound_off_full);
         }
         
        if (Registr_ ==  3) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("3.    OPEN", 22, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("3.    OPEN", 22, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 22, 129);
          delay_ms(500);
            while (1) {
               Registr_ = button_num(); // опрос кнопок
               if ( Registr_ < 10) break;
            }
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 22, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("  WAIT...", 22, 129);
          
            rf_num = Registr_;
            sound_press_but(sound_off_full);
            Registr_ = 88;
            
            //  код сохранения в файле SD
            // выбрать нужный файл для записи
                  switch (rf_num) {
                    case 0: Mmc_Fat_Assign("open1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("open2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("open3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("open4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("open5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("open6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("open7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("open8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("open9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("open10.TXT", 0xA0);  break;
                  }
                 // записать в файл SD
                  Mmc_Fat_Rewrite();                     // Очистить файл и перезаписать данные
                  i = 0;
                  while (i!=200){   //200
                    data_word = rf_code_1[i];
                    data_1 = Lo(data_word);
                    Mmc_Fat_Write(&data_1, 1);
                    data_1 = Hi(data_word);
                    Mmc_Fat_Write(&data_1, 1);
                    i++;
                  }
                  // записать номер частоты для выбранного кода посылки
                  data_1 = rf_num_save_freq;
                  Mmc_Fat_Write(&data_1, 1);
                  // записать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                  data_1 = 9;
                  Mmc_Fat_Write(&data_1, 1);
                  
                  // записать dammy
                  i_tmp = 88;
                  Mmc_Fat_Write(&data_1, 1);
              
              delay_ms(500);
              //
              TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
              TFT_Write_Text("  WAIT...", 22, 129);
              TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
              TFT_Write_Text("3.    OPEN", 22, 129);
              sound_duble(sound_off_full);

         }


         if (Registr_ ==  4) {   // нажата кнопка 1 = команда ОТКРЫТЬ
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("4.  CLOSE", 202, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("4.  CLOSE", 202, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 202, 129);
          delay_ms(500);
            while (1) {
               Registr_ = button_num(); // опрос кнопок
               if ( Registr_ < 10) break;
            }
          TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
          TFT_Write_Text("CODE 0-9", 202, 129);
          TFT_Set_Font(Arial21x24_Regular, CL_RED, FO_HORIZONTAL);
          TFT_Write_Text("  WAIT...", 202, 129);

            rf_num = Registr_;
            sound_press_but(sound_off_full);
            Registr_ = 88;

            //  код сохранения в файле SD
            // выбрать нужный файл для записи
                  switch (rf_num) {
                    case 0: Mmc_Fat_Assign("close1.TXT", 0xA0);  break;
                    case 1: Mmc_Fat_Assign("close2.TXT", 0xA0);  break;
                    case 2: Mmc_Fat_Assign("close3.TXT", 0xA0);  break;
                    case 3: Mmc_Fat_Assign("close4.TXT", 0xA0);  break;
                    case 4: Mmc_Fat_Assign("close5.TXT", 0xA0);  break;
                    case 5: Mmc_Fat_Assign("close6.TXT", 0xA0);  break;
                    case 6: Mmc_Fat_Assign("close7.TXT", 0xA0);  break;
                    case 7: Mmc_Fat_Assign("close8.TXT", 0xA0);  break;
                    case 8: Mmc_Fat_Assign("close9.TXT", 0xA0);  break;
                    case 9: Mmc_Fat_Assign("close10.TXT", 0xA0);  break;
                  }
                 // записать в файл SD
                  Mmc_Fat_Rewrite();                     // Очистить файл и перезаписать данные
                  i = 0;
                  while (i!=200){   //200
                    data_word = rf_code_1[i];
                    data_1 = Lo(data_word);
                    Mmc_Fat_Write(&data_1, 1);
                    data_1 = Hi(data_word);
                    Mmc_Fat_Write(&data_1, 1);
                    i++;
                  }
                  // записать номер частоты для выбранного кода посылки
                  data_1 = rf_num_save_freq;
                  Mmc_Fat_Write(&data_1, 1);
                  // записать признак - нужно ли отправлять эту посылку - 1 или нет - 0
                  data_1 = 9;
                  Mmc_Fat_Write(&data_1, 1);

                  // записать dammy
                  i_tmp = 88;
                  Mmc_Fat_Write(&data_1, 1);

              delay_ms(500);
              //
              TFT_Set_Font(Arial21x24_Regular, CL_Black, FO_HORIZONTAL);
              TFT_Write_Text("  WAIT...", 202, 129);
              TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
              TFT_Write_Text("4.  CLOSE", 202, 129);
              sound_duble(sound_off_full);

         }


      

        Registr_ = 88;
        delay_ms(200);
      }

   }
   
   
   
 
 
}