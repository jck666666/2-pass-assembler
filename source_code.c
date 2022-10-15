#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#define MAX 17
//pFile = fopen ("/Users/jck/Desktop/4108033007組語.c/source.txt", "r");
//pFile = fopen("/Users/jck/Desktop/4108033007組語.c/opcode.txt", "r");

typedef struct source
{
	/* data */
	char label[MAX];
	char operation[MAX];
	char operand[MAX];
} source;

typedef struct optable
{
	char operation[MAX];
	char opcode[3];
} optable;

typedef struct symtable
{
	char label[MAX];
	int addr; //dec
} symtable;

int slen = 0; //len of source.txt
int olen = 0; //len of opcode.txt
int flag = 0; //record symtableAry current len
source sourceAry[1000];
optable optableAry[1000];
symtable symtableAry[1000];
int loc[1000];		   //LOCCTR
char objcode[1000][7]; // 6->3bytes

void Input_SourceFile();
void Print_Statement();

void Input_OpCodefile();
void Print_OpTable();

void Print_symTable();
void Out_symTable();

void Print_intermediate();
void Out_intermediate();

void Out_sourceProgram();
void Out_finalProgram();

void pass1();
void pass2();
int hexTodec(char[]);
bool search_label(char[]);
bool search_opeartion(char[]);
int search_operand(char[]);

int main()
{

	Input_SourceFile();
	//Print_Statement();

	Input_OpCodefile();
	//Print_OpTable();

	pass1();

	Print_symTable();
	Out_symTable();
	//Print_intermediate();
	Out_intermediate();

	pass2();
	Out_sourceProgram();
	Out_finalProgram();

	return 0;
}

void Input_SourceFile()
{
	char ch;
	slen = 0;
	FILE *pFile;
	pFile = fopen("source.txt", "r");
	if (pFile == NULL)
	{
		printf("source.txt open fail\n");
	}
	else
	{
		printf("source.txt open success\n");

		while (feof(pFile) == 0) //count how many line in statement
		{
			if (fgetc(pFile) != '\n') //detect next line
				continue;
			else
				slen++;
		}
		slen++; //last line
		printf("source.txt 有 %d 行\n", slen);

		rewind(pFile); //reopen source.txt

		//initialized sourceAry
		//source sourceAry[slen];
		for (int i = 0; i < slen; i++)
		{
			//sourceAry[i] = (source){"ooooooooooooooooo","ooooooooooooooooo","ooooooooooooooooo"};
			//sourceAry[i] = (source){"               ", "               ", "               "};
			//sourceAry[i] = (source){0};
			sourceAry[i] = (source){"", "", ""};
		}

		for (int i = 0; i < slen; i++)
		{
			//
			//label
			int j;
			for (j = 0; j < MAX; j++)
			{
				ch = fgetc(pFile);
				if (ch == '\t') //skip tab
					break;
				else
				{
					if (ch == ' ')
						continue;
					sourceAry[i].label[j] = ch;
				}
			}
			//sourceAry[i].label[++j] ='\0';

			//
			//operation
			for (j = 0; j < MAX; j++)
			{
				ch = fgetc(pFile);
				if (ch == '\r' || ch == '\t' || ch == EOF) //skip tab , next_line ,EOF
				{
					if (ch == '\r')
					{
						fgetc(pFile); //read '\n' to ignore
					}
					break;
				}
				else
				{
					if (ch == ' ')
						continue;
					sourceAry[i].operation[j] = ch;
				}
			}
			if (ch == '\r')
				continue; //next line
			if (ch == EOF)
				break;

			//
			//operand
			for (j = 0; j < MAX; j++)
			{
				ch = fgetc(pFile);
				if (ch == '\r' || ch == '\t' || ch == EOF) //skip tab , next_line ,EOF
				{
					if (ch == '\r')
					{
						fgetc(pFile); //read '\n' to ignore
					}
					break;
				}
				else
				{
					if (ch == ' ')
						continue;
					sourceAry[i].operand[j] = ch;
				}
			}

			if (ch == EOF)
				break;
		}
	}
	fclose(pFile);
	printf("source.txt close success\n\n");
}

void Print_Statement() //sourceAry[]
{
	printf("---This is STATEMENT---\n");
	for (int i = 0; i < slen; i++)
	{
		// printf("oo%sLL\t", sourceAry[i].label);
		// printf("oo%sLL\t", sourceAry[i].operation);
		// printf("oo%sLL\n", sourceAry[i].operand);

		// printf("%s", sourceAry[i].label);
		// printf("%s", sourceAry[i].operation);
		// printf("%s\n", sourceAry[i].operand);

		printf("%s\t", sourceAry[i].label);
		printf("%s\t", sourceAry[i].operation);
		printf("%s\n", sourceAry[i].operand);
	}
	printf("---STATEMENT END---\n");
}

