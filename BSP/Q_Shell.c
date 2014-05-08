#include "Q_Shell.h"
#include "string.h"
#include "stdio.h"

#if USE_Q_SHELL_FUNCTION      
//����������ռ���ڴ�
extern int qShellFunTab$$Base;	//������¼�ζ���
extern int qShellFunTab$$Limit;	//������¼�ζ�β
extern int qShellVarTab$$Base;	//������¼�ζ���
extern int qShellVarTab$$Limit;	//������¼�ζ�β

#define QSH_FUN_PARA_MAX_NUM 4//�������֧���ĸ�����

typedef unsigned int (*QSH_FUNC_TYPE)();//ͳһ��shell��������֧�ֲ�ͬ�����Ĳ���

typedef enum{
	QSCT_ERROR,        //��Ч����
	QSCT_LIST_FUN,     //�г����пɱ�shell���ʵĺ���
	QSCT_LIST_VAR,     //�г����пɱ�shell���ʵı���
	QSCT_CALL_FUN,     //���ú���
	QSCT_READ_VAR,     //�鿴������ֵ
	QSCT_WRITE_VAR,    //��������ֵ
	QSCT_READ_REG,     //���Ĵ���
	QSCT_WRITE_REG     //д�Ĵ���
}Q_SH_CALL_TYPE;       //shell��������

typedef enum{
	QSFPT_NOSTRING,   //���ַ����Ͳ���
	QSFPT_STRING      //�ַ����Ͳ���
}Q_SH_FUN_PARA_TYPE;  //������������

typedef struct{
	char               *CmdStr;    //�����ַ����׵�ַ
	Q_SH_CALL_TYPE     CallType;   //shell��������
	unsigned char      CallStart;  //shell�������������ַ����е���ʼλ��                   
	unsigned char      CallEnd;    //shell�������������ַ����еĽ���λ�� 
	unsigned char      ParaNum;    //shell���õĲ�������
	unsigned char      ParaStart[QSH_FUN_PARA_MAX_NUM];//ÿ�������������ַ����е���ʼλ��   
	unsigned char      ParaEnd[QSH_FUN_PARA_MAX_NUM];  //ÿ�������������ַ����еĽ���λ��   
	Q_SH_FUN_PARA_TYPE ParaTypes[QSH_FUN_PARA_MAX_NUM];//ÿ������������               
}Q_SH_LEX_RESU_DEF;
/* 
���ܣ��ں�����¼��Ѱ��ָ���ĺ���
��Σ�������
���أ���Ӧ�ĺ�����¼
 */
static QSH_RECORD* Q_Sh_SearchFun(char* Name)
{
	QSH_RECORD* Index;
	for (Index = (QSH_RECORD*) &qShellFunTab$$Base; Index < (QSH_RECORD*) &qShellFunTab$$Limit; Index ++)
	{
		if (strcmp(Index->name, Name) == 0)
			return Index;
	}
	return (void *)0;
}
/* 
���ܣ��ں�����¼��Ѱ��ָ���ı���
��Σ�������
���أ���Ӧ�ı�����¼
 */
static QSH_RECORD* Q_Sh_SearchVar(char* Name)
{
	QSH_RECORD* Index;
	for (Index = (QSH_RECORD*) &qShellVarTab$$Base; Index < (QSH_RECORD*) &qShellVarTab$$Limit; Index ++)
	{
		if (strcmp(Index->name, Name) == 0)
			return Index;
	}
	return (void *)0;
}
/* 
���ܣ��г��������е����к�����¼�������ַ���
��Σ���
���أ���
 */
static void Q_Sh_ListFuns(void)
{
	QSH_RECORD* Index;
	for (Index = (QSH_RECORD*) &qShellFunTab$$Base; Index < (QSH_RECORD*) &qShellFunTab$$Limit; Index++)
	{	
		printf("%s\r\n",Index->desc);
	}
}
/*
���ܣ��г��������е����б�����¼�������ַ���
��Σ���
���أ��� 
 */
static void Q_Sh_ListVars(void)
{
	QSH_RECORD* Index;
	for (Index = (QSH_RECORD*) &qShellVarTab$$Base; Index < (QSH_RECORD*) &qShellVarTab$$Limit; Index++)
	{
		printf("%s\r\n",Index->desc);
	}
}
/*
���ܣ�����m��n�η�
��Σ�m ���� n ��ָ��
���أ������� 
 */
