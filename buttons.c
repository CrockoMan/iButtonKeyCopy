#include <mega8.h>
#include "buttons.h"

#define MASK_BUTTONS 	(1<<BUTTON_1)|(1<<BUTTON_2)     //|(1<<BUTTON_3)|(1<<BUTTON_4)
#define ClearBit(reg, bit)       (reg) &= (~(1<<(bit)))
#define SetBit(reg, bit)         (reg) |= (1<<(bit))	
#define BitIsClear(reg, bit)     ((reg & (1<<(bit))) == 0)
#define BitIsSet(reg, bit)       ((reg & (1<<(bit))) != 0)

volatile unsigned char pressedKey = 0;
unsigned char comp = 0;

//������������� ������
void BUT_Init(void)
{
  DDRX_BUTTON &= ~(MASK_BUTTONS); 
  PORT_BUTTON |= MASK_BUTTONS;
}

//����� ���������� � �������������
void BUT_Debrief(void)
{
unsigned char key;

  //���������������� ����� ������� ��
  if (BitIsClear(PIN_BUTTON, BUTTON_1))     
    key = KEY_BUTTON_1;
  else if (BitIsClear(PIN_BUTTON, BUTTON_2))    
    key = KEY_BUTTON_2;
  else if (BitIsClear(PIN_BUTTON, BUTTON_3))        
    key = KEY_BUTTON_3;        
  else if (BitIsClear(PIN_BUTTON, BUTTON_4))      
    key = KEY_BUTTON_4;
  else {
    key = KEY_NULL;
  }

  //���� �� ��������� ���������� ���-�� ����
  if (key) {
  
    //� ���� ������ ������������ �����
	//�������� �� ����� � ����� 
    if (comp == THRESHOLD) {
	  comp = THRESHOLD+10; 
      pressedKey = key;
      return;
    }
	else if (comp < (THRESHOLD+5)) comp++;
	
  } 
  else comp=0;
}

//������ ���������� � ����� ������ � ��������� ����������
unsigned char BUT_GetKey(void)
{
  unsigned char key = pressedKey;
  pressedKey = KEY_NULL;
  return key;
}


//������ ���� ������ � �����
void BUT_SetKey(unsigned char key)
{
    pressedKey = key;
}

