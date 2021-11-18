#include "lexical_analyzer.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

bool analyse(const char *expression)
{
    int type = -2;
    int previous_type = -2;
    bool parentesesAberto = false;
    char aux[(int)(strlen(expression) / sizeof(char)) + 1];
    char symbol[(int)(strlen(expression) / sizeof(char)) + 1];
    strcpy(aux, expression);
    strcpy(symbol, "");

    do
    {
        if (((int)(strlen(aux) / sizeof(char))) > 0 && !get_symbol(aux, symbol))
            return true;

        previous_type = type;
        type = get_type(aux, symbol, previous_type, parentesesAberto);

        print_type(symbol, type);

        if(type == SYM_PARENTESES)
            parentesesAberto = strncasecmp("(", symbol, strlen(symbol)) == 0;
    } while ((int)(strlen(aux) / sizeof(char)) > 0 && type != SYM_ERRO);

    type = valid_last_type(type);

    return type != SYM_ERRO;
}

// Imprime o simbolo e o tipo correspondente
void print_type(char symbol[], int type)
{
    printf("%s ", symbol);

    switch (type)
    {
        case SYM_INTEGER:
            printf("=> número inteiro\n");
            break;
        case SYM_FLOAT:
            printf("=> ponto flutuante\n");
            break;
        case SYM_UNARY_OPERATOR:
            printf("=> operador unário\n");
            break;
        case SYM_BINARY_OPERATOR:
            printf("=> operador binário\n");
            break;
        case SYM_COMMAND:
            printf("=> comando\n");
            break;
        case SYM_PARENTESES:
            printf("=> parenteses\n");
            break;
        case SYM_ERRO:
            printf("=> erro\n");;
    }
}


// retorna o tipo correspondente ao simbolo atual
int get_type(char *expression, char symbol[], int previous_type, bool parentesesAberto)
{
    int type = check_type(expression, symbol);

    return valid_type(previous_type, type, symbol, parentesesAberto);
}

// retorna o tipo correspondente ao simbolo passado
int check_type(char *expression, char symbol[])
{
    char copy[(int)(strlen(expression) / sizeof(char)) + 1];
    strcpy(copy, expression);

    int type = is_sym_number(copy, symbol);
    if (type == -1)
        type = is_sym_operator(symbol);
    if (type == -1)
        type = is_sym_command(symbol);
    if (type == -1)
        type = is_sym_parenteses(symbol);

    strcpy(expression, copy);

    return type;
}

// valida lexicamente o tipo correspondente do ultimo simbolo na expressao
int valid_last_type(int type)
{
    if(type != SYM_COMMAND && type != SYM_FLOAT && type != SYM_INTEGER)
        return -1;

    return type;
}

// valida lexicamente o tipo correspondente ao simbolo na expressao
int valid_type(int previous_type, int type, char symbol[], bool parentesesAberto)
{
    // Verificar se tem dois paranteses aberto
    if(type == SYM_PARENTESES && strncasecmp("(", symbol, strlen(symbol)) == 0 && parentesesAberto)
        type = -1;

    // Verifica se fechou parenteses mas nao abriu ele
    if(type == SYM_PARENTESES && strncasecmp(")", symbol, strlen(symbol)) == 0 && !parentesesAberto)
        type = -1;

    // Verifica se nao colocou dois enters seguidos
    if(previous_type == SYM_COMMAND && type != SYM_COMMAND)
        type = -1;

    // Verifica se nao colocou dois operadores aritmeticos seguidos
    if((previous_type == SYM_BINARY_OPERATOR || previous_type == SYM_UNARY_OPERATOR) && (type == SYM_BINARY_OPERATOR || type == SYM_UNARY_OPERATOR))
        type = -1;

    // Verifica se map colocou dois pontos flutuantes seguidos
    if (previous_type == SYM_FLOAT&& type == SYM_FLOAT)
        type = -1;

    // Verifica se nao comecou por um operador binario aritmetico
    if (previous_type == -2 && type == SYM_BINARY_OPERATOR)
        type = -1;

    return type;
}

// Número: (\+ | \-){0, 1}\d{1, 8}(\.\d{1, 6}){0, 1}(\E(\+ | \-){0, 1}\d{1, 6}){0, 1}
int is_sym_number(char expression[], char symbol[])
{
    char original[(int)(strlen(expression) / sizeof(char)) + 1];
    char number[(int)(strlen(expression) / sizeof(char)) + 1];
    bool is_float = false;        // Indica se é um ponto flutuante
    int counter = 1;              // Conta a quantidade de tokens no número
    strcpy(original, expression); // Salva a string original em casos de não ser um número
    strcpy(number, symbol);

    // Verifica se há um número no início ou após o sinal
    if (!is_number2(symbol))
    {
        strcpy(expression, original);
        strcpy(symbol, number);
        return -1;
    }

    // Enquanto for um número com menos de 9 dígitos, continua
    while (is_number2(symbol) && counter < 9 && (int)(strlen(expression) / sizeof(char)) > 0)
    {
        get_symbol(expression, symbol);
        strcat(number, symbol);
        counter++;
    }

    // Verifica se após o número há um ponto
    if (strcmp(symbol, ".") == 0)
    {
        is_float = true;
        if ((int)strlen(expression) == 0)
        {
            strcpy(expression, original);
            return -1;
        }
        get_symbol(expression, symbol);
        strcat(number, symbol);
        counter++;

        if (!is_number2(symbol))
        {
            strcpy(expression, "");
            strcpy(symbol, number);
            return -1;
        }
    }

    // Enquanto for um número, continua
    while (is_number2(symbol) && (int)(strlen(expression) / sizeof(char)) > 0)
    {
        get_symbol(expression, symbol);
        strcat(number, symbol);
        counter++;
    }

    // Verifica se após o número há uma exponenciação de 10
    if (strcasecmp(symbol, "e") == 0)
    {
        if ((int)strlen(expression) == 0)
        {
            strcpy(expression, original);
            return -1;
        }
        get_symbol(expression, symbol);
        strcat(number, symbol);
        counter++;

        if (strcmp(symbol, "-") == 0 || strcmp(symbol, "+") == 0)
        {
            if ((int)strlen(expression) == 0)
            {
                strcpy(expression, original);
                return -1;
            }
            get_symbol(expression, symbol);
            strcat(number, symbol);
            counter++;
        }

        // Verifica se há um número após o sinal
        if (!is_number2(symbol))
        {
            strcpy(expression, original);
            strcpy(symbol, number);
            return -1;
        }

        // Enquanto for um número, continua
        while (is_number2(symbol) && (int)(strlen(expression) / sizeof(char)) > 0)
        {
            get_symbol(expression, symbol);
            strcat(number, symbol);
            counter++;
        }
    }

    // Reinserção do símbolo removido
    if (!is_number2(symbol))
    {
        strcpy(original, "<");
        strcat(original, symbol);
        strcat(original, ">");
        strcat(original, expression);
        strcpy(expression, original);

        // Remoção do símbolo adicional
        int idx = strlen(number) - strlen(symbol);
        strncpy(symbol, number, idx);
        symbol[idx] = '\0';
    }
    else
    {
        strcpy(symbol, number);
    }

    return is_float ? SYM_FLOAT : SYM_INTEGER;
}

int is_sym_operator(char symbol[])
{
    char aux[((int)(strlen(symbol) / sizeof(char))) + 3];
    strcpy(aux, "");
    strcat(aux, "<");
    strcat(aux, symbol);
    strcat(aux, ">");

    if (!is_operator(aux))
        return -1;
    if (strcasecmp(symbol, "sen") == 0 || strcasecmp(symbol, "cos") == 0)
        return SYM_UNARY_OPERATOR;
    return SYM_BINARY_OPERATOR;
}

int is_sym_command(char symbol[])
{
    char aux[((int)(strlen(symbol) / sizeof(char))) + 3];
    strcpy(aux, "");
    strcat(aux, "<");
    strcat(aux, symbol);
    strcat(aux, ">");

    if (!is_command(aux))
        return -1;
    return SYM_COMMAND;
}

int is_sym_parenteses(char symbol[])
{
    char aux[((int)(strlen(symbol) / sizeof(char))) + 3];
    strcpy(aux, "");
    strcat(aux, "<");
    strcat(aux, symbol);
    strcat(aux, ">");

    if (!is_parenteses(aux))
        return -1;
    return SYM_PARENTESES;
}

// Busca pelo primeiro token da string
bool get_symbol(char expression[], char symbol[])
{
    char *startAtPtr = strstr(expression, "<");                        // Ponteiro para o primeiro '<'
    char *endAtPtr = strstr(expression, ">");                          // Ponteiro para o primeiro '>'
    int startAt = (startAtPtr == NULL ? -1 : startAtPtr - expression); // Índice para o primeiro '<'
    int endAt = (endAtPtr == NULL ? -1 : endAtPtr - expression);       // Índice para o primeiro '>'

    if (startAt != 0)
        return false;
    if (endAt == -1 || startAt == -1)
        return false;
    if (startAt > endAt)
        return false;

    memset(symbol, 0, strlen(symbol));
    strncpy(symbol, expression + 1, ++endAt - 2);
    symbol[endAt - 2] = '\0';

    if ((int)(strlen(expression) / sizeof(char) - endAt) == 0)
    {
        memset(expression, 0, endAt);
    }
    else
    {
        char aux[(int)(strlen(expression) / sizeof(char)) - endAt];
        strncpy(aux, expression + endAt, (int)(strlen(expression) / sizeof(char)));
        strcpy(expression, aux);
    }

    return true;
}

bool is_number2(const char symbol[])
{
    return strstr("0123456789", symbol) != NULL;
}
