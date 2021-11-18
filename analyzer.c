#include "analyzer.h"
#include "token.h"
#include <stdio.h>
#include <string.h>

bool analyse(const char *expression)
{
    int type = -2;
    int previous_type = -2;
    int qtdParentesesAberto = 0;
    char aux[(int)(strlen(expression) / sizeof(char)) + 1];
    char symbol[(int)(strlen(expression) / sizeof(char)) + 1];
    strcpy(aux, expression);
    strcpy(symbol, "");
    bool valido = true;

    do
    {
        if (((int)(strlen(aux) / sizeof(char))) > 0 && !get_symbol(aux, symbol))
            return true;

        previous_type = type;

        printf("%s ", symbol);

        type = get_type(aux, symbol, previous_type, qtdParentesesAberto);
        print_type(symbol, type);

        if(type == SYM_PARENTESES)
            qtdParentesesAberto += strncasecmp("(", symbol, strlen(symbol)) == 0 ? 1 : -1;

        if(type == SYM_ERRO)
            valido = false;
    } while ((int)(strlen(aux) / sizeof(char)) > 0);

    type = valid_last_type(type, qtdParentesesAberto);

    if(type == SYM_ERRO) {
        valido = false;
    }

    return valido;
}

// Imprime o simbolo e o tipo correspondente
void print_type(char symbol[], int type)
{

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
    }
}


// retorna o tipo correspondente ao simbolo atual
int get_type(char *expression, char symbol[], int previous_type, int qtdParentesesAberto)
{
    int type = check_type(expression, symbol);

    return valid_type(expression, previous_type, type, symbol, qtdParentesesAberto);
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
int valid_last_type(int type, int qtdParentesesAberto)
{

    if(type == SYM_BINARY_OPERATOR || type == SYM_UNARY_OPERATOR) {
        printf("erro: apos o operador aritmetico faltou numero");
        return -1;
    }

    if(qtdParentesesAberto>0) {
        printf("erro: faltou encerrar parenteses\n");
        return -1;
    }


    if(type != SYM_COMMAND && type != SYM_FLOAT && type != SYM_INTEGER) {
        printf("erro: faltou encerrar a expressão com um numero ou comando\n");
        return -1;<3><enter><5><+><3><enter><5><+>
    }

    return type;
}

// valida lexicamente o tipo correspondente ao simbolo na expressao
int valid_type(char *expression, int previous_type, int type, char symbol[], int qtdParentesesAberto)
{

    if(type == SYM_ERRO) {
        printf("=> erro: simbolo nao reconhecido\n");
        return -1;
    }

    // Verificar se tem parenteses aberto correspodente
    if(type == SYM_PARENTESES && strncasecmp(")", symbol, strlen(symbol)) == 0 && qtdParentesesAberto <= 0) {
        printf("=> erro: nao tem parenteses aberto correspondente\n");
        return -1;
    }

    // Verifica se nao colocou dois enters seguidos
    if(type == SYM_COMMAND && ((int)(strlen(expression) / sizeof(char))) > 0) {
        printf("=> erro: comando tem que ser utilizado no final\n");
        return -1;
    }

    if(previous_type == SYM_UNARY_OPERATOR && (type == SYM_COMMAND || type == SYM_BINARY_OPERATOR || type == SYM_UNARY_OPERATOR)) {
        printf("=> erro: operador aritmetico unario usado incorretamente");
        return -1;
    }


    if(previous_type == SYM_BINARY_OPERATOR && (type == SYM_COMMAND || type == SYM_BINARY_OPERATOR)) {
        printf("=> erro: operador aritmetico binario usado incorretamente");
        return -1;
    }

    // Verifica se colocou dois pontos flutuantes seguidos
    if (previous_type == SYM_FLOAT&& type == SYM_FLOAT){
        printf("=> erro: mais de dois pontos flutuante em um numero\n");
        return -1;
    }

    // Verifica se nao comecou por um operador binario aritmetico
    if (previous_type == -2 && type == SYM_BINARY_OPERATOR){
        printf("=> erro: comecou por um operador binario\n");
        return -1;
    }

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
