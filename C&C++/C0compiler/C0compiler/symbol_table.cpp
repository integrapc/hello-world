#include "lexical_analyzer.h"
#include "headeclare.h"
#include "grammar_semantic.h"
#include "symbol_table.h"

extern int lineNo;
extern int err;
extern int c;
extern rebound reb,reb1,reb2;				//�ʷ�����ȡ���������ʴ�Ŵ�
extern int line1,line2;						//���������reb1��reb2���к�
extern int curLine;							// ��ǰ�������ڵ��к�
extern symtable Table[SYMTOP];					// ���ű�
extern int Top;								// ���ű�ջ��ָ��
extern int level;								// ���
extern int subIndex[SYMTOP];					// �ֳ���������
extern FILE *outfp;
extern int label;								// ������


void tableinsert(char n[],int t,int kind)			// ������������ű���
{
	if (Top>=SYMTOP)
	{
		Error(curLine,"Symbol table overflow!");
	}
	if (Isintable(n))
	{
		Error(curLine,"Identifer redefined!");
	}
	Table[Top].kind=kind;
	Table[Top].lev=level;
	Table[Top].type=t-TYPEOFF;
	strcpy(Table[Top].name,n);
	Top++;
	if (kind==PROC || kind==FUNC)
		subIndex[++level]=Top;				
}

bool Isintable(char n[])					// �鿴Ҫ������ű�ı�ʶ���Ƿ��ڷ��ű����Ѿ�����
{
	int i;
	i=Top;
	do
	{
		if(i==0)
			return false;
		i--;
		if (strcmp(Table[i].name,n)==0)
			return true;
	}while (Table[i].lev==level);	
	return false;
}

int Lookup(char n[],int &l,int &j)		// ����ű����ز�ź�ƫ��ֵ
{
	int k=Top-1;
	while (Table[k].lev==level)			// ������ű�
	{
		if (strcmp(n,Table[k].name)==0)
		{
			l=level;
			j=k-subIndex[l]+1;
			return 1;
		}
		k--;
	}
	k=0;
	while (Table[k].lev==0)				// ȫ�ַ��ű�
	{
		if (strcmp(n,Table[k].name)==0)
		{
			l=0;
			j=k+1;
			return 1;
		}
		k++;
	}
	return -1;
}
int Lookup(char n[])
{
	int k=Top-1;
	while (Table[k].lev==level)			// ������ű�
	{
		if (strcmp(n,Table[k].name)==0)
		{
			return Table[k].kind;
		}
		k--;
	}
	k=0;
	while (Table[k].lev==0)				// ȫ�ַ��ű�
	{
		if (strcmp(n,Table[k].name)==0)
		{
			return Table[k].kind;
		}
		k++;
	}
	return -1;
}

bool Lookuproc(char n[],int &i,int &z)		// ���ݺ������ֲ���ű������ط��ű���λ�úͲ�������
{
	int k=0;
	while (k<Top)
	{
		if (strcmp(n,Table[k].name)==0)
		{
			i=k;
			while (Table[++k].kind==PARA)
				;
			z=k-i-1;
			return true;
		}
		k++;		
	}
	return false;
}

bool Lookuproc(char n[],int &type)		// ���ݺ������ֲ���ű����غ�������ֵ����
{
	int k=0;
	while (k<Top)
	{
		if (strcmp(n,Table[k].name)==0)
		{
			type = Table[k].type;
			return true;
		}
		k++;		
	}
	return false;
}
void LOAD(int c)			// ��������ѹ������ջ
{
	Print("LOADI",c);
}

void LOAD(float c)		// ��������ѹ������ջ
{
	Print("LOADI",c);
}

void LOAD(char c)			// ���ַ�ѹ������ջ
{
	Print("LOADI",c);
}

void STO(int t,int s,int l,int j)			// ������ջ�е����ݷ�������ջ
{
	if ((t==INTEGER || t==CHAR) && s==FLOAT)
		Print("CONVERI $Top");
	if (t==FLOAT && (s==INTEGER || s==CHAR))
		Print("CONVERF $Top");
	Print("STO",l,j);
}

