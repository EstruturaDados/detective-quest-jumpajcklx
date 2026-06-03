#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAMANHO_HASH 5

// ==========================================
// ESTREUTURAS DE DADOS
// ==========================================

// --- Nível Novato: Árvore Binária de Salas ---
typedef struct Sala {
    char nome[50];
    struct Sala *esquerda;
    struct Sala *direita;
} Sala;

// --- Nível Aventureiro: BST de Pistas ---
typedef struct Pista {
    char texto[100];
    struct Pista *esquerda;
    struct Pista *direita;
} Pista;

// --- Nível Mestre: Tabela Hash de Suspeitos ---
typedef struct NodoPista {
    char texto[100];
    struct NodoPista *proximo;
} NodoPista;

typedef struct Suspeito {
    char nome[50];
    int totalPistas;
    NodoPista *listaPistas; // Lista encadeada de pistas associadas
    struct Suspeito *proximo; // Para tratamento de colisão na Hash
} Suspeito;

Suspeito* tabelaHash[TAMANHO_HASH];

// ==========================================
// FUNÇÕES - NÍVEL NOVATO (SALAS)
// ==========================================
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala*)malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

Sala* montarMansao() {
    Sala *hall = criarSala("Hall de Entrada");
    Sala *biblioteca = criarSala("Biblioteca");
    Sala *cozinha = criarSala("Cozinha");
    Sala *sottao = criarSala("Sótão");
    Sala *escritorio = criarSala("Escritório");
    Sala *jardim = criarSala("Jardim de Inverno");
    Sala *porao = criarSala("Porão Escuro");

    // Montando a árvore fixa
    hall->esquerda = biblioteca;
    hall->direita = cozinha;
    
    biblioteca->esquerda = sottao;
    biblioteca->direita = escritorio;
    
    cozinha->esquerda = jardim;
    cozinha->direita = porao;

    return hall;
}

// ==========================================
// FUNÇÕES - NÍVEL AVENTUREIRO (BST PISTAS)
// ==========================================
Pista* inserirBST(Pista *raiz, const char *texto) {
    if (raiz == NULL) {
        Pista *novo = (Pista*)malloc(sizeof(Pista));
        strcpy(novo->texto, texto);
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }
    if (strcmp(texto, raiz->texto) < 0) {
        raiz->esquerda = inserirBST(raiz->esquerda, texto);
    } else if (strcmp(texto, raiz->texto) > 0) {
        raiz->direita = inserirBST(raiz->direita, texto);
    }
    return raiz;
}

void exibirEmOrdem(Pista *raiz) {
    if (raiz != NULL) {
        exibirEmOrdem(raiz->esquerda);
        printf("- %s\n", raiz->texto);
        exibirEmOrdem(raiz->direita);
    }
}

// ==========================================
// FUNÇÕES - NÍVEL MESTRE (TABELA HASH)
// ==========================================
void inicializarHash() {
    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }
}

int funcaoHash(const char *nome) {
    int soma = 0;
    for (int i = 0; nome[i] != '\0'; i++) {
        soma += nome[i];
    }
    return soma % TAMANHO_HASH;
}

Suspeito* buscarOuInserirSuspeito(const char *nome) {
    int indice = funcaoHash(nome);
    Suspeito *atual = tabelaHash[indice];
    
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->proximo;
    }
    
    // Se não existir, cria o suspeito na tabela (Encaixe no início da lista da colisão)
    Suspeito *novo = (Suspeito*)malloc(sizeof(Suspeito));
    strcpy(novo->nome, nome);
    novo->totalPistas = 0;
    novo->listaPistas = NULL;
    novo->proximo = tabelaHash[indice];
    tabelaHash[indice] = novo;
    return novo;
}

void vincularPistaAoSuspeito(const char *nomeSuspeito, const char *textoPista) {
    Suspeito *s = buscarOuInserirSuspeito(nomeSuspeito);
    
    NodoPista *novaPista = (NodoPista*)malloc(sizeof(NodoPista));
    strcpy(novaPista->texto, textoPista);
    novaPista->proximo = s->listaPistas;
    
    s->listaPistas = novaPista;
    s->totalPistas++;
}

void listarSuspeitos() {
    printf("\n=== DOSSIÊ DE SUSPEITOS (TABELA HASH) ===\n");
    for (int i = 0; i < TAMANHO_HASH; i++) {
        Suspeito *s = tabelaHash[i];
        while (s != NULL) {
            printf("\nSuspeito: %s (Pistas coletadas: %d)\n", s->nome, s->totalPistas);
            NodoPista *p = s->listaPistas;
            while (p != NULL) {
                printf("  ↳ Evidência: %s\n", p->texto);
                p = p->proximo;
            }
            s = s->proximo;
        }
    }
}

