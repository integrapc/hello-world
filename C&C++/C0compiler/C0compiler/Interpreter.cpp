#include "Interpreter.h"

int Locate(char s[])						// ����ָ�������ָ�����
{						
	int i;
	for (i=0;i<CNO;i++)
		if (strcmp(code[i],s)==0)
			return i;
	return -1;
}

int LocLabel(char s[])						// ��λ��ţ����ر�����ڵ�λ��
{	
	char lab[255];
	strcat(s,":");
	fseek(fp,0,SEEK_SET);
	fscanf(fp,"%s",lab);
	while (strcmp(s,lab)!=0)
		fscanf(fp,"%s",lab);
	return 1;
}

int getAdd(int &ll,int &on,char s[])		// ����ַ����Ϊ��ź�ƫ��ֵ
{
	int i=0;
	ll=on=0;
	while (s[++i]!=',')
	{
		ll=ll*10+(s[i]-'0');
	}
	while (s[++i]!='>')
	{
		on=on*10+(s[i]-'0');
	}
	return 1;
}

int Cal(int t)								// ����ADD��SUB��MULT��DIV, tΪ����������
{
	int t1,t2;
	float f1,f2;
	bool flag=true;
	if (data[dptr-1].type==FLOAT && data[dptr-2].type==FLOAT){
		f1=data[--dptr].v.fval;
		f2=data[--dptr].v.fval;
		switch(t)
		{
		case 1:f1+=f2; break;
		case 2:f1=f2-f1; break;
		case 3:f1*=f2; break;
		case 4:f1=f2/f1; break;
		}
		data[dptr].type=FLOAT;
		data[dptr++].v.fval=f1;
	}
	else if (data[dptr-1].type==INTEGER && data[dptr-2].type==INTEGER){
		t1=data[--dptr].v.ival;
		t2=data[--dptr].v.ival;
		switch(t)
		{
		case 1:t1+=t2; break;
		case 2:t1=t2-t1; break;
		case 3:t1*=t2; break;
		case 4:									// �����������ʱ�����ܲ���������
			if (t2%t1==0)
				t1=(int)t2/t1; 
			else
			{
				f1=(float)t2/t1;
				flag=false;
			}
			break;
		}
		if (flag)
		{
			data[dptr].type=INTEGER;
			data[dptr].v.ival=t1;
			data[dptr++].v.fval=(float)t1;
		}
		else
		{
			data[dptr].type=FLOAT;
			data[dptr++].v.fval=f1;
		}
	}
	else if (data[dptr-1].type==FLOAT && data[dptr-2].type==INTEGER)
	{
		f1=data[--dptr].v.fval;
		t1=data[--dptr].v.ival;
		switch(t)
		{
		case 1:f1+=t1; break;
		case 2:f1=t1-f1; break;
		case 3:f1*=t1; break;
		case 4:f1=t1/f1; break;
		}
		data[dptr].type=FLOAT;
		data[dptr++].v.fval=f1;
	}
	else if (data[dptr-1].type==INTEGER && data[dptr-2].type==FLOAT)
	{
		t1=data[--dptr].v.ival;
		f1=data[--dptr].v.fval;
		switch(t)
		{
		case 1:f1+=t1; break;
		case 2:f1=f1-t1; break;
		case 3:f1*=t1; break;
		case 4:f1=f1/t1; break;
		}
		data[dptr].type=FLOAT;
		data[dptr++].v.fval=f1;
	}
	return 1;
}

int GetData(float f[],int t[],int &type)
{
	int i,temp;
	char s[2];
	for (i=0;i<2;i++)
	{
		temp=data[--dptr].type;
		if (temp==INTEGER)
		{
			s[i]='1';
			t[i]=data[dptr].v.ival;
		}
		else
		{
			f[i]=data[dptr].v.fval;
			s[i]='2';
		}
	}
	type=atoi(s);
	return 1;
}

