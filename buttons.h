//***************************************************************************
//
//  Description.: ������� 4-�� ���������� ���������
//
//  Data........: 25.10.09
//
//***************************************************************************
#ifndef	BUTTONS_h
#define	BUTTONS_h

//#include <ioavr.h>

//����, � �������� ���������� ������
#define PORT_BUTTON 	PORTB
#define PIN_BUTTON 	    PINB
#define DDRX_BUTTON 	DDRB

//������ �������, � ������� ���������� ������
#define BUTTON_1 		3
#define BUTTON_2 		4
#define BUTTON_3		6
#define BUTTON_4 		4

//����, ������� ����� ������������ � �����
#define KEY_NULL      0
#define KEY_BUTTON_1  1
#define KEY_BUTTON_2  2
#define KEY_BUTTON_3  3
#define KEY_BUTTON_4  4

//������� ������ ������ ������ ������ ������������
#define THRESHOLD 5

#pragma used+
/**************************************************************************
*   Function name : BUT_Init
*   Returns :       ���
*   Parameters :    ���
*   Purpose :       ������������� ������ �����/������
*                   ���������� ������ � ������ main`a
****************************************************************************/
void BUT_Init(void);

/**************************************************************************
*   Function name : BUT_Debrief
*   Returns :       ���
*   Parameters :    ���
*   Purpose :       ���������� ������. ���������� ������ �� ����������
*                   ���� ������ ������ � ������� 20 ����������,
*                   �� ����� ������������ � �����
****************************************************************************/
void BUT_Debrief(void);

/**************************************************************************
*   Function name : BUT_GetKey
*   Returns :       ����� ������� ������
*   Parameters :    ���
*   Purpose :       ���������� ���������� ���������� ������
*                   ��� ���� ����� ���������
*                   ���������� ������ � main`e � ����� while
*
****************************************************************************/
unsigned char BUT_GetKey(void);


/**************************************************************************
*   Function name : BUT_Init
*   Returns :       ���
*   Parameters :    ����� ������
*   Purpose :       ���������� � ��������� ����� ��������
*                   ��������� ������ ��� �������� ������� ������
****************************************************************************/
void BUT_SetKey(unsigned char key);
#pragma used-

#endif //BUTTONS_H
