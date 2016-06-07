#include "Digrafo.c"

/* Recebe digrafo G com custos positivos nos arcos e um vértice s. Calcula uma SPT com raiz s e armazena a SPT no vetor parent. As distâncias a partir de s são armazenadas no vetor dist.

O digrafo é representado por suas listas de adjacência. A função supõe que a soma dos custos de todos os arcos é estritamente menor que a constante INFINITO. Supõe também que o digrafo tem no máximo maxV vértices.*/

void dijkstra( Digrafo G){ /*Algoritmo de Dijkstra*/
   Vertex v0, w, frj[100];
   No a; int c,i;
   for(i=1;i<G->V;i++){
       initialize( G, i, G->parent[i], G->dist[i], frj);

       while (1) {
          int mindist = INFINITO;
          for (w = 0; w < G->V; w++){
             if (G->parent[i][w] == -1 && mindist > G->dist[i][w])
                mindist = G->dist[i][v0=w];
          }
          if (mindist == INFINITO) break;
          G->parent[i][v0] = frj[v0];
          for (a = G->adj[v0]; a != NULL; a = a->prox) {
             w = a->ID, c = a->custo;
             if (G->parent[i][w] == -1 && G->dist[i][w] > G->dist[i][v0] + c) {
                G->dist[i][w] = G->dist[i][v0] + c;
                frj[w] = v0;
             }
          }
       }
   }
}

void initialize( Digrafo G, Vertex s, Vertex parent[], int dist[], Vertex frj[]){
                /*Inicializar os vetores necessarios para o Algoritmo de Dijkstra*/
   Vertex w; No a; int c;
   for (w = 0; w < G->V; w++) {
      parent[w] = -1;
      dist[w] = INFINITO;
   }
   parent[s] = s;
   dist[s] = 0;
   for (a = G->adj[s]; a != NULL; a = a->prox) {
      w = a->ID, c = a->custo;
      dist[w] = c;
      frj[w] = s;
   }
}

void distribuirPontos(Digrafo G,int** matriz, int* vetor, int xMatriz, int tamanhoVet){
    int i=0,j=0,menor=0;
    Vertex vMenor;
    int *posicao;
    for(i = 0; i < tamanhoVet && vetor[i] != 0 && j<xMatriz; i++){
        matriz[j][0] = vetor[i];
        j++;
    }
    if(vetor[i]==0) return;
    posicao = malloc(xMatriz*sizeof(int));
    for(i=0;i<xMatriz;i++)
        posicao[i]=1;
    for(;i<tamanhoVet && vetor[i] != 0;i++){
        menor = INFINITO;
        for(j=0;j<xMatriz;j++){
            if(G->dist[matriz[j][(posicao[j]-1)]][vetor[i]] < menor){
                menor=G->dist[matriz[j][(posicao[j]-1)]][vetor[i]];
                vMenor = j;
            }
        }
        matriz[vMenor][posicao[vMenor]++]=vetor[i];
    }
}

bool existeCaminho(Digrafo G, Vertex v, Vertex w){ /*Retorna se existe caminho que encontrar de v até w no grafo G*/
    No b = G->adj[v];
    while(b != NULL){
        if (b->ID == w) return true;
        if(existeCaminho(G,b->ID,w)) return true;
        b = b->prox;
    }
    return false;
}
