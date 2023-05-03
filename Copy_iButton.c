/*****************************************************
Chip type               : ATMega8
Program type            : Application
AVR Core Clock frequency: 4,000000 MHz
*****************************************************/

#include <mega8.h>
#include <HD44780.h> 
#include <delay.h>
#include <stdio.h>
#include <crc8.h>
#include "buttons.h"


#define SetBit(y,x)    y |= (1<<x) 
#define ClearBit(y,x)  y &=~(1<<x) 
#define InvertBit(y,x) y ^= (1<<x) 

#define IsBitSet(y, x)   ((y)  & (1 << (x)))
#define IsBitClear(y, x) (~(y) & (1 << (x)))

//----------------------------- ����������� ����������� 1-Wire
#define Wire1Bit    1
#define Wire1DDR    DDRC
#define Wire1Port   PORTC
#define Wire1Pin    PINC
//----------------------------- 1-Wire

#define FALSE 0
#define TRUE  1

#define KEY_READ     1
#define KEY_WRITE    2

#define RW1990_1    1
#define RW1990_2    2
#define TM2004      3



void Wire1_WriteBit_1990(unsigned char );
void Wire1_SendByte_1990(unsigned char);

char iB_Presence(void);
void Wire1_WriteBit0(void);
void Wire1_WriteBit1(void);
void Wire1_SendByte(char byte);
char Wire1_ReadByte(void);

/*
{0x01,0x94,0x69,0x64,0x00,0x00,0x00,0x2C};      // default1
{0x01,0x0C,0xF9,0x3E,0x10,0x00,0x00,0x02};      // default2
*/

unsigned char cByte[8]={0x01,0x94,0x69,0x64,0x00,0x00,0x00,0x2C};
volatile char cTick=0, cLed=0; 

char iB_Presence(void);

interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
    TCNT0 = 0xD9;   //������������ �������
    BUT_Debrief();  //����� ����������
    cTick=cTick+1;
    if(cLed++==20)
    {
        cLed=0;
        InvertBit(PORTC,3);
    } 
}


