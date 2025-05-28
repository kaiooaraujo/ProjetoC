#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>   // Para getch() e kbhit()
#include <windows.h> // Para cores e sons

// Definições de cores
#define VERMELHO 12
#define VERDE 10
#define AMARELO 14
#define AZUL 9
#define BRANCO 15
#define CIANO 11
#define ROXO 13

// Definições de sons
#define BEEP_SUCCESS 523
#define BEEP_FAILURE 220
#define BEEP_SELECT 349

#define MAX_PERGUNTAS 100
#define TAM_ENUNCIADO 500
#define TAM_ALTERNATIVA 200
#define TOTAL_NIVEIS 5
#define PERGUNTAS_POR_NIVEL 3

typedef struct
{
    char enunciado[TAM_ENUNCIADO];
    char alternativaA[TAM_ALTERNATIVA];
    char alternativaB[TAM_ALTERNATIVA];
    char alternativaC[TAM_ALTERNATIVA];
    char alternativaD[TAM_ALTERNATIVA];
    char respostaCorreta;
    int nivelDificuldade;
} Pergunta;

typedef struct
{
    Pergunta perguntas[PERGUNTAS_POR_NIVEL];
    int usado;
} NivelPerguntas;

Pergunta bancoPerguntas[MAX_PERGUNTAS];
int totalPerguntas = 0;
NivelPerguntas niveis[TOTAL_NIVEIS];
int premios[15] = {1000, 2000, 3000, 5000, 10000, 20000, 30000, 50000,
                   100000, 200000, 300000, 500000, 750000, 900000, 1000000};

// Funções para manipulação de cores
void textcolor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void playSound(int frequency, int duration)
{
    Beep(frequency, duration);
}

void inicializarNiveis()
{
    for (int i = 0; i < TOTAL_NIVEIS; i++)
    {
        niveis[i].usado = 0;
    }
}

void limparBuffer()
{
    while (getchar() != '\n')
        ;
}

void limparTela()
{
    system("cls");
}

void exibirCabecalho(const char *titulo)
{
    textcolor(AZUL);
    printf("\n=== %s ===\n", titulo);
    textcolor(BRANCO);
}

void substituirVirgulas(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] == ',')
        {
            str[i] = ';';
        }
    }
}

