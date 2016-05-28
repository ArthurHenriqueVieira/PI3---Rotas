#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define Vertex int /*Vertex é o mesmo que int, mas é usado para denominar vértices*/
#define LARG 800 /*Largura da janela*/
#define ALT 600 /*Altura da janela*/
#define FPS 60 /*Quadros por segundo do programa*/
#define INFINITO 9999

/*No grafo por lista de adjacência, a lista na posição x é composta por vértices que têm
ligação com o vértice de índice x*/

typedef struct no{
    Vertex ID; /*Numero do vertice*/
    int custo; /*Distancia*/
    struct no *prox; /*Próximo nó da lista*/
} *No;

typedef struct digrafo {
    int V; /*Número de vértices*/
    int A; /*Número de arestas*/
    No *adj; /*Lista de adjacência*/
    int *x; /*Coordenada x do vertice*/
    int *y; /*Coordenada y do vertice*/
    Vertex **parent; /*Vetor de cada que conduz ao caminho mais curto*/
    int **dist; /*Distancia minina entre cada vertice e o vertice seguinte*/
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

bool existeCaminho(Digrafo G, Vertex v, Vertex w){ /*Retorna se existe caminho que encontrar de v até w no grafo G*/
    No b = G->adj[v];
    while(b != NULL){
        if (b->ID == w) return true;
        if(existeCaminho(G,b->ID,w)) return true;
        b = b->prox;
    }
    return false;
}

void inserirRetas(Digrafo G){ /*Insere no mapa as retas que condizem ao mapa demonstado no grafo*/
    int i=1;
    No b;
    for(i=1;i<G->V;i++){
        if(G->adj[i]!=NULL){
            b = G->adj[i];
            while(b!=NULL){
                al_draw_line(G->x[i],G->y[i],G->x[b->ID],G->y[b->ID],al_map_rgb(0,0,0),3);
                b=b->prox;
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

void desenharCaminho(Digrafo G, Vertex v, Vertex w){ /*Desenha o caminho mais curto de v até w segundo o algoritmo de dijkstra*/
    int i=0,x = 0;
    if(G->parent[v][w]!=-1){
        x = G->parent[v][w];
        do{
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],al_map_rgb(255,0,0),1.5);
            w = x;
            x = G->parent[v][x];
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],al_map_rgb(255,0,0),1.5);
        }while(x!=v);
    }
}
void zerarVetor(bool vetor[],int tamanho){ /*Zera o vetor booleano que for passado*/
    int i=0;
    for(i=0;i<tamanho;i++)
        vetor[i]=false;
}
bool vetorVazio(bool pontos[],int tamanho){ /*Checa se o vetor booleano está vazio*/
    int i;
    for(i=0;i<tamanho;i++)
        if(pontos[i])
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
    double menor=INFINITO,temp;
    int IDVertice;
    for(i=1;i<G->V;i++){
        temp = ((mousex - G->x[i])*(mousex - G->x[i]))+((mousey - G->y[i])*(mousey - G->y[i]));
        temp = sqrt(temp);
        if(temp<menor)
            menor = temp,IDVertice = i;
    }
    return IDVertice;
}

void inserirPonto(Digrafo G, int pontos[], ALLEGRO_BITMAP *flag){ /*Desenha uma bandeira no ponto inserido*/
    int i;

    for(i=0;i<G->V;i++)
        if(pontos[i])
            al_draw_bitmap(flag,G->x[i]-2,G->y[i]-32,0);
}

int main(){
    bool exit = false; /*false se o usuario ainda nao saiu do programa*/
    int i,j,fase=0,OP; /*i e j variaveis auxiliares, fase é o controlador de status do programa, OP é a opção do status 0*/
    Vertex base=consultarBase(); /*a base recebe inicialmente o que está no arquivo base.txt*/
    Digrafo G = iniciarGrafo(33); /*Grafo no qual se baseia o mapa*/
    bool *pontos = malloc(G->V*sizeof(int)); /*Pontos onde o usuario clicou*/


    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *imagem = NULL;
    ALLEGRO_BITMAP *pin = NULL;
    ALLEGRO_BITMAP *flag = NULL;
    ALLEGRO_EVENT_QUEUE *fila_de_eventos = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_FONT *fonte = NULL;
    iniciarMapaAugusta(G);
    dijkstra(G);
    zerarVetor(pontos,G->V);

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
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();
    al_set_system_mouse_cursor(janela, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    fonte = al_load_font("Amble-Light.ttf", 18, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar fonte!\n");
        return -1;
    }
    imagem = al_load_bitmap("fundo.png");
    al_draw_bitmap(imagem,0,0,0);
    pin = al_load_bitmap("pin.png");
    flag = al_load_bitmap("flag.png");
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
        switch(fase){
        case 0:
            al_draw_bitmap(imagem,0,0,0);
            inserirRetas(G);
            al_wait_for_event(fila_de_eventos,&evento);
            al_draw_filled_rectangle(150,150,LARG-150,ALT-150,al_map_rgb(0,0,0));
            if(consultarBase()!=0) al_draw_textf(fonte, al_map_rgb(255,255,255), LARG/2, ALT/2-50, ALLEGRO_ALIGN_CENTRE, "Carregar base (%d)",base);
            al_draw_textf(fonte, al_map_rgb(255,255,255), LARG/2, ALT/2, ALLEGRO_ALIGN_CENTRE, "Definir nova base");
            al_draw_textf(fonte, al_map_rgb(255,255,255), LARG/2, ALT/2+50, ALLEGRO_ALIGN_CENTRE, "Sair");
            if(consultarBase()!=0) al_draw_rectangle(LARG/2-90,ALT/2-60,LARG/2+90,ALT/2-25,al_map_rgb(255,255,255),2);
            al_draw_rectangle(LARG/2-75,ALT/2-10,LARG/2+75,ALT/2+25,al_map_rgb(255,255,255),2);
            al_draw_rectangle(LARG/2-50,ALT/2+40,LARG/2+50,ALT/2+75,al_map_rgb(255,255,255),2);
            al_flip_display();
            if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) exit = true;
            }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                exit = true;
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(evento.mouse.x>=LARG/2-90 && evento.mouse.x<=LARG/2+90
                && evento.mouse.y>=ALT/2-60 && evento.mouse.y<=ALT/2-25 && consultarBase()!=0) OP = 1;
                if(evento.mouse.x>=LARG/2-75 && evento.mouse.x<=LARG/2+75
                && evento.mouse.y>=ALT/2-10 && evento.mouse.y<=ALT/2+25) OP = 2;
                if(evento.mouse.x>=LARG/2-50 && evento.mouse.x<=LARG/2+50
                && evento.mouse.y>=ALT/2+40 && evento.mouse.y<=ALT/2+75) OP = 3;
            }
            switch(OP){
            case 1:
                fase = 1;
                break;
            case 2:
                i=1;
                while(1){
                    al_wait_for_event(fila_de_eventos,&evento);
                    if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                        if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) OP=0;break;
                    }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                        exit = true; break;
                    }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                        if(evento.mouse.x>=LARG-90 && evento.mouse.x<=LARG-60
                        && evento.mouse.y>=ALT-30 && evento.mouse.y<=ALT-5){
                            fase=1;
                            base = i;
                            alterarBase(i);
                            break;
                        }
                        if(evento.mouse.x>=LARG-95 && evento.mouse.x<=LARG-55
                        && evento.mouse.y>=ALT-140 && evento.mouse.y<=ALT-115)
                            if(i<32)
                                i++;
                        if(evento.mouse.x>=LARG-95 && evento.mouse.x<=LARG-55
                        && evento.mouse.y>=ALT-60 && evento.mouse.y<=ALT-35)
                            if(i>1)
                                i--;
                        if(evento.mouse.x<=LARG-150 || evento.mouse.y<=ALT-150)
                            i = ondeClicou(G,evento.mouse.x,evento.mouse.y);
                    }
                    al_draw_bitmap(imagem,0,0,0);
                    inserirRetas(G);
                    al_draw_filled_rectangle(LARG,ALT,LARG-150,ALT-150,al_map_rgb(0,0,0));
                    al_draw_filled_triangle(LARG-55,ALT-115,LARG-95,ALT-115,LARG-75,ALT-140,al_map_rgb(255,255,255));
                    al_draw_filled_triangle(LARG-55,ALT-60,LARG-95,ALT-60,LARG-75,ALT-35,al_map_rgb(255,255,255));
                    al_draw_rectangle(LARG-95,ALT-65,LARG-55,ALT-110,al_map_rgb(255,255,255),2);
                    al_draw_textf(fonte, al_map_rgb(255,255,255), LARG-80, ALT-95, ALLEGRO_ALIGN_CENTRE, "%2d",i);
                    al_draw_textf(fonte, al_map_rgb(255,255,255), LARG-75, ALT-25, ALLEGRO_ALIGN_CENTRE, "OK");
                    al_draw_bitmap(pin,G->x[i]-16,G->y[i]-32,0);

                    al_flip_display();
                }
                break;
            case 3:
                exit = true;
                break;
            }
            break;
        case 1:
            al_wait_for_event(fila_de_eventos,&evento);
            al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            al_flip_display();
            if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) exit = true;
            }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                exit = true;
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                desenharCaminho(G,base,ondeClicou(G,evento.mouse.x,evento.mouse.y));
                if(ondeClicou(G,evento.mouse.x,evento.mouse.y)!=base)
                    al_draw_textf(fonte, al_map_rgb(255,0,0), LARG-100, ALT-50, ALLEGRO_ALIGN_CENTRE,
                                  "Distancia: %d m",G->dist[base][ondeClicou(G,evento.mouse.x,evento.mouse.y)]);
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                al_draw_bitmap(imagem,0,0,0);
                inserirRetas(G);
                al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            }
        break;
        }
    }
    return 0;
}

