#include "lexical_analyzer.h"
#include "headeclare.h"
#include "grammar_semantic.h"
#include "symbol_table.h"
#include "mainproc.h"

void main()
{
	char s[VARLENGTH+1];
	int ch;
	printf("����������Դ�ļ��ļ���:\n");
	scanf("%s",s);
	if ((infp=fopen(s,"r"))==NULL)
	{
		printf("�����ڸ��ļ�\t����ļ����ٳ����ļ�����\n");
		return;
	}
	printf("��������Ҫ������ļ��ļ���:\n");
	scanf("%s",s);
	if ((outfp=fopen(s,"w"))==NULL)
	{
		printf("�ļ���������\n");
		return;
	}
	maininitial();
	Program();
	printf("Total %d errors!\n",err);
	if(err==0)
		printf("����ɹ�!\n\nP-CODE�����:%s\n\n\n�Ƿ�Ҫ���г���(y/n):\n\n",s);
	else goto L0;
	while((ch = getchar())=='\n')
		;
	if(ch!='y')
		return;
	fclose(infp);
	fclose(outfp);
	printf("\n\n�������н������:\n\n");
	if(err==0)
		Interpreter(s);
L0:
	printf("\n\n�밴���������...\n\n");
	getchar();
	if(getchar())
		exit(0);
}
