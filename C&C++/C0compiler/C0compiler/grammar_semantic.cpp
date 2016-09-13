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

//������Program�� ::=�ۣ�����˵�����֣��ݣۣ�����˵�����֣���{���з���ֵ�������岿�֣�|���޷���ֵ�������岿�֣�}����������
void Program()
{
	if (reb.type==CONST)
		Constdec();
	if (istype(reb.type) && (reb2.type!=LEFTP))
		Vardec();		
	Print("JSR","main");//��ת�����������
	while (istype(reb.type) && reb2.type==LEFTP||reb.type==VOIDSYM && reb1.type==IDENTF)				
	{
		if(reb.type==VOIDSYM)// �޷���ֵ��������
			Procdef();
		else Funcdef();// �з���ֵ��������
	}			
	if (reb.type==VOIDSYM && reb1.type==MAINSYM)
		Mainproc();
	else
		Error(curLine,"Lack of main function!");
}
//������˵������Constdec�� ::=  const���������壾;{ const���������壾;}
void Constdec()
{
	do
	{
		READ();
		Constdef();
		if (reb.type!=SEMICOLON)
		{
			Error(curLine,"Lack of ';'!");
			return;
		}
		READ();
	}while (reb.type==CONST);
}
/*����������Constdef��::=
int����ʶ��������������{,����ʶ��������������} 
| float����ʶ��������ʵ����{,����ʶ��������ʵ����}
| char����ʶ���������ַ���{,����ʶ���������ַ���}*/
void Constdef()
{
	int tp; 
	char n[STRINGLENGTH+1]; 
	int s; 
	int l,j;
	rebound c;
	tp=reb.type;
	if (!istype(reb.type))
	{
		Error(curLine,"unrecognized type!");
		ignorance(SEMICOLON);
		return;
	}
	do
	{
		READ();
		if (reb.type!=IDENTF)
			Error(curLine,"Lack of identifer in const definition!");
		else
		{
			strcpy(n,reb.Str);
			READ();
		}
		if (reb.type!=EVALUE)
		{
			Error(curLine,"The const is used without being initialized!");
			ignorance(SEMICOLON);
			return;
		}
		READ();
		copystruct(c,reb);
		Print("ALLOCATE",1);
		switch (tp)
		{
			case INTSYM: 
				Integer(s,c.intval);
				tableinsert(n,tp,CONSTANT);
				LOAD(c.intval);
				break;
			case FLOATSYM: 
				Float(s,c.realval);
				tableinsert(n,tp,CONSTANT);
				LOAD(c.realval);
				break;
			case CHARSYM: 
				Char(s,c.ch);
				tableinsert(n,tp,CONSTANT);
				LOAD(c.ch);
				break;
		}
		Lookup(n,l,j);
		STO(tp,s,l,j);
	}while (reb.type==COMMA);
}
//������˵������Vardec��::=<��������>;{<��������>;}
void Vardec()
{
	do{
		Vardef();
		if (reb.type!=SEMICOLON)
		{
			Error(curLine,"Lack of ';'!");
			ignorance(SEMICOLON);
			continue;
		}
		READ();
	}while(istype(reb.type) && reb2.type!=LEFTP);
}
//����������Vardef��::=�����ͱ�ʶ��������ʶ����{,����ʶ���� }
void Vardef()
{
	int t; 
	char n[STRINGLENGTH+1];
	if (!istype(reb.type))
	{
		Error(curLine,"Illegal type defined!");
		return;
	}
	t=reb.type;
	do{
		READ();
		if (reb.type!=IDENTF)
		{
			Error(curLine,"Lack of identifer!");
			ignorance(SEMICOLON);
			break;
		}
		strcpy(n,reb.Str);
		tableinsert(n,t,VAR);
		Print("ALLOCATE",1);
		READ();
	}while(reb.type==COMMA);
}
/*���з���ֵ�������岿��Funcdef��::= ������ͷ����'(' ��������')'
 '{' ��������䣾'}' */
