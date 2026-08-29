#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<stdbool.h>

char op_name[50][10];
char op_code[50][10];
int opcnt = 0;

char label[100][20];
char opcode[100][20];
char operand[100][20];

char sym_label[100][20];
int sym_address[100];
int symcnt = 0;

int error_flag = 0;
int prog_len=0;
int start_address = 0;

int i = 0;
int n;

void load_opcodes()
{
    FILE *fp = fopen("opcodes.txt", "r");
    while (fscanf(fp, "%s %s", op_name[opcnt], op_code[opcnt]) != EOF)
    {
        opcnt++;
    }
    fclose(fp);
}

void read_input()
{
    char line[100];
    FILE *fp = fopen("sample_input.txt", "r");

    while (fgets(line, sizeof(line), fp))
    {

        if (line[0] == '.')
            continue;

        label[i][0] = opcode[i][0] = operand[i][0] = '\0';

        int count = sscanf(line, "%s %s %s", label[i], opcode[i], operand[i]);

        if (count == 2)
        {
            strcpy(operand[i], opcode[i]);
            strcpy(opcode[i], label[i]);
            strcpy(label[i], "-");
        }

        if (count == 1)
        {
            strcpy(opcode[i], label[i]);
            strcpy(label[i], "-");
            strcpy(operand[i], "-");
        }

        i++;
    }

    fclose(fp);
}
bool find_opcode(char* opcode){
    for (int j = 0; j < opcnt; j++)
    {
        if (strcmp(opcode, op_name[j]) == 0)
            return true;
    }
    return false;
}
void pass_1()
{

    n = i;
    i = 0;

    FILE *fp = fopen("intermediate.txt", "w");

    int LOCCTR = 0;

    while (i < n)
    {

        if (strcmp(opcode[i], "START") == 0)
        {
            start_address = strtol(operand[i], NULL, 16);
            LOCCTR = start_address;
        }

        if (strcmp(label[i], "-") != 0)
        {
            strcpy(sym_label[symcnt], label[i]);
            sym_address[symcnt] = LOCCTR;
            symcnt++;
        }

        fprintf(fp, "%04X %s %s %s\n", LOCCTR, label[i], opcode[i], operand[i]);

        if (strcmp(opcode[i], "END") == 0)
            break;
        else if (find_opcode(opcode[i])==1 && strcmp(opcode[i], "START") != 0)
            LOCCTR += 3;
        else if (strcmp(opcode[i], "WORD") == 0)
            LOCCTR += 3;

        else if (strcmp(opcode[i], "RESW") == 0)
            LOCCTR += 3 * atoi(operand[i]);

        else if (strcmp(opcode[i], "RESB") == 0)
            LOCCTR += atoi(operand[i]);

        else if (strcmp(opcode[i], "BYTE") == 0)
        {

            int length;

            if (operand[i][0] == 'C')
                length = strlen(operand[i]) - 3;

            else if (operand[i][0] == 'X')
                length = (strlen(operand[i]) - 3) / 2;

            LOCCTR += length;
        }
        else error_flag = 1;
        i++;
    }
    prog_len = LOCCTR-start_address;
    fclose(fp);
}

char *get_opcode_value(char *opcode)
{
    for (int j = 0; j < opcnt; j++)
    {
        if (strcmp(opcode, op_name[j]) == 0)
            return op_code[j];
    }
    return NULL;
}

int get_symbol_address(char *symbol)
{
    for (int j = 0; j < symcnt; j++)
    {
        if (strcmp(sym_label[j], symbol) == 0)
            return sym_address[j];
    }
    return -1;
}

void pass_2()
{

    FILE *fp = fopen("intermediate.txt", "r");

    if (fp == NULL)
    {
        printf("Error opening intermediate.txt\n");
        return;
    }

    char label2[20], opcode2[20], operand2[20];
    int address;
    char text_record[100] = "";
    int text_length = 0;
    int text_start = 0;
    while (fscanf(fp, "%x %s %s %s", &address, label2, opcode2, operand2) != EOF)
    {
        if (strcmp(opcode2, "START") == 0)
        {
            printf("H%-6s%06X%06X\n", label2, address, prog_len);
            continue;
        }
        
        if (strcmp(opcode2, "END") == 0)
        {
            if (text_length > 0)
                printf("T%06X%02X%s\n", text_start, text_length, text_record);
                
            printf("E%06X\n", get_symbol_address(operand2));
            break;
        }

        char object_code[20] = "";
        if (strcmp(opcode2, "RSUB") == 0)
        {
            strcpy(object_code, "4C0000");
        }
        else if (strcmp(opcode2, "WORD") == 0)
        {
            sprintf(object_code, "%06X", atoi(operand2));
        }
        else if (strcmp(opcode2, "BYTE") == 0)
        {
            if (operand2[0] == 'C')
            {
                int len = strlen(operand2);
                object_code[0] = '\0';
                for (int i = 2; i < len - 1; i++)
                {
                    char temp[3];
                    sprintf(temp, "%02X", operand2[i]);
                    strcat(object_code, temp);
                }
            }
            else if (operand2[0] == 'X')
            {
                strncpy(object_code, operand2 + 2, strlen(operand2) - 3);
                object_code[strlen(operand2) - 3] = '\0';
            }
        }
        else if (strcmp(opcode2, "RESW") == 0 || strcmp(opcode2, "RESB") == 0)
        {
            if (text_length > 0)
            {
                printf("T%06X%02X%s\n", text_start, text_length, text_record);

                text_length = 0;
                text_record[0] = '\0';
            }
            continue;
        }
        else
        {
            int indexed = 0;
            char temp[20];
            strcpy(temp, operand2);

            char *pos = strstr(temp, ",X");

            if (pos != NULL)
            {
                indexed = 1;
                *pos = '\0';
            }
            char *opcode_value = get_opcode_value(opcode2);

            if (opcode_value == NULL)
                continue;
            int symbol_address = get_symbol_address(temp);

            if (indexed)
                symbol_address += 0x8000;

            sprintf(object_code, "%s%04X", opcode_value, symbol_address);
        }

        int obj_bytes = strlen(object_code) / 2;

        if (text_length == 0)
            text_start = address;

        if (text_length + obj_bytes > 30)
        {
            printf("T%06X%02X%s\n", text_start, text_length, text_record);

            text_length = 0;
            text_record[0] = '\0';
            text_start = address;
        }

        strcat(text_record, object_code);
        text_length += obj_bytes;
    }

    fclose(fp);
}

int main()
{

    load_opcodes();
    read_input();
    pass_1();
    pass_2();
    return 0;
}