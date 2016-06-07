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

void desenharVertices(Digrafo G) {
    int i=1;
    No b;
    for(i=1;i<G->V;i++){
        al_draw_circle(G->x[i], G->y[i], 5, al_map_rgb(150,150,150),2);
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

void desenharCaminho(Digrafo G, Vertex v, Vertex w, ALLEGRO_COLOR cor){ /*Desenha o caminho mais curto de v até w segundo o algoritmo de dijkstra*/
    int i=0,x = 0;
    if(G->parent[v][w]!=-1){
        x = G->parent[v][w];
        do{
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],cor,4);
            w = x;
            x = G->parent[v][x];
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],cor,4);
        }while(x!=v);
    }
}

void definirCores(ALLEGRO_COLOR *cor,int tamanho){
    int i=0;
    ALLEGRO_COLOR tabelaCores[6];
    tabelaCores[0]=al_map_rgb(255,0,0);
    tabelaCores[1]=al_map_rgb(0,255,0);
    tabelaCores[2]=al_map_rgb(0,0,255);
    tabelaCores[3]=al_map_rgb(255,255,0);
    tabelaCores[4]=al_map_rgb(255,0,255);
    tabelaCores[5]=al_map_rgb(0,255,255);
    for(i=0;i<tamanho;i++){
        cor[i] = tabelaCores[i];
    }
}

void desenharCaminhos(Digrafo G, Vertex base, Vertex pontos[], ALLEGRO_COLOR cor){ /*Desenha o caminho mais curto de v até w segundo o algoritmo de dijkstra*/

    Vertex ultimo = base;
    int i;
    for (i = 0; i < tamanhoVetor(pontos,G->V); i++) {
        desenharCaminho(G, ultimo, pontos[i],cor);
        ultimo = pontos[i];
    }
}

int tamanhoVetor (Vertex *v, int tamanho){
    int i=0;
    for(i=0;i<tamanho && v[i]!=0;i++);
    return i;
}

void distribuirPontos(int** matriz, int* vetor, int xMatriz, int tamanhoVet){
    int i=0,j=0,k=0;
    for(i = 0; i < tamanhoVet && vetor[i] != 0; i++){
        if(j == xMatriz){
            j=0;
            k++;
        }
        matriz[j][k] = vetor[i];
        j++;
    }
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
    for(i=0;pontos[i]!=0 && i<G->V;i++)
            al_draw_bitmap(flag,G->x[pontos[i]]-2,G->y[pontos[i]]-32,0);
}