static unsigned int Q_Sh_Pow(unsigned int m,unsigned int n)
{
	unsigned int Result=1;	 
	while(n--)
		Result*=m;    
	return Result;
}
/*
���ܣ����ַ�ת�������֣�֧��16���ƣ���Сд��ĸ���ɣ�����֧�ָ���
��Σ�Str �����ַ��� Res �����ŵĵ�ַ
���أ�0 �ɹ� ��0 ʧ��
 */
static unsigned int Q_Sh_Str2Num(char*Str,unsigned int *Res)
{
	unsigned int Temp;
	unsigned int Num=0;			  
	unsigned int HexDec=10;
	char *p;
	p=Str;
	*Res=0;
	while(1)
	{
		if((*p>='A'&&*p<='F')||(*p=='X'))
			*p=*p+0x20;
		else if(*p=='\0')break;
		p++;
	}
	p=Str;
	while(1)
	{
		if((*p<='9'&&*p>='0')||(*p<='f'&&*p>='a')||(*p=='x'&&Num==1))
		{
			if(*p>='a')HexDec=16;	
			Num++;					
		}else if(*p=='\0')break;	
		else return 1;				
		p++; 
	} 
	p=Str;			    
	if(HexDec==16)		
	{
		if(Num<3)return 2;			
		if(*p=='0' && (*(p+1)=='x'))
		{
			p+=2;	
			Num-=2;
		}else return 3;
	}else if(Num==0)return 4;
	while(1)
	{
		if(Num)Num--;
		if(*p<='9'&&*p>='0')Temp=*p-'0';	
		else Temp=*p-'a'+10;				 
		*Res+=Temp*Q_Sh_Pow(HexDec,Num);		   
		p++;
		if(*p=='\0')break;
	}
	return 0;
}	
/*
���ܣ���ĸ�ַ����нص����ַ���
��Σ�Base ĸ�ַ����׵�ַ Start �������ַ�����ʼλ�� End �����ַ�������λ��
���أ����ַ����׵�ַ
 */
static char *Q_Sh_StrCut(char *Base,unsigned int Start,unsigned int End)
{
	Base[End+1]=0;
	return (char *)((unsigned int)Base+Start);
}
/*
���ܣ�������������ַ������дʷ�����
��Σ� CmdStr �����ַ����׵�ַ pQShLexResu ���������Ŵ�
���أ�0 �ʷ�����ʧ�� 1 �ʷ������ɹ�
 */
