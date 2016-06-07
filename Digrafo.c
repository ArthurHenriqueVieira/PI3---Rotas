#include "No.c"

#define INFINITO 9999

typedef struct digrafo {
    int V; /*Número de vértices*/
    int A; /*Número de arestas*/
    No *adj; /*Lista de adjacência*/
    int *x; /*Coordenada x do vertice*/
    int *y; /*Coordenada y do vertice*/
    Vertex **parent; /*Vetor de cada que conduz ao caminho mais curto*/
    int **dist; /*Distancia minina entre cada vertice e o vertice seguinte*/
} *Digrafo;

Digrafo iniciarGrafo(int V) { /*Inicia o grafo com V vértices (O(V))*/
    Vertex v;
    Digrafo G = malloc( sizeof *G);
    G->V = V;
    G->A = 0;
    G->adj = malloc( V * sizeof (No));
    G->x = malloc(V * sizeof (No));
    G->y = malloc(V * sizeof (No));
    G->parent = malloc(V * sizeof(Vertex *));
    G->dist = malloc(V * sizeof(int *));
    for (v = 0; v < V; v++){
        G->parent[v] = malloc( V * sizeof (int));
        G->dist[v] = malloc( V * sizeof (int));
        G->adj[v] = NULL;
    }
    return G;
}

void inserirAresta( Digrafo G, Vertex v, Vertex w,int x) { /*Insere uma aresta no grafo G, do vertice v até o vértice x, com o peso X*/
   No a;
   for (a = G->adj[v]; a != NULL; a = a->prox)
      if (a->ID == w) return;
   G->adj[v] = novoNo(w,G->adj[v],x);
   G->A++;
}

void imprimirArestas( Digrafo G) { /* Imprime os vértices e suas respectivas arestas (O(V))*/
    Vertex v;
    No w;
    for (v = 0; v < G->V; v++) {
        printf( "%2d:", v);
        for (w = G->adj[v]; w != NULL; w=w->prox)
            if (w != NULL)
                printf( " %2d(%2d)", w->ID,w->custo);
        printf( "\n");
    }
}

void iniciarMapaAugusta(Digrafo G){ /*Inicia o grafo com as arestas apresentadas no arquivo arestas.txt e
                                  as coordenadas x,y de cada vertice apresentadas no arquivo coordenadas.txt*/
    char arestas[]="arestas.txt",coord[]="coordenadas.txt";
	int v, w, x,i=0;
	FILE *arqArestas,*arqCoord;

	arqArestas = fopen(arestas, "r");
	if(arqArestas == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");
	else
		while( (fscanf(arqArestas,"%d %d %d\n", &v, &w, &x))!=EOF )
			inserirAresta(G,v,w,x);

	fclose(arqArestas);

	arqCoord = fopen(coord, "r");
	if(arqCoord == NULL)
			printf("Erro, nao foi possivel abrir o arquivo\n");
	else
		while( (fscanf(arqCoord,"%d,%d\n", &v, &w))!=EOF ){
			G->x[i]=v;
			G->y[i]=w;
			i++;
		}

	fclose(arqCoord);
}

int tamanhoVetor (Vertex *v, int tamanho){
    int i=0;
    for(i=0;i<tamanho && v[i]!=0;i++);
    return i;
}

void inserirElemento (Digrafo G,Vertex x, Vertex *v,Vertex base, int tamanho){
    int i = 0,j=0;
    if(tamanhoVetor(v,tamanho)== 0){
        v[0] = x;
    }else if(tamanhoVetor(v,tamanho)==1){
        if(G->dist[base][v[0]] >= G->dist[base][x]){
            v[1] = v[0];
            v[0] = x;
        }else if(G->dist[base][v[0]] <= G->dist[base][x]){
            v[1] = x;
        }
    }else{
        for(i=0;i<tamanho && v[i]!=0;i++){
            if(G->dist[base][v[i]] <= x && G->dist[base][v[i+1]] >= x){
                for(j=tamanhoVetor(v,tamanho);j>i+1;j--){
                    v[j] = v[j-1];
                }
                v[i] = x;
                break;
            }
            if(v[i+1]==0){
                v[i+1] = x;
                break;
            }
        }
    }
}

bool existeElemento (Vertex x, Vertex *v, int tamanho){
    int i = 0;
    for(i=0;i<tamanho && v[i]!=0;i++){
        if(v[i] == x)
            return true;
    }
    return false;
}

void removeElemento (Vertex x, Vertex *v, int tamanho){ /*remove um elemento x do vetor v*/
    int i = 0,j=0;
    for(i=0;i<tamanho && v[i]!=0;i++){
        if(v[i] == x){
            v[i]=0;
            for(j=i+1;j<tamanho || v[j]!=0;j++){
                v[j-1] = v[j];
            }
            i--;
        }
    }
}

void imprimirVetor(int v[],int tamanho){
    int i;
    for(i=0;i<tamanho && v[i]!=0;i++)
        printf("%d ",v[i]);
}

void zerarVetor(int vetor[],int tamanho){ /*Zera o vetor de inteiros que for passado*/
    int i=0;
    for(i=0;i<tamanho;i++)
        vetor[i]=0;
}

bool vetorVazio(int *pontos,int tamanho){ /*Checa se o vetor de inteiros está vazio*/
    int i;
    for(i=0;i<tamanho;i++)
        if(pontos[i]!=0)
            return false;
    return true;
}

void alterarBase(int valor){ /*Altera a base*/
    char endBase[]="base.txt";
    FILE *arqBase;
    arqBase = fopen(endBase, "w");
    putc((valor/10)+48,arqBase);
    putc((valor%10)+48,arqBase);
    fclose(arqBase);
}

Vertex consultarBase(){ /*Retorna a base atual*/
    Vertex base;
    char endBase[]="base.txt";
    FILE *arqBase;
    arqBase = fopen(endBase, "r");
    if(arqBase == NULL){
        printf("Nao existe base salva.\n");
        return 0;
    }
    fscanf(arqBase,"%d",&base);
    fclose(arqBase);
    return base;
}

int ondeClicou(Digrafo G, int mousex, int mousey){ /*Retorna o vértice mais perto do clique do usuario*/
    int i=1;
    double menor = INFINITO,temp;
    int IDVertice;
    for(i=1;i<G->V;i++){
        temp = ((mousex - G->x[i])*(mousex - G->x[i]))+((mousey - G->y[i])*(mousey - G->y[i]));
        temp = sqrt(temp);
        if(temp<menor)
            menor = temp,IDVertice = i;
    }
    return IDVertice;
}
