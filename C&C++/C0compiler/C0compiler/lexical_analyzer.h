
#define		NUL					0			// null
#define		IDENTF				1			// identifer��ʶ��
#define		INTEGER				2			// ���� int
#define		FLOAT			    3		    // float
#define		CHAR				4			// �ַ� char
#define		STRING				5			// �ַ��� string

#define		PLUS				10			// +   ���
#define		MINUS				11			// -
#define		MUL			     	12			// *
#define		DIV			 	    13			// /
#define		LEFTP				14			// (
#define		RIGHTP				15			// )
#define		LBRACKET            16			// {
#define		RBRACKET			17			// }
#define		COMMA				18			// ,
#define     COLON               19          // :
#define     SEMICOLON			20			// ;
#define		EVALUE				21			// =
#define		UNEQL				22			// !=
#define		LESSTHAN			23			// <
#define		MORETHAN			24			// >
#define		NLESSTHAN			25			// <=
#define		NMORETHAN			26			// >=
#define		EQUAL				27			// ==
#define		QUOTATION			28			// "

#define		IF				100			// if     ������䱣����
#define		ELSE			101			// else
#define		WHILE			102			// while
#define		FOR				103			// for
#define		PRINTF			104			// printf
#define		SCANF			105			// scanf
#define		RETURN			106			// return
#define		CONST			107			// const

#define		VOIDSYM				108			// void  ��������
#define		INTSYM				109			// int
#define		FLOATSYM			110			// float
#define		CHARSYM				111			// char
#define		MAINSYM				112			// main
#define		OFFSET				100			// �����ֵ�ƫ��ֵ

#define     STRINGLENGTH        255          //�ַ�����󳤶�
#define		VARLENGTH			16		  	 // ����������󳤶�	
#define		INTLENGTH           2047483647	// ���������ֵ
#define		FLOATLENGTH      3.402823466E+38	// �����������

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

const char reserved[13][10]={"if","else","while","for","printf","scanf","return","const","void","int","float","char","main"};

struct rebound
{
	int type;                  //ȡ������
	char ch;				   //�����ַ�
	char Str[STRINGLENGTH];    //�����ַ���
	int intval;                //����������
	float realval;             //����ʵ��
};