static unsigned int Q_Sh_LexicalAnalysis(char *CmdStr,Q_SH_LEX_RESU_DEF *pQShLexResu)
{
	unsigned int Index=0,TmpPos=0,ParamNum=0;
		
	while(1)
	{
		if( !( ( CmdStr[Index]>='a' && CmdStr[Index]<='z' ) || ( CmdStr[Index]>='A' && CmdStr[Index]<='Z' ) || ( CmdStr[Index]>='0' && CmdStr[Index]<='9' ) || CmdStr[Index]=='_' || CmdStr[Index]=='(') )
		{
			pQShLexResu->CallType=QSCT_ERROR;
			printf("Lexical Analysis Error: Function or Call name is illegal!!!\r\n");
			return 0;
		}
		if(CmdStr[Index]=='(')
		{
			if(Index==0)
			{
				pQShLexResu->CallType=QSCT_ERROR;
				printf("Lexical Analysis Error: Function or Call name is empty!!!\r\n");
				return 0;
			}
			pQShLexResu->CallEnd=(Index-1);
			TmpPos=++Index;
			break;
		}
		Index++;
	}
	
	while(1)
	{
		if( CmdStr[Index]<33||CmdStr[Index]>126 )
		{
			pQShLexResu->CallType=QSCT_ERROR;
			printf("Lexical Analysis Error: parameter include illegal character!!!\r\n");
			return 0;
		}
		if(CmdStr[Index]==',')
		{
			if(Index==TmpPos)
			{
				pQShLexResu->CallType=QSCT_ERROR;
				printf("Lexical Analysis Error: parameter include illegal comma!!!\r\n");
				return 0;
			}
			if(CmdStr[Index-1]=='\"')
			{
				if( ParamNum<QSH_FUN_PARA_MAX_NUM )
				{
					pQShLexResu->ParaTypes[ParamNum]=QSFPT_STRING;
					pQShLexResu->ParaStart[ParamNum]=TmpPos+1;
					pQShLexResu->ParaEnd[ParamNum]=Index-2;
					ParamNum++;
				}
				else
				{
					ParamNum++;
				}
				TmpPos=Index+1;
			}
			else
			{
				if( ParamNum<QSH_FUN_PARA_MAX_NUM )
				{
					pQShLexResu->ParaTypes[ParamNum]=QSFPT_NOSTRING;
					pQShLexResu->ParaStart[ParamNum]=TmpPos;
					pQShLexResu->ParaEnd[ParamNum]=Index-1;
					ParamNum++;
				}
				else
				{
					ParamNum++;
				}
				TmpPos=Index+1;
			}
		}
		else if(CmdStr[Index]==')')
		{
			if(Index==pQShLexResu->CallEnd+2)
			{
				pQShLexResu->ParaNum=0;
				break;
			}
			if(Index==TmpPos)
			{
				pQShLexResu->CallType=QSCT_ERROR;
				printf("Lexical Analysis Error: parameter include illegal comma!!!\r\n");
				return 0;
			}
			if(CmdStr[Index-1]=='\"')
			{
				if( ParamNum<QSH_FUN_PARA_MAX_NUM )
				{
					pQShLexResu->ParaTypes[ParamNum]=QSFPT_STRING;
					pQShLexResu->ParaStart[ParamNum]=TmpPos+1;
					pQShLexResu->ParaEnd[ParamNum]=Index-2;
					ParamNum++;
				}
				else
				{
					ParamNum++;
				}
				pQShLexResu->ParaNum=ParamNum;
				break;
			}
			else
			{
				if( ParamNum<QSH_FUN_PARA_MAX_NUM )
				{
					pQShLexResu->ParaTypes[ParamNum]=QSFPT_NOSTRING;
					pQShLexResu->ParaStart[ParamNum]=TmpPos;
					pQShLexResu->ParaEnd[ParamNum]=Index-1;
					ParamNum++;
				}
				else
				{
					ParamNum++;
				}
				pQShLexResu->ParaNum=ParamNum;
				break;
			}	
		}
		Index++;
	}
	pQShLexResu->CmdStr=CmdStr;
	pQShLexResu->CallStart=0;
	if( strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"lf")==0 )
		pQShLexResu->CallType=QSCT_LIST_FUN;
	else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"lv")==0)
		pQShLexResu->CallType=QSCT_LIST_VAR;
	else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"get")==0)
		pQShLexResu->CallType=QSCT_READ_VAR;
	else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"set")==0)
		pQShLexResu->CallType=QSCT_WRITE_VAR;
	else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"read")==0)
		pQShLexResu->CallType=QSCT_READ_REG;
	else if(strcmp(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd),"write")==0)
		pQShLexResu->CallType=QSCT_WRITE_REG;
	else
		pQShLexResu->CallType=QSCT_CALL_FUN;
	return 1;
}
/*
���ܣ��Դʷ������Ľ�������﷨��������ִ��
��Σ� pQShLexResu �ʷ����������Ŵ�
���أ�0 ʧ�� 1 �ɹ�
 */
