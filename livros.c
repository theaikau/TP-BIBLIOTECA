#include <stdio.h>
#include <stdlib.h>
#include "livros.h"

// Recebe um arquivo e retorna a quantidade de
// livros contidos nele.
int LivrosNoArquivo (FILE *x) {
    fseek(x, 0, SEEK_END);
    int tamanho = ftell(x)/sizeof(LIVRO);
    fseek(x, 0, SEEK_SET);
    return tamanho;
}

// Prepara os dois arquivos iniciais do MergeSort Externo.
// Para isso, recebe um ponteiro para cada arquivo, um ponteiro
// do arquivo lista_de_livros e a capacidade da memória como
// parâmetros da função. No final da execução, os dois arquivos
// iniciais do MergeSort Externo estão com todos os livros da lista
// e o arquivo original é deletado.
void PreparaArquivos (FILE *x1, FILE *x2, FILE *lista_de_livros, int livros_memoria) {

    int turno = 0; // Turno = 0 (Primeiro Arquivo) -  Turno = 1 (Segundo Arquivo)
    int total_livros = LivrosNoArquivo(lista_de_livros); // Total de livros na lista.

    // Aloca memória para ordenação interna baseada na capacidade de memória.
    LIVRO *livros_RAM = (LIVRO*)malloc(sizeof(LIVRO)*(livros_memoria));

    // Lê um bloco de livros do arquivo e registra na memória interna.
    // Logo após, ordena os livros na memória interna e escreve
    // em um arquivo binário. Esse processo é repetido até que todos
    // os blocos máximos sejam lidos.
    for(int i=0; i<(total_livros/livros_memoria); i++) {
        fread(livros_RAM, sizeof(LIVRO), livros_memoria, lista_de_livros);
        OrdenaRAM(livros_RAM, livros_memoria);
        if(turno == 0)
            fwrite(livros_RAM, sizeof(LIVRO), livros_memoria, x1);
        if(turno == 1)
            fwrite(livros_RAM, sizeof(LIVRO), livros_memoria, x2);
        turno = 1 - turno;
    }

    free(livros_RAM);

    // Calcula se há restante de livros (quantidade menor que um bloco de leitura).
    int restante = total_livros%livros_memoria;

    // Se o restante é apenas um livro, apenas realiza a leitura e escreve num arquivo.
    if(restante == 1) {
        LIVRO x;
        fread(&x, sizeof(LIVRO), 1, lista_de_livros);
        if(turno == 0)
            fwrite(&x, sizeof(x), 1, x1);
        if(turno == 1)
            fwrite(&x, sizeof(x), 1, x2);
    }

    // Se o restante é maior que 1, aloca um bloco menor de memória e
    // realiza processo semelhante a primeira etapa da função.
    if (restante > 1) {
        LIVRO *livros_restantes = (LIVRO*)malloc(sizeof(LIVRO)*(restante));
        fread(livros_restantes, sizeof(LIVRO), restante, lista_de_livros);
        OrdenaRAM(livros_restantes, restante);
        if(turno == 0)
            fwrite(livros_restantes, sizeof(LIVRO), restante, x1);
        if(turno == 1)
            fwrite(livros_restantes, sizeof(LIVRO), restante, x2);
        free(livros_restantes);
    }

    remove("lista_de_livros");


}

// Lê um livro em um arquivo binário x e escreve no arquivo binário y.
// O índice é atualizado.
void RegistraRestante(FILE *x, FILE *y, int *i_x) {
    LIVRO aux;
    fread(&aux, sizeof(LIVRO), 1, x);
    fwrite(&aux, sizeof(LIVRO), 1, y);
    *(i_x) = *i_x + 1;
}