void calcularCulpado() {
    Suspeito *maisProvavel = NULL;
    int maxPistas = -1;

    for (int i = 0; i < TAMANHO_HASH; i++) {
        Suspeito *s = tabelaHash[i];
        while (s != NULL) {
            if (s->totalPistas > maxPistas) {
                maxPistas = s->totalPistas;
                maisProvavel = s;
            }
            s = s->proximo;
        }
    }

    printf("\n==========================================\n");
    if (maisProvavel != NULL && maxPistas > 0) {
        printf("🕵️‍♂️ CONCLUSÃO DO CASO:\n");
        printf("O suspeito com mais evidências contra si é: %s!\n", maisProvavel->nome);
        printf("Total de pistas incriminatórias: %d\n", maisProvavel->totalPistas);
    } else {
        printf("🕵️‍♂️ CONCLUSÃO DO CASO:\nNenhuma pista suficiente foi coletada para acusar alguém.\n");
    }
    printf("==========================================\n");
}

// --- Gatilho de automação de eventos do jogo ---
void checarPistasDaSala(Sala *sala, Pista **raizBST) {
    if (strcmp(sala->nome, "Biblioteca") == 0) {
        printf("\n🔍 PISTA ENCONTRADA: Você achou um Diário Rasgado!\n");
        *raizBST = inserirBST(*raizBST, "Diario Rasgado");
        vincularPistaAoSuspeito("Mordomo", "Diario Rasgado descreve reuniões secretas");
    } 
    else if (strcmp(sala->nome, "Sótão") == 0) {
        printf("\n🔍 PISTA ENCONTRADA: Você achou uma Chave Antiga!\n");
        *raizBST = inserirBST(*raizBST, "Chave Antiga");
        vincularPistaAoSuspeito("Coveiro", "Chave Antiga abre o cadeado do galpão");
    } 
    else if (strcmp(sala->nome, "Escritório") == 0) {
        printf("\n🔍 PISTA ENCONTRADA: Você achou uma Apólice de Seguro!\n");
        *raizBST = inserirBST(*raizBST, "Apolice de Seguro");
        vincularPistaAoSuspeito("Mordomo", "Apolice de Seguro coloca ele como beneficiário");
    }
    else if (strcmp(sala->nome, "Porão Escuro") == 0) {
        printf("\n🔍 PISTA ENCONTRADA: Você achou Luvas Sujas de Lama!\n");
        *raizBST = inserirBST(*raizBST, "Luvas de Lama");
        vincularPistaAoSuspeito("Coveiro", "Luvas de Lama com marcas idênticas às do jardim");
        vincularPistaAoSuspeito("Coveiro", "Ferramenta oculta sob as caixas");
    }
}

// ==========================================
// PROGRAMA PRINCIPAL
// ==========================================
int main() {
    inicializarHash();
    Sala *salaAtual = montarMansao();
    Pista *minhasPistas = NULL;
    char comando;

    printf("=========================================\n");
    printf("     DETECTIVE QUEST - MANSÃO WAYNE      \n");
    printf("=========================================\n");

    while (salaAtual != NULL) {
        printf("\n📍 Você está na sala: [%s]\n", salaAtual->nome);
        checarPistasDaSala(salaAtual, &minhasPistas);

        printf("\nPara onde deseja ir?\n");
        if (salaAtual->esquerda != NULL) printf(" -> [e] Esquerda: %s\n", salaAtual->esquerda->nome);
        if (salaAtual->direita != NULL)   printf(" -> [d] Direita: %s\n", salaAtual->direita->nome);
        printf(" -> [r] Revisar evidências (Listar BST)\n");
        printf(" -> [h] Ver dossiê de suspeitos (Tabela Hash)\n");
        printf(" -> [s] Sair da Mansão e Encerrar Caso\n");
        printf("Escolha: ");
        
        scanf(" %c", &comando);

        if (comando == 'e' || comando == 'E') {
            if (salaAtual->esquerda != NULL) salaAtual = salaAtual->esquerda;
            else printf("⚠️ Não há salas para a esquerda!\n");
        } 
        else if (comando == 'd' || comando == 'D') {
            if (salaAtual->direita != NULL) salaAtual = salaAtual->direita;
            else printf("⚠️ Não há salas para a direita!\n");
        } 
        else if (comando == 'r' || comando == 'R') {
            printf("\n--- PISTAS COLETADAS (Ordem Alfabética via BST) ---\n");
            if (minhasPistas == NULL) printf("(Nenhuma pista coletada ainda)\n");
            else exibirEmOrdem(minhasPistas);
            printf("--------------------------------------------------\n");
        } 
        else if (comando == 'h' || comando == 'H') {
            listarSuspeitos();
        } 
        else if (comando == 's' || comando == 'S') {
            break;
        } 
        else {
            printf("⚠️ Comando inválido!\n");
        }
    }

    // Fim do Jogo: Processamento dos dados estruturados
    calcularCulpado();
    printf("\nObrigado por jogar Detective Quest!\n");

    return 0;
}
