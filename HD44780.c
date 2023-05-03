#include <mega8.h>

//**********************************************************
//
// DieHard
// Альтернативная библиотека LCD для HD44780 МЭЛТ 
//
//*********************************************************

#include <delay.h>
#include <HD44780.h> 

#define SetBit(x)    |= (1<<x) 
#define ClearBit(x)  &=~(1<<x) 

void lcd_puthex(unsigned char i)
{
    char hi,lo;

    hi=i&0xF0;               // High nibble
    hi=hi>>4;
    hi+='0';
    if (hi>'9')
        hi+=7;

    lo=(i&0x0F)+'0';         // Low nibble
    if (lo>'9')
        lo=lo+7;

    lcd_putchar(hi); lcd_putchar(lo);
}

    
//Запись команд в HD44780   4 bit mode
void lcd(unsigned char cStr)
 {
    LCDPORT SetBit(LCD_E);   
                                                    // Вывод старшего ниббла
    if( ((cStr&0x80)>>7) )  LCDPORT SetBit(LCD_DB7);
    else                    LCDPORT ClearBit(LCD_DB7);
    if( ((cStr&0x40)>>6) )  LCDPORT SetBit(LCD_DB6);
    else                    LCDPORT ClearBit(LCD_DB6);
    if( ((cStr&0x20)>>5) )  LCDPORT SetBit(LCD_DB5);
    else                    LCDPORT ClearBit(LCD_DB5);
    if( ((cStr&0x10)>>4) )  LCDPORT SetBit(LCD_DB4);
    else                    LCDPORT ClearBit(LCD_DB4);

    LCDPORT ClearBit(LCD_E);                        // Запись
    delay_ms(1);

    LCDPORT SetBit(LCD_E);                          // enable = 1

                                                    // Выводим младшего ниббла
    if( ((cStr&0x08)>>3) )  LCDPORT SetBit(LCD_DB7);
    else                    LCDPORT ClearBit(LCD_DB7);
    if( ((cStr&0x04)>>2) )  LCDPORT SetBit(LCD_DB6);
    else                    LCDPORT ClearBit(LCD_DB6);
    if( ((cStr&0x02)>>1) )  LCDPORT SetBit(LCD_DB5);
    else                    LCDPORT ClearBit(LCD_DB5);
    if( (cStr&0x01) )       LCDPORT SetBit(LCD_DB4);
    else                    LCDPORT ClearBit(LCD_DB4);
    LCDPORT ClearBit(LCD_E);                        // Запись
    delay_ms(1);
 }
    

// Запись команды в HD44780
void lcd_cmd(unsigned char cStr)
{
    LCDPORT ClearBit(LCD_RS);                       // Режим записи команд
    lcd(cStr);                                      // Запись
}

// Запись данных в HD44780
void lcd_putchar(unsigned char cStr)
 {
    LCDPORT SetBit(LCD_RS);                         // Режим записи команд
    lcd(cStr);                                      // Запись
 }     

// Запись команды очистки в HD44780
void lcd_clear(void)
{
    lcd_cmd(0x01);
//    delay_ms(1);
}


// Инициализация порта и HD44780
// nPage - номер страницы знакогенератора 0 или 1 1-Рус(MELT)
void lcd_init(char nPage)
{

    LCDDDR=(1<<LCD_RS)|(1<<LCD_RD)|(1<<LCD_E)|(1<<LCD_DB4)|
           (1<<LCD_DB5)|(1<<LCD_DB6)|(1<<LCD_DB7);
     
    delay_ms(20);                                   // Предварительная задержка
    lcd_cmd(0x33); 
    lcd_cmd(0x32); 
    if(nPage==0)   lcd_cmd(0x28);                   // 4 бита, 2 строки кодовая страница 0
    else           lcd_cmd(0x2A);                   // 4 бита, 2 строки кодовая страница 1 для МЭЛТ-Русская
                                                    // Русская для MELT
    
    lcd_cmd(0x08);                                  // Выключение ЖКИ
    lcd_clear();                                    // Очистка ЖКИ
    lcd_cmd(0x06);                                  // Сдвиг курсора вправо
    lcd_cmd(0x0C);                                  // Включение без курсора
}         

// Вывод строки на экран
void lcd_puts(unsigned char *pStr)
{
    unsigned char i;
#pragma warn-
    while (i=*pStr)
    {
        lcd_putchar(i);
        pStr++;
    }
#pragma warn+
}

// Вывод строки из Flash на экран
void lcd_putsf(char flash *pStr)
{
    unsigned char i;
#pragma warn-
    while (i=*pStr)
    {
        lcd_putchar(i);
        pStr++;
    }
#pragma warn+
}

// Запись команды перевода курсора в нужную позицию в HD44780
void lcd_gotoxy(unsigned char x, unsigned char y)
{
    lcd_cmd((0x80+y*64)+x);                         // Рассчет позиции. 0х80 - лев. верхн. угол
}
