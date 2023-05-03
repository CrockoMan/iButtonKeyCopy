/*** ����������� ������� ����� **************************************************************************/
#define W_PORTA  PORTC        // ����, �� ������� ���������� 1-wire ����������
#define W_PINA   PINC         // ���� �����, �� ������� ���������� 1-wire ����������
#define W_DDRA   DDRC         // ����������� �����, �� ������� ���������� 1-wire ����������
#define W_MASKA  0b11111111   // ����� ����� �����, ������� ������������ ��� ����������� 1-wire ���������
#define W_CTRL_DDR DDRD       // ����������� �����, �� ������� �������� ����������
#define W_CTRL_PRT PORTD      // ����, �� ������� �������� ����������
#define W_CTRL_A   4          // ����������� ��� ����� �
#define W_CTRL_B   3          // ����������� ��� ����� �

/*** ����������� ������� ����� **************************************************************************/
#define W_PORTB  PORTA        // ����, �� ������� ���������� 1-wire ����������
#define W_PINB   PINA         // ���� �����, �� ������� ���������� 1-wire ����������
#define W_DDRB   DDRA         // ����������� �����, �� ������� ���������� 1-wire ����������
#define W_MASKB  0b11111111   // ����� ����� �����, ������� ������������ ��� ����������� 1-wire ���������

/*** ��������� 1-wire ��������� *************************************************************************/
#define W_RES_DELAY  700      // ������������ �������� ������, � �������������
#define W_DS_FAMILY  0x28     // ��� ��������� DS18B20. ������������ ��� �������� ������� ����������

/*** ������� ROM 1-wire ���������� (DS18B20) ************************************************************/
#define W_READ_ROM       0x33
#define W_MATCH_ROM      0x55
#define W_SKIP_ROM       0xCC
#define W_SEARCH_ROM     0xF0
#define W_ALARM_SEARCH   0xEC
 
/*** ������� RAM 1-wire ���������� (DS18B20) ************************************************************/
#define W_WRITE_SCRATCHPAD     0x4E
#define W_READ_SCRATCHPAD      0xBE
#define W_COPY_SCRATCHPAD      0x48
#define W_CONVERT_T            0x44
#define W_RECALL_E2            0xB8
#define W_READ_POWER           0xB4

/*** ����������� ���������� ��� 1-wire � ���������� (DS18B20) *******************************************/
unsigned char  w_data[16];    // ����������� ������ (�� ������ ����� �� ����������)
unsigned int    devices=0;    // ���������� ���������� (������������� ����� ������ w_init();)
signed int     w_temp[16];    // ���������� ����������� � ������� DS18B20
char      w_temp_t[16][5];    // �����������, ������������������ � �����

/***************           ������������ ����� �� ����                     ***************/
void w_set_in(void){
 W_DDRA&=~W_MASKA;
 W_DDRB&=~W_MASKB;
 W_CTRL_PRT.W_CTRL_A=0;
 W_CTRL_PRT.W_CTRL_B=0;
}

/***************           ������������ ����� �� �����                    ***************/
void w_set_out(void){
 W_DDRA=W_DDRA|W_MASKA;
 W_DDRB=W_DDRB|W_MASKB;
 W_CTRL_PRT.W_CTRL_A=1;
 W_CTRL_PRT.W_CTRL_B=1;
}

/***************           ��������� 1                                    ***************/
void set_1(void){
 W_PORTA|=W_MASKA;
 W_PORTB|=W_MASKB;
}

/***************           ��������� 0                                    ***************/
void set_0(void){
 W_PORTA&=~W_MASKA;
 W_PORTB&=~W_MASKB;
}

/***************           ����� 1-wire ���������                         ***************/
void w_reset(void){
 w_set_out();
 set_0();
 delay_us(W_RES_DELAY);
 w_set_in();
 delay_us(80);
 delay_us(250);
}

