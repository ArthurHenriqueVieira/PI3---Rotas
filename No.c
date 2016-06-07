#define Vertex int /*Vertex é o mesmo que int, mas é usado para denominar vértices*/

typedef struct no{
    Vertex ID; /*Numero do vertice*/
    int custo; /*Distancia*/
    struct no *prox; /*Próximo nó da lista*/
} *No;

No novoNo(Vertex ID, No prox,int custo) { /*Retorna um novo nó, de acordo com os parâmetros inseridos (O(1))*/
    No a = malloc( sizeof (struct no));
    a->ID = ID;
    a->custo = custo;
    a->prox = prox;
    return a;
}
