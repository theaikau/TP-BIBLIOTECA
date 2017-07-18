#ifndef TP1
#define TP1

#define TAM_MAX 51 // Tamanho máximo do título do livro.
#define ARQ_ORD 4 // Quantidade de arquivos de ordenação externa.

typedef struct LIVRO {
    char titulo[TAM_MAX];
    char disponivel;
}LIVRO;

int LivrosNoArquivo (FILE *x);

void PreparaArquivos (FILE *x1, FILE *x2, FILE *lista_de_livros, int livros_memoria);

void RegistraRestante(FILE *x, FILE *y, int *i_x);

void IntercalaArquivos(FILE *x1, FILE *x2, FILE *y1, FILE *y2, int bloco);

void OrdenaLivros (FILE *lista_de_livros, int livros_memoria);

int TrocaPermitida (char *x, char *y);

void TrocaLivros (LIVRO *x, LIVRO *y);

void RegistraString (FILE *entrada, char *destino);

void RegistraDigito (FILE *entrada, char *destino);

int ComparaTitulos (char *t1, char *t2);

void OrdenaRAM (LIVRO *RAM, int livros_memoria);

void CriaIndice (FILE **estantes, FILE *indice, int total_estantes);

char BuscaBinariaEstante (FILE *estante, char *titulo, int *local);

void BuscaLivro (FILE **estantes, FILE *indice, char *titulo, int total_estantes);

void ImprimeLivro (LIVRO *x);

void ImprimeArquivo (FILE *x);

#endif