void exportarParaCSV()
{
    limparTela();
    exibirCabecalho("EXPORTAR PERGUNTAS PARA CSV");

    if (totalPerguntas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada para exportar!\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    char nomeArquivo[100];
    textcolor(CIANO);
    printf("Digite o nome do arquivo para exportar (sem extensao): ");
    textcolor(BRANCO);
    fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
    nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';

    strcat(nomeArquivo, ".csv");

    FILE *arquivo = fopen(nomeArquivo, "w");
    if (arquivo == NULL)
    {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("Erro ao criar o arquivo %s!\n", nomeArquivo);
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    // Escreve cabeçalho do CSV
    fprintf(arquivo, "Nivel,Dificuldade,Enunciado,Alternativa A,Alternativa B,Alternativa C,Alternativa D,Resposta Correta\n");

    // Escreve cada pergunta
    for (int i = 0; i < totalPerguntas; i++)
    {
        Pergunta p = bancoPerguntas[i];

        // Substitui vírgulas por ponto-e-vírgula nos textos
        substituirVirgulas(p.enunciado);
        substituirVirgulas(p.alternativaA);
        substituirVirgulas(p.alternativaB);
        substituirVirgulas(p.alternativaC);
        substituirVirgulas(p.alternativaD);

        fprintf(arquivo, "%d,", p.nivelDificuldade);

        // Escreve a descrição do nível
        switch (p.nivelDificuldade)
        {
        case 1:
            fprintf(arquivo, "Muito facil,");
            break;
        case 2:
            fprintf(arquivo, "Facil,");
            break;
        case 3:
            fprintf(arquivo, "Medio,");
            break;
        case 4:
            fprintf(arquivo, "Dificil,");
            break;
        case 5:
            fprintf(arquivo, "Muito dificil,");
            break;
        }

        fprintf(arquivo, "\"%s\",", p.enunciado);
        fprintf(arquivo, "\"%s\",", p.alternativaA);
        fprintf(arquivo, "\"%s\",", p.alternativaB);
        fprintf(arquivo, "\"%s\",", p.alternativaC);
        fprintf(arquivo, "\"%s\",", p.alternativaD);
        fprintf(arquivo, "%c\n", p.respostaCorreta);
    }

    fclose(arquivo);

    playSound(BEEP_SUCCESS, 200);
    textcolor(VERDE);
    printf("\n%d perguntas exportadas com sucesso para %s!\n", totalPerguntas, nomeArquivo);
    textcolor(BRANCO);
    printf("\nPressione qualquer tecla para continuar...");
    getch();
}

void menuPrincipal()
{
    limparTela();
    exibirCabecalho("JOGO DO MILHAO - FUTEBOL");

    textcolor(AMARELO);
    printf("1. Gerenciar banco de perguntas\n");
    printf("2. Jogar\n");
    printf("3. Visualizar perguntas por nivel\n");
    printf("4. Sair\n");

    textcolor(VERDE);
    printf("\nEscolha uma opcao: ");
    textcolor(BRANCO);
}

void menuBancoPerguntas()
{
    limparTela();
    exibirCabecalho("BANCO DE PERGUNTAS");

    textcolor(AMARELO);
    printf("1. Cadastrar nova pergunta\n");
    printf("2. Listar todas as perguntas\n");
    printf("3. Pesquisar pergunta\n");
    printf("4. Alterar pergunta\n");
    printf("5. Excluir pergunta\n");
    printf("6. Importar de CSV\n");
    printf("7. Exportar para CSV\n");
    printf("8. Voltar\n");

    textcolor(VERDE);
    printf("\nEscolha uma opcao: ");
    textcolor(BRANCO);
}

void cadastrarPergunta()
{
    limparTela();
    exibirCabecalho("CADASTRO DE NOVA PERGUNTA");

    if (totalPerguntas >= MAX_PERGUNTAS)
    {
        textcolor(VERMELHO);
        printf("Limite maximo de perguntas atingido!\n");
        textcolor(BRANCO);
        return;
    }

    Pergunta nova;

    textcolor(CIANO);
    printf("Enunciado: ");
    textcolor(BRANCO);
    fgets(nova.enunciado, TAM_ENUNCIADO, stdin);
    nova.enunciado[strcspn(nova.enunciado, "\n")] = '\0';

    textcolor(CIANO);
    printf("Alternativa A: ");
    textcolor(BRANCO);
    fgets(nova.alternativaA, TAM_ALTERNATIVA, stdin);
    nova.alternativaA[strcspn(nova.alternativaA, "\n")] = '\0';

    textcolor(CIANO);
    printf("Alternativa B: ");
    textcolor(BRANCO);
    fgets(nova.alternativaB, TAM_ALTERNATIVA, stdin);
    nova.alternativaB[strcspn(nova.alternativaB, "\n")] = '\0';

    textcolor(CIANO);
    printf("Alternativa C: ");
    textcolor(BRANCO);
    fgets(nova.alternativaC, TAM_ALTERNATIVA, stdin);
    nova.alternativaC[strcspn(nova.alternativaC, "\n")] = '\0';

    textcolor(CIANO);
    printf("Alternativa D: ");
    textcolor(BRANCO);
    fgets(nova.alternativaD, TAM_ALTERNATIVA, stdin);
    nova.alternativaD[strcspn(nova.alternativaD, "\n")] = '\0';

    do
    {
        textcolor(CIANO);
        printf("Resposta correta (A/B/C/D): ");
        textcolor(BRANCO);
        scanf(" %c", &nova.respostaCorreta);
        nova.respostaCorreta = toupper(nova.respostaCorreta);
        limparBuffer();
    } while (nova.respostaCorreta != 'A' && nova.respostaCorreta != 'B' &&
             nova.respostaCorreta != 'C' && nova.respostaCorreta != 'D');

    do
    {
        textcolor(CIANO);
        printf("Nivel de dificuldade (1-5):\n");
        textcolor(ROXO);
        printf("1 - Muito facil\n");
        printf("2 - Facil\n");
        printf("3 - Medio\n");
        printf("4 - Dificil\n");
        printf("5 - Muito dificil\n");
        textcolor(CIANO);
        printf("Escolha: ");
        textcolor(BRANCO);
        scanf("%d", &nova.nivelDificuldade);
        limparBuffer();
    } while (nova.nivelDificuldade < 1 || nova.nivelDificuldade > 5);

    bancoPerguntas[totalPerguntas++] = nova;
    playSound(BEEP_SUCCESS, 200);
    textcolor(VERDE);
    printf("\nPergunta cadastrada com sucesso!\n");
    textcolor(BRANCO);
    Sleep(1000);
}

void exibirPergunta(Pergunta p, int numero)
{
    textcolor(AMARELO);
    printf("\nPergunta #%d ", numero);

    switch (p.nivelDificuldade)
    {
    case 1:
        textcolor(VERDE);
        printf("(Muito facil)\n");
        break;
    case 2:
        textcolor(CIANO);
        printf("(Facil)\n");
        break;
    case 3:
        textcolor(AZUL);
        printf("(Medio)\n");
        break;
    case 4:
        textcolor(ROXO);
        printf("(Dificil)\n");
        break;
    case 5:
        textcolor(VERMELHO);
        printf("(Muito dificil)\n");
        break;
    }

    textcolor(BRANCO);
    printf("Enunciado: %s\n", p.enunciado);

    textcolor(VERDE);
    printf("A) %s\n", p.alternativaA);
    textcolor(CIANO);
    printf("B) %s\n", p.alternativaB);
    textcolor(AZUL);
    printf("C) %s\n", p.alternativaC);
    textcolor(ROXO);
    printf("D) %s\n", p.alternativaD);

    textcolor(AMARELO);
    printf("Resposta correta: %c\n", p.respostaCorreta);
    textcolor(BRANCO);
}

void listarPerguntas()
{
    limparTela();
    exibirCabecalho("LISTA DE PERGUNTAS");

    if (totalPerguntas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada.\n");
        textcolor(BRANCO);
        return;
    }

    for (int i = 0; i < totalPerguntas; i++)
    {
        exibirPergunta(bancoPerguntas[i], i + 1);
    }

    printf("\nPressione qualquer tecla para continuar...");
    getch();
}

void pesquisarPergunta()
{
    limparTela();
    exibirCabecalho("PESQUISAR PERGUNTA");

    char termo[TAM_ENUNCIADO];
    int encontradas = 0;

    textcolor(CIANO);
    printf("Digite um termo para pesquisar: ");
    textcolor(BRANCO);
    fgets(termo, TAM_ENUNCIADO, stdin);
    termo[strcspn(termo, "\n")] = '\0';

    textcolor(AMARELO);
    printf("\nResultados da pesquisa por '%s':\n", termo);
    textcolor(BRANCO);

    for (int i = 0; i < totalPerguntas; i++)
    {
        if (strstr(bancoPerguntas[i].enunciado, termo) != NULL ||
            strstr(bancoPerguntas[i].alternativaA, termo) != NULL ||
            strstr(bancoPerguntas[i].alternativaB, termo) != NULL ||
            strstr(bancoPerguntas[i].alternativaC, termo) != NULL ||
            strstr(bancoPerguntas[i].alternativaD, termo) != NULL)
        {

            exibirPergunta(bancoPerguntas[i], i + 1);
            encontradas++;
        }
    }

    if (encontradas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta encontrada com o termo '%s'.\n", termo);
        textcolor(BRANCO);
    }
    else
    {
        textcolor(VERDE);
        printf("\nTotal de perguntas encontradas: %d\n", encontradas);
        textcolor(BRANCO);
    }

    printf("\nPressione qualquer tecla para continuar...");
    getch();
}

void alterarPergunta()
{
    limparTela();
    exibirCabecalho("ALTERAR PERGUNTA");

    if (totalPerguntas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada para alterar.\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    listarPerguntas();

    textcolor(CIANO);
    printf("\nDigite o numero da pergunta que deseja alterar (1-%d): ", totalPerguntas);
    textcolor(BRANCO);
    int num;
    scanf("%d", &num);
    limparBuffer();

    if (num < 1 || num > totalPerguntas)
    {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("Numero de pergunta invalido!\n");
        textcolor(BRANCO);
        Sleep(1000);
        return;
    }

    Pergunta *p = &bancoPerguntas[num - 1];

    textcolor(AMARELO);
    printf("\nAlterando pergunta #%d\n", num);
    textcolor(BRANCO);

    printf("Novo enunciado [atual: %s]: ", p->enunciado);
    fgets(p->enunciado, TAM_ENUNCIADO, stdin);
    p->enunciado[strcspn(p->enunciado, "\n")] = '\0';

    printf("Nova alternativa A [atual: %s]: ", p->alternativaA);
    fgets(p->alternativaA, TAM_ALTERNATIVA, stdin);
    p->alternativaA[strcspn(p->alternativaA, "\n")] = '\0';

    printf("Nova alternativa B [atual: %s]: ", p->alternativaB);
    fgets(p->alternativaB, TAM_ALTERNATIVA, stdin);
    p->alternativaB[strcspn(p->alternativaB, "\n")] = '\0';

    printf("Nova alternativa C [atual: %s]: ", p->alternativaC);
    fgets(p->alternativaC, TAM_ALTERNATIVA, stdin);
    p->alternativaC[strcspn(p->alternativaC, "\n")] = '\0';

    printf("Nova alternativa D [atual: %s]: ", p->alternativaD);
    fgets(p->alternativaD, TAM_ALTERNATIVA, stdin);
    p->alternativaD[strcspn(p->alternativaD, "\n")] = '\0';

    do
    {
        printf("Nova resposta correta (A/B/C/D) [atual: %c]: ", p->respostaCorreta);
        scanf(" %c", &p->respostaCorreta);
        p->respostaCorreta = toupper(p->respostaCorreta);
        limparBuffer();
    } while (p->respostaCorreta != 'A' && p->respostaCorreta != 'B' &&
             p->respostaCorreta != 'C' && p->respostaCorreta != 'D');

    do
    {
        printf("Novo nivel de dificuldade (1-5) [atual: %d]: ", p->nivelDificuldade);
        scanf("%d", &p->nivelDificuldade);
        limparBuffer();
    } while (p->nivelDificuldade < 1 || p->nivelDificuldade > 5);

    playSound(BEEP_SUCCESS, 200);
    textcolor(VERDE);
    printf("Pergunta alterada com sucesso!\n");
    textcolor(BRANCO);
    Sleep(1000);
}

void excluirPergunta()
{
    limparTela();
    exibirCabecalho("EXCLUIR PERGUNTA");

    if (totalPerguntas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada para excluir.\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    listarPerguntas();

    textcolor(CIANO);
    printf("\nDigite o numero da pergunta que deseja excluir (1-%d): ", totalPerguntas);
    textcolor(BRANCO);
    int num;
    scanf("%d", &num);
    limparBuffer();

    if (num < 1 || num > totalPerguntas)
    {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("Numero de pergunta invalido!\n");
        textcolor(BRANCO);
        Sleep(1000);
        return;
    }

    textcolor(AMARELO);
    printf("\nPergunta a ser excluida:\n");
    textcolor(BRANCO);
    printf("Enunciado: %s\n", bancoPerguntas[num - 1].enunciado);

    textcolor(VERMELHO);
    printf("Tem certeza que deseja excluir esta pergunta? (S/N): ");
    textcolor(BRANCO);
    char confirmacao;
    scanf(" %c", &confirmacao);
    limparBuffer();

    if (toupper(confirmacao) == 'S')
    {
        for (int i = num - 1; i < totalPerguntas - 1; i++)
        {
            bancoPerguntas[i] = bancoPerguntas[i + 1];
        }

        totalPerguntas--;
        playSound(BEEP_SUCCESS, 200);
        textcolor(VERDE);
        printf("Pergunta excluida com sucesso!\n");
        textcolor(BRANCO);
    }
    else
    {
        playSound(BEEP_FAILURE, 200);
        textcolor(AMARELO);
        printf("Operacao cancelada.\n");
        textcolor(BRANCO);
    }
    Sleep(1000);
}

void importarDeCSV() {
    limparTela();
    exibirCabecalho("IMPORTAR PERGUNTAS DE CSV");

    char nomeArquivo[100];
    textcolor(CIANO);
    printf("Digite o nome do arquivo CSV para importar (com extensao): ");
    textcolor(BRANCO);
    fgets(nomeArquivo, sizeof(nomeArquivo), stdin);
    nomeArquivo[strcspn(nomeArquivo, "\n")] = '\0';

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("Erro ao abrir o arquivo %s!\n", nomeArquivo);
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    char linha[1024]; // Buffer para cada linha do CSV
    int perguntasImportadas = 0;
    int linhaAtual = 0;

    while (fgets(linha, sizeof(linha), arquivo) && totalPerguntas < MAX_PERGUNTAS) {
        linhaAtual++;
        
        // Pular linha de cabeçalho
        if (linhaAtual == 1) continue;

        // Remover quebra de linha no final
        linha[strcspn(linha, "\r\n")] = '\0';

        Pergunta nova;
        char *campo;
        int numCampos = 0;
        
        // Campos esperados: Nivel, Dificuldade, Enunciado, A, B, C, D, Resposta
        campo = strtok(linha, ",");
        while (campo != NULL && numCampos < 8) {
            // Remover aspas se existirem
            if (campo[0] == '"' && campo[strlen(campo)-1] == '"') {
                memmove(campo, campo+1, strlen(campo));
                campo[strlen(campo)-1] = '\0';
            }

            switch(numCampos) {
                case 0: // Nível
                    nova.nivelDificuldade = atoi(campo);
                    break;
                case 2: // Enunciado
                    strncpy(nova.enunciado, campo, TAM_ENUNCIADO);
                    nova.enunciado[TAM_ENUNCIADO-1] = '\0';
                    break;
                case 3: // Alternativa A
                    strncpy(nova.alternativaA, campo, TAM_ALTERNATIVA);
                    nova.alternativaA[TAM_ALTERNATIVA-1] = '\0';
                    break;
                case 4: // Alternativa B
                    strncpy(nova.alternativaB, campo, TAM_ALTERNATIVA);
                    nova.alternativaB[TAM_ALTERNATIVA-1] = '\0';
                    break;
                case 5: // Alternativa C
                    strncpy(nova.alternativaC, campo, TAM_ALTERNATIVA);
                    nova.alternativaC[TAM_ALTERNATIVA-1] = '\0';
                    break;
                case 6: // Alternativa D
                    strncpy(nova.alternativaD, campo, TAM_ALTERNATIVA);
                    nova.alternativaD[TAM_ALTERNATIVA-1] = '\0';
                    break;
                // Modifique esta parte da função de importação
                case 7: // Resposta Correta
                    // Aceita resposta em qualquer posição da string e converte para maiúscula
                    for(int i = 0; campo[i]; i++) {
                        char c = toupper(campo[i]);
                        if(c == 'A' || c == 'B' || c == 'C' || c == 'D') {
                            nova.respostaCorreta = c;
                            break;
        }
    }
    break;
            }
            
            numCampos++;
            campo = strtok(NULL, ",");
        }

        // Validar os dados
        if (numCampos == 8 && 
            nova.nivelDificuldade >= 1 && nova.nivelDificuldade <= 5 &&
            (nova.respostaCorreta == 'A' || nova.respostaCorreta == 'B' || 
             nova.respostaCorreta == 'C' || nova.respostaCorreta == 'D')) {
            
            bancoPerguntas[totalPerguntas++] = nova;
            perguntasImportadas++;
        } else {
            textcolor(AMARELO);
            printf("Aviso: Linha %d ignorada - formato inválido ou dados incorretos\n", linhaAtual);
            printf("Campos encontrados: %d, Nível: %d, Resposta: %c\n", 
                  numCampos, nova.nivelDificuldade, nova.respostaCorreta);
            textcolor(BRANCO);
        }
    }

    fclose(arquivo);
    
    if (perguntasImportadas > 0) {
        playSound(BEEP_SUCCESS, 200);
        textcolor(VERDE);
        printf("\n%d perguntas importadas com sucesso de %s!\n", perguntasImportadas, nomeArquivo);
    } else {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("\nNenhuma pergunta válida foi encontrada no arquivo %s!\n", nomeArquivo);
        printf("Verifique se o formato do arquivo está correto.\n");
    }
    
    textcolor(BRANCO);
    printf("\nPressione qualquer tecla para continuar...");
    getch();
}

void gerenciarBancoPerguntas()
{
    int opcao;

    do
    {
        menuBancoPerguntas();
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao)
        {
        case 1:
            cadastrarPergunta();
            break;
        case 2:
            listarPerguntas();
            break;
        case 3:
            pesquisarPergunta();
            break;
        case 4:
            alterarPergunta();
            break;
        case 5:
            excluirPergunta();
            break;
        case 6:
            importarDeCSV();
            break;
        case 7:
            exportarParaCSV();
            break;
        case 8:
            playSound(BEEP_SELECT, 100);
            printf("Retornando ao menu principal...\n");
            Sleep(500);
            break;
        default:
            playSound(BEEP_FAILURE, 200);
            textcolor(VERMELHO);
            printf("Opcao invalida! Tente novamente.\n");
            textcolor(BRANCO);
            Sleep(1000);
        }
    } while (opcao != 8);
}

int selecionarPerguntaPorNivel(int nivel)
{
    int perguntasDisponiveis[MAX_PERGUNTAS];
    int totalDisponiveis = 0;

    for (int i = 0; i < totalPerguntas; i++)
    {
        if (bancoPerguntas[i].nivelDificuldade == nivel)
        {
            perguntasDisponiveis[totalDisponiveis++] = i;
        }
    }

    if (totalDisponiveis == 0)
    {
        textcolor(VERMELHO);
        printf("Nao ha perguntas cadastradas para o nivel %d!\n", nivel);
        textcolor(BRANCO);
        return -1;
    }

    srand(time(NULL));
    int tentativas = 0;
    int indice;

    while (tentativas < totalDisponiveis)
    {
        indice = rand() % totalDisponiveis;
        int perguntaIndex = perguntasDisponiveis[indice];

        int jaUsada = 0;
        for (int i = 0; i < TOTAL_NIVEIS; i++)
        {
            for (int j = 0; j < PERGUNTAS_POR_NIVEL; j++)
            {
                if (niveis[i].usado && memcmp(&niveis[i].perguntas[j], &bancoPerguntas[perguntaIndex], sizeof(Pergunta)) == 0)
                {
                    jaUsada = 1;
                    break;
                }
            }
            if (jaUsada)
                break;
        }

        if (!jaUsada)
        {
            return perguntaIndex;
        }

        tentativas++;
    }

    textcolor(VERMELHO);
    printf("Todas as perguntas do nivel %d ja foram usadas!\n", nivel);
    textcolor(BRANCO);
    return -1;
}

void prepararJogo()
{
    inicializarNiveis();

    for (int nivel = 1; nivel <= TOTAL_NIVEIS; nivel++)
    {
        for (int i = 0; i < PERGUNTAS_POR_NIVEL; i++)
        {
            int index = selecionarPerguntaPorNivel(nivel);
            if (index != -1)
            {
                niveis[nivel - 1].perguntas[i] = bancoPerguntas[index];
                niveis[nivel - 1].usado = 1;
            }
            else
            {
                strcpy(niveis[nivel - 1].perguntas[i].enunciado, "Pergunta nao disponivel");
                strcpy(niveis[nivel - 1].perguntas[i].alternativaA, "A");
                strcpy(niveis[nivel - 1].perguntas[i].alternativaB, "B");
                strcpy(niveis[nivel - 1].perguntas[i].alternativaC, "C");
                strcpy(niveis[nivel - 1].perguntas[i].alternativaD, "D");
                niveis[nivel - 1].perguntas[i].respostaCorreta = 'A';
                niveis[nivel - 1].perguntas[i].nivelDificuldade = nivel;
            }
        }
    }
}

void exibirPremio(int valor)
{
    if (valor == 1000000)
    {
        textcolor(AMARELO);
        printf("R$%d\n", valor);
        textcolor(BRANCO);
    }
    else if (valor >= 500000)
    {
        textcolor(ROXO);
        printf("R$%d\n", valor);
        textcolor(BRANCO);
    }
    else if (valor >= 50000)
    {
        textcolor(AZUL);
        printf("R$%d\n", valor);
        textcolor(BRANCO);
    }
    else if (valor >= 5000)
    {
        textcolor(CIANO);
        printf("R$%d\n", valor);
        textcolor(BRANCO);
    }
    else
    {
        printf("R$%d\n", valor);
    }
}

void jogar()
{
    if (totalPerguntas < 15)
    {
        playSound(BEEP_FAILURE, 200);
        textcolor(VERMELHO);
        printf("Eh necessario ter pelo menos 15 perguntas cadastradas para jogar!\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    prepararJogo();

    limparTela();
    textcolor(AMARELO);
    printf("\n=== JOGO DO MILHAO - FUTEBOL ===\n");
    textcolor(BRANCO);
    printf("Voce esta prestes a comecar o jogo!\n");
    printf("Sao 15 perguntas divididas em 5 niveis de dificuldade.\n");
    printf("Boa sorte!\n\n");

    printf("Preparado? (Enter para comecar)");
    getchar();

    int premioAtual = 0;
    int perguntaAtual = 0;

    for (int nivel = 0; nivel < TOTAL_NIVEIS; nivel++)
    {
        for (int i = 0; i < PERGUNTAS_POR_NIVEL; i++)
        {
            perguntaAtual++;
            Pergunta p = niveis[nivel].perguntas[i];

            limparTela();
            textcolor(AMARELO);
            printf("\nPergunta #%d ", perguntaAtual);
            printf("(Valor: ");
            exibirPremio(premios[perguntaAtual - 1]);
            printf(")\n");

            switch (nivel + 1)
            {
            case 1:
                textcolor(VERDE);
                printf("Nivel: Muito facil\n");
                break;
            case 2:
                textcolor(CIANO);
                printf("Nivel: Facil\n");
                break;
            case 3:
                textcolor(AZUL);
                printf("Nivel: Medio\n");
                break;
            case 4:
                textcolor(ROXO);
                printf("Nivel: Dificil\n");
                break;
            case 5:
                textcolor(VERMELHO);
                printf("Nivel: Muito dificil\n");
                break;
            }

            textcolor(BRANCO);
            printf("\n%s\n", p.enunciado);
            textcolor(VERDE);
            printf("\nA) %s\n", p.alternativaA);
            textcolor(CIANO);
            printf("B) %s\n", p.alternativaB);
            textcolor(AZUL);
            printf("C) %s\n", p.alternativaC);
            textcolor(ROXO);
            printf("D) %s\n", p.alternativaD);

            char resposta;
            do
            {
                textcolor(AMARELO);
                printf("\nSua resposta (A/B/C/D ou S para sair): ");
                textcolor(BRANCO);
                resposta = toupper(getch());
                printf("%c\n", resposta);

                if (resposta == 'S')
                {
                    textcolor(AMARELO);
                    printf("\nVoce decidiu sair do jogo com ");
                    exibirPremio(premioAtual);
                    textcolor(BRANCO);
                    printf("\nPressione qualquer tecla para continuar...");
                    getch();
                    return;
                }
            } while (resposta != 'A' && resposta != 'B' &&
                     resposta != 'C' && resposta != 'D');

            if (resposta == p.respostaCorreta)
            {
                premioAtual = premios[perguntaAtual - 1];
                playSound(BEEP_SUCCESS, 300);
                textcolor(VERDE);
                printf("\nRESPOSTA CORRETA! Voce agora tem ");
                exibirPremio(premioAtual);
                textcolor(BRANCO);

                if (perguntaAtual == 5 || perguntaAtual == 10)
                {
                    playSound(BEEP_SUCCESS, 500);
                    textcolor(AMARELO);
                    printf("\nPARABENS! Voce atingiu o marco de seguranca de ");
                    exibirPremio(premioAtual);
                    printf("Agora voce nao sai do jogo com menos que este valor.\n");
                    textcolor(BRANCO);
                }
            }
            else
            {
                playSound(BEEP_FAILURE, 500);
                textcolor(VERMELHO);
                printf("\nRESPOSTA ERRADA! A resposta correta era %c.\n", p.respostaCorreta);

                if (perguntaAtual > 5 && perguntaAtual <= 10)
                {
                    printf("Voce sai com ");
                    exibirPremio(5000);
                }
                else if (perguntaAtual > 10)
                {
                    printf("Voce sai com ");
                    exibirPremio(50000);
                }
                else
                {
                    printf("Voce sai com R$0\n");
                }

                textcolor(BRANCO);
                printf("\nFIM DE JOGO!\n");
                printf("Seu premio final: ");
                exibirPremio(perguntaAtual > 5 ? (perguntaAtual > 10 ? 50000 : 5000) : 0);
                printf("\nPressione qualquer tecla para continuar...");
                getch();
                return;
            }

            if (perguntaAtual < 15)
            {
                printf("\nPreparado para a proxima pergunta? (Enter para continuar)");
                getchar();
            }
        }
    }

    playSound(BEEP_SUCCESS, 1000);
    textcolor(AMARELO);
    printf("\nPARABENS! VOCE GANHOU O JOGO DO MILHAO!\n");
    printf("Premio final: ");
    exibirPremio(1000000);
    textcolor(BRANCO);
    printf("\nPressione qualquer tecla para continuar...");
    getch();
}

void visualizarPorNivel()
{
    limparTela();
    exibirCabecalho("VISUALIZAR PERGUNTAS POR NIVEL");

    if (totalPerguntas == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada.\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    int nivel;
    do
    {
        textcolor(CIANO);
        printf("Selecione o nivel:\n");
        textcolor(ROXO);
        printf("1 - Muito facil\n");
        printf("2 - Facil\n");
        printf("3 - Medio\n");
        printf("4 - Dificil\n");
        printf("5 - Muito dificil\n");
        printf("0 - Voltar\n");
        textcolor(CIANO);
        printf("Escolha: ");
        textcolor(BRANCO);
        scanf("%d", &nivel);
        limparBuffer();

        if (nivel == 0)
            return;
    } while (nivel < 1 || nivel > 5);

    int pagina = 0;
    int perguntasNivel = 0;
    Pergunta perguntas[MAX_PERGUNTAS];

    for (int i = 0; i < totalPerguntas; i++)
    {
        if (bancoPerguntas[i].nivelDificuldade == nivel)
        {
            perguntas[perguntasNivel++] = bancoPerguntas[i];
        }
    }

    if (perguntasNivel == 0)
    {
        textcolor(VERMELHO);
        printf("Nenhuma pergunta cadastrada para este nivel.\n");
        textcolor(BRANCO);
        printf("\nPressione qualquer tecla para continuar...");
        getch();
        return;
    }

    do
    {
        limparTela();
        exibirCabecalho("PERGUNTAS POR NIVEL");

        textcolor(AMARELO);
        printf("Nivel: ");
        switch (nivel)
        {
        case 1:
            textcolor(VERDE);
            printf("Muito facil\n\n");
            break;
        case 2:
            textcolor(CIANO);
            printf("Facil\n\n");
            break;
        case 3:
            textcolor(AZUL);
            printf("Medio\n\n");
            break;
        case 4:
            textcolor(ROXO);
            printf("Dificil\n\n");
            break;
        case 5:
            textcolor(VERMELHO);
            printf("Muito dificil\n\n");
            break;
        }
        textcolor(BRANCO);

        int inicio = pagina * 3;
        int fim = (inicio + 3) > perguntasNivel ? perguntasNivel : (inicio + 3);

        for (int i = inicio; i < fim; i++)
        {
            exibirPergunta(perguntas[i], i + 1);
            printf("\n");
        }

        textcolor(AMARELO);
        printf("\nPerguntas %d-%d de %d\n", inicio + 1, fim, perguntasNivel);
        textcolor(BRANCO);

        if (pagina > 0)
        {
            printf("(A) Anterior | ");
        }
        if (fim < perguntasNivel)
        {
            printf("(P) Proxima | ");
        }
        printf("(V) Voltar");

        char opcao;
        do
        {
            opcao = toupper(getch());
        } while ((pagina == 0 && opcao == 'A') ||
                 (fim == perguntasNivel && opcao == 'P') ||
                 (opcao != 'A' && opcao != 'P' && opcao != 'V'));

        playSound(BEEP_SELECT, 100);

        if (opcao == 'A' && pagina > 0)
        {
            pagina--;
        }
        else if (opcao == 'P' && fim < perguntasNivel)
        {
            pagina++;
        }
        else if (opcao == 'V')
        {
            break;
        }
    } while (1);
}


int main()
{
    int opcao;

    // Cria alguns dados de exemplo
    if (totalPerguntas == 0)
    {
        strcpy(bancoPerguntas[0].enunciado, "Qual time ganhou a Copa do Mundo de 2002?");
        strcpy(bancoPerguntas[0].alternativaA, "Brasil");
        strcpy(bancoPerguntas[0].alternativaB, "Alemanha");
        strcpy(bancoPerguntas[0].alternativaC, "Franca");
        strcpy(bancoPerguntas[0].alternativaD, "Italia");
        bancoPerguntas[0].respostaCorreta = 'A';
        bancoPerguntas[0].nivelDificuldade = 1;
        totalPerguntas++;

        strcpy(bancoPerguntas[1].enunciado, "Quem e o maior artilheiro da historia da Selecao Brasileira?");
        strcpy(bancoPerguntas[1].alternativaA, "Pele");
        strcpy(bancoPerguntas[1].alternativaB, "Ronaldo");
        strcpy(bancoPerguntas[1].alternativaC, "Neymar");
        strcpy(bancoPerguntas[1].alternativaD, "Romario");
        bancoPerguntas[1].respostaCorreta = 'D';
        bancoPerguntas[1].nivelDificuldade = 2;
        totalPerguntas++;
    }

    inicializarNiveis();

    do
    {
        menuPrincipal();
        scanf("%d", &opcao);
        limparBuffer();

        switch (opcao)
        {
        case 1:
            gerenciarBancoPerguntas();
            break;
        case 2:
            jogar();
            break;
        case 3:
            visualizarPorNivel();
            break;
        case 4:
            playSound(BEEP_SELECT, 100);
            textcolor(AMARELO);
            printf("Saindo do jogo...\n");
            textcolor(BRANCO);
            Sleep(1000);
            break;
        default:
            playSound(BEEP_FAILURE, 200);
            textcolor(VERMELHO);
            printf("Opcao invalida! Tente novamente.\n");
            textcolor(BRANCO);
            Sleep(1000);
        }
    } while (opcao != 6);

    return 0;
}

