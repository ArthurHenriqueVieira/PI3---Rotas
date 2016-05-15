#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define Vertex int /*Vertex é o mesmo que int, mas é usado para denominar vértices*/
#define LARG 800
#define ALT 600
#define FPS 60
#define INFINITO 9999

/*No grafo por lista de adjacência, a lista na posição x é composta por vértices que têm
ligação com o vértice de índice x*/

typedef struct no{
    Vertex ID; /*Numero do vertice*/
    int custo; /*Distancia*/
    struct no *prox;
} *No;

typedef struct digrafo {
    int V; /*Número de vértices*/
    int A; /*Número de arestas*/
    No *adj; /*Lista de adjacência*/
    int *x; /*Coordenada x do vertice*/
    int *y; /*Coordenada y do vertice*/
} *Digrafo;

No novoNo(Vertex ID, No prox,int custo) { /*Retorna um novo nó, de acordo com os parâmetros inseridos (O(1))*/
    No a = malloc( sizeof (struct no));
    a->ID = ID;
    a->custo = custo;
    a->prox = prox;
    return a;
}

Digrafo iniciarGrafo(int V) { /*Inicia o grafo com V vértices (O(V))*/
    Vertex v;
    Digrafo G = malloc( sizeof *G);
    G->V = V;
    G->A = 0;
    G->adj = malloc( V * sizeof (No));
    G->x = malloc(V * sizeof (No));
    G->y = malloc(V * sizeof (No));
    for (v = 0; v < V; v++)
        G->adj[v] = NULL;
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

void iniciarMapaAugusta(Digrafo G){
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

bool existeCaminho(Digrafo G, Vertex v, Vertex w){ /*Retorna se existe caminho que encontrar de v até w no grafo G*/
    No b = G->adj[v];
    while(b != NULL){
        if (b->ID == w) return true;
        if(existeCaminho(G,b->ID,w)) return true;
        b = b->prox;
    }
    return false;
}

void inserirRetas(Digrafo G){
    int i=1;
    No b;
    for(i=1;i<G->V;i++){
        if(G->adj[i]!=NULL){
            b = G->adj[i];
            while(b!=NULL){
                al_draw_line(G->x[i],G->y[i],G->x[b->ID],G->y[b->ID],al_map_rgb(0,0,0),1.25);
                b=b->prox;
            }
        }
    }
}




void initialize( Digrafo G, Vertex s, Vertex parent[], int dist[], Vertex frj[]){
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

/* Recebe digrafo G com custos positivos nos arcos e um vértice s. Calcula uma SPT com raiz s e armazena a SPT no vetor parent. As distâncias a partir de s são armazenadas no vetor dist.

O digrafo é representado por suas listas de adjacência. A função supõe que a soma dos custos de todos os arcos é estritamente menor que a constante INFINITO. Supõe também que o digrafo tem no máximo maxV vértices.*/

void dijkstra( Digrafo G, Vertex s, Vertex parent[], int dist[]){
   Vertex v0, w, frj[100];
   No a; int c;
   initialize( G, s, parent, dist, frj);

   while (1) {
      int mindist = INFINITO;
      for (w = 0; w < G->V; w++){
         if (parent[w] == -1 && mindist > dist[w])
            mindist = dist[v0=w];
      }
      if (mindist == INFINITO) break;
      parent[v0] = frj[v0];
      for (a = G->adj[v0]; a != NULL; a = a->prox) {
         w = a->ID, c = a->custo;
         if (parent[w] == -1 && dist[w] > dist[v0] + c) {
            dist[w] = dist[v0] + c;
            frj[w] = v0;
         }
      }
   }
}

void desenharCaminho(Digrafo G, Vertex v, Vertex w, Vertex parent[], int dist[]){
    int i=0,x = 0;
    if(parent[w]!=-1){
        x = parent[w];
        do{
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],al_map_rgb(255,0,0),2);
            w = x;
            x = parent[x];
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],al_map_rgb(255,0,0),2);
        }while(x!=v);
    }
}

int ondeClicou(Digrafo G, int mousex, int mousey){
    int i=1;
    double menor=INFINITO,temp;
    int IDVertice;
    for(i=1;i<G->V;i++){
        temp = ((mousex - G->x[i])*(mousex - G->x[i]))+((mousey - G->y[i])*(mousey - G->y[i]));
        temp = sqrt(temp);
        printf("%d - %.2lf ( %lf )\n",i,temp,menor);
        if(temp<menor)
            menor = temp,IDVertice = i;
    }
    return IDVertice;
}

int main(){
    bool exit = false;
    int i;
    Digrafo G = iniciarGrafo(33);
    Vertex *parent=malloc(G->V*sizeof(Vertex));
    int *dist=malloc(G->V*sizeof(int));


    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *imagem = NULL;
    ALLEGRO_BITMAP *pin = NULL;
    ALLEGRO_EVENT_QUEUE *fila_de_eventos = NULL;
    ALLEGRO_EVENT evento;
    iniciarMapaAugusta(G);
    imprimirArestas(G);
    dijkstra(G,13,parent,dist);
    for(i=1;i<G->V;i++){
        printf("%d - %d ( %d )\n",i,parent[i],dist[i]);
    }
    if(!al_init()){
        fprintf(stderr, "Falha ao iniciar Allegro5!\n");
        return -1;
    }

    janela = al_create_display(LARG,ALT);
    if(!janela){
        fprintf(stderr, "Falha ao criar janela!\n");
        return -1;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer){
        fprintf(stderr, "Falha ao criar timer!\n");
        return -1;
    }

    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_install_mouse();
    al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

    imagem = al_load_bitmap("fundo.png");
    al_draw_bitmap(imagem,0,0,0);
    pin = al_load_bitmap("pin.png");
    al_draw_bitmap(pin,420-16,308-32,0);
    inserirRetas(G);
    al_flip_display();

    srand(time(NULL));
    fila_de_eventos = al_create_event_queue();
    if(!fila_de_eventos){
        fprintf(stderr, "Falha ao criar fila de eventos!\n");
        return -1;
    }


    al_register_event_source(fila_de_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_de_eventos, al_get_display_event_source(janela));
    al_register_event_source(fila_de_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_de_eventos, al_get_mouse_event_source());
    al_start_timer(timer);

    while(!exit){
        al_flip_display();
        al_wait_for_event(fila_de_eventos,&evento);
        if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
            if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) exit = true;
            if(evento.keyboard.keycode == ALLEGRO_KEY_D) desenharCaminho(G,13,ondeClicou(G,evento.mouse.x,evento.mouse.y),parent,dist);
        }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            exit = true;
        }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
            desenharCaminho(G,13,ondeClicou(G,evento.mouse.x,evento.mouse.y),parent,dist);
        }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
            al_draw_bitmap(imagem,0,0,0);
            inserirRetas(G);
            al_draw_bitmap(pin,G->x[13]-16,G->y[13]-32,0);
        }
    }
    return 0;
}

