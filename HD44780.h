//**********************************************************
//
// DieHard
// Header ��� �������������� ���������� LCD ��� HD44780 ���� 
//
//**********************************************************

#define LCDPORT PORTD
#define LCDDDR  DDRD
#define LCDPIN  PINCD

#define LCD_RS  0
#define LCD_RD  1
#define LCD_E   2
#define LCD_DB4 4
#define LCD_DB5 5
#define LCD_DB6 6
#define LCD_DB7 7


#pragma used+
void lcd_init(char );                               // ������������� LCD � ������ ��������� ��������������� (0/1)
void lcd_clear(void);                               // ������� LCD
void lcd_gotoxy(unsigned char , unsigned char );    // ���������������� �������
void lcd_putchar(unsigned char );                   // ����� ������� � ������� ������� �������
void lcd_puthex(unsigned char );                    // ����� Hex � ������� ������� �������
void lcd_puts(unsigned char *);                     // ����� ������ �� ������� � ������� ������� �������
void lcd_putsf(char flash *);                       // ����� ������ �� Flash � ������� ������� �������

void lcd(unsigned char );
void lcd_cmd(unsigned char );
#pragma used-