void Input_OpCodefile()
{

	char ch;
	FILE *pFile;
	pFile = fopen("opcode.txt", "r");
	if (pFile == NULL)
	{
		printf("opcode.txt open fail\n");
	}
	else
	{
		printf("opcode.txt open success\n");
		while (feof(pFile) == 0) //count how many line in opcode.txt
		{
			if (fgetc(pFile) != '\n') //detect next line
				continue;
			else
				olen++;
		}
		olen++; //last line
		printf("opcode.txt 有 %d 行\n", olen);

		rewind(pFile); //reopen opcode.txt

		//initialized sourceAry
		//optable optableAry[olen];
		for (int i = 0; i < olen; i++)
		{
			optableAry[i] = (optable){"", ""};
			//optableAry[i] = (optable){0};
			//optableAry[i] = (optable){"",""};
		}

		for (int i = 0; i < olen; i++)
		{
			//operation
			for (int j = 0; j < MAX; j++)
			{
				ch = fgetc(pFile);
				if (ch == ' ') //skip space , next_line ,EOF
					break;
				else
				{
					if (ch == ' ')
						continue;
					optableAry[i].operation[j] = ch;
				}
			}
			//opcode
			for (int j = 0; j < 3; j++)
			{
				ch = fgetc(pFile);
				if (ch == '\r' || ch == EOF)
				{
					fgetc(pFile);
					break;
				}
				else
				{
					if (ch == ' ')
						continue;
					optableAry[i].opcode[j] = ch;
				}
			}
			if (ch == EOF)
				break;
		}
	}

	fclose(pFile);
	printf("opcode.txt close success\n\n");
}

void Print_OpTable() //optableAry[]

{
	printf("---This is OP TABLE---\n");
	for (int i = 0; i < olen; i++)
	{
		printf("%s ", optableAry[i].operation);
		printf("%s\n", optableAry[i].opcode);
	}
	printf("---OP TABLE END---\n");
}

void Print_symTable()
{
	printf("---This is SYMBOL TABLE---\n");
	for (int i = 0; i < flag; i++)
	{
		printf("%s\t", symtableAry[i].label);
		printf("%X\n", symtableAry[i].addr);
	}
	printf("---SYMBOL TABLE END---\n");
}

void Print_intermediate()
{
	printf("---This is INTERMEDIATE---\n");
	for (int i = 0; i < slen; i++)
	{
		if (i == slen - 1)
			printf("\t");
		else
			printf("%X\t", loc[i]);

		printf("%s\t", sourceAry[i].label);
		printf("%s\t", sourceAry[i].operation);
		printf("%s\n", sourceAry[i].operand);
	}
	printf("---INTERMEDIATE END---\n");
}

int hexTodec(char hex[])
{
	int len = strlen(hex);
	int sum = 0;
	for (int i = 0; i < len; i++)
	{
		if (hex[i] >= 'A' && hex[i] <= 'Z')
			hex[i] = (int)(hex[i] - 'A') + 10 + '0';
		sum += ((hex[i] - '0') * (pow(16, len - 1 - i)));
	}
	return sum;
}

bool search_label(char label[]) // search to check duplicated in symtaleAry
{
	//search if this label is already in symtaleAry
	for (int i = 0; i < slen; i++)
	{
		if (strcmp(symtableAry[i].label, label) == 0)
			return true;
	}
	return false;
}

bool search_opeartion(char opeartion[]) //search in optableAry
{
	for (int i = 0; i < olen; i++)
	{
		if (strcmp(optableAry[i].operation, opeartion) == 0) //search if this opeartion is  in optableAry
		{

			return true;
		}
	}
	return false;
}

int search_operand(char operand[]) // search in symtableAry
{
	int operandlen = strlen(operand);

	if (operand[operandlen - 1] == 'X') //last char is X  (,X)
	{
		operandlen -= 2; //,X
	}

	for (int i = 0; i < flag; i++)
	{
		if (strncmp(symtableAry[i].label, operand, operandlen) == 0) //search if this operand is a label in symtableAry
			return symtableAry[i].addr;
		else
			continue;
	}
	return -1;
}