void main(void)
{
unsigned char i, cCRC=0, cKey=0, c1Byte[2]={0,0}, cButtonType=0, cError;
char cStr[10],cTmpByte[8];


PORTB =0x00;
DDRB = 0x00;

PORTC=0x03;                     // ���������� �� PORTC 2 3
DDRC=0x03;

BUT_Init();

//TIMER0 initialize - prescale:1024
// desired value: 100Hz
// actual value: 100,160Hz (0,2%)
TCCR0 = 0x00; //stop
TCNT0 = 0xD9; //set count
TCCR0 = 0x05; //start timer

TIMSK = 0x01; //timer interrupt sources
#asm("sei")


lcd_init(1);
lcd_gotoxy(0,0);
lcd_putsf("����� ");

while (1)
{
        if(cTick>=20) { cKey=BUT_GetKey(); cTick=0; }
        else          { cKey=0;   }

//-------------------------------------------------------------------------------------------------- ������ �����
        if(cKey==KEY_WRITE)
        {
            cKey=0;

            cButtonType=0;
            
            lcd_gotoxy(0,0);
            lcd_putsf("������");
//--------------------------------- ����������� ��������� RW1990.1

                iB_Presence();
                Wire1_SendByte(0xD1);
                Wire1_WriteBit1();
                delay_us(16);
                iB_Presence();
                Wire1_SendByte(0xB5);
                c1Byte[0]=Wire1_ReadByte();
                if(c1Byte[0]==0xFE)
                {
                    cButtonType=RW1990_1;
                }

//--------------------------------- ����������� ��������� RW1990.2
            if( cButtonType==0 )
            {
                iB_Presence();
                Wire1_SendByte(0x1D);
                Wire1_WriteBit1();
                delay_us(16);
                iB_Presence();
                Wire1_SendByte(0x1E);
                c1Byte[0]=Wire1_ReadByte();
                if(c1Byte[0]==0xFE)
                {
                    cButtonType=RW1990_2;
                }
/*
                for(i=0; i<4; i++)
                {
                    iB_Presence();
//                Wire1_SendByte(0x1E);
                    Wire1_SendByte(0xC4);
                    c1Byte[0]=Wire1_ReadByte();
                    if(c1Byte[0]==0xFE)
                    {
                        i=4;
                        cButtonType=RW1990_2;
                    }
                }
*/
            }            

//--------------------------------- ����������� ��������� TM 2004
            if( cButtonType==0 )
            {
                cButtonType=TM2004;
                if(iB_Presence()==TRUE)         // ���� ���������� �� ����� !!!
                {
                    Wire1_SendByte(0x33);       // �������� ������� ������ ��������� ������
                    for(i=0;i<8;i++)
                    {
                        cTmpByte[i]=Wire1_ReadByte();     // ������ ������ ����� �� ����������
                    }
                    
                    iB_Presence();
                    Wire1_SendByte(0x23);       // �������� ������� ������ ��������� ������
                    for(i=0;i<8;i++)
                    {
                        if(cTmpByte[i]!=Wire1_ReadByte())   cButtonType=0;
                        
                    }
                }
            }
            iB_Presence();
            
//            cButtonType=RW1990;    
//------------------------------------------- ������ ��������� RW1990.1
            if(cButtonType==RW1990_1)
            {
                lcd_putsf(" RW1990.1 ");
                if(iB_Presence()==TRUE)
                {
                    Wire1_SendByte(0xD1);

                    Wire1_WriteBit0();
                    delay_us(16);

//                iB_Presence();
//                Wire1_SendByte(0x1E);
//                c1Byte[1]=Wire1_ReadByte();
                    iB_Presence();
                    Wire1_SendByte(0xD5);
                    for(i=0; i<8; i++)
                    {
                        Wire1_SendByte_1990(~cByte[i]);   // �������� � ������ ����� � ��������� RW1990.1
                    }
                }
                iB_Presence();
            } //RW1990.1

//------------------------------------------- ������ ��������� RW1990.1
            if(cButtonType==RW1990_2)
            {
                lcd_putsf(" RW1990.2 ");
//                for(i=0; i<4; i++)
//                {
//                    iB_Presence();
//                }
//                Wire1_SendByte(0x1E);
//                c1Byte[0]=Wire1_ReadByte();
//                delay_us(13);
                if(iB_Presence()==TRUE)
                {
                    Wire1_SendByte(0x1D);

                    Wire1_WriteBit1();
                    delay_us(16);

//                iB_Presence();
//                Wire1_SendByte(0x1E);
//                c1Byte[1]=Wire1_ReadByte();
                    iB_Presence();
                    Wire1_SendByte(0xD5);
                    for(i=0; i<8; i++)
                    {
                        Wire1_SendByte_1990(cByte[i]);   // �������� � ������ ����� � ��������� RW1990.2
                    }
                }
                iB_Presence();
            } //RW1990.2


//------------------------------------------- ������ ��������� TM-2004
            if(cButtonType==TM2004)
            {
                lcd_putsf(" TM2004   ");
                if(iB_Presence()==TRUE)             // ���� ���������� �� ����� !!!
                {
                    Wire1_SendByte(0x3C);           // �������� ������� ������ ��������� ������
                    Wire1_SendByte(0x00);           // �������� ������ ������ ������� ������
                    Wire1_SendByte(0x00);           // �������� ������ ������ ������� ������

                    for(i=0; i<8; i++)
                    {
                        Wire1_SendByte(cByte[i]);       // �������� ����� ��� ������
                        c1Byte[0]=Wire1_ReadByte();     // ������ CRC

//--------------------------------------------------------------------------- ��������������� ������� ��� �� 2004
                        delay_us(600);
                        SetBit(Wire1DDR, Wire1Bit);     // ������ � ���� 0
                        delay_us(5);                    // ���������� ���� � 0 5 ���;
                        ClearBit(Wire1DDR, Wire1Bit);   // ������ � ���� 1
                        delay_ms(50);
//--------------------------------------------------------------------------- ��������������� ������� ��� �� 2004

                        c1Byte[1]=Wire1_ReadByte();     // ������ ����������� �����
                    }
/*
                    cCRC=0;
                    crc_8(0x3C, &cCRC);
                    crc_8(0x00, &cCRC);
                    crc_8(0x00, &cCRC);
                    crc_8(cByte[0], &cCRC);
*/
                }
/*
                sprintf(cStr, "%2X %2X  (%2X %2X)", cCRC, c1Byte[0], cByte[0], c1Byte[1]);
                lcd_gotoxy(0,0);
                lcd_puts(cStr);
*/
                iB_Presence();
            }   // TM2004

            if(cButtonType==0)
            {
                lcd_gotoxy(8,0);
                lcd_putsf(" ������ ");
                delay_ms(2500);
            }
            else
            {
                cError=FALSE;
                iB_Presence();
                Wire1_SendByte(0x33);       // �������� ������� ������ ��������� ������
                for(i=0;i<8;i++)
                {
                    if(cByte[i]!=Wire1_ReadByte()) cError=TRUE;    // �������� ������
                }

                lcd_gotoxy(0,0);
                if(cError==TRUE)
                {
                    lcd_putsf("������ O�����    ");
                    delay_ms(2500);
                }
                else
                {
                    lcd_putsf("������ OK        ");
                    delay_ms(1500);
                }
                cButtonType=0;
            }
            lcd_gotoxy(0,0);
            lcd_putsf("�����            ");
        }
        
//-------------------------------------------------------------------------------------------------- ������ �����
        if(cKey==KEY_READ)
        {
            cKey=0;
            lcd_clear(); 
            lcd_putsf("������");
            cCRC=0;

            if(iB_Presence()==TRUE)         // ���� ���������� �� ����� !!!
            {
                Wire1_SendByte(0x33);       // �������� ������� ������ ��������� ������
                for(i=0;i<8;i++)
                {
                    cByte[i]=0;
                    cByte[i]=Wire1_ReadByte();     // ������ ������ ����� �� ����������
                }
                for(i=0;i<8;i++)
                {
                    if(i<7) crc_8(cByte[i], &cCRC);
                    if(cByte[i]>0x0f)   sprintf(cStr, "%2X", cByte[i]);
                    else                sprintf(cStr, "0%1X", cByte[i]);            
                    lcd_gotoxy(i*2,1);
                    lcd_puts(cStr);
                }

                lcd_gotoxy(14,0);
                sprintf(cStr, "%2X ", cCRC);
                lcd_puts(cStr);

                lcd_gotoxy(0,0);
                if(cCRC==cByte[7])
                {
//                    lcd_gotoxy(0,0);
                    lcd_putsf("������ OK");
                    delay_ms(2500);
                }
                else
                {
//                    lcd_gotoxy(0,0);
                    lcd_putsf("������ O����� CRC");
                    delay_ms(2500);
                }

            }
            else
            {
                lcd_gotoxy(0,0);
                lcd_putsf("������ ������ Presence");
                delay_ms(2500);
                lcd_clear();
            }
            lcd_gotoxy(0,0);
            lcd_putsf("�����            ");
      }
    };   // while();
}