int main(){
    bool exit=false,selecionouPontos = false; /*false se o usuario ainda nao saiu do programa*/
    int i,j,quantCiclistas=0,fase=0,OP; /*i e j variaveis auxiliares, fase é o controlador de status do programa, OP é a opção do status 0*/
    Vertex base=consultarBase(); /*a base recebe inicialmente o que está no arquivo base.txt*/
    Digrafo G = iniciarGrafo(33); /*Grafo no qual se baseia o mapa*/
    int **pontos ; /*Pontos onde o usuario clicou*/
    int *pontosIniciais = malloc(G->V*sizeof(int));

    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *imagem = NULL;
    ALLEGRO_BITMAP *backgroundMenu = NULL;
    ALLEGRO_BITMAP *pin = NULL;
    ALLEGRO_BITMAP *flag = NULL;
    ALLEGRO_BITMAP *startFlag = NULL;
    ALLEGRO_EVENT_QUEUE *fila_de_eventos = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_COLOR *cor;
    iniciarMapaAugusta(G);
    dijkstra(G);

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
    fonte = al_load_font("orange juice.ttf", 32, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar fonte!\n");
        return -1;
    }

    imagem = al_load_bitmap("fundo.png");
    backgroundMenu = al_load_bitmap("algusta.jpg");

    al_draw_bitmap(backgroundMenu,0,0,0);
    pin = al_load_bitmap("pin.png");
    flag = al_load_bitmap("flag.png");
    startFlag = al_load_bitmap("racing-flags.png");

    desenharVertices(G);
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
            al_draw_bitmap(backgroundMenu,0,0,0);
            al_wait_for_event(fila_de_eventos,&evento);
            if(consultarBase()!=0){
                al_draw_textf(fonte, al_map_rgb(150,0,0), LARG/2, ALT/2-50, ALLEGRO_ALIGN_CENTRE, "Carregar base (%d)",base);
            }

            al_draw_textf(fonte, al_map_rgb(150,0,0), LARG/2, ALT/2, ALLEGRO_ALIGN_CENTRE, "Definir nova base");
            al_draw_textf(fonte, al_map_rgb(150,0,0), LARG/2, ALT/2+50, ALLEGRO_ALIGN_CENTRE, "Sair");

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
                fase = 2;
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
                            if(evento.mouse.x >= LARG-90 && evento.mouse.x <= LARG-60
                            && evento.mouse.y >= ALT-50 && evento.mouse.y <= ALT-25){
                                fase = 2;
                                base = i;
                                alterarBase(i);
                                break;
                            }
                            if(evento.mouse.x<=LARG-150 || evento.mouse.y<=ALT-150)
                                i = ondeClicou(G,evento.mouse.x,evento.mouse.y);
                        }
                        al_draw_bitmap(imagem,0,0,0);
                        desenharVertices(G);
                        al_draw_textf(fonte, al_map_rgb(0,0,0), LARG-75, ALT-50, ALLEGRO_ALIGN_CENTRE, "OK");
                        al_draw_bitmap(pin,G->x[i]-16,G->y[i]-32,0);

                        al_flip_display();
                }
                break;
            case 3:
                exit = true;
                break;
            }
            break;
        case 2:
            i=1;
            while(1){
                al_wait_for_event(fila_de_eventos,&evento);
                if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                    if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                        exit=true;break;
                    }
                }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    exit = true; break;
                }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                    if(evento.mouse.x>=LARG-290 && evento.mouse.x<=LARG-260
                    && evento.mouse.y>=ALT-230 && evento.mouse.y<=ALT-205){
                        fase=1;
                        quantCiclistas = i;
                        pontos = malloc(quantCiclistas * sizeof(Vertex *));
                        cor = malloc(quantCiclistas * sizeof(ALLEGRO_COLOR));
                        definirCores(cor,quantCiclistas);
                        for (i = 0; i < quantCiclistas; i++){
                            pontos[i] = malloc( G->V * sizeof (Vertex));
                            zerarVetor(pontos[i],G->V);
                        }
                        break;
                    }
                    if(evento.mouse.x>=LARG - 300 && evento.mouse.x<=LARG-250
                    && evento.mouse.y>=220 && evento.mouse.y<=250)
                        if(i<5)
                            i++;
                    if(evento.mouse.x>=LARG - 300 && evento.mouse.x<=LARG-250
                    && evento.mouse.y>=ALT-275 && evento.mouse.y<=ALT-250)
                        if(i>1)
                            i--;
                }
                al_draw_bitmap(imagem,0,0,0);
                desenharVertices(G);
                al_draw_filled_rectangle(200,200,LARG-200,ALT-200,al_map_rgb(235,231,220));
                al_draw_filled_triangle(LARG-300,250,LARG-250,250,LARG-275,220,al_map_rgb(0,150,0));
                al_draw_filled_triangle(LARG-300,ALT-270,LARG-250,ALT-270,LARG-275,ALT-240,al_map_rgb(0,150,0));
                al_draw_rectangle(LARG-300,255,LARG-250,ALT-275,al_map_rgb(0,150,0),2);
                al_draw_textf(fonte, al_map_rgb(0,150,0), LARG-275, ALT-320, ALLEGRO_ALIGN_CENTRE, "%2d",i);
                al_draw_textf(fonte, al_map_rgb(0,150,0), LARG-275, ALT-230, ALLEGRO_ALIGN_CENTRE, "OK");
                al_draw_textf(fonte, al_map_rgb(0,150,0), 320, 275, ALLEGRO_ALIGN_CENTRE, "Escolha o numero");
                al_draw_textf(fonte, al_map_rgb(0,150,0), 320, ALT-300, ALLEGRO_ALIGN_CENTRE, "de ciclistas");
                al_flip_display();
            }
            break;
        case 1:
            i=0;
            al_wait_for_event(fila_de_eventos,&evento);

            if (selecionouPontos && !vetorVazio(pontosIniciais,G->V)) {
                for(i=0;i<quantCiclistas;i++)
                    desenharCaminhos(G, base, pontos[i],cor[i]);
            }
            if(!vetorVazio(pontosIniciais,G->V) && !selecionouPontos && tamanhoVetor(pontosIniciais,G->V)>=quantCiclistas){
                al_draw_bitmap(startFlag,LARG-130,ALT-130,0);
            }
            for(i=0;i<quantCiclistas;i++)
                inserirPonto(G,pontosIniciais,flag);

            al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            al_flip_display();

            if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    fase = 0;
                    OP = 0;
                    selecionouPontos = false;
                    zerarVetor(pontosIniciais,G->V);
                    for(i=0;i<quantCiclistas;i++)
                        zerarVetor(pontos[i],G->V);
                }
                if(evento.keyboard.keycode == ALLEGRO_KEY_SPACE){
                    if(selecionouPontos && !vetorVazio(pontosIniciais, G->V)){
                        selecionouPontos = false;
                        zerarVetor(pontosIniciais,G->V);
                        for(i=0;i<quantCiclistas;i++)
                            zerarVetor(pontos[i],G->V);
                        al_draw_bitmap(imagem,0,0,0);
                        desenharVertices(G);
                        al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
                    }
                }
            }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                exit = true;
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(!vetorVazio(pontosIniciais,G->V)){
                    if(evento.mouse.x >= LARG-120 && evento.mouse.x <= LARG-80
                    && evento.mouse.y >= ALT-120 && evento.mouse.y <= ALT-80){
                        if(!selecionouPontos && tamanhoVetor(pontosIniciais,G->V)>=quantCiclistas){
                            selecionouPontos = true;
                            distribuirPontos(pontos,pontosIniciais,quantCiclistas,G->V);
                        }
                    }
                }
                if(!selecionouPontos && ondeClicou(G,evento.mouse.x,evento.mouse.y)!=base){
                    if(existeElemento(ondeClicou(G,evento.mouse.x,evento.mouse.y),pontosIniciais,G->V)){
                        removeElemento(ondeClicou(G,evento.mouse.x,evento.mouse.y),pontosIniciais,G->V);
                    }else{
                        inserirElemento(G,ondeClicou(G,evento.mouse.x,evento.mouse.y),pontosIniciais,base,G->V);
                    }
                }
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                al_draw_bitmap(imagem,0,0,0);
                desenharVertices(G);
                al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            }
        break;
        }
    }
    return 0;
}

