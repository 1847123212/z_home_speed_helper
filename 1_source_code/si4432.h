


void Hopping_step(unsigned int h_step);
void BW_setting(unsigned short bw);
void Si_Activate();
void NOP();
void Write0();
void Write1();
short Si_Read_Data(unsigned short command);
void Si_Write_Data(unsigned short command, unsigned short data_);
void Frequency_setting(float RX_Frequency_temp);
void si_4313_registrs_settings();
float round(float x);
void n_SEL_Hi();
void n_SEL_Lo();
void ant_TX();
void ant_RX();
void Si_SDI_(unsigned short Dat);
void rx_mode();
void tx_mode();
short read_si_data(unsigned short rssi_tmp_);