// Função Merge da Ordenação Externa.
// Arquivos (x1) e (x2) são os arquivos de entrada.
// Arquivos (y1) e (y2) são os arquivos de saída.
// O bloco é q quantidade de registros de cada arquivo que
// serão intercalados.
void IntercalaArquivos(FILE *x1, FILE *x2, FILE *y1, FILE *y2, int bloco) {
    int i_x1 = 0; // Índice do bloco do arquivo x1.
    int i_x2 = 0; // Índice do bloco do arquivo x2.

    int j_x1 = 0; // Índice global do arquivo x1.
    int j_x2 = 0; // Índice global do arquivo x2.

    int turno = 0; // Turno que indica em qual dos dois arquivos de saída será escrito.

    int leitura_x1 = 1; // Indica se o próximo livro do arquivo x1 deve ser lido na passada do loop.
    int leitura_x2 = 1; // Indica se o próximo livro do arquivo x2 deve ser lido na passada do loop.

    int impresso_x1 = 0; // Indica se o livro do arq. x1 já foi registrado na memória externa.
    int impresso_x2 = 0; // Indica se o livro do arq. x2 já foi registrado na memória externa.

    int tamanho_x1 = LivrosNoArquivo(x1); // Quantidade de livros no arquivo x1.
    int tamanho_x2 = LivrosNoArquivo(x2); // Quantidade de livros no arquivo x2.

    LIVRO a; // Livro buffer do arquivo x1.
    LIVRO b; // Livro buffer do arquivo x2.

    // Loop que intercala até que o primeiro arquivo seja totalmente lido.
    while (j_x1 < tamanho_x1 && j_x2 < tamanho_x2) {

        if(i_x1 < bloco || i_x2 < bloco) {

            if(i_x1 < bloco && i_x2 < bloco) {
                // Lê um livro no arquivo x1 (se a variável leitura_x1 indicar que é necessário).
                if(leitura_x1) fread(&a, sizeof(a), 1, x1);
            }
            else if(i_x1 >= bloco && i_x2 < bloco) {
                // Se o bloco do arquivo x1 terminou, imprime livros do arquivo x2.
                if(turno == 0) fwrite(&b, sizeof(LIVRO), 1, y1);
                if(turno == 1) fwrite(&b, sizeof(LIVRO), 1, y2);
                i_x2++; j_x2++; impresso_x2 = 1;
                if(i_x2 < bloco) fread(&b, sizeof(b), 1, x2);
            }

            if(i_x2 < bloco && i_x1 < bloco) {
                // Lê um livro no arquivo x2 (se a variável leitura_x2 indicar que é necessário).
                if(leitura_x2) fread(&b, sizeof(b), 1, x2);
            }
            else if(i_x2 >= bloco && i_x1 < bloco) {
                // Se o bloco do arquivo x2 terminou, imprime livros do arquivo x1.
                if(turno == 0) fwrite(&a, sizeof(LIVRO), 1, y1);
                if(turno == 1) fwrite(&a, sizeof(LIVRO), 1, y2);
                i_x1++; j_x1++; impresso_x1 = 1;
                if(i_x1 < bloco) fread(&a, sizeof(a), 1, x1);
            }

            if(i_x1 < bloco && i_x2 < bloco) {
                // Compara os títulos e imprime o que vier primeiro em ordem alfabética.
                if(TrocaPermitida(a.titulo, b.titulo)) {
                    if(turno == 0) fwrite(&b, sizeof(LIVRO), 1, y1);
                    if(turno == 1) fwrite(&b, sizeof(LIVRO), 1, y2);
                    i_x2++; j_x2++; leitura_x1 = 0; leitura_x2 = 1;
                    impresso_x2 = 1; impresso_x1 = 0;
                }
                else {
                    if(turno == 0) fwrite(&a, sizeof(LIVRO), 1, y1);
                    if(turno == 1) fwrite(&a, sizeof(LIVRO), 1, y2);
                    i_x1++; j_x1++; leitura_x1 = 1; leitura_x2 = 0;
                    impresso_x2 = 0; impresso_x1 = 1;
                }

            }

        }
        // Se o bloco atual já foi intercalado, começa um novo bloco.
        else if(i_x1 >= bloco && i_x2 >= bloco) {
            turno = 1 - turno;
            i_x1 = 0;
            i_x2 = 0;
            leitura_x1 = 1;
            leitura_x2 = 1;
        }
    }

    // Intercala o restante do arquivo x1, se sobrar.
    while(j_x1 < tamanho_x1) {
        if(i_x1 >= bloco) {
            turno = 1 - turno;
            i_x1 = 0;
        }
        if(impresso_x1 == 1) fread(&a, sizeof(a), 1, x1);
        impresso_x1 = 0;
        if(turno == 0) fwrite(&a, sizeof(LIVRO), 1, y1);
        if(turno == 1) fwrite(&a, sizeof(LIVRO), 1, y2);
        i_x1++; j_x1++;
        if(i_x1 < tamanho_x1) fread(&a, sizeof(a), 1, x1);
    }

    // Intercala o restante do arquivo x2, se sobrar.
    while(j_x2 < tamanho_x2) {
        if(i_x2 >= bloco) {
            turno = 1 - turno;
            i_x2 = 0;
        }
        if(impresso_x2 == 1) fread(&b, sizeof(b), 1, x2);
        impresso_x2 = 0;
        if(turno == 0) fwrite(&b, sizeof(LIVRO), 1, y1);
        if(turno == 1) fwrite(&b, sizeof(LIVRO), 1, y2);
        i_x2++; j_x2++;
        if(i_x2 < tamanho_x2) fread(&b, sizeof(b), 1, x2);
    }

}
// Ordena um arquivo com uma lista de livros.
// lista_de_livros = arquivo com os livros
// livros_memoria = capacidade de armazenamento da memória interna
void OrdenaLivros (FILE *lista_de_livros, int livros_memoria) {
    int tamanho_bloco = livros_memoria; // Tamanho do bloco a ser intercalado.
    int merge_turno = 0; // Define quais serão os arquivos de saída e de entrada.
    int rodada = 1;

    // Prepara o nome dos quatro arquivos necessários para a ordenação.
    char arq_temp_nomes[4][10];

    for(int i=0; i<ARQ_ORD; i++) {
        sprintf(arq_temp_nomes[i], "disc%d.ord", i);
    }

    // Inicializa os quatro ponteiros para os quatro arquivos.
    FILE *x1 = fopen(arq_temp_nomes[0], "wb");
    FILE *x2 = fopen(arq_temp_nomes[1], "wb");
    FILE *y1 = NULL;
    FILE *y2 = NULL;

    // Faz o processamento da lista de livros para os dois primeiros arquivos de saída.
    PreparaArquivos(x1, x2, lista_de_livros, tamanho_bloco);

    fclose(x1);
    fclose(x2);

    // Abre os dois arquivos para leitura.
    x1 = fopen(arq_temp_nomes[0], "rb");
    x2 = fopen(arq_temp_nomes[1], "rb");

    // Registra o tamanho dos arquivos.
    int tamanho_x1 = LivrosNoArquivo(x1);
    int tamanho_x2 = LivrosNoArquivo(x2);


    fclose(x1);
    fclose(x2);

    // Enquanto os dois arquivos de leitura do MergeSort têm livros registrados neles
    // é repetido o loop que intercala os dois arquivos.
    while(tamanho_x1 != 0 && tamanho_x2 != 0) {
        if(merge_turno == 0) {
            // Abre os arquivos de leitura.
            x1 = fopen(arq_temp_nomes[0], "rb");
            x2 = fopen(arq_temp_nomes[1], "rb");

            // Abre os arquivos de registro.
            y1 = fopen(arq_temp_nomes[2], "wb");
            y2 = fopen(arq_temp_nomes[3], "wb");

            if(rodada > 1) tamanho_bloco = tamanho_bloco*2; // Dobra o tamanho do bloco.
            IntercalaArquivos(x1, x2, y1, y2, tamanho_bloco); // Intercala os arquivos.

            fclose(x1);
            fclose(x2);

            fclose(y1);
            fclose(y2);

            // Apaga os livros de leitura.
            remove(arq_temp_nomes[0]);
            remove(arq_temp_nomes[1]);

            // Abre os arquivos de registro para reconhecer o tamanho.
            x1 = fopen(arq_temp_nomes[2], "rb");
            x2 = fopen(arq_temp_nomes[3], "rb");

            tamanho_x1 = LivrosNoArquivo(x1);
            tamanho_x2 = LivrosNoArquivo(x2);


            fclose(x1);
            fclose(x2);

            merge_turno = 1 - merge_turno;
            rodada++;

        }
        else if(merge_turno == 1) {
            // Abre os arquivos de leitura.
            x1 = fopen(arq_temp_nomes[2], "rb");
            x2 = fopen(arq_temp_nomes[3], "rb");

            // Abre os arquivos de registro.
            y1 = fopen(arq_temp_nomes[0], "wb");
            y2 = fopen(arq_temp_nomes[1], "wb");

            if(rodada > 1) tamanho_bloco = tamanho_bloco*2; // Dobra o tamanho do bloco.
            IntercalaArquivos(x1, x2, y1, y2, tamanho_bloco); // Intercala os arquivos.

            fclose(x1);
            fclose(x2);

            fclose(y1);
            fclose(y2);

            // Apaga os livros de leitura.
            remove(arq_temp_nomes[2]);
            remove(arq_temp_nomes[3]);

            // Abre os arquivos de registro para reconhecer o tamanho.
            x1 = fopen(arq_temp_nomes[0], "rb");
            x2 = fopen(arq_temp_nomes[1], "rb");

            tamanho_x1 = LivrosNoArquivo(x1);
            tamanho_x2 = LivrosNoArquivo(x2);

            fclose(x1);
            fclose(x2);

            merge_turno = 1 - merge_turno;
            rodada++;

        }
    }


    // Renomeia o arquivo final para livros_ordenados.
    if(merge_turno == 0) {
        remove(arq_temp_nomes[1]);
        rename(arq_temp_nomes[0], "livros_ordenados");
    }
    else if(merge_turno == 1) {
        remove(arq_temp_nomes[3]);
        rename(arq_temp_nomes[2], "livros_ordenados");
    }

}

