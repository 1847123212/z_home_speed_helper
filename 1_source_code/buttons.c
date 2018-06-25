

sbit button1 at GPIOA_IDR.B8;
sbit button2 at GPIOA_IDR.B9;
sbit button3 at GPIOA_IDR.B10;
sbit button4 at GPIOA_IDR.B11;
sbit button5 at GPIOA_IDR.B12;

sbit buttonA at GPIOB_ODR.B15;
sbit buttonB at GPIOB_ODR.B14;
sbit buttonC at GPIOB_ODR.B13;

void buttons_init(){
 GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_8, _GPIO_CFG_DIGITAL_INPUT);
 GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_9, _GPIO_CFG_DIGITAL_INPUT);
 GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_10, _GPIO_CFG_DIGITAL_INPUT);
 GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_11, _GPIO_CFG_DIGITAL_INPUT);
 GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_12, _GPIO_CFG_DIGITAL_INPUT);
 
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_15, _GPIO_CFG_DIGITAL_OUTPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_14, _GPIO_CFG_DIGITAL_OUTPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_13, _GPIO_CFG_DIGITAL_OUTPUT);
}

unsigned short button_num(){
   // 0 - 9 - согласно кнопкам
   // стрелка - 10
   // Плюс - 11 (обе кнопки)
   // М+ - 12
   // М- -13
   // по умолчанию - 88
 unsigned short but=88;
 buttonA = 0;
 buttonB = 0;
 buttonC = 0;
 buttonA = 1;
 if (button1 = 1) but=12; // M+
 if (button2 = 1) but=11;  //+
 if (button3 = 1) but=7;
 if (button4 = 1) but=8;
 if (button5 = 1) but=9;
 buttonA = 0;
 buttonB = 1;
 if (button1 = 1) but=13;  //-
 if (button2 = 1) but=11;  //+
 if (button3 = 1) but=4;
 if (button4 = 1) but=5;
 if (button5 = 1) but=6;
 buttonB = 0;
 buttonC = 1;
 if (button1 = 1) but=10; // стрелка
 if (button2 = 1) but=0;
 if (button3 = 1) but=1;
 if (button4 = 1) but=2;
 if (button5 = 1) but=3;
 buttonC = 0;
  //  if (but>0) sound_press_button();
    return but;
}