void pass1()
{
	int startAddr = 0;
	int currentAddr;

	if (strcmp(sourceAry[0].operation, "START") == 0)
	{
		startAddr = hexTodec(sourceAry[0].operand);
		loc[0] = startAddr;
		loc[1] = startAddr;
	}
	else
	{
		loc[0] = startAddr;
	}
	currentAddr = startAddr;

	int i = 1; //scan sourceAry[i] from i=1
	flag = 0;  //record symtableAry current len
	while (strcmp(sourceAry[i].operation, "END") != 0)
	{
		// printf("%X	",currentAddr);
		// printf("%s\n",sourceAry[i].operation);

		//build symtableAry
		if (strcmp(sourceAry[i].label, "") != 0)
		{
			if (!search_label(sourceAry[i].label)) //if it isn't in symtableAry
			{
				strcpy(symtableAry[flag].label, sourceAry[i].label);
				symtableAry[flag].addr = currentAddr;
				flag++;
			}
		}

		//store current_LOCCTR , calculate next_LOCCTR
		loc[i] = currentAddr;
		if (search_opeartion(sourceAry[i].operation) == true) //if this operation is in optableAry
		{
			currentAddr += 3; //next LOCCTR
		}
		else if (strcmp(sourceAry[i].operation, "WORD") == 0) //if operation=="WORD"
		{
			currentAddr += 3; //next LOCCTR
		}
		else if (strcmp(sourceAry[i].operation, "RESW") == 0) //if operation=="RESW"
		{
			currentAddr += 3 * atoi(sourceAry[i].operand); //next LOCCTR
		}
		else if (strcmp(sourceAry[i].operation, "RESB") == 0) //if operation=="RESB"
		{
			currentAddr += atoi(sourceAry[i].operand); //next LOCCTR
		}
		else if (strcmp(sourceAry[i].operation, "BYTE") == 0) //if operation=="BYTE"
		{
			if (sourceAry[i].operand[0] == 'C')
			{
				int len = strlen(sourceAry[i].operand);
				currentAddr += (len - 3); //next LOCCTR
			}
			else if (sourceAry[i].operand[0] == 'X')
			{
				int len = strlen(sourceAry[i].operand);
				int cntByte = (len - 3) / 2;
				currentAddr += cntByte; //next LOCCTR
			}
		}
		else
		{
			//invalid opeartion code
		}

		//next sourceAry[i].operation
		i++;
	}
	loc[i] = currentAddr; //last line
}

void Out_symTable()
{
	printf("Out symtableAry is opearting.\n");

	FILE *pFile = fopen("symbol table.txt", "w");
	fputs("---This is SYMBOL TABLE---\n\n", pFile);
	fputs("Label\tAddress\n\n", pFile);
	for (int i = 0; i < flag; i++)
	{
		fprintf(pFile, "%s\t", symtableAry[i].label);
		fprintf(pFile, "%X\n", symtableAry[i].addr);
	}
	fputs("\n---SYMBOL TABLE END---\n", pFile);

	fclose(pFile);
	printf("Out symtableAry is finished.\n\n");
}

void Out_intermediate()
{
	printf("Out intermediate is opearting.\n");
	FILE *pFile = fopen("Intermediate.txt", "w");
	fputs("---This is INTERMEDIATE---\n\n", pFile);

	fputs("LOC\t\tSTATEMENT\n\n", pFile);
	for (int i = 0; i < slen; i++)
	{
		if (i == (slen - 1))
			fprintf(pFile, "\t");
		else
			fprintf(pFile, "%X\t", loc[i]);

		fprintf(pFile, "%s\t", sourceAry[i].label);
		fprintf(pFile, "%s\t", sourceAry[i].operation);
		fprintf(pFile, "%s\n", sourceAry[i].operand);
	}

	fputs("\n---INTERMEDIATE END---\n", pFile);

	fclose(pFile);
	printf("Out intermediate is finished.\n\n");
}