// Recebe dois títulos de livro e retorna 0 se o
// título do primeiro parâmetro é o primeiro em ordem
// alfabética e retorna 1 caso contrário.
int TrocaPermitida (char *x, char *y) {
    int i = 0;
    for(i=0; i<TAM_MAX; i++) {
        if (x[i] > y[i] && x[i]!= ' ' && y[i]!= ' ') return 1;
        if (x[i] < y[i] && x[i]!= ' ' && y[i]!= ' ') return 0;
    }
    return 0;
}
// Troca dois livros de posição.
void TrocaLivros (LIVRO *x, LIVRO *y) {
    LIVRO aux = *y;
    *y = *x;
    *x = aux;
}

// Lê uma string de um arquivo de entrada (pode ser a entrada padrão)
// e registra numa variável.
void RegistraString (FILE *entrada, char *destino) {
    fscanf(entrada, "%s", destino);
}

// Lê uma string de um arquivo de entrada (pode ser a entrada padrão)
// e registra numa variável.
void RegistraDigito (FILE *entrada, char *destino) {
    int disponibilidade = 0;
    fscanf(entrada, "%d", &disponibilidade);
    if(disponibilidade == 1) *destino = '1';
    if(disponibilidade == 0) *destino = '0';
}

// Compara se dois títulos são iguais.
int ComparaTitulos (char *t1, char *t2) {
    int i = 0;
    while (t1[i] != '\0' && t2[i] != '\0') {
        if(t1[i] != t2[i]) return 0;
        else i++;
    }
    return 1;
}

