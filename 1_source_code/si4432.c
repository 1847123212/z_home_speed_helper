
//' Порты для подключения SI4313
sbit Si_nSEL at GPIOB_ODR.B3;
sbit Si_SCLK at GPIOB_ODR.B4;
sbit Si_SDI at GPIOB_ODR.B5;
sbit Si_SDO at GPIOB_IDR.B6;
sbit Si_PaC at GPIOB_ODR.B7;
//sbit Si_GP0 at GPIOB_IDR.B7;



extern unsigned short Crystal_Oscill_cap;


void Si_Activate(){    //  'включить Si
  GPIO_Clk_Enable(&GPIOB_BASE);
  GPIO_Clk_Enable(&GPIOC_BASE);
  GPIO_Alternate_Function_Enable(&_GPIO_MODULE_SWJ_JTAGDISABLE);
/*GPIO_Config(&GPIOB_BASE,
            _GPIO_PINMASK_3 | _GPIO_PINMASK_4 | _GPIO_PINMASK_5 | _GPIO_PINMASK_6 | _GPIO_PINMASK_7 | _GPIO_PINMASK_8,
            _GPIO_CFG_MODE_OUTPUT | _GPIO_CFG_SPEED_MAX | _GPIO_CFG_OTYPE_PP);*/
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_3, _GPIO_CFG_DIGITAL_OUTPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_4, _GPIO_CFG_DIGITAL_OUTPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_5, _GPIO_CFG_DIGITAL_OUTPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_6, _GPIO_CFG_DIGITAL_INPUT);
 GPIO_Config(&GPIOB_BASE, _GPIO_PINMASK_7, _GPIO_CFG_DIGITAL_OUTPUT);

 delay_ms(100);
 //Si_POWER = 0;
 delay_ms(300);
 //Si_SDN = 0;
 delay_ms(10);
 Si_nSEL = 1;
 Si_SCLK = 0;
 delay_ms(10);
 

}

void NOP(){
    asm {
     nop
     nop
     nop
    }
}

void Write0(){
    Si_SCLK = 0;
    NOP();
    Si_SDI = 0;
    NOP();
    Si_SCLK = 1;
    NOP();
}

void Write1(){
    Si_SCLK = 0;
    NOP();
    Si_SDI = 1;
    NOP();
    Si_SCLK = 1;
    NOP();
}


short Si_Read_Data(unsigned short command){  //   'прочитать данные
  unsigned short n=8;
  unsigned short Result, i;
  //send 8-bit
    Si_nSEL = 1;
    Si_SCLK = 0;
    NOP();
    Si_nSEL = 0;
    command = command & 0x7f;
    while(n--){
         if(command&0x80)
          Write1();
         else
          Write0();
          command = command << 1;
    }
    Si_SCLK = 0;
  //read 8-bit
    Si_SCLK = 0;
    Result=0;
    for(i=0;i<8;i++){
       Result=Result<<1;
       Si_SCLK = 1;
       NOP();
       if(Si_SDO == 1) Result|=1;

       Si_SCLK = 0;
       NOP();
     }
    return(Result);
}

void Si_Write_Data(unsigned short command, unsigned short data_){   //    'отправить данные
  unsigned short n=8;
  //send 8-bit
    Si_nSEL = 1;
    Si_SCLK = 0;
    NOP();
    Si_nSEL = 0;
    NOP();
    NOP();
    command = command | 0x80;
    while(n--){
         if(command&0x80) // i &= 0x7f
          Write1();
         else
          Write0();
          command = command << 1;
    }
    Si_SCLK = 0;
    
    n=8;
    while(n--){
         if(data_&0x80)   //80
          Write1();
         else
          Write0();
          data_ = data_ << 1;
    }
    Si_SCLK = 0;
    
}

void Hopping_step(unsigned int h_step){ // ' установка шага перестройки по сетке частот (кГц)
    unsigned int H_step_temp;
    H_step_temp = H_step / 10;

    Si_Write_Data(0x7A, h_step_temp);    //             ' hopping step x10 kHz
    delay_ms(1);
}

void BW_setting(unsigned short bw){  //' установка фильтра полосы пропускания (BW)
  Si_Write_Data(0x1C, bw);
  delay_ms(1);
}


