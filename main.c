#include <stdio.h>
#include <stdlib.h>
#include "livros.h"

int main() {

// --- Declarações iniciais e leituras iniciais ---
    int i;
    LIVRO buffer;

    int total_livros = 0;
    int livros_memoria = 0;
    int total_estantes = 0;
    int estantes_capacidade = 0;
    int total_consultas = 0;

    fscanf(stdin, "%d", &total_livros);
    fscanf(stdin, "%d", &livros_memoria);
    fscanf(stdin, "%d", &total_estantes);
    fscanf(stdin, "%d", &estantes_capacidade);
    fscanf(stdin, "%d", &total_consultas);

    // Para usar apenas o necessário na memória principal.
    if(total_livros < livros_memoria) {
        livros_memoria = total_livros;
    }

// --- Criação do arquivo lista_de_livros ---

    FILE *lista_de_livros = fopen("lista_de_livros", "wb");

    for(i=0; i<total_livros; i++) {
        RegistraString(stdin, buffer.titulo);
        RegistraDigito(stdin, &buffer.disponivel);
        fwrite(&buffer, sizeof(buffer), 1, lista_de_livros);
    }

    fclose(lista_de_livros);

// --- Ordenação do arquivo lista_de_livros ---

    lista_de_livros = fopen("lista_de_livros", "rb");

    OrdenaLivros(lista_de_livros, livros_memoria);

// --- Criação e distribuição dos livros nas estantes ---

    FILE *livros_ordenados = fopen("livros_ordenados", "rb");

    // Ponteiros para os arquivos das estantes.
    FILE **estantes = (FILE**)malloc(sizeof(FILE)*(total_estantes));
    if(estantes == NULL) {
        printf("Memória indisponível para alocação.\n");
        return -1;
    }

    // Strings para nomes dos arquivos das estantes.
    char **estantes_nomes = (char**)malloc(sizeof(char*)*(total_estantes));
    if(estantes_nomes == NULL) {
        printf("Memória indisponível para alocação.\n");
        return -1;
    }

    for(int i=0; i<total_estantes; i++) {
        estantes_nomes[i] = (char*)malloc(sizeof(char)*(20));
        if(estantes_nomes[i] == NULL) {
            printf("Memória indisponível para alocação.\n");
            return -1;
        }
        sprintf(estantes_nomes[i], "estante%d", i);
        estantes[i] = fopen(estantes_nomes[i], "wb");
    }

    FILE *livros_ordenados_nb = fopen("temp_name", "w");

    int k = 0;
    for(int i=0; i<total_estantes; i++) {
        for(int j=0; j<estantes_capacidade; j++) {
            if(k < total_livros) {
            fread(&buffer, sizeof(LIVRO), 1, livros_ordenados);
            fprintf(livros_ordenados_nb, "%s %c\n", buffer.titulo, buffer.disponivel);
            fwrite(&buffer, sizeof(LIVRO), 1, estantes[i]);
            k++;
            }
        }
        fclose(estantes[i]);
        estantes[i] = fopen(estantes_nomes[i], "rb");
    }

// --- Criação do arquivo índice ---

    FILE *indice = NULL;
    indice = fopen("indice", "w");

    CriaIndice(estantes, indice, total_estantes);

// --- Processamento das consultas ao acervo ---

    for(int i=0; i<total_consultas; i++) {
        fclose(indice);
        fscanf(stdin, "%s", buffer.titulo);
        indice = fopen("indice", "r");
        BuscaLivro(estantes, indice, buffer.titulo, total_estantes);
    }

// --- Encerramento da execução ---

    for(int i=0; i<total_estantes; i++) {
        fclose(estantes[i]);
        free(estantes_nomes[i]);
    }

    remove("livros_ordenados");
    rename("temp_name", "livros_ordenados");

    fclose(livros_ordenados_nb);
    fclose(indice);
    fclose(livros_ordenados);
    fclose(lista_de_livros);
    free(estantes_nomes);
    free(estantes);

    return 0;
}