// Ordena RAM por Inserção.
void OrdenaRAM (LIVRO *RAM, int livros_memoria) {
    int i, j;
    LIVRO aux;
        for(i = 1; i < livros_memoria; i++) {
            aux = RAM[i];
            j = i - 1;
        while((j >= 0) && TrocaPermitida(RAM[j].titulo, aux.titulo)) {
            RAM[j + 1] = RAM[j];
            j--;
        }
        RAM[j + 1] = aux;
    }
}

// Cria o arquivo índice.
void CriaIndice (FILE **estantes, FILE *indice, int total_estantes) {
    LIVRO buffer;
    int tamanho_estante = 0;
    for(int i=0; i<total_estantes; i++) {
        fseek(estantes[i], 0, SEEK_END);
        tamanho_estante = ftell(estantes[i])/sizeof(LIVRO);
        fseek(estantes[i], 0, SEEK_SET);
        if(tamanho_estante == 0) {
            fprintf(indice, "#\n");
        }
        else if(tamanho_estante == 1) {
            fread(&buffer, sizeof(LIVRO), 1, estantes[i]);
            fprintf(indice, "%s %s\n", buffer.titulo, buffer.titulo);
        }
        else if(tamanho_estante > 1) {
            fseek(estantes[i], 0, SEEK_SET);
            fread(&buffer, sizeof(LIVRO), 1, estantes[i]);
            fprintf(indice, "%s ", buffer.titulo);
            fseek(estantes[i], (tamanho_estante-1)*sizeof(LIVRO), SEEK_SET);
            fread(&buffer, sizeof(LIVRO), 1, estantes[i]);
            fprintf(indice, "%s\n", buffer.titulo);
        }
    }

}