/***************           ������� ������ �����                           ***************/
void w_send_byte(char s_byte){
 char sh_byte;
 char send_bit;
 char i;
 for (i=0;i<8;i++){
  delay_us(5);                 // �������� ����� �������� ����������� ����
  set_0();                     // ��������� ������ � ����
  w_set_out();
  delay_us(2);                 // ������ ����
  sh_byte  = s_byte >> i;
  send_bit = sh_byte & 1;
  if (send_bit){               // ���� �������� �������...
   w_set_in();                 // �� ������ ����������� �� ����
  } else {       
   set_0();                    // ����� ������ ���������� ������� �����
  }
  delay_us(60);                // � ������ ��������� ��������� � ������� 60 ���.
  w_set_in();                  // ����� ���� ����������� �� ����
 }
 w_set_in();                   // �� ��������� �������� ����������� �� ����
}

/***************           ������ ������ ����� �� ���� ���������          ***************/
void w_read_bytes(void){
char i,j;
char w_pins[2];
 for (i=0;i<16;i++){           // �������� ��� ��������� ����������� ����� �����
  w_data[i]=0;
 }
 for (i=0;i<8;i++){
  set_0();                     // ���������� ����� � �����
  w_set_out();
  delay_us(4);                 // ��������, �� �� �����.
  w_set_in();                  // ����� ���� ������������� �� ����
  delay_us(8);                 // � ����� 8 ���. (� ����� ����� 12 ���.)...
  w_pins[0]=W_PINA;            // ������ �������������...
  w_pins[1]=W_PINB;            // � �٨ �������������!!!
  for (j=0;j<8;j++){           // �� � ��������� �� ��������� ������ �� ������ ����...
#pragma warn-                  // ��������� ��� ���� � ������ ���������
    w_data[  j]=w_data[  j]|(((w_pins[0]>>(7-j))&0x01)<<i);
    w_data[j+8]=w_data[j+8]|(((w_pins[1]>>j)&0x01)<<i);
#pragma warn+
  }
  delay_us(50);                // �� � ��� 50 ���., � ������� ������� ������ ����� ������� ���������...
 }
}

/***************           ������������� 1-wire ���������� DS18B20        ***************/
void w_init(void){
unsigned int i;
 devices=0;
 w_reset();                    // ���������� ����� (��� ������� ����� �����)
 w_send_byte(W_READ_ROM);      // �������, ��� ����� ��������� ��� ���������, �������� ����� � �.�.
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
 w_send_byte(W_SKIP_ROM);      // ��� ��������� ���������
}

/*************** ������ ����������� (��������� � w_temp � w_outA, w_outB) ***************/
void w_get_temp(void){
unsigned int i;
 w_send_byte(W_CONVERT_T);                            // �������� ������� ���������
 w_set_out();                                         // ����������� ���� �� �����
 set_1();                                             // ������������� ������� �� ����� ���������
 delay_ms(800);                                       // ��� 800 ��. ��� ���������
 w_set_in();
   w_init();                                            // ����� ���� ���������� ������,
   w_send_byte(W_READ_SCRATCHPAD);                      // �������� ������� ������ ���������
   w_read_bytes();                                      // ������ ������� ����
   for (i=0;i<16;i++){
    w_temp[i]=w_data[i];                                // ��������� ��������
   }
   w_read_bytes();                                      // ������ ������� ����
   for (i=0;i<16;i++){
    w_temp[i]=w_temp[i]|(((signed int)(w_data[i]))<<8); // ��������� ������ ��������
    if (!(devices & BIT(i))) w_temp[i]=0;
    sprintf(w_temp_t[i],"%X%X%X%X",(char)((w_temp[i]>>12) & 0x000F),
                                   (char)((w_temp[i]>> 8) & 0x000F),
                                   (char)((w_temp[i]>> 4) & 0x000F),
                                   (char)((w_temp[i]>> 0) & 0x000F));
   }
}