void Funcdef()
{
	int t;
	int j,k;
	char n[STRINGLENGTH+1];
	if (!Dechead(t,n))
	{
		ignorance(SEMICOLON);
		goto LDComsent;
	}
	Labelinsert(n);
	tableinsert(n,t,FUNC);
	j=0;
	if (reb.type!=LEFTP)
	{
		Error(curLine,"Lack of '('!");
		ignorance(SEMICOLON);
		goto LDComsent;
	}
	READ();
	Parameter(j,k);
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
	{
		allocspace(k);
		READ();
	}
	if (reb.type!=LBRACKET) 
		Error(curLine,"Lack of '{'!");
	else 
		READ();
LDComsent:	
	Comsentence();
	if (reb.type!=RBRACKET)
	{
		Error(curLine,"Lack of '}'!");
		return;
	}
	QuitTable();
	READ();
}
//������ͷ��DeclHead��::=  int����ʶ����|float ����ʶ���� |char����ʶ����
bool Dechead(int &t,char n[])
{
	if (!istype(reb.type))
		Error(curLine,"Unrecognised type!");
	t=reb.type;
	READ();
	if (reb.type!=IDENTF)
	{
		Error(curLine,"Lack of identifer!");
		return false;
	}
	strcpy(n,reb.Str);
	READ();
	return true;
}
//���޷���ֵ�������岿��Procdef��::=void<��ʶ��> '(' <����> ��n ')' '{'  <�������> '}' 
void Procdef()
{
	int t;
	int j,k;
	char n[STRINGLENGTH+1];										
	t=NUL;
	READ();
	strcpy(n,reb.Str);
	Labelinsert(n);
	tableinsert(n,t,PROC);
	j=0;
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	READ();
	Parameter(j,k);
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
	{
		allocspace(k);
		READ();
	}
	if (reb.type!=LBRACKET)
		Error(curLine,"Lack of '{'!");
	else
		READ();
	Comsentence();
	if (reb.type!=RBRACKET)
	{
		Error(curLine,"Lack of '}'!");
		return;
	}
	QuitTable();
	Print("RETURN");
	READ();
}
//��������Mainproc��::=void main  '(' ')' '{' ��������䣾 '}' 
void Mainproc()
{
	char n[STRINGLENGTH+1];
	int t;
	int j,k;
	if (reb.type!=VOIDSYM)
		Error(curLine,"The type of the main procedure should be void!");
	t=NUL;
	READ();                                
	strcpy(n,reb.Str);
	Labelinsert(n);
	Print("LOADI",-1);
	tableinsert(n,t,PROC);
	j=0;
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	else
		READ();
	Parameter(j,k);
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
	{
		allocspace(k);
		READ();
	}
	if (reb.type!=LBRACKET)
		Error(curLine,"Lack of '{'!");
	else
		READ();
	Comsentence();
	QuitTable();
	Print("RETURN");
	if (reb.type!=RBRACKET)
		Error(curLine,"Lack of '}'!");
	READ();
}
//���������Comsentence��::=�ۣ�����˵�����֣��ݣۣ�����˵�����֣��ݣ�����У�
void Comsentence()
{
	if (reb.type==CONST)
		Constdec();
	if (istype(reb.type))
		Vardec();
	if (isfirsts(reb.type))
		Sentencelist();
}
//�������Sentencelist��::=����䣾������䣾��
void Sentencelist()
{
	do
	{
		if (reb.type!=SEMICOLON)
			Sentence();
		else
			READ();
	}while (isfirsts(reb.type) || reb.type==SEMICOLON);
}
/*�����Sentence�� ::=��������䣾����ѭ����䣾��'{'<�����>'}'
�����з���ֵ����������䣾; |���޷���ֵ����������䣾;
������ֵ��䣾;��������䣾;����д��䣾;�����գ�;����������䣾;*/
void Sentence()
{
	if (reb.type==IF)
		Ifsent();
	else if (reb.type==WHILE||reb.type==FOR)
		Loopsent();
	else if (reb.type==SCANF)
	{
		Readsent();
		if (reb.type!=SEMICOLON)
			Error(curLine,"Lack of ';'!");
		READ();
	}
	else if (reb.type==PRINTF)
	{
		Writesent();
	    if (reb.type!=SEMICOLON)
			Error(curLine,"Lack of ';'!");
		READ();
	}
	else if (reb.type==IDENTF && reb1.type==LEFTP)
	{
		if(Lookup(reb.Str)==FUNC)
		{
			Funccall();
		}
		else Proccall();
		if (reb.type!=SEMICOLON)
			Error(curLine,"Lack of ';'!");
		else
			READ();
	}
	else if (reb.type==RETURN)
	{
		Returnsent();
		if (reb.type!=SEMICOLON)
			Error(curLine,"Lack of ';'!");
		READ();
	}
	else if (reb.type==IDENTF && reb1.type==EVALUE)
	{
		Assignsent();
		if (reb.type!=SEMICOLON)
			Error(curLine,"Lack of ';'!");
		READ();
	}
	else if (reb.type==LBRACKET)
	{
		READ();
		Sentencelist();
		if (reb.type!=RBRACKET)
			Error(curLine,"Lack of '}'!");
		READ();
	}
	else
	{
		if (reb.type!=';')
			Error(curLine,"Lack of ';'!");
		READ();
	}
}
//����ֵ���Assignsent��::=����ʶ�����������ʽ��
void Assignsent()
{
	int s;
	int l,j,t;
	if(Lookup(reb.Str,l,j)!=1)
	{
		tableinsert(reb.Str,reb.type,VAR);
		Error(curLine,"Undefined identifer!");
	}
	Type(l,j,t);
	if (Table[subIndex[l]+j-1].kind==CONSTANT)
			Error(curLine,"Constant can't in the left of equation");
	READ();
	if (reb.type!=EVALUE)
	{
		Error(curLine,"Lack of '=' in assign sentence!");
		ignorance(SEMICOLON);
		return;
	}
	READ();
	Expression(s);
	STO(t,s,l,j);
}
//���������Ifsent��::=if '('��������')' ����䣾��else ����䣾��
void Ifsent()
{
	char y[STRINGLENGTH+1],z[STRINGLENGTH+1];
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	else
		READ();
	Condition();
	Labelproduce(y);
	Print("BRF",y);
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
		READ();
	Sentence();
	if (reb.type!=ELSE)
		Labelinsert(y);
	else{
		Labelproduce(z);
		Print("BR",z);
		Labelinsert(y);
		READ();
		Sentence();
		Labelinsert(z);
	}
}
/*������Condition��::=�����ʽ������ϵ������������ʽ��
�������ʽ��
���ʽΪ0����Ϊ�٣�����Ϊ��*/
void Condition()
{
	int t;
	int t1,t2;
	Expression(t1);
	t=reb.type;
	if (t==LESSTHAN || t==NLESSTHAN || t==MORETHAN || t==NMORETHAN|| t==EQUAL || t==UNEQL)
	{
		READ();
		Expression(t2);
		conjump(t1,t2,t);
		return;
	}
}
//��ѭ�����Loopsent��::=while  '('�������� ')' ����䣾
//| for'(' ����ʶ�����������ʽ���� <����>������ʶ����������ʶ���� (+|-)<����>')'<���>
void Loopsent()
{
	char r[STRINGLENGTH+1],f[STRINGLENGTH+1];
	int l,j,op;
	int l1,l2,j1,j2,stepvalue;
//while  '('�������� ')' ����䣾
	if(reb.type==WHILE)
	{

		Labelproduce(r);
		Labelinsert(r);
		READ();
		if (reb.type!=LEFTP)
		{
			Error(curLine,"Lack of '('!");
			ignorance(SEMICOLON);
			goto LOSentence;
		}
		READ();
		Condition();
		Labelproduce(f);
		Print("BRF",f);
		if (reb.type!=RIGHTP)
		{
			Error(curLine,"Lack of ')'!");
			ignorance(SEMICOLON);
			goto LOSentence;
		}
		READ();
	LOSentence:
		Sentence();
		Print("BR",r);
		Labelinsert(f);
	}
//for'('����ʶ�����������ʽ��;��������;����ʶ����������ʶ����(+|-)��������')'����䣾
	else 
	{
		READ();
		if (reb.type!=LEFTP)
		{
			Error(curLine,"Lack of '('!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		READ();
		if (reb.type!=IDENTF)
		{
			Error(curLine,"There should be an identfer!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		Assignsent();
		if (reb.type!=SEMICOLON)
		{
			Error(curLine,"Lack of ';'!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		Labelproduce(r);
		Labelinsert(r);
		READ();
		Condition();
		Labelproduce(f);
		Print("BRF",f);
		if (reb.type!=SEMICOLON)
		{
			Error(curLine,"Lack of ';'!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		READ();	
		if (reb.type!=IDENTF)
		{
			Error(curLine,"There should be an identfer!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		else
		{
			if(!Lookup(reb.Str,l,j))
			{
				Error(curLine,"Undefined identifer!");
				ignorance(RIGHTP);
				goto LSentence;
			}
			l1=l;
			j1=j;
			READ();
		}
		if (reb.type!=EVALUE)
		{
			Error(curLine,"Lack of '='!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		READ();
		if (reb.type!=IDENTF)
		{
			Error(curLine,"There should be an identfer!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		else
		{
			if(!Lookup(reb.Str,l,j))
			{
				Error(curLine,"Undefined identifer!");
				ignorance(RIGHTP);
				goto LSentence;
			}
			l2=l;
			j2=j;
			
			READ();
		}
		if(reb.type==PLUS||reb.type==MINUS)
		{
			if(reb.type==PLUS)
				op=0;
			else
				op=1;
		}
		else
		{
			Error(curLine,"Lack of '+' or '-'!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		READ();
		if(reb.type!=INTEGER||reb.intval==0)
		{
			Error(curLine,"Step error!");
			ignorance(RIGHTP);
			goto LSentence;
		}
		else
		{
			stepvalue=reb.intval;
			
			READ();
		}
		if (reb.type!=RIGHTP)
		{
			Error(curLine,"Lack of ')'!");
			ignorance(SEMICOLON);
			goto LSentence;
		}
		READ();
	LSentence:
		Sentence();
		Print("LOAD",l2,j2);
		Print("LOADI",stepvalue);	
		if(op==0)
			Print("ADD");
		else
			Print("SUB");
		Print("STO",l1,j1);
		Print("BR",r);
		Labelinsert(f);
	}
}
//���з���ֵ�����������Funccall��::=����ʶ���� '('��ֵ������ ')'
void Funccall()
{
	char n[STRINGLENGTH+1],y[STRINGLENGTH+1];
	int i,z;
	strcpy(n,reb.Str);
	if (!Lookuproc(n,i,z))
		Error(curLine,"Undefined function!");
	Labelproduce(y);
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	else
	{
		READ();
		Paralist(i,z);
	}
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
	{
		Print("LOADI",atoi(y));
		Print("JSR",n);
		Labelinsert(y);
		READ();
	}
}
//���޷���ֵ�����������Proccall��::=����ʶ����'('��ֵ������ ')' 
void Proccall()
{
	char n[STRINGLENGTH+1],y[STRINGLENGTH+1];
	int i,z;
	strcpy(n,reb.Str);
	if (!Lookuproc(n,i,z))
		Error(curLine,"Undefined function!");
	Labelproduce(y);
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	else
	{
		READ();
		Paralist(i,z);
	}
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
	{
		Print("LOADI",atoi(y));
		Print("JSR",n);
		Labelinsert(y);
		READ();
	}
}
//�������Readsent��::=scanf '('����ʶ���� {������ʶ����} ')'
void Readsent()
{
	char n[STRINGLENGTH+1];
	int l,j;
	int type;
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	do
	{
		READ();
		if (reb.type!=IDENTF)
			Error(curLine,"Lack of identifer!");
		else
		{
			strcpy(n,reb.Str);
			Lookup(n,l,j);
			Type(l,j,type);
			Print("READ");
			if(type == INTEGER ||type == CHAR)
				Print("CONVERI","$Top");
			Print("STO",l,j);
			READ();
		}
	}while(reb.type==COMMA);
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else READ();
}
//��д���Writesent��::=printf '(' <�ַ���>,<���ʽ> ')'
//|printf '(' <�ַ���> ')'|printf '('<���ʽ> ')'
void Writesent()
{
	char n[STRINGLENGTH+1];
	int t;
	READ();
	if (reb.type!=LEFTP)
		Error(curLine,"Lack of '('!");
	else
		READ();
	if (reb.type==STRING)
	{
		strcpy(n,reb.Str);
		Print("PRINTS",n);
		READ();
		if (reb.type==RIGHTP)
			goto LDCjump;
		else if(reb.type!=COMMA)
			Error(curLine,"Lack of ','!");
		else
			READ();
	}
	if (isfirste(reb.type)||reb.type==LEFTP)
	{
		Expression(t);
		Write(t);
	}
LDCjump:
	if (reb.type!=RIGHTP)
		Error(curLine,"Lack of ')'!");
	else
		READ();
}
//���������Returnsent��::=return['('�����ʽ��')']
void Returnsent()
{
	int t;
	READ();
	if (reb.type==LEFTP)
	{
		READ();
		if (isfirste(reb.type))
		{
			Expression(t);
		}
		if (reb.type!=RIGHTP)
			Error(curLine,"Lack of ')'!");
		else
			READ();
	}
	Print("RETURN");
}
//�����ʽExpression��::=�ۣ������ݣ��{���ӷ�����������}
void Expression(int &t)
{
	int s1,s2;
	int flag=0;
	bool isMinus=false;							// ���ʽ�Ƿ���и���
	if (reb.type==MINUS)
	{
		Print("LOADI",-1);//�˴�load-1Ҫ��ջ��ת��
		isMinus=true;
	}
	if (reb.type==PLUS || reb.type==MINUS)	
		READ();
	Term(s1);
	while(1)
	{
		if (reb.type!=PLUS && reb.type!=MINUS)
			break;
		flag=reb.type;//flag�д���������
		READ();
		Term(s2);
		Opr(s1,s2,flag);
	}
	if (isMinus)
		Opr(s1,INTEGER,MUL);
	t=s1;
}
//����Term��::=�����ӣ� {���˷�������������ӣ�}
void Term(int &t)
{
	int s1,s2;
	int flag;
	Factor(s1);
	while(1)
	{
		if (reb.type!=MUL && reb.type!=DIV)
			break;
		flag=reb.type;
		READ();
		Factor(s2);
		Opr(s1,s2,flag);
	}
	t=s1;
}
//������Factor��::=����ʶ������'('�����ʽ��')'���������������з���ֵ����������䣾|<ʵ��>|���ַ���
void Factor(int &t)
{
	int t1;
	int j,l;
	rebound c;
	int op=1,flag=0;
	char n[STRINGLENGTH+1];
	if (reb.type==IDENTF && reb1.type!=LEFTP)		// �����ʶ��
	{
		strcpy(n,reb.Str);
		if (Lookup(n,l,j)==-1)							// ���ű����޶���
			Error(curLine,"Undefined identifer!");
		else
		{
			Type(l,j,t1);
			Print("LOAD",l,j);							// ������ѹջ
		}
		READ();
		copystruct(c,reb);
	}
	
	else if (reb.type==PLUS || reb.type==MINUS)
	{
		while(reb1.type==PLUS || reb1.type==MINUS)
		{
			flag=1;
			if(reb.type==PLUS)
				op*=1;
			else op*=-1;
			READ();
		}
		if(op!=1)
			reb.type = MINUS;
		if(reb.type==INTEGER)
		{
			Integer(t1,c.intval);
			Print("LOADI",c.intval);
		}
		else if(reb.type==FLOAT)
		{
			Float(t1,c.realval);
			Print("LOADI",c.realval);
		}
	}
	else if(reb.type==INTEGER)
	{
		Integer(t1,c.intval);
		Print("LOADI",c.intval);
	}
	else if(reb.type==FLOAT)
	{
		Float(t1,c.realval);
		Print("LOADI",c.realval);
	}
	else if (reb.type==CHAR)
	{
		Char(t1,c.ch);
		Print("LOADI",c.ch);
	}
	else if (reb.type==IDENTF && reb1.type==LEFTP)
	{
		Lookuproc(reb.Str,t1);
		Funccall();
	}
	else if (reb.type==LEFTP)
	{
		READ();
		Expression(t1);
		if (reb.type!=RIGHTP)
			Error(curLine,"Lack of ')'!");
		else
			READ();
	}
	else
		Error(curLine,"Illegal Factor!");
	t=t1;
}
//������Parameter��::=��������
void Parameter(int j,int &k)
{
	Paratable(j,k);
}
//��������ParaTable��::=�����ͱ�ʶ��������ʶ����{�������ͱ�ʶ��������ʶ���� }| <��>
void Paratable(int j,int &k)
{
	int t; 
	char n[STRINGLENGTH+1];
	while(istype(reb.type))
	{
		t=reb.type;
		READ();
		if (reb.type!=IDENTF)
			Error(curLine,"Lack of identfer!");
		else
		{
			strcpy(n,reb.Str);
			tableinsert(n,t,PARA);
			Upcnt(j,k); 
			j=k;
			READ();
		}
		if (reb.type!=COMMA)
			break;
		READ();
		if (!istype(reb.type))
			Error(curLine,"Unrecognized type!");
	}
	k=j;
}
//��ֵ������ParaList��::=�����ʽ�� {�������ʽ�� }�����գ�
void Paralist(int i,int z)
{
	int t;
	int m=0;
	while(isfirste(reb.type))
	{
		Expression(t);
		Chktype(t,i,m,z);
		if (reb.type!=COMMA)
			break;
		READ();
		if (!isfirste(reb.type))
			Error(curLine,"Illegal Expression!");
	}
	ChkLength(z);
}
//������Integer��::= �ۣ������ݣ��������֣��������֣�����0
void Integer(int &s,int &c)
{
	s=INTEGER;
	c=1;
	if (reb.type==PLUS || reb.type==MINUS)
	{
		if (reb.type==MINUS)
			c=-1;
		READ();
	}
	if (reb.type==INTEGER)
	{
		c*=reb.intval;
		READ();
	}
	else
	{
		Error(curLine,"An Integer shuold be here!");
		if(reb.type == FLOAT)
			c*=(int)reb.realval;
		if(reb.type == CHAR)
			c=reb.ch;
		READ();
	}
}
//��ʵ��Real��::=�ۣ�������<����>.[��С�����֣�]
void Float(int &s,float &c)
{
	s=FLOAT;
	c=1;
	if (reb.type==PLUS || reb.type==MINUS)
	{
		if (reb.type==MINUS)
			c=-1;
		READ();
	}
	if (reb.type==FLOAT)
	{
		c*=reb.realval;
		READ();
	}
	else
	{
		Error(curLine,"A Float shuold be here!");
		if(reb.type == INTEGER)
			c*=reb.intval;
		if(reb.type == CHAR)
			c*=reb.ch;
		READ();
	}
}

void Char(int &s,char &c)
{
	s=CHAR;
	if(reb.type == CHAR)
	{
		c=reb.ch;
		READ();
	}
	else
	{
		Error(curLine,"A Char shuold be here!");
		if(reb.type == INTEGER)
			c*=reb.intval;
		if(reb.type == FLOAT)
			c*=(char)reb.realval;
		READ();
	}
}

bool istype(int t)						// �жϻ�ȡ�ĵ����Ƿ�Ϊ���ͱ�ʶ��
{								
	if (t==INTSYM || t==FLOATSYM || t==CHARSYM)
		return true;
	return false;
}

bool isfirsts(int t)					// �жϻ�ȡ���������Ƿ���<���>��First����
{
	int k;
	for (k=0;k<8;k++)
		if (t==firsts[k])
			return true;
	return false;
}

bool isfirste(int t)					// �жϻ�ȡ���������Ƿ���<���ʽ>��First����
{
	int k;
	for (k=0;k<6;k++)
		if (t==firste[k])
			return true;
	return false;
}

void ignorance(int i)					//����������������������ָ�ķ���ǰ��
{
	do
	{
		READ();
	}
	while(reb.type!=i);
	READ();
}

void READ()								//���뺯�������̽�����������ʼ����к�
{
	reb=reb1;
	reb1=reb2;
	reb2=lex();
	curLine=line1;
	line1=line2;
	line2=lineNo;
}

void copystruct(rebound &a,rebound &b)//������strcpy()�Ĺ��ܣ����������ṹ
{
	a.ch=b.ch;
	a.intval=b.intval;
	a.realval=b.realval;
	strcpy(a.Str,b.Str);
	a.type=b.type;
}