// Efetua uma busca binária na estante.
// Recebe o título e um ponteiro para um inteiro, que é usado para indicar
// a posição que o livro ocupa na estante.
char BuscaBinariaEstante (FILE *estante, char *titulo, int *local) {
    int tamanho_estante = LivrosNoArquivo(estante);
    LIVRO buffer;
    int i_esquerda = 1;
    int i_direita = tamanho_estante;
    int i_meio = ((i_esquerda + i_direita)/2);

    while(i_esquerda < i_direita) {
        fseek(estante, (i_meio-1)*sizeof(LIVRO), SEEK_SET);
        fread(&buffer, sizeof(LIVRO), 1, estante);
        if(ComparaTitulos(titulo, buffer.titulo)) {
            *local = i_meio-1;
            return buffer.disponivel;
        }
        if(TrocaPermitida(titulo, buffer.titulo)) {
            i_esquerda = i_meio+1;
            i_meio = ((i_esquerda + i_direita)/2);
        }
        else {
            i_direita = i_meio-1;
            i_meio = ((i_esquerda + i_direita)/2);
        }
    }
    fseek(estante, (i_meio-1)*sizeof(LIVRO), SEEK_SET);
    fread(&buffer, sizeof(LIVRO), 1, estante);

    if(ComparaTitulos(titulo, buffer.titulo)) {
        *local = i_meio-1;
        return buffer.disponivel;
    }

    return '2';
}

// Busca um livro no acervo.
// Primeiro acessa o arquivo índice e depois faz a busca binária no arquivo da estante.
void BuscaLivro (FILE **estantes, FILE *indice, char *titulo, int total_estantes) {
    char resultado_da_busca = 0;
    char titulo_primeiro[TAM_MAX]; // Título do primeiro livro da estante.
    char titulo_final[TAM_MAX]; // Título do último livro da estante.
    int local = 0; // Posição do livro na estante.
    int i = 0;
    int i_estante = 0;

    // Lê os títulos das estantes e procura correspondência com o título procurado.
    fscanf(indice, "%s", titulo_primeiro);
    fscanf(indice, "%s", titulo_final);
    while(i_estante < total_estantes) {
        if(TrocaPermitida(titulo, titulo_final)) {
            if(ComparaTitulos(titulo, titulo_final)) break;
            fscanf(indice, "%s", titulo_primeiro);
            fscanf(indice, "%s", titulo_final);
            i_estante++;
            i++;
        }
        else break;
    }

    // Se nenhuma estante satisfaz a busca, o livro não está no acervo.
    if(i_estante == total_estantes) {
        printf("livro nao encontrado\n");
        return;
    }


    // Faz a busca binária na estante indicada.
    resultado_da_busca = BuscaBinariaEstante(estantes[i], titulo, &local);

    // Imprime o resultado da busca.

    if(resultado_da_busca == '0')
        printf("emprestado\n");

    if(resultado_da_busca == '1')
        printf("disponivel na posicao %d na estante %d\n", local, i);

    if(resultado_da_busca == '2')
        printf("livro nao encontrado\n");

}

void ImprimeLivro (LIVRO *x) {
    printf("Título: %s | [%c]\n", x->titulo, x->disponivel);
}

void ImprimeArquivo (FILE *x) {
    fseek(x, 0, SEEK_SET);
    LIVRO aux;
    int tamanho = LivrosNoArquivo(x);
    printf("--- ARQUIVO --------------------------------------------------------------------\n");
    for(int i = 0; i<tamanho; i++) {
        fread(&aux, sizeof(LIVRO), 1, x);
        ImprimeLivro(&aux);
    }
    printf("--------------------------------------------------------------------------------\n");
    fseek(x, 0, SEEK_SET);
}
