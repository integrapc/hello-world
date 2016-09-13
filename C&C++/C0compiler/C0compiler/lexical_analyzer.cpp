#include"lexical_analyzer.h"
#include "headeclare.h"
extern int lineNo;
extern int c;				
extern FILE*infp;

void GETCHAR()
{

	static int line=1;					// ��ǰ�ַ����ڵ��к�
	lineNo=line;						// ����ĵ���ʵ�������к�(����Ԥ�����lineNo���ͺ���line)
	if (feof(infp))
		c=-1;
	else
	{
		c=fgetc(infp);	
		while ((c=='\n'))
		{
			line++;
			c=fgetc(infp);
		}
		
	}
}

int isreserved(char s[])//�Ƿ�Ϊ������
{
	int i;
	for (i=0;i<13;i++)
		if (strcmp(s,reserved[i])==0)
			return OFFSET+i;
	return -1;
}

rebound lex()//�ʷ�����
{
	int i,k=0;
	rebound re;
	re.Str[0]='\0';
	while(c==' '||c=='\t')
		GETCHAR();	
	if(isalpha(c)||c =='_')
	{
		re.Str[k++]=c;
		GETCHAR();
		while(isalpha(c)||isdigit(c)||c =='_')
		{
			if(k<VARLENGTH)
				re.Str[k++]=c;
			GETCHAR();
		}
		if (k>VARLENGTH)				// �жϱ�ʶ���Ƿ񳬹�ָ������
		{						
			Error(lineNo,"Identifer is too long!");
			re.Str[VARLENGTH]='\0';
		}
		else  re.Str[k]='\0';
		if((i=isreserved(re.Str))!=-1)
			re.type=i;
		else re.type=IDENTF;
	}
	else if(isdigit(c))
	{
		if(c!='0')
		{	while (isdigit(c))
			{
				if (k<STRINGLENGTH)
					re.Str[k++]=c;
				else
					k++;
				GETCHAR();
			}
		}
		else
		{
			GETCHAR();
			if(isdigit(c)||isalpha(c))
			{
				Error(lineNo,"Illegal number in this line!");
				while(isdigit(c)||isalpha(c))
					GETCHAR();
			}
			re.Str[k++]='0';
		}
		re.type=INTEGER;
		if (c=='.')
		{							// �ж��Ǹ�������������
			re.Str[k++]=c;
			GETCHAR();
			if(!isdigit(c))
				re.Str[k]='\0';
			while (isdigit(c))
			{
				if (k<STRINGLENGTH)
					re.Str[k++]=c;
				else 
					k++;
				GETCHAR();
			}
			re.type=FLOAT;
		}
		if (k<STRINGLENGTH)
		{
			if(re.Str[k-1]=='.')//���� 1.�� ��������
				re.Str[k-1]='\0';
			else re.Str[k]='\0';
		}
		else
			re.Str[STRINGLENGTH]='\0';

		if ((re.intval=atoi(re.Str))>INTLENGTH)		// �ж������Ƿ񳬳���Χ
			Error(lineNo,"The integer in your program is out of range!");
		if ((re.realval=(float)atof(re.Str))>FLOATLENGTH)			// �жϸ������Ƿ񳬳���Χ
			Error(lineNo,"The realnum in your program is out of range!");
	}
	else switch (c)
		{
		case '\'':							// �����ַ�����
			GETCHAR();
			re.ch=c;
			re.type=CHAR;
			GETCHAR();
			if (c!='\'')
				Error(lineNo," The char is lack of \' ");
			else
				GETCHAR();	
			break;
		case '"':
			GETCHAR();
			while (c!='"')
			{
				if (k<STRINGLENGTH)
					re.Str[k++]=c;
				else
					k++;
				GETCHAR();
			}
			if (c=='"') 
				GETCHAR();
			if (k<=STRINGLENGTH)							// �ж��ַ����Ƿ����
				re.Str[k]='\0';
			else
			{
				Error(lineNo,"The string is too long!");
				re.Str[STRINGLENGTH-1]='\0';
			}
			re.type=STRING;
			break;
		case '=':
			GETCHAR();
			if (c=='=')
			{
				re.type=EQUAL;
				GETCHAR();
			}
			else
				re.type=EVALUE;
			break;
		case '<':										// ���� <= �� <
			GETCHAR();
			if (c=='=')
			{
				re.type=NLESSTHAN;
				GETCHAR();
			}
			else
				re.type=LESSTHAN;
			break;
		case '>':							// ���� >= �� >
			GETCHAR();
			if (c=='=')
			{
				re.type=NMORETHAN;
				GETCHAR();
			}
			else
				re.type=MORETHAN;
			break;
		case '!':							// ���� !=
			GETCHAR();
			if (c=='=')
			{
				re.type=UNEQL;
				GETCHAR();
			}
			else
			{
				Error(lineNo,"Illegal words in this line");
				GETCHAR();
			}
			break;
		case '+': 
			re.type=PLUS;
			GETCHAR(); 
			break;
		case '-': 
			re.type=MINUS; 
			GETCHAR();
			break;
		case '*': 
			re.type=MUL;
			GETCHAR();
			break;
		case '/':
			re.type=DIV; 
			GETCHAR();
			break;
		case '(':
			re.type=LEFTP;
			GETCHAR(); 
			break;
		case ')': 
			re.type=RIGHTP;
			GETCHAR();
			break;
		case '{': 
			re.type=LBRACKET;
			GETCHAR();
			break;
		case '}':
			re.type=RBRACKET;
			GETCHAR();
			break;
		case ',': 
			re.type=COMMA;	
			GETCHAR();
			break;
		case ';':
			re.type=SEMICOLON;
			GETCHAR();
			break;
		case ':':
			re.type=COLON; 
			GETCHAR();
			break;
		case  -1:
			re.type=-1;
			GETCHAR();
			break;
		default: 
			Error(lineNo,"Your character is beyond the grammar");
			GETCHAR();
	   }
	   if(re.type == IDENTF)//��ʶ�������ִ�Сд
	   {
		   for(int x=0;re.Str[x]!='\0';x++)
		   {
			   if(re.Str[x]>='a'&&re.Str[x]<='z')
				   re.Str[x]-='a'-'A';
		   }
	   }
	return re;
}