void si_4313_registrs_settings(){ // 'настройка основных регистров
    unsigned short Temp_capacitance;
    
    Si_Write_Data(0x71, 0x11);                //'ook modulation   11-ook
    delay_ms(1);
    Si_Write_Data(0x07, 0x03);            //    'TUNE mode    03
    delay_ms(1);                           //
  //' незадокументированно 200kbs
    Si_Write_Data(0x20, 0x0A);            //    'Rb
    delay_ms(1);
    Si_Write_Data(0x21, 0x06);            //    'отправить данные
    delay_ms(1);
    Si_Write_Data(0x22, 0x66);            //    'отправить данные
    delay_ms(1);
    Si_Write_Data(0x23, 0x66);            //    'отправить данные
    delay_ms(1);
    Si_Write_Data(0x24, 0x07);            //    'отправить данные
    delay_ms(1);
    Si_Write_Data(0x25, 0xFF);            //    'отправить данные
    delay_ms(1);
    
    Si_Write_Data(0x1C, 0x8e);           // BW
    

    /*Si_Write_Data(0x07, 0x03); //                'TUNE mode    03
    delay_ms(100);

    Si_Write_Data(0x71, 0x11); //                'ook modulation   1-ook 2-FSK  3-GFSK     11
    delay_ms(100); //*/

 // ' незадокументированно 200kbs
    /*Si_Write_Data(0x20, 0x0A); //               'Rb
    delay_ms(1);
    Si_Write_Data(0x21, 0x06);//                'отправить данные
    delay_ms(1);
    Si_Write_Data(0x22, 0x66);//                'отправить данные
    delay_ms(1);
    Si_Write_Data(0x23, 0x66);//                'отправить данные
    delay_ms(1);
    Si_Write_Data(0x24, 0x07);//                'отправить данные
    delay_ms(1);
    Si_Write_Data(0x25, 0xFF);//                'отправить данные
    delay_ms(1);*/

 //'   Si_Write_Data(0x0A, %00000111)                'временно......
 //'  delay_ms(1)
//'' обычный режим 128kbs
/*Si_Write_Data(0x6E, 0x33);                //
    Si_Write_Data(0x6F, 0x33);                //
    Si_Write_Data(0x70, 0x04);                //

    Si_Write_Data(0x1C, 0x06);                // 'BW
    Si_Write_Data(0x20, 0x14);              //  'Rb  200 KBPS
    Si_Write_Data(0x21, 0x06);              //  '
    Si_Write_Data(0x22, 0x66);              //  '
    Si_Write_Data(0x23, 0x66);              //  '
    Si_Write_Data(0x24, 0x06);               // '
    Si_Write_Data(0x25, 0x68);              //  '*/
    
     Temp_capacitance = 127;
    //Temp_capacitance = Crystal_Oscill_cap
    //if (Add_Crystal_Capac = 2)
       Temp_capacitance = Temp_capacitance | 0x80;
     //else
    //   Temp_capacitance = Temp_capacitance & 0x7f;

    Si_Write_Data(0x09, Temp_capacitance); //                ' вн. конденсаторы  макс 127
    delay_ms(1); //

    Si_Write_Data(0x73, 0); //                ' коррекция частоты   00      %10100000 - minus 50     plus  -  %01100000
    delay_ms(10);
    Si_Write_Data(0x74, 0); //                ' коррекция частоты   03      0- minus 50              plus -   %00000011
    delay_ms(10);
  //  Si_Write_Data(0x6D, 3); //                '  power
  //  delay_ms(10);
    
    delay_ms(10);
    

    Si_Write_Data(0x1D, 6); //                  ' выкл AFC      6 - off  70-on
    delay_ms(10);


}



void tx_mode(){

  Si_Write_Data(0x07, 0x09); //                'статус 0x03 - TUNE     0x09 - TX      0x05 - RX

}

void rx_mode(){

  Si_Write_Data(0x07, 0x05); //                'статус 0x03 - TUNE     0x09 - TX      0x05 - RX

}

float round(float x){
   float float_1;
   float float_2;
   float_2 = modf(x, &float_1);
   float_2 = float_2 * 2;

   if (float_2 >= 1)
   return float_1 + 1;
       else return float_1;
}


void Frequency_setting(float RX_Frequency_temp){ // ' установка частоты
     unsigned short reg_75;
     unsigned short reg_76;
     unsigned short reg_77;
     bit hbsel_bit;
     float Freq_temp_float;
     float Freq_temp_float_1;
     unsigned int Freq_temp_word;
     unsigned short fb_bit_;


    if (RX_Frequency_temp >= 480.0)
      hbsel_bit = 1;
        else
      hbsel_bit = 0;

    Freq_temp_float = (RX_Frequency_temp / (10*(1+ hbsel_bit)));
    //'Float_Write (Freq_temp_float, @TFT_defaultFont, CL_white, FO_HORIZONTAL, 200, 62) '(цифра, шрифт, цвет, вертик/гориз, x, y)
    Freq_temp_float = floor(Freq_temp_float); //  ' только целое число
    Freq_temp_float = Freq_temp_float - 24; //
    fb_bit_ = Freq_temp_float; //
    reg_75 = 64 + 32 * hbsel_bit + fb_bit_ ; //

    Freq_temp_float = (RX_Frequency_temp / (10*(1+ hbsel_bit)));
    Freq_temp_float = modf(Freq_temp_float, &Freq_temp_float_1); //

    Freq_temp_float = Freq_temp_float * 64000;
    Freq_temp_float = round(Freq_temp_float); // ' округлить
    Freq_temp_word = Freq_temp_float;

    reg_77 = Freq_temp_word & 0x00FF; // Lo byte
    reg_76 = Freq_temp_word >> 8;     // Hi byte


   // ' Freq settings
    Si_Write_Data(0x75, reg_75);
    NOP();
    Si_Write_Data(0x76, reg_76);
    NOP();
    Si_Write_Data(0x77, reg_77);

}

void n_SEL_Hi(){
 Si_nSEL = 1;
}

void n_SEL_Lo(){
 Si_nSEL = 0;
}

void ant_TX(){
 Si_PaC = 1;
}

void ant_RX(){
 Si_PaC = 0;
}

void Si_SDI_(unsigned short Dat){
 Si_SDI = Dat;
}

short read_si_data(unsigned short rssi_tmp_){
     unsigned short reg_tmp;

     reg_tmp = Si_Read_Data(0x26);
     reg_tmp = 130 - reg_tmp;
     if (reg_tmp <= rssi_tmp_) reg_tmp = 0;
     else reg_tmp = 1;
    return reg_tmp;

}