char iB_Presence()
{
    char i=0, j=0, lPresence=FALSE;
//--------------------------------------- ������������� 1-Wire, �������� ������� Presence  � �����
    ClearBit(Wire1Port, Wire1Bit);
    delay_us(250);
    SetBit(Wire1DDR, Wire1Bit);     // ������ � ���� 0
    delay_us(500);                  // ��������� ������� � ������� ����� 480 �����������
    ClearBit(Wire1DDR, Wire1Bit);   // ���� � ����� ������

    delay_us(60);                   // �������� ��������� ������

    for(i=0;i<60;i++)
    {
        delay_us(1);
        if( IsBitClear(Wire1Pin, Wire1Bit) )  // ����� ������� 1-Wire �� ����� 
        {
            lPresence=TRUE;
            for(j=0;j<(180-i);j++)
            {
                delay_us(1);
                if( IsBitSet(Wire1Pin, Wire1Bit) )   j=180;   //  ���������� ��������� �������
            }                                                 // ������� Presence � �����    
            i=60;
        }
    }
    delay_us(420);                  // ����� �� ���������� iButton
    
    return lPresence;
}


// ------------------------------------------------------------------------
//                     ������ ���� � ��������� RW1990
// ------------------------------------------------------------------------
void Wire1_WriteBit_1990(unsigned char cBit)
{
    SetBit(Wire1DDR, Wire1Bit);     // ������ � ���� 0
    if(cBit==0)     delay_us(60);
    else            delay_us(10);
    ClearBit(Wire1DDR, Wire1Bit);   // ���� � ����� ������

    delay_ms(13);
}



