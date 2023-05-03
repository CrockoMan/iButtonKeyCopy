//**********************************************************
//
// DieHard
// Header для альтернативной библиотеки LCD для HD44780 МЭЛТ 
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
void lcd_init(char );                               // Инициализация LCD с нужной страницей знакогенератора (0/1)
void lcd_clear(void);                               // Очистка LCD
void lcd_gotoxy(unsigned char , unsigned char );    // Позиционирование курсора
void lcd_putchar(unsigned char );                   // Вывод символа в текущую позицию курсора
void lcd_puthex(unsigned char );                    // Вывод Hex в текущую позицию курсора
void lcd_puts(unsigned char *);                     // Вывод строки из массива в текущую позицию курсора
void lcd_putsf(char flash *);                       // Вывод строки из Flash в текущую позицию курсора

void lcd(unsigned char );
void lcd_cmd(unsigned char );
#pragma used-
