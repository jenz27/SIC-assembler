#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

char op_name[50][10];
char op_code[50][10];
int opcnt = 0;

char label[100][20];
char opcode[100][20];
char operand[100][20];

/* " */
typedef struct
{
    char symbol[20];
    int text_index;
} forward_ref;

forward_ref frt[100];
int frtcnt = 0;

typedef struct
{
    char symbol[20];
    int address;
    int defined;
} symtab_entry;

symtab_entry symtab[100];
int symcnt = 0;

typedef struct
{
    int address;
    char object_code[20];
} text_record;

text_record text[500];
int textcnt = 0;

int error_flag = 0;
int prog_len = 0;
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
bool find_opcode(char *opcode)
{
    for (int j = 0; j < opcnt; j++)
    {
        if (strcmp(opcode, op_name[j]) == 0)
            return true;
    }
    return false;
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
int find_symbol(char *symbol)
{
    for (int j = 0; j < symcnt; j++)
    {
        if (strcmp(symtab[j].symbol, symbol) == 0)
            return j;
    }
    return -1;
}
void pass_1()
{

    n = i;
    i = 0;
    int check;
    int LOCCTR = 0;
    char object_code[20] = "";
    while (i < n)
    {

        if (strcmp(opcode[i], "START") == 0)
        {
            start_address = strtol(operand[i], NULL, 16);
            LOCCTR = start_address;
        }
        if (strcmp(label[i], "-") != 0){
            int check = find_symbol(label[i]);
            if (check == -1)
            {
                strcpy(symtab[symcnt].symbol, label[i]);
                symtab[symcnt].address = LOCCTR;
                symtab[symcnt].defined = 1;
                symcnt++;
            }
            else
            {
                symtab[check].defined = 1;
                symtab[check].address = LOCCTR;

                for (int j = 0; j < frtcnt; j++)
                {
                    if (strcmp(frt[j].symbol, label[i]) == 0)
                    {
                        int idx = frt[j].text_index;

                        char opcode_part[3];
                        opcode_part[0] = text[idx].object_code[0];
                        opcode_part[1] = text[idx].object_code[1];
                        opcode_part[2] = '\0';

                        sprintf(text[idx].object_code, "%s%04X",
                                opcode_part,
                                LOCCTR);

                        strcpy(frt[j].symbol, "");
                    }
                }
            }  
        }
        

        if (strcmp(opcode[i], "END") == 0)
            break;
        if (find_opcode(opcode[i]) == 1){
            check = find_symbol(operand[i]);
            char* opcode_value = get_opcode_value(opcode[i]);
            if (check == -1)
            {
                strcpy(symtab[symcnt].symbol, operand[i]);
                symtab[symcnt].address = 0;
                symtab[symcnt].defined = 0;
                strcpy(frt[frtcnt].symbol , operand[i]);
                sprintf(object_code, "%s%04X", opcode_value, 0);
                frt[frtcnt].text_index = textcnt;
                symcnt++;
                frtcnt++;
            }
            else{
                if(symtab[check].defined==0){
                    strcpy(frt[frtcnt].symbol, operand[i]);
                    frt[frtcnt].text_index = textcnt;
                    sprintf(object_code, "%s%04X", opcode_value, 0);
                    frtcnt++;
                }
                else sprintf(object_code, "%s%04X", opcode_value, symtab[check].address);
            }
            strcpy(text[textcnt].object_code ,object_code);
            text[textcnt].address = LOCCTR;
            textcnt++;
            
            LOCCTR += 3;
        }
           
        else if (strcmp(opcode[i], "WORD") == 0){
            sprintf(object_code, "%06X", atoi(operand[i]));
            strcpy(text[textcnt].object_code, object_code);
            text[textcnt].address = LOCCTR;
            textcnt++;
            LOCCTR += 3;
        }
            
        else if (strcmp(opcode[i], "RESW") == 0)
            LOCCTR += 3 * atoi(operand[i]);

        else if (strcmp(opcode[i], "RESB") == 0)
            LOCCTR += atoi(operand[i]);

        else if (strcmp(opcode[i], "BYTE") == 0)
        {
            int length;

            if (operand[i][0] == 'C'){
                length = strlen(operand[i]) - 3;
                object_code[0] = '\0';
                for (int j = 2; j <= length + 1; j++)
                {
                    char temp[3];
                    sprintf(temp, "%02X", operand[i][j]);
                    strcat(object_code, temp);
                }
            }
                
            else if (operand[i][0] == 'X'){
                length = (strlen(operand[i]) - 3) / 2;
                strncpy(object_code, operand[i] + 2, strlen(operand[i]) - 3);
                object_code[strlen(operand[i]) - 3] = '\0';
            }
            strcpy(text[textcnt].object_code, object_code);
            text[textcnt].address = LOCCTR;
            textcnt++;
            LOCCTR += length;
        }
        else
            error_flag = 1;
        i++;
    }
    

    prog_len = LOCCTR - start_address;
}
void print_HTE()
{
    char text_record[100] = "";
    int text_length = 0;
    int text_start = text[0].address;
    int prev_end = text[0].address;

    printf("H%-6s%06X%06X\n", label[0], start_address, prog_len);

    for (int i = 0; i < textcnt; i++)
    {
        char *object_code = text[i].object_code;
        int address = text[i].address;
        int obj_bytes = strlen(object_code) / 2;

        if (obj_bytes == 0)
            continue;

        if (text_length > 0 && address != prev_end)
        {
            printf("T%06X%02X%s\n", text_start, text_length, text_record);

            text_record[0] = '\0';
            text_length = 0;
            text_start = address;
        }

        if (text_length + obj_bytes > 30)
        {
            printf("T%06X%02X%s\n", text_start, text_length, text_record);

            text_record[0] = '\0';
            text_length = 0;
            text_start = address;
        }

        if (text_length == 0)
            text_start = address;

        strcat(text_record, object_code);
        text_length += obj_bytes;
        prev_end = address + obj_bytes;
    }

    if (text_length > 0)
        printf("T%06X%02X%s\n", text_start, text_length, text_record);

    printf("E%06X\n", start_address);
}

int main()
{
    load_opcodes();
    read_input();
    pass_1();
    print_HTE();
    return 0;
}