// ------------------------------------------------------------------------
//                     ������ ����� � ��������� RW1990
// ------------------------------------------------------------------------
void Wire1_SendByte_1990(unsigned char data)
{
    unsigned char temp;
    unsigned char i;

    for (i = 0; i < 8; i++)         // �������� ���� � �����
    {
        temp = data & 0x01;
        if (temp)
        {
            Wire1_WriteBit_1990(1);
        }
        else
        {
            Wire1_WriteBit_1990(0);
        }
        data >>= 1;

    }
}




//------------------------------------------------------------------------------
//                      ������ ���� �� ����
//------------------------------------------------------------------------------
unsigned char Wire1_ReadBit()
{
    unsigned char ReadBit=0;

    SetBit(Wire1DDR, Wire1Bit);     // ������ � ���� 0
    delay_us(2);
    ClearBit(Wire1DDR, Wire1Bit);   // ���� � ����� ������
    delay_us(14);

// ������ ����� 1-Wire
    if( IsBitSet(Wire1Pin, Wire1Bit) ) ReadBit=1;    // � ����� 1
    delay_us(60);
    
    return ReadBit;
}


//------------------------------------------------------------------------------
//                      ������ ����� �� ����
//------------------------------------------------------------------------------
unsigned char Wire1_ReadByte()
{
    unsigned char data;
    unsigned char i;

    data = 0x00;

    for (i = 0; i < 8; i++)
    {
        data >>= 1;
        if (Wire1_ReadBit())
        {
            data |= 0x80;
        }
    }
    delay_us(30);
    return data;
}



//------------------------------------------------------------------------------
//                         ������ � ���� 1
//------------------------------------------------------------------------------
void Wire1_WriteBit1(void)
{
    SetBit(Wire1DDR, Wire1Bit);       // ������ � ���� 0
    delay_us(1);                      // �������� �� ����� ��� 1 ���;
    ClearBit(Wire1DDR, Wire1Bit);     // ������ � ���� 1
    delay_us(60);                     // �������� ��������� ���������;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//                         ������ � ���� 0
//------------------------------------------------------------------------------
void Wire1_WriteBit0(void)
{
    SetBit(Wire1DDR, Wire1Bit);       // ������ � ���� 0
    delay_us(60);                     // ���������� ���� � 0 60 ���;
    ClearBit(Wire1DDR, Wire1Bit);     // ������ � ���� 1
    delay_us(1);                      // �������� ��������� ���������;
}
//------------------------------------------------------------------------------




//------------------------------------------------------------------------------
//                      ������� ����� � ����
//------------------------------------------------------------------------------
void Wire1_SendByte(char byte)
{
    char count = 0;
    for( count = 0; count < 8; count ++)
    {
       // ����� ���������� ���� ������� � ��������
       if(byte & 0x01)
         {
           Wire1_WriteBit1();
         }
       else 
         {
           Wire1_WriteBit0();
         }
       byte >>= 1;  //�������� ������ ������
    }
    delay_us(30);
}


