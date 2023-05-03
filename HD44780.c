#include <mega8.h>

//**********************************************************
//
// DieHard
// �������������� ���������� LCD ��� HD44780 ���� 
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

    
//������ ������ � HD44780   4 bit mode
void lcd(unsigned char cStr)
 {
    LCDPORT SetBit(LCD_E);   
                                                    // ����� �������� ������
    if( ((cStr&0x80)>>7) )  LCDPORT SetBit(LCD_DB7);
    else                    LCDPORT ClearBit(LCD_DB7);
    if( ((cStr&0x40)>>6) )  LCDPORT SetBit(LCD_DB6);
    else                    LCDPORT ClearBit(LCD_DB6);
    if( ((cStr&0x20)>>5) )  LCDPORT SetBit(LCD_DB5);
    else                    LCDPORT ClearBit(LCD_DB5);
    if( ((cStr&0x10)>>4) )  LCDPORT SetBit(LCD_DB4);
    else                    LCDPORT ClearBit(LCD_DB4);

    LCDPORT ClearBit(LCD_E);                        // ������
    delay_ms(1);

    LCDPORT SetBit(LCD_E);                          // enable = 1

                                                    // ������� �������� ������
    if( ((cStr&0x08)>>3) )  LCDPORT SetBit(LCD_DB7);
    else                    LCDPORT ClearBit(LCD_DB7);
    if( ((cStr&0x04)>>2) )  LCDPORT SetBit(LCD_DB6);
    else                    LCDPORT ClearBit(LCD_DB6);
    if( ((cStr&0x02)>>1) )  LCDPORT SetBit(LCD_DB5);
    else                    LCDPORT ClearBit(LCD_DB5);
    if( (cStr&0x01) )       LCDPORT SetBit(LCD_DB4);
    else                    LCDPORT ClearBit(LCD_DB4);
    LCDPORT ClearBit(LCD_E);                        // ������
    delay_ms(1);
 }
    

// ������ ������� � HD44780
void lcd_cmd(unsigned char cStr)
{
    LCDPORT ClearBit(LCD_RS);                       // ����� ������ ������
    lcd(cStr);                                      // ������
}

// ������ ������ � HD44780
void lcd_putchar(unsigned char cStr)
 {
    LCDPORT SetBit(LCD_RS);                         // ����� ������ ������
    lcd(cStr);                                      // ������
 }     

// ������ ������� ������� � HD44780
void lcd_clear(void)
{
    lcd_cmd(0x01);
//    delay_ms(1);
}


// ������������� ����� � HD44780
// nPage - ����� �������� ��������������� 0 ��� 1 1-���(MELT)
void lcd_init(char nPage)
{

    LCDDDR=(1<<LCD_RS)|(1<<LCD_RD)|(1<<LCD_E)|(1<<LCD_DB4)|
           (1<<LCD_DB5)|(1<<LCD_DB6)|(1<<LCD_DB7);
     
    delay_ms(20);                                   // ��������������� ��������
    lcd_cmd(0x33); 
    lcd_cmd(0x32); 
    if(nPage==0)   lcd_cmd(0x28);                   // 4 ����, 2 ������ ������� �������� 0
    else           lcd_cmd(0x2A);                   // 4 ����, 2 ������ ������� �������� 1 ��� ����-�������
                                                    // ������� ��� MELT
    
    lcd_cmd(0x08);                                  // ���������� ���
    lcd_clear();                                    // ������� ���
    lcd_cmd(0x06);                                  // ����� ������� ������
    lcd_cmd(0x0C);                                  // ��������� ��� �������
}         

// ����� ������ �� �����
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

// ����� ������ �� Flash �� �����
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

// ������ ������� �������� ������� � ������ ������� � HD44780
void lcd_gotoxy(unsigned char x, unsigned char y)
{
    lcd_cmd((0x80+y*64)+x);                         // ������� �������. 0�80 - ���. �����. ����
}