void pass2()
{
	//initialized objcodeAry
	for (int i = 0; i < slen; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			objcode[i][j] = '\0';
		}
	}

	int i = 1; //start from sourceAry[1]

	while (strcmp(sourceAry[i].operation, "END") != 0)
	{
		char opcode_char[3] = "";
		char operandaddr_char[5] = "";

		if (search_opeartion(sourceAry[i].operation) == true) //search if this opeartion is  in optableAry
		{
			/*find opcode of label(operation) begin*/
			for (int j = 0; j < olen; j++) //to find opcode of opeartion in  optableAry
			{

				if (strcmp(optableAry[j].operation, sourceAry[i].operation) == 0)
				{
					strcpy(opcode_char, optableAry[j].opcode);
					break;
				}
			}
			/*find opcode of label(operation) end*/

			if (strcmp(sourceAry[i].operand, "") != 0) //operand field isn't null
			{
				int operandaddr_int = 0;
				if ((operandaddr_int = search_operand(sourceAry[i].operand)) != -1) //search if this operand is a label in symtableAry
				{
					int operandlen = strlen(sourceAry[i].operand);

					if (sourceAry[i].operand[operandlen - 1] == 'X')
					{
						operandaddr_int += 32768; //operandaddr_char[0] add 8,add 8000(hex)=>add 32768(dec)
					}

					char tmp[5] = "";
					sprintf(tmp, "%X", operandaddr_int); //operandaddr_int transform to  tmp[]
					if (strlen(tmp) == 4)
					{
						for (int k = 0; k < 4 - strlen(tmp); k++) //fill 0 to the vacancy of operandaddr_char[5]
						{
							operandaddr_char[k] = '0';
						}
						strcat(operandaddr_char, tmp);
					}
					else
					{
						printf("strlen(tmp)>4  error!!!!!\n"); //error
					}
				}
				else // this operand isn't a label in symtableAry
				{
					//store 0000 as operand address
					strcpy(operandaddr_char, "0000");
				}
			}
			else //operand field is null
			{
				//store 0000 as operand address
				strcpy(operandaddr_char, "0000");
			}

			//gernate object code
			//put opcode_char,operandaddr_char together
			strcat(objcode[i], opcode_char);
			strcat(objcode[i], operandaddr_char);
		}
		else if (strcmp(sourceAry[i].operation, "BYTE") == 0)
		{
			if (sourceAry[i].operand[0] == 'C')
			{
				int ascii;
				int cnt = 0;										//at most 3
				char tmp[7] = "";									//record complete data constant
				char tmp1[3] = "";									//record a data constant per time
				for (int j = 2; sourceAry[i].operand[j] != 39; j++) //39 is '
				{
					cnt++; //count how many data constant character
					ascii = sourceAry[i].operand[j];
					sprintf(tmp1, "%X", ascii);
					strcat(tmp, tmp1);
				}
				for (int k = 0; k < 6 - (2 * cnt); k++) //fill 0 to the vacancy of objcode[i]
				{
					objcode[i][k] = '0';
				}

				//generate object code
				strcat(objcode[i], tmp);
			}
			else if (sourceAry[i].operand[0] == 'X')
			{
				for (int j = 2; sourceAry[i].operand[j] != 39; j++) //39 is '
				{
					//generate object code
					int m = j - 2;
					objcode[i][m] = sourceAry[i].operand[j];
				}
			}
		}
		else if (strcmp(sourceAry[i].operation, "WORD") == 0)
		{
			char tmp[7] = "";
			int num = atoi(sourceAry[i].operand);
			sprintf(tmp, "%X", num);
			for (int k = 0; k < 6 - strlen(tmp); k++)
			{
				objcode[i][k] = '0';
			}

			//generate object code
			strcat(objcode[i], tmp);
		}
		i++;
	}
}

void Out_sourceProgram()
{
	printf("Out sourceProgram is opearting.\n");

	FILE *pFile = fopen("source program.txt", "w");
	fputs("---This is SOURCE PROGRAM--- \n\n", pFile);
	fputs("Loc\t\tStatement\tObject code\n\n", pFile);
	for (int i = 0; i < slen; i++)
	{
		if (i == (slen - 1))
			fprintf(pFile, "\t");
		else
			fprintf(pFile, "%X\t", loc[i]);

		fprintf(pFile, "%s\t", sourceAry[i].label);
		fprintf(pFile, "%s\t", sourceAry[i].operation);
		fprintf(pFile, "%s\t", sourceAry[i].operand);
		fprintf(pFile, "%s\n", objcode[i]);
	}
	fputs("\n---SOURCE PROGRAM end--- \n", pFile);

	fclose(pFile);
	printf("Out sourceProgram  is finished.\n\n");
}

void Out_finalProgram()
{
	printf("Out finalProgram is opearting.\n");

	FILE *pFile = fopen("final object program.txt", "w");
	fputs("---This is FINAL PROGRAM---\n\n", pFile);

	//H
	char programName[100] = "";
	int fillblank = 6 - strlen(sourceAry[0].label);
	strcpy(programName, sourceAry[0].label);
	for (int i = 0; i < fillblank; i++)
	{
		strcat(programName, " ");
	}
	fprintf(pFile, "H%s%06X%06X\n", (programName), (loc[0]), (loc[slen - 1] - loc[0]));

	//T
	int i = 0;
	int startaddr = loc[1];
	int now = 1;
	while (i <= slen - 1)
	{
		char pool[1000] = "";
		int poolLen = 0; //dec
		int k = i;
		fprintf(pFile, "T%06X", loc[++k]);
		for (int j = 0; j < 10; j++) // 10 objectcode per line
		{
			i++;
			strcat(pool, objcode[i]);
			poolLen = strlen(pool);
			// poolLen/2 (Bytes)
		}
		fprintf(pFile, "%02X", poolLen / 2);

		for (int j = 0; j < 10; j++)
		{
			if (strcmp(objcode[now], "") != 0)
			{
				fprintf(pFile, "%s", objcode[now]);
			}

			now++;
			if (now == slen)
				break;
		}
		fprintf(pFile, "\n");
	}

	//E
	fprintf(pFile, "E%06X", loc[1]);



	fputs("\n\n---FINAL PROGRAM end---\n", pFile);

	fclose(pFile);
	printf("Out finalProgram  is finished.\n\n");
}
