//#include "tft_lib.h"

extern sfr sbit TFT_CS, TFT_RST, TFT_RS, TFT_WR, TFT_BLED, TFT_VDD;
extern sfr unsigned int TFT_DataPort;
extern sfr unsigned short TFT_Disp_Rotation = 0;
extern unsigned short Screen_rotation;


/*void pin_output(){
GPIO_Digital_Output(&GPIOB_BASE, _GPIO_PINMASK_9);
}*/


void TFT_Set_Pin_Directions(){
// TFT module connections
GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_LOW, _GPIO_CFG_DIGITAL_OUTPUT);//  Установка линии порта C на вывод
GPIO_Config(&GPIOD_BASE, _GPIO_PINMASK_2, _GPIO_CFG_DIGITAL_OUTPUT);//  Установка линии порта B на вывод
GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_9, _GPIO_CFG_DIGITAL_OUTPUT);//  подсветка
GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_12, _GPIO_CFG_DIGITAL_OUTPUT);// DC
GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_8, _GPIO_CFG_DIGITAL_OUTPUT);//  WR
GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_4, _GPIO_CFG_DIGITAL_OUTPUT);//  Установка линии порта B на вывод

TFT_VDD  = 1;
delay_ms(50);

TFT_WR = 1;
}

void TFT_Set_Index_(unsigned short index){      //   short
  TFT_DataPort = index | 0x0000;
  TFT_WR = 0;
  TFT_WR = 1;
}

void TFT_Write_Command_(unsigned short cmd){     //short
  TFT_DataPort = cmd | 0xFE00;
  TFT_WR = 0;
  TFT_WR = 1;
}

void TFT_Set_Reg_(unsigned short index, unsigned short value){    //short
  TFT_Set_Index_Ptr(index);
  TFT_Write_Command_Ptr(value);
}

void TFT_Write_Data_(unsigned int _data){
  TFT_DataPort = (_data>>8)| 0xFF00;
  TFT_WR = 0;
  TFT_WR = 1;
  TFT_DataPort = (_data)| 0xFF00;
  TFT_WR = 0;
  TFT_WR = 1;

}

void TFT_Set_Address_LS022(unsigned x, unsigned y){
  TFT_Set_Index_Ptr(0x2A);
  TFT_Write_Command_Ptr(x>>8);    // TFT_Write_Command_Ptr(x>>8);
  TFT_Write_Command_Ptr(x);
  TFT_Set_Index_Ptr(0x2B);
  TFT_Write_Command_Ptr(y>>8);     //  TFT_Write_Command_Ptr(y>>8);
  TFT_Write_Command_Ptr(y);
  TFT_Set_Index_Ptr(0x2C);
}

void TFT_Reset_LS022(){
  TFT_WR = 1;
  TFT_RST = 0;
  delay_ms(100);
  TFT_RST = 1;

  delay_ms(20);
  TFT_Set_Index_Ptr(0x01);
  delay_ms(20);
  TFT_Set_Index_Ptr(0x11); //'       // Выход дисплея из спячки
  TFT_Set_Index_Ptr(0x29); //'       // Включение дисплея
  delay_ms(10);
  TFT_Set_Index_Ptr(0x13); //'       // Normal_Mode
  TFT_Set_Index_Ptr(0x20); //'       // Инверсия выключена

  //SendCom(0x26)'       // настройка Гаммы
  //SendDat (0x01)
  //SendCom(0x29)'       // Включение дисплея
  delay_ms(10);
  TFT_Set_Index_Ptr(0x36);
 // Screen_rotation = 3;
  TFT_Write_Command_Ptr(0xA0);
  
    /*switch (Screen_rotation){
      case 1: TFT_Write_Command_Ptr(%01100000); //'(%01100000)'(0x00)'(0x60)'(0xC0)'//альбом . Книжная ориентация дисплея      (%10100000) - поворот на 180 градусов
      case 2: TFT_Write_Command_Ptr(0xC0); //'(%01100000)'(0x00)'(0x60)'(0xC0)'//альбом . Книжная ориентация дисплея      (%10100000) - поворот на 180 градусов
      case 3: TFT_Write_Command_Ptr(%10100000); //'(%01100000)'(0x00)'(0x60)'(0xC0)'//альбом . Книжная ориентация дисплея      (%10100000) - поворот на 180 градусов
      case 4: TFT_Write_Command_Ptr(%00000000); //
    }*/

  TFT_Set_Index_Ptr(0x3A);
  TFT_Write_Command_Ptr(0x05);  //  16 битныйрежим цвета

  
  TFT_Set_Index_Ptr(0x2A);
  TFT_Write_Command_Ptr(0);
  TFT_Write_Command_Ptr(0);
  TFT_Write_Command_Ptr((319) >> 8);
  TFT_Write_Command_Ptr(319);
  TFT_Set_Index_Ptr(0x2B);
  TFT_Write_Command_Ptr(0);
  TFT_Write_Command_Ptr(0);
  TFT_Write_Command_Ptr((239) >> 8);
  TFT_Write_Command_Ptr(239);

}

void TFT_Init_LS022_8bit(unsigned int display_width, unsigned int display_height){
 
 TFT_Set_Pin_Directions();

  if (Is_TFT_Set() != 1) {
    TFT_Set_Index_Ptr = TFT_Set_Index_;
    TFT_Write_Command_Ptr = TFT_Write_Command_;
    TFT_Write_Data_Ptr = TFT_Write_Data_;
    TFT_Set_Address_Ptr = TFT_Set_Address_LS022;
  }

  TFT_DISP_WIDTH = display_width;
  TFT_DISP_HEIGHT = display_height;
  
if (display_width >= display_height)
      TFT_Disp_Rotation = 0;
  else
      TFT_Disp_Rotation = 90;


  TFT_Set_Pen(CL_BLACK, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);

  TFT_Move_Cursor(0, 0);
  //TFT_Clear_Fonts();
   ExternalFontSet = 0;

  TFT_Reset_LS022();
  TFT_Set_Address_Ptr = TFT_Set_Address_LS022;
}

void TFT_Activate(){     //'включить TFT
 TFT_BLED = 0;                    //' подсветка дисплея
 TFT_Init_LS022_8bit(320, 240);  // ' инициализация дисплея
 TFT_Set_Index_Ptr(0x28);       // Выключение дисплея
 TFT_Set_Index_Ptr(0x29);       // Включение дисплея
 //TFT_Set_Index_Ptr(0x10); // sleep IN
 TFT_Set_Index_Ptr(0x11); // sleep OUT
 //TFT_Fill_Screen(CL_black);
}