static unsigned int Q_Sh_ParseAnalysis(Q_SH_LEX_RESU_DEF *pQShLexResu)
{
	QSH_RECORD *pRecord;
	unsigned int FunPara[QSH_FUN_PARA_MAX_NUM];
	unsigned int FunResu;
	unsigned int VarSet;
	unsigned int VarAddr;
	unsigned char i;
	unsigned char Ret=1;
	switch(pQShLexResu->CallType)
	{
		case QSCT_CALL_FUN:
		{
			pRecord=Q_Sh_SearchFun(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->CallStart,pQShLexResu->CallEnd));
			if( pRecord == 0 )
			{
				printf("Parse Analysis Error: the input function have not been regist!!!\r\n");
				Ret=0;
				break;
			}			
			if(pQShLexResu->ParaNum>QSH_FUN_PARA_MAX_NUM)
			{	
				printf("Parse Analysis Error: function's parameter number can't over four!!!\r\n");
				Ret=0;
				break;
			}
			for(i=0;i<pQShLexResu->ParaNum;i++)
			{
				if(pQShLexResu->ParaTypes[i]==QSFPT_STRING)
				{
					FunPara[i]=(unsigned int)Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[i],pQShLexResu->ParaEnd[i]);					
				}
				else
				{
					if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[i],pQShLexResu->ParaEnd[i]),&(FunPara[i]))!=0)
					{
						printf("Parse Analysis Error: the input number string is illegal!!!\r\n");
						Ret=0;
						break;
					}
				}
			}
			FunResu=((QSH_FUNC_TYPE)(pRecord->addr))(FunPara[0],FunPara[1],FunPara[2],FunPara[3]);
			printf("return %d\r\n",FunResu);
		}
		break;
		
		case QSCT_READ_VAR:
		{
			pRecord=Q_Sh_SearchVar(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[0],pQShLexResu->ParaEnd[0]));
			if( pRecord == 0 )
			{
				printf("Parse Analysis Error: the input variable have not been regist!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaNum!=1)
			{	
				printf("Parse Analysis Error: this call must have only one parameter!!!\r\n");
				Ret=0;
				break;
			}
			if(strcmp(pRecord->typedesc,"u8")==0)
				printf("%s=%d\r\n",pRecord->name,*(unsigned char *)(pRecord->addr));
			else if(strcmp(pRecord->typedesc,"u16")==0)
				printf("%s=%d\r\n",pRecord->name,*(unsigned short *)(pRecord->addr));
			else if(strcmp(pRecord->typedesc,"u32")==0)
				printf("%s=%d\r\n",pRecord->name,*(unsigned int *)(pRecord->addr));
			else
			{
				printf("the describe of variable's type is illegal!!!\r\n");
				Ret=0;
				break;
			}
		}
		break;

		case QSCT_WRITE_VAR:
		{
			pRecord=Q_Sh_SearchVar(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[0],pQShLexResu->ParaEnd[0]));
			if( pRecord == 0 )
			{
				printf("Parse Analysis Error: the input variable have not been regist!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaNum!=2)
			{	
				printf("Parse Analysis Error: this call must have two parameter!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaTypes[1]==QSFPT_STRING)
			{
				printf("Parse Analysis Error: this call's second parameter can't be string type!!!\r\n");
				Ret=0;
				break;
			}
			if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[1],pQShLexResu->ParaEnd[1]),&VarSet)!=0)
			{
				printf("Parse Analysis Error: the input number string is illegal!!!\r\n");
				Ret=0;
				break;
			}
			if(strcmp(pRecord->typedesc,"u8")==0)
			{
				*(unsigned char *)(pRecord->addr)=VarSet;
			    printf("%s=%d\r\n",pRecord->name,*(unsigned char *)(pRecord->addr));	
			}
			else if(strcmp(pRecord->typedesc,"u16")==0)
			{
				*(unsigned short *)(pRecord->addr)=VarSet;
			    printf("%s=%d\r\n",pRecord->name,*(unsigned short *)(pRecord->addr));	
			}
			else if(strcmp(pRecord->typedesc,"u32")==0)
			{
				*(unsigned int *)(pRecord->addr)=VarSet;
			    printf("%s=%d\r\n",pRecord->name,*(unsigned int *)(pRecord->addr));	
			}
			else
			{
				printf("the describe of variable's type is illegal!!!\r\n");
				Ret=0;
				break;
			}
		}
		break;
		
		case QSCT_READ_REG:
		{
			if(pQShLexResu->ParaNum!=1)
			{	
				printf("Parse Analysis Error: this call must have only one parameter!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaTypes[0]==QSFPT_STRING)
			{
				printf("Parse Analysis Error: this call's parameter can not be string type!!!\r\n");
				Ret=0;
				break;
			}
			if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[0],pQShLexResu->ParaEnd[0]),&VarAddr)!=0)
			{
				printf("Parse Analysis Error: the input number string is illegal!!!\r\n");
				Ret=0;
				break;
			}
			printf("*(0x%x)=0x%x\r\n",VarAddr,*(unsigned int *)VarAddr);
		}
		break;
		
		case QSCT_WRITE_REG:
		{
			if(pQShLexResu->ParaNum!=2)
			{	
				printf("Parse Analysis Error: this call must have two parameter!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaTypes[0]==QSFPT_STRING)
			{
				printf("Parse Analysis Error: this call's first parameter can not be string type!!!\r\n");
				Ret=0;
				break;
			}
			if(pQShLexResu->ParaTypes[1]==QSFPT_STRING)
			{
				printf("Parse Analysis Error: this call's second parameter can not be string type!!!\r\n");
				Ret=0;
				break;
			}
			if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[0],pQShLexResu->ParaEnd[0]),&VarAddr)!=0)
			{
				printf("Parse Analysis Error: the input number string is illegal!!!\r\n");
				Ret=0;
				break;
			}
			if(Q_Sh_Str2Num(Q_Sh_StrCut(pQShLexResu->CmdStr,pQShLexResu->ParaStart[1],pQShLexResu->ParaEnd[1]),&VarSet)!=0)
			{
				printf("Parse Analysis Error: the input number string is illegal!!!\r\n");
				Ret=0;
				break;
			}
			*(unsigned int *)VarAddr=VarSet;
			printf("*(0x%x)=0x%x\r\n",VarAddr,*(unsigned *)VarAddr);
		}
		break;
		
		case QSCT_LIST_FUN:
		{
			if(pQShLexResu->ParaNum>0)
			{	
				printf("Parse Analysis Error: this call must have no parameter!!!\r\n");
				Ret=0;
				break;
			}
			Q_Sh_ListFuns();
		}
		break;
		
		case QSCT_LIST_VAR:
		{
			if(pQShLexResu->ParaNum>0)
			{	
				printf("Parse Analysis Error: this call must have no parameter!!!\r\n");
				Ret=0;
				break;
			}
			Q_Sh_ListVars();
		}
		break;
		
		default:
		{	
			printf("Parse Analysis Error: can't get here!!!\r\n");
			Ret=0;
		}
	}
	return Ret;
}
/* 
shell�����Ŵ����������һ��ʹ��shell��Ҫ���������
 */
