#define		SYMTOP				10001

#define		CONSTANT			1001				
#define     VAR					1002
#define     PROC				1003
#define     FUNC				1004
#define		PARA				1005

#define     TYPEOFF				107         // ���ͱ�ʶ����ʵ�����Ͷ���ֵ�Ĳ�
#define     NIL					3			// ������ʽ��������С

struct symtable							// ���ű�ṹ
{
	char name[STRINGLENGTH+1];			// ����
	int type;							// ����
	int kind;							// ����
	char cval;							// �����ַ������ڴ�
	int ival;							// ���������������ڴ�
	float fval;							// ���Ǹ��������ڴ�
	int lev;							// ���
};

