

void Sound_ON(){
 GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_10, _GPIO_CFG_DIGITAL_OUTPUT);
}

void Sound_OFF(){
 GPIO_Config(&GPIOC_BASE, _GPIO_PINMASK_10, _GPIO_CFG_DIGITAL_INPUT);
}


void sound_press_but(char sound_){
  if (sound_==1) {
   Sound_ON();
   Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
   Sound_OFF();
  }
}

void sound_duble(char mute_){
 if (mute_==1){
  Sound_ON();
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(100);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  Sound_OFF();
  }
}

void sound_perform(char mute_){
if (mute_==1){
 Sound_ON();
 Sound_Play( 880, 50);
 Sound_Play(1046, 50);
 Sound_Play(1318, 50);
 Sound_OFF();
 }
 
}

void sound_perform_off(char mute_){
if (mute_==1){
 Sound_ON();
 Sound_Play(1318, 50);
 Sound_Play( 880, 50);
 Sound_Play(1046, 50);
 Sound_OFF();
 }
}

void sound_alarm_1(char mute_){
if (mute_==1){
  Sound_ON();
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(100);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  Sound_OFF();
  }
}

void sound_alarm_2(char mute_){
if (mute_==1){
  Sound_ON();
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(100);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(100);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms

  Sound_OFF();
  }
}

void sound_alarm_3(char mute_){
if (mute_==1){
  Sound_ON();
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(50);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(50);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(50);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(50);
  Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
  delay_ms(50);
  Sound_OFF();
  }
}


/*delay_ms(2000);
 Sound_ON();
 Sound_Play(659, 50);   // Frequency = 659Hz, duration = 250ms
 delay_ms(1000);
 Sound_Play( 880, 50);
 Sound_Play(1046, 50);
 Sound_Play(1318, 50);
 Sound_OFF();*/