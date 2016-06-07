#define Vertex int /*Vertex � o mesmo que int, mas � usado para denominar v�rtices*/

typedef struct no{
    Vertex ID; /*Numero do vertice*/
    int custo; /*Distancia*/
    struct no *prox; /*Pr�ximo n� da lista*/
} *No;

No novoNo(Vertex ID, No prox,int custo) { /*Retorna um novo n�, de acordo com os par�metros inseridos (O(1))*/
    No a = malloc( sizeof (struct no));
    a->ID = ID;
    a->custo = custo;
    a->prox = prox;
    return a;
}
