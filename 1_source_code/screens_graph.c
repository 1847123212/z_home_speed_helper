#include "resources.h"



void screen_main(char sound) {
// главный экран

  TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(5, 2, save_jpg);
 // TFT_Image_Jpeg(60, 2, gps_jpg);
  TFT_Image_Jpeg(115, 2, bt_jpg);
  if (sound ==0) TFT_Image_Jpeg(170, 2, mute_jpg);
  if (sound ==1) TFT_Image_Jpeg(170, 2, mute_off_jpg);
//  TFT_Image_Jpeg(170, 2, mute_off_jpg);
  TFT_Image_Jpeg(225, 2, ant_1_jpg);
  TFT_Image_Jpeg(280, 2, setting_jpg);

  TFT_Image_Jpeg(125, 60, point_jpg);

  TFT_Set_Pen(CL_WHITE, 1);
  TFT_H_Line(0, 320, 39);
  TFT_H_Line(0, 320, 150);

  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);
  TFT_Rectangle_Round_Edges(10, 55, 115, 85, 15);
  TFT_Rectangle_Round_Edges(205, 55, 310, 85, 15);
  TFT_Rectangle_Round_Edges(10, 105, 115, 135, 15);
  TFT_Rectangle_Round_Edges(205, 105, 310, 135, 15);

  TFT_Rectangle_Round_Edges(205, 165, 310, 230, 15);

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("m.", 28, 58);
 // TFT_Write_Text("Limit 150", 218, 58);
  TFT_Write_Text("#", 32, 109);
  TFT_Write_Text("M-  DEL", 221, 109);
  TFT_Write_Text("km/h", 138, 167);
  TFT_Write_Text("alarm", 230, 167);

 // TFT_Set_Font(Arial_Narrow17x43_Bold, CL_RED, FO_HORIZONTAL);
 // TFT_Write_Text("135", 233, 187);

  }
  
    void screen_main_help_on() {
// отрисовка на главном экране пиктограмм продсказок после нажатия стрелки
    TFT_Set_Pen(CL_RED, 1);
    TFT_Set_Brush(1, CL_RED, 0, 0, 0, 0);
    TFT_Circle(36, 25, 10);
    TFT_Circle(90, 25, 10);
    TFT_Circle(145, 25, 10);
    TFT_Circle(200, 25, 10);
    TFT_Circle(255, 25, 10);
    TFT_Circle(305, 25, 10);
    TFT_Set_Font(TFT_defaultFont, CL_white, FO_HORIZONTAL);
    TFT_Write_Text("M", 32, 18);
    TFT_Write_Text("7", 87, 18);
    TFT_Write_Text("8", 143, 18);
    TFT_Write_Text("+", 196, 18);
    TFT_Write_Text("0", 252, 18);
    TFT_Write_Text("9", 302, 18);
  }
  
  void screen_main_help_off(char sound) {
// удаление на главном экране пиктограмм продсказок после нажатия стрелки
    TFT_Set_Pen(CL_BLACK, 1);
    TFT_Set_Brush(1, CL_BLACK, 0, 0, 0, 0);
    TFT_Circle(36, 25, 10);
    TFT_Circle(90, 25, 10);
    TFT_Circle(145, 25, 10);
    TFT_Circle(200, 25, 10);
    TFT_Circle(255, 25, 10);
    TFT_Circle(305, 25, 10);
    TFT_Set_Font(TFT_defaultFont, CL_BLACK, FO_HORIZONTAL);
    TFT_Write_Text("M", 32, 18);
    TFT_Write_Text("7", 87, 18);
    TFT_Write_Text("8", 143, 18);
    TFT_Write_Text("+", 196, 18);
    TFT_Write_Text("0", 252, 18);
    TFT_Write_Text("9", 302, 18);
    
    TFT_Image_Jpeg(5, 2, save_jpg);
    TFT_Image_Jpeg(60, 2, gps_jpg);
    TFT_Image_Jpeg(115, 2, bt_jpg);
    if (sound ==0) TFT_Image_Jpeg(170, 2, mute_jpg);
    if (sound ==1) TFT_Image_Jpeg(170, 2, mute_off_jpg);
    TFT_Image_Jpeg(225, 2, ant_1_jpg);
    TFT_Image_Jpeg(280, 2, setting_jpg);
  }
  
  void screen_RF_tool() {
// экран работы с RF модулем

  TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(20, 30, ant_1_big_jpg);

  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);
  TFT_Rectangle_Round_Edges(180, 25, 310, 55, 15);
  TFT_Rectangle_Round_Edges(180, 65, 310, 95, 15);
  TFT_Rectangle_Round_Edges(180, 105, 310, 135, 15);
  TFT_Rectangle_Round_Edges(180, 145, 310, 175, 15);
  TFT_Rectangle_Round_Edges(180, 185, 310, 215, 15);
  TFT_Rectangle_Round_Edges(25, 185, 150, 215, 15);


  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  /*TFT_Write_Text("1.     OPEN", 195, 29);
  TFT_Write_Text("2.   CLOSE", 195, 69);
  TFT_Write_Text("3.     SCAN", 195, 109);
  TFT_Write_Text("4.   BREAK", 195, 149);
  TFT_Write_Text("5.      N/A", 195, 189);
  TFT_Write_Text("0.     BACK", 40, 189);*/
  
  TFT_Write_Text("1. OPEN +", 195, 29);
  TFT_Write_Text("2.CLOSE +", 195, 69);
  TFT_Write_Text("3.     SCAN", 195, 109);
  TFT_Write_Text("4.   #OPEN", 195, 149);
  TFT_Write_Text("5. #CLOSE", 195, 189);
  TFT_Write_Text("0.     BACK", 40, 189);
  }
  
  void screen_RF_scan() {
// экран RF сканирования

  TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(20, 20, ant_1_jpg);


  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);
  TFT_Rectangle_Round_Edges(10, 75, 130, 105, 15);
  TFT_Rectangle_Round_Edges(10, 125, 130, 155, 15);
  TFT_Rectangle_Round_Edges(10, 175, 130, 205, 15);
  
  TFT_Rectangle_Round_Edges(190, 75, 310, 105, 15);
  TFT_Rectangle_Round_Edges(190, 125, 310, 155, 15);
  TFT_Rectangle_Round_Edges(190, 175, 310, 205, 15);
  
  TFT_Image_Jpeg(143, 72, ant_2_jpg);
  TFT_Image_Jpeg(143, 122, save_jpg);
  TFT_Image_Jpeg(143, 172, list_jpg);


  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("SCAN", 60, 30);
  
  TFT_Write_Text("1.   START", 22, 79);
  TFT_Write_Text("3.    OPEN", 22, 129);
  TFT_Write_Text("SENS:    ", 22, 179);
  TFT_Write_Text("2.    FREQ", 202, 79);
  TFT_Write_Text("4.  CLOSE", 202, 129);
  TFT_Write_Text("0.    BACK", 202, 179);

  }
  
  void screen_RF_list() {
// экран работы с листом RF кодов

/*TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(20, 20, list_jpg);


  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);

  TFT_Rectangle_Round_Edges(190, 20, 310, 50, 15);

  TFT_Rectangle_Round_Edges(190, 75, 310, 105, 15);
  TFT_Rectangle_Round_Edges(190, 115, 310, 145, 15);
  TFT_Rectangle_Round_Edges(190, 155, 310, 185, 15);
  TFT_Rectangle_Round_Edges(190, 195, 310, 225, 15);

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("CODE LIST", 60, 24);

  TFT_Write_Text("0.    BACK", 202, 24);

  TFT_Write_Text("9.    NEXT", 202, 79);
  TFT_Write_Text("8.    SEND", 202, 119);
  TFT_Write_Text("5.DELETE", 202, 159);
  TFT_Write_Text("7.   PREV.", 202, 199);

  TFT_Rectangle_Round_Edges(20, 75, 170, 225, 15);
  TFT_Write_Text("CODE #", 60, 82);
  TFT_Write_Text("25", 83, 107);
  TFT_Write_Text("creation time", 35, 140);
  TFT_Write_Text("25.05.2015", 45, 165);
  TFT_Write_Text("18:15:42", 55, 190);*/

  }
  
    void screen_RF_brut_force() {
// экран работы RF brutforce

/*TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(13, 10, ant_1_jpg);

  TFT_Set_Pen(CL_RED, 1);
  TFT_H_Line(20, 130, 100);
  TFT_Set_Pen(CL_WHITE, 1);
  TFT_H_Line(130, 300, 100);

  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);

  TFT_Rectangle_Round_Edges(190, 10, 310, 40, 15);

  TFT_Rectangle_Round_Edges(10, 55, 130, 85, 15);
  TFT_Rectangle_Round_Edges(190, 55, 310, 85, 15);

  TFT_Rectangle_Round_Edges(10, 115, 100, 145, 15);
  TFT_Rectangle_Round_Edges(10, 155, 130, 185, 15);
  TFT_Rectangle_Round_Edges(10, 195, 130, 225, 15);

  TFT_Rectangle_Round_Edges(110, 115, 210, 145, 15);

  TFT_Rectangle_Round_Edges(220, 115, 310, 145, 15);
  TFT_Rectangle_Round_Edges(190, 155, 310, 185, 15);
  TFT_Rectangle_Round_Edges(190, 195, 310, 225, 15);

  TFT_Image_Jpeg(143, 53, ant_2_jpg);
  TFT_Image_Jpeg(143, 152, save_jpg);
  TFT_Image_Jpeg(143, 192, list_jpg);



  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("BREAK-IN", 52, 14);
  TFT_Write_Text("9.CAME", 202, 14);

  TFT_Write_Text("7.   START", 22, 59);
  TFT_Write_Text("8.  PAUSE", 202, 59);

  TFT_Write_Text("4.  <<<", 22, 119);
  TFT_Write_Text("1.   OPEN", 22, 159);
  TFT_Write_Text("2.   CODE.", 22, 199);

  TFT_Write_Text("5.SEND", 122, 119);

  TFT_Write_Text("6.  >>>", 232, 119);
  TFT_Write_Text("3.  CLOSE", 202, 159);
  TFT_Write_Text("0.   BACK", 202, 199);*/

  }
  
    void screen_Save_location() {
// экран работы с листом RF кодов

  TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(20, 20, save_jpg);


  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);

  TFT_Rectangle_Round_Edges(10, 75, 310, 105, 15);
 // TFT_Rectangle_Round_Edges(10, 115, 310, 145, 15);
 // TFT_Rectangle_Round_Edges(10, 155, 310, 185, 15);
  
  TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
 // TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("SAVE  GPS  LOCATION", 63, 24);

  TFT_Write_Text("  +   Add GPS location", 22, 79);
 // TFT_Write_Text("1.   Add GPS location & direction", 22, 119);
 // TFT_Write_Text("2.   Add speed limit", 22, 159);
  TFT_Write_Text("0.   BACK", 22, 199);
 // TFT_Write_Text("9.   HELP", 212, 199);
  }
  
  void screen_Settings() {
// экран работы с настройками

  TFT_Fill_Screen(CL_BLACK);
  TFT_Image_Jpeg(20, 10, setting_jpg);


  TFT_Set_Pen(CL_WHITE, 1);
  TFT_Set_Brush(0, 0, 0, 0, 0, 0);

  TFT_Rectangle_Round_Edges(10, 195, 120, 225, 15);
//  TFT_Rectangle_Round_Edges(200, 195, 310, 225, 15);

  TFT_Set_Font(Arial21x24_Regular, CL_WHITE, FO_HORIZONTAL);
  TFT_Write_Text("SETTINGS", 63, 14);

  TFT_Write_Text("1.   Alarm  distance (m)", 5, 55);
  TFT_Write_Text("2.   Default  speed (km/h)", 5, 75);
  TFT_Write_Text("3.   Local  time  zone", 5, 95);
  TFT_Write_Text("4.   Erase memory (not SD card)", 5, 115);
  TFT_Write_Text("5.   Password", 5, 135);
  TFT_Write_Text("6.   Data  BackUP", 5, 155);
  TFT_Write_Text("0.   BACK", 22, 199);
//  TFT_Write_Text("9.   SAVE", 212, 199);
  //time_corretion_hh=3
  //alarm_speed = 60;
  //alarm_distance_m = 100.0;
  }