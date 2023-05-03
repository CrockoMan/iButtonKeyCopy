/*** Определения первого порта **************************************************************************/
#define W_PORTA  PORTC        // Порт, на который подключены 1-wire устройства
#define W_PINA   PINC         // Пины порта, на который подключены 1-wire устройства
#define W_DDRA   DDRC         // Направление порта, на который подключены 1-wire устройства
#define W_MASKA  0b11111111   // Маска пинов порта, которые используются для подключения 1-wire устройств
#define W_CTRL_DDR DDRD       // Направление порта, на который выведено управление
#define W_CTRL_PRT PORTD      // Порт, на который выведено управление
#define W_CTRL_A   4          // Управляющий пин порта А
#define W_CTRL_B   3          // Управляющий пин порта В

/*** Определения второго порта **************************************************************************/
#define W_PORTB  PORTA        // Порт, на который подключены 1-wire устройства
#define W_PINB   PINA         // Пины порта, на который подключены 1-wire устройства
#define W_DDRB   DDRA         // Направление порта, на который подключены 1-wire устройства
#define W_MASKB  0b11111111   // Маска пинов порта, которые используются для подключения 1-wire устройств

/*** Параметры 1-wire устройств *************************************************************************/
#define W_RES_DELAY  700      // Длительность импульса сброса, в микросекундах
#define W_DS_FAMILY  0x28     // Код семейства DS18B20. Используется для проверки наличия устройства

/*** Функции ROM 1-wire термометра (DS18B20) ************************************************************/
#define W_READ_ROM       0x33
#define W_MATCH_ROM      0x55
#define W_SKIP_ROM       0xCC
#define W_SEARCH_ROM     0xF0
#define W_ALARM_SEARCH   0xEC
 
/*** Функции RAM 1-wire термометра (DS18B20) ************************************************************/
#define W_WRITE_SCRATCHPAD     0x4E
#define W_READ_SCRATCHPAD      0xBE
#define W_COPY_SCRATCHPAD      0x48
#define W_CONVERT_T            0x44
#define W_RECALL_E2            0xB8
#define W_READ_POWER           0xB4

/*** Определения переменных для 1-wire и термометра (DS18B20) *******************************************/
unsigned char  w_data[16];    // Прочитанные данные (по одному байту на устройство)
unsigned int    devices=0;    // Ответившие устройства (действительно после вызова w_init();)
signed int     w_temp[16];    // Измеренная температура в формате DS18B20
char      w_temp_t[16][5];    // Температура, отконвертированная в текст

/***************           Переключение порта на вход                     ***************/
void w_set_in(void){
 W_DDRA&=~W_MASKA;
 W_DDRB&=~W_MASKB;
 W_CTRL_PRT.W_CTRL_A=0;
 W_CTRL_PRT.W_CTRL_B=0;
}

/***************           Переключение порта на выход                    ***************/
void w_set_out(void){
 W_DDRA=W_DDRA|W_MASKA;
 W_DDRB=W_DDRB|W_MASKB;
 W_CTRL_PRT.W_CTRL_A=1;
 W_CTRL_PRT.W_CTRL_B=1;
}

/***************           Установка 1                                    ***************/
void set_1(void){
 W_PORTA|=W_MASKA;
 W_PORTB|=W_MASKB;
}

/***************           Установка 0                                    ***************/
void set_0(void){
 W_PORTA&=~W_MASKA;
 W_PORTB&=~W_MASKB;
}

/***************           Сброс 1-wire устройств                         ***************/
void w_reset(void){
 w_set_out();
 set_0();
 delay_us(W_RES_DELAY);
 w_set_in();
 delay_us(80);
 delay_us(250);
}

/***************           Посылка одного байта                           ***************/
void w_send_byte(char s_byte){
 char sh_byte;
 char send_bit;
 char i;
 for (i=0;i<8;i++){
  delay_us(5);                 // Задержка после передачи предыдущего бита
  set_0();                     // Установка выхода в ноль
  w_set_out();
  delay_us(2);                 // Держим ноль
  sh_byte  = s_byte >> i;
  send_bit = sh_byte & 1;
  if (send_bit){               // Если посылаем единицу...
   w_set_in();                 // то просто переключаем на вход
  } else {       
   set_0();                    // иначе просто продолжаем держать нолик
  }
  delay_us(60);                // и держим посланное состояние в течение 60 мкс.
  w_set_in();                  // после чего переключаем на вход
 }
 w_set_in();                   // По окончанию передачи переключаем на вход
}

/***************           Чтение одного байта со всех устройств          ***************/
void w_read_bytes(void){
char i,j;
char w_pins[2];
 for (i=0;i<16;i++){           // Обнуляем для надёжности прочитанные ранее байты
  w_data[i]=0;
 }
 for (i=0;i<8;i++){
  set_0();                     // Выставляем нолик в линию
  w_set_out();
  delay_us(4);                 // короткий, но не очень.
  w_set_in();                  // после чего переключаемся на вход
  delay_us(8);                 // и через 8 мкс. (в сумме через 12 мкс.)...
  w_pins[0]=W_PINA;            // ЧИТАЕМ ПОЛРЕЗУЛЬТАТА...
  w_pins[1]=W_PINB;            // И ЕЩЁ ПОЛРЕЗУЛЬТАТА!!!
  for (j=0;j<8;j++){           // Ну и поскольку мы прочитали только по одному биту...
#pragma warn-                  // Сохраняем эти биты и читаем следующие
    w_data[  j]=w_data[  j]|(((w_pins[0]>>(7-j))&0x01)<<i);
    w_data[j+8]=w_data[j+8]|(((w_pins[1]>>j)&0x01)<<i);
#pragma warn+
  }
  delay_us(50);                // Ну и ждём 50 мкс., в течение которых датчик может немного отдохнуть...
 }
}

/***************           Инициализация 1-wire термометра DS18B20        ***************/
void w_init(void){
unsigned int i;
 devices=0;
 w_reset();                    // Сбрасываем линию (даём длинный такой нолик)
 w_send_byte(W_READ_ROM);      // Говорим, что хотим прочитать код семейства, серийный номер и т.д.
 w_read_bytes();
 for(i=0; i<16; i++){
  if (w_data[i]==W_DS_FAMILY){
    devices = devices | BIT(i);
  }
 }
 w_read_bytes();
 w_read_bytes();
 w_read_bytes();
 w_read_bytes();
 w_read_bytes();
 w_read_bytes();
 w_read_bytes();
 w_reset();
 w_send_byte(W_SKIP_ROM);      // Код семейства прочитали
}

/*************** Чтение температуры (результат в w_temp и w_outA, w_outB) ***************/
void w_get_temp(void){
unsigned int i;
 w_send_byte(W_CONVERT_T);                            // Посылаем команду измерения
 w_set_out();                                         // Переключаем порт на выход
 set_1();                                             // Устанавливаем единицу на время измерения
 delay_ms(800);                                       // Ждём 800 мс. для надёжности
 w_set_in();
   w_init();                                            // После чего сбрасываем датчик,
   w_send_byte(W_READ_SCRATCHPAD);                      // Посылаем команду чтения регистров
   w_read_bytes();                                      // Читаем младший байт
   for (i=0;i<16;i++){
    w_temp[i]=w_data[i];                                // Сохраняем половину
   }
   w_read_bytes();                                      // Читаем старший байт
   for (i=0;i<16;i++){
    w_temp[i]=w_temp[i]|(((signed int)(w_data[i]))<<8); // Сохраняем вторую половину
    if (!(devices & BIT(i))) w_temp[i]=0;
    sprintf(w_temp_t[i],"%X%X%X%X",(char)((w_temp[i]>>12) & 0x000F),
                                   (char)((w_temp[i]>> 8) & 0x000F),
                                   (char)((w_temp[i]>> 4) & 0x000F),
                                   (char)((w_temp[i]>> 0) & 0x000F));
   }
}