void Interpreter(char *fileName)			// ����ִ��
{
	fp=fopen(fileName,"r");
	dptr=0; abp=1; rptr=4; preabp=-1;
	char s[15];	
	int i,temp,ll,on,tp,type;
	float f[2];
	int t[2];
	while(!feof(fp)){
		do{
			fscanf(fp,"%s",s);
		}while (s[strlen(s)-1]==':');
		tp=Locate(s);
		switch(tp)
		{
		case 0:								// ����ALLOCATE���
			fscanf(fp,"%d",&i);
			rptr+=i;
			break;
		case 1:								// ����STO���
			fscanf(fp,"%s",s);
			getAdd(ll,on,s);
			if (ll==0)						// �ж���ȫ�ֱ������Ǳ���ı���
			{
				run[on-1]=data[--dptr];
			}
			else
			{
				run[abp+on-1]=data[--dptr];
			}
			break;
		case 2:								// ����LOAD���		
			fscanf(fp,"%s",s);
			if (strcmp(s,"$Top")!=0)
			{
				getAdd(ll,on,s);
				if (ll==0)
					if (run[on-1].type==FLOAT || run[on-1].type==INTEGER)		// ��ֹ��û�н��г�ʼ���ı���������
						data[dptr++]=run[on-1];
					else
						data[dptr++].type=2;
				else
					if (run[abp+on-1].type==FLOAT || run[abp+on-1].type==INTEGER)	// ��ֹ��û�н��г�ʼ���ı���������
						data[dptr++]=run[abp+on-1];
					else
						data[dptr++].type=2;
			}
			else{
				data[dptr]=data[dptr-1];
				dptr++;
			}
			break;
		case 3:								// ����LOADI���
			fscanf(fp,"%s",s);
			if (strchr(s,'.')==NULL)
			{
				data[dptr].type=INTEGER;
				data[dptr].v.ival=atoi(s);
				data[dptr++].v.fval=(float)(atof(s));
			}
			else
			{
				data[dptr].type=FLOAT;
				data[dptr++].v.fval=(float)atof(s);
			}
			break;
		case 4:								// ����POP���
			dptr--;
			break;
		case 5:								// ����JSR���
			fscanf(fp,"%s",s);
			LocLabel(s);
			run[rptr+2].type=INTEGER;			// ������¼������abp,preabp
			run[rptr+2].v.ival=abp;
			abp=rptr;
			rptr=rptr+3;
			break;	
		case 6:								// ����RETURN���
			temp=run[abp].v.ival;				// ȡ���ص�ַ
			itoa(temp,s,10);
			if (temp!=-1)						// ��1Ϊ��������¼�ĵ��û���ַ
			{
				LocLabel(s);
				temp=abp;
				abp=run[abp+2].v.ival;				// �������¼ abp=preabp;
				rptr=temp;
			}
			break;
		case 7: Cal(1); break;					// ����ADD���
		case 8: Cal(2); break;					// ����SUB���
		case 9: Cal(3); break;					// ����MULT���
		case 10:Cal(4); break;					// ����DIV���
		case 11:								// ����EQ���
			GetData(f,t,type);
			data[dptr].type=INTEGER;	
			if ((type==11 && t[1]==t[0]) || (type==22 && f[1]==f[0])
				|| (type==12 && f[1]==t[0]) || (type==21 && t[1]==f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 12:								// ����NEQ���
			GetData(f,t,type);
			data[dptr].type=INTEGER;
			if ((type==11 && t[1]!=t[0]) || (type==22 && f[1]!=f[0])
				|| (type==12 && f[1]!=t[0]) || (type==21 && t[1]!=f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 13:								// ����GT���
			GetData(f,t,type);
			data[dptr].type=INTEGER;
			if ((type==11 && t[1]>t[0]) || (type==22 && f[1]>f[0])
				|| (type==12 && f[1]>t[0]) || (type==21 && t[1]>f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 14:								// ����LES���
			GetData(f,t,type);
			data[dptr].type=INTEGER;
			if ((type==11 && t[1]<t[0]) || (type==22 && f[1]<f[0])
				|| (type==12 && f[1]<t[0]) || (type==21 && t[1]<f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 15:								// ����GE���
			GetData(f,t,type);
			data[dptr].type=INTEGER;
			if ((type==11 && t[1]>=t[0]) || (type==22 && f[1]>=f[0])
				|| (type==12 && f[1]>=t[0]) || (type==21 && t[1]>=f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 16:								// ����LE���
			GetData(f,t,type);
			data[dptr].type=INTEGER;
			if ((type==11 && t[1]<=t[0]) || (type==22 && f[1]<=f[0])
				|| (type==12 && f[1]<=t[0]) || (type==21 && t[1]<=f[0]))
					data[dptr++].v.ival=1;
			else
					data[dptr++].v.ival=0;
			break;
		case 17:								// ����BRF���
			fscanf(fp,"%s",s);
			temp=data[--dptr].v.ival;
			if (temp==0)
				LocLabel(s);
			break;
		case 18:								// ����BR���
			fscanf(fp,"%s",s);
			LocLabel(s);
			break;
		case 19:								// ����PRINTI���
			printf("%d",data[--dptr].v.ival);
			break;
		case 20:								// ����PRINTS���
			c=fgetc(fp);
			while ((c=fgetc(fp))!='\n' && c!='\t')
			{
				if (c!='\\')
					printf("%c",c);
				else
				{
					c=fgetc(fp);
					if (c=='\n' && c!='\t')
						break;
					if (c=='n')
						printf("\n");
				}
			}
			break;
		case 21:								// ����CONVERF���
			fscanf(fp,"%s",s);
			if (strcmp(s,"$Top"))
				i=2;
			else 
				i=1;
			data[dptr-i].type=FLOAT;
			data[dptr-i].v.fval=(float)data[dptr-i].v.ival;
			break;
		case 22:								// ����READ���
			scanf("%s",s);
			if (s[0]>='0' && s[0]<='9'||s[0] =='+'&&strlen(s)>=2&&(s[1]>='0' && s[1]<='9') ||s[0] =='-'&&strlen(s)>=2&&(s[1]>='0' && s[1]<='9'))
			{
				if (strchr(s,'.')==NULL)
				{
					data[dptr].type=2;
					data[dptr].v.ival=atoi(s);
					data[dptr++].v.fval=(float)atof(s);
				}
				else
				{
					data[dptr].type=3;
					data[dptr].v.ival=atoi(s);
					data[dptr++].v.fval=(float)atof(s);
				}
			}
			else
			{
				temp=strlen(s);
				data[dptr].type=2;
				data[dptr].v.ival=s[0];
				data[dptr++].v.fval=(float)s[0];
			}
			break;
		case 23:								// ����PRINTF���
			printf("%f",data[--dptr].v.fval);
			break;
		case 24:
			printf("%c",data[--dptr].v.ival);	// ����PRINTC���
			break;
		case 25:
			fscanf(fp,"%s",s);
			if (strcmp(s,"$Top"))				//	����CONVERI���
				i=2;
			else 
				i=1;
			data[dptr-i].type=INTEGER;
			data[dptr-i].v.ival=(int)data[dptr-i].v.fval;
			break;
		}
	
	}
}