static const char Q_ShellPassWord[]="123456";

/* 
���ܣ�shell����Ľӿڣ�ִ������
��Σ�IfCtrl ָʾ�Ƿ���յ�һ�������ַ� CmdStr �Ӵ��ڵõ��������ַ���
���أ�1 �ɹ� 0 ʧ��
 */
unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl,char *CmdStr)
{
	Q_SH_LEX_RESU_DEF Q_Sh_LexResu;
	static unsigned char IfPass=0,IfWaitInput=0;
	if(IfPass)
	{
		if(IfCtrl==0)
		{
			if(((unsigned short *)CmdStr)[0]==0x445b)
			{}
			else if(((unsigned short *)CmdStr)[0]==0x435b)
			{}
			else
				printf("CtrlCode:%x\n\r",((unsigned short *)CmdStr)[0]);
			return 0;
		}
		if( Q_Sh_LexicalAnalysis(CmdStr,&Q_Sh_LexResu) == 0 )//�ʷ�����
			return 0;
		if( Q_Sh_ParseAnalysis(&Q_Sh_LexResu) == 0 )//�﷨����
			return 0;	 
		return 1;
	}
	else
	{
		if(IfWaitInput==0)
		{
			printf("Please input q_shell password:");
			IfWaitInput=1;
		}
		else
		{
			if(strcmp(CmdStr,Q_ShellPassWord)==0)
			{
				IfPass=1;
				printf("password right!\r\n");
			}
			else
			{
				printf("password wrong!\r\n");
				printf("Please input q_shell password again:");
			}
		}
		return 0;
	}
}

#else
/*
��USE_Q_SHELL_FUNCTION=0��shell���ܱ��رպ�Ϊ�˲���Ҫ�޸�ԭ���������
 */
unsigned int Q_Sh_CmdHandler(unsigned int IfCtrl,char *CmdStr)
{
	return 0;
}

#endif