void Labelproduce(char lab[])				// �������
{
	char *p=new char[4];
	strcpy(lab,"\0");
	itoa(label,p,10);
	strcat(lab,p);
	label++;
}

void Labelinsert(char s[])				// �������������
{
	if(err!=0)
		return;
	char ss[STRINGLENGTH+1];
	strcpy(ss,s);
	strcat(ss,": ");
	fprintf(outfp,"%s\n",ss);
}

void allocspace(int k)					// Ϊ��������洢�ռ�
{
	int i;
	if (k!=0)
		Print("ALLOCATE",k);				// ���淵�ص�ַ
	Print("STO",level,1-NIL);
	for (i=k;i>0;i--)
		Print("STO",level,i);				// �������ֵ
}
void QuitTable()						// ����������ʱ�˷��ű�
{
	int lv;
	lv=level;
	while(Top>=0 && Table[Top].lev==lv)
	{
		Top--;
	}
	Top++;
	while(Table[Top].kind==PARA)
	{
		strcpy(Table[Top].name,"\0");
		Table[Top].lev=lv-1;
		Top++;
	}
	level--;
}

void Type(int l,int j,int &t1)		// ͨ��������ر�ʶ��������
{
	t1=Table[subIndex[l]+j-1].type;
}

void conjump(int t1,int t2,int t)		// ������������е�����������ת�ƴ���
{
	if (t1==FLOAT && t2==INTEGER)
		Print("CONVERF","$top");
	if (t1==INTEGER && t2==FLOAT)
		Print("CONVERF","$top-1");
	switch (t)
	{
	case EQUAL:  Print("EQ"); break;
	case UNEQL: Print("NOTEQ"); break;
	case LESSTHAN:  Print("LES"); break;
	case MORETHAN:  Print("GT"); break;
	case NLESSTHAN:  Print("LE"); break;
	case NMORETHAN:  Print("GE"); break;
	}
}

void Write(int t)									// ����������
{
	switch(t)
	{
	case INTEGER: Print("PRINTI"); break;
	case FLOAT: Print("PRINTF"); break;
	case CHAR: Print("PRINTC"); break;
	}
}

void Opr(int &s1,int s2,int tp)			// �� �� �� ��(��ͬ���������͵�ת��)
{
	if (s1==FLOAT && (s2==INTEGER || s2==CHAR))
	{
		Print("CONVERF $top-1");
		s1=FLOAT;
	}
	if ((s1==INTEGER || s1==CHAR) && s2==FLOAT)
	{
		Print("CONVERF $top");
		s1=FLOAT;
	}
	if (s1!=FLOAT && s2!=FLOAT)
		s1=INTEGER;
	switch (tp)
	{
	case PLUS:  Print("ADD");
		break;
	case MINUS: Print("SUB");
		break;
	case MUL: Print("MULT");
		break;
	case DIV:  Print("DIV");
	}		
}

void Upcnt(int j,int &k)					// ͳ�Ʋ�������
{
	k=++j;
}

bool Chktype(int t,int i,int &m,int &z)		// ��麯������������ͣ����������ʱ�������͸����Ƿ�ƥ��
{
	if (z<1)
	{
		Error(curLine,"Incorrect number of parameters!");
		return false;
	}
	m++;
	if (t==INTEGER && Table[i+m].kind==FLOAT)
	{
		Error(curLine,"Incorrect parameter type!");
		return false;
	}
	z--;
	return true;
}

bool ChkLength(int z)							// ��麯���βθ�����ʵ�θ����Ƿ����
{
	if (z!=0)
	{
		Error(curLine,"Incorrect number of parameters!");
		return false;
	}
	return true;
}
void Print(char p[])
{
	fprintf(outfp,"   %s\n",p);
}
void Print(char p[],int i)
{
	fprintf(outfp,"   %s %d\n",p,i);
}

void Print(char p[],int i,int j)
{
	fprintf(outfp,"   %s <%d,%d>\n",p,i,j+NIL);
}

void Print(char p[],float f)
{
	fprintf(outfp,"   %s %f\n",p,f);
}

void Print(char p[],char s[])
{
	fprintf(outfp,"   %s %s\n",p,s);
}