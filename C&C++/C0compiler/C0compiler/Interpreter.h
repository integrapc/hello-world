#include "lexical_analyzer.h"


#define DTOP				500			// ����ջջ��ֵ
#define CMAX                15          // ָ������ָ���
#define CNO					26			// ָ���ָ��ĸ���

struct sc
{
	int type;							
	struct// ��ʾv�����ݵ�����
	{						
		int ival;
		float fval;
	} v;
};

sc data[DTOP];							// ����ջ��ֻ������ݶ����ָ�루���ã�
int dptr;								// ����ջָ��
int abp;								// ���¼�Ļ���ַ
int preabp;								// ���û��¼�Ļ���ַ
char c;									// ����WRITES��䣬��ӡ�ַ���

sc run[DTOP];							// ����ջ
int rptr;								// ����ջջ��ָ��

FILE *fp;								// �ļ�ָ��

const char code[CNO][CMAX]={"ALLOCATE","STO","LOAD","LOADI","POP","JSR","RETURN",
							"ADD","SUB","MULT","DIV","EQ","NOTEQ","GT","LES","GE",
							"LE","BRF","BR","PRINTI","PRINTS","CONVERF","READ","PRINTF","PRINTC","CONVERI"};