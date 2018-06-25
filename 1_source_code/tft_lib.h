//void pin_output();

unsigned int TFT_DataPort at GPIOC_ODR;
sbit TFT_RST at GPIOD_ODR.B2;
sbit TFT_BLED at GPIOB_ODR.B9;

sbit TFT_CS at GPIOC_ODR.B13;   // чтобы не ломать основную библиотеку примитивов (вывод не подключен, подтянут к 0/I)
sbit TFT_RS at GPIOC_ODR.B12;
sbit TFT_WR at GPIOC_ODR.B8;
sbit TFT_VDD at GPIOA_ODR.B4;

unsigned int TFT_Disp_Rotation;

void TFT_Activate();
void TFT_Set_Pin_Directions();

void TFT_Set_Index_(unsigned short index);
void TFT_Write_Command_(unsigned short cmd);
void TFT_Set_Reg_(unsigned short index, unsigned short value);

void TFT_Set_Address_(unsigned int x, unsigned int y);
void TFT_Write_Data_(unsigned int _data);
void TFT_Set_Address_LS022(unsigned int x, unsigned int y);
void TFT_Reset_LS022();
void TFT_Init_LS022_8bit(unsigned int display_width, unsigned int display_height);