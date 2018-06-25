
sbit Mmc_Chip_Select at GPIOA_ODR.B3;   // CS_SD
char filename[10]            = "R2_D2.TXT";          // имя файла
unsigned short loop, loop2;
const LINE_LEN = 10;
char file_contents[LINE_LEN] = "XX<TEST> ";      // XX подставить номер записи



// создать новый файл 
void M_Create_New_File() {
  //filename[7] = 'A';
  //Mmc_Fat_Set_File_Date(2015,1,12,11,9,0);   // установить дату создания и время
    Mmc_Fat_Assign(&filename, 0xA0);            // найти существующий или создать новый с именем
    Mmc_Fat_Rewrite();                          // очистить и начать с новыми данными
    for(loop = 1; loop <= 10; loop++) {
      //UART0_Write('.');                      // вставить отрисовку линии прогресса записи данных на карту
      file_contents[0] = loop / 10 + 48;
      file_contents[1] = loop % 10 + 48;
      Mmc_Fat_Write(file_contents, LINE_LEN-1); // записать данные в выбранный файл
    }
 }
 
 void Init_Ext_Mem() {
  GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_3, _GPIO_CFG_DIGITAL_OUTPUT);//  CS_SD
    //--- set up SPI for MMC init (low speed)
  SPI1_Init_Advanced(_SPI_FPCLK_DIV64, _SPI_MASTER | _SPI_8_BIT |
                                                 _SPI_CLK_IDLE_LOW | _SPI_FIRST_CLK_EDGE_TRANSITION |
                                                 _SPI_MSB_FIRST | _SPI_SS_DISABLE | _SPI_SSM_ENABLE | _SPI_SSI_1,
                                                 &_GPIO_MODULE_SPI1_PA567);

  //Mmc_Fat_QuickFormat("r2_d2");        // форматирование карты SD

  Delay_ms(10);
  
  //--инит FAT лабы
  if (!Mmc_Fat_Init()) {
    // рестартнуть spi со скоростью побольше
    SPI1_Init_Advanced(_SPI_FPCLK_DIV4, _SPI_MASTER | _SPI_8_BIT |
                                                 _SPI_CLK_IDLE_LOW | _SPI_FIRST_CLK_EDGE_TRANSITION |
                                                 _SPI_MSB_FIRST | _SPI_SS_DISABLE | _SPI_SSM_ENABLE | _SPI_SSI_1,
                                                 &_GPIO_MODULE_SPI1_PA567);
  }
  else {
   TFT_Set_Font(TFT_defaultFont, CL_YELLOW, FO_HORIZONTAL);
   TFT_Write_Text("SD_ERROR...", 110, 130);
   Delay_ms(5000);
  }

}

 void Format_Init_Ext_Mem() {
  GPIO_Config(&GPIOA_BASE, _GPIO_PINMASK_3, _GPIO_CFG_DIGITAL_OUTPUT);//  CS_SD
    //--- установить SPI для MMC init (low speed)
  SPI1_Init_Advanced(_SPI_FPCLK_DIV64, _SPI_MASTER | _SPI_8_BIT |
                                                 _SPI_CLK_IDLE_LOW | _SPI_FIRST_CLK_EDGE_TRANSITION |
                                                 _SPI_MSB_FIRST | _SPI_SS_DISABLE | _SPI_SSM_ENABLE | _SPI_SSI_1,
                                                 &_GPIO_MODULE_SPI1_PA567);

  Mmc_Fat_QuickFormat("r2d2");        // форматирование карты SD

  Delay_ms(10);

  //--- инит FAT лабы
  if (!Mmc_Fat_Init()) {
    // // рестартнуть spi со скоростью побольше
    SPI1_Init_Advanced(_SPI_FPCLK_DIV8, _SPI_MASTER | _SPI_8_BIT |
                                                 _SPI_CLK_IDLE_LOW | _SPI_FIRST_CLK_EDGE_TRANSITION |
                                                 _SPI_MSB_FIRST | _SPI_SS_DISABLE | _SPI_SSM_ENABLE | _SPI_SSI_1,
                                                 &_GPIO_MODULE_SPI1_PA567);
  }
  else {
   TFT_Set_Font(TFT_defaultFont, CL_YELLOW, FO_HORIZONTAL);
   TFT_Write_Text("SD_ERROR...", 110, 130);
   Delay_ms(5000);
  }

}