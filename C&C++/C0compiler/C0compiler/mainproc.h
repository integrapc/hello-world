FILE*infp;
FILE*outfp;

int lineNo;
int err;
int c;

rebound reb,reb1,reb2;					// �ʷ�����ȡ���������ʴ�Ŵ�
int line1,line2;						// ���������reb1��reb2���к�
int curLine;							// ��ǰ�������ڵ��к�

symtable Table[SYMTOP];					// ���ű�
int Top;								// ���ű�ջ��ָ��
int level;								// ���
int subIndex[2];						// �ֳ���������

int label;								// ������

void maininitial()
{
	lineNo=1;
	err=0;
	Top=0;							
	level=0;							
	subIndex[0]=0;				
	label=0;		
	GETCHAR();
	reb=lex();
	reb1=lex();
	reb2=lex();
}