#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "Dijkstra.c"

#define Vertex int /*Vertex é o mesmo que int, mas é usado para denominar vértices*/
#define LARG 800 /*Largura da janela*/
#define ALT 600 /*Altura da janela*/
#define FPS 60 /*Quadros por segundo do programa*/

void desenharVertices(Digrafo G) {
    int i=1;
    No b;
    for(i=1;i<G->V;i++){
        al_draw_circle(G->x[i], G->y[i], 5, al_map_rgb(150,150,150),2);
    }
}

void iniciarVetorBooleano(bool *v, int tamanho){
    int i;
    for(i=0;i<tamanho;i++)
        v[i]=true;
}

int desenharCaminho(Digrafo G, Vertex v, Vertex w, ALLEGRO_COLOR cor){ /*Desenha o caminho mais curto de v até w segundo o algoritmo de dijkstra*/
    int i=0,x = 0,distancia=0;
    if(G->parent[v][w]!=-1){
        x = G->parent[v][w];
        distancia+=G->dist[v][w];
        do{
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],cor,4);
            w = x;
            x = G->parent[v][x];
            distancia+=G->dist[v][w];
            al_draw_line(G->x[w],G->y[w],G->x[x],G->y[x],cor,4);
        }while(x!=v);
    }
    return distancia;
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

int desenharCaminhos(Digrafo G, Vertex base, Vertex pontos[], ALLEGRO_COLOR cor){ /*Desenha o caminho mais curto de v até w segundo o algoritmo de dijkstra*/
    int DistanciaTotal=0;
    Vertex ultimo = base;
    int i;
    for (i = 0; i < tamanhoVetor(pontos,G->V); i++) {
        DistanciaTotal+=desenharCaminho(G, ultimo, pontos[i],cor);
        ultimo = pontos[i];
    }
    return DistanciaTotal;
}

void inserirPonto(Digrafo G, int pontos[], ALLEGRO_BITMAP *flag){ /*Desenha uma bandeira no ponto inserido*/
    int i;
    for(i=0;pontos[i]!=0 && i<G->V;i++)
            al_draw_bitmap(flag,G->x[pontos[i]]-2,G->y[pontos[i]]-32,0);
}

int main(){
    bool exit=false,selecionouPontos = false,menuEnabled=false,*mostrarCiclista,timeClock = false; /*false se o usuario ainda nao saiu do programa*/
    clock_t tempoInicial, tempoFinal;
    float tempoGasto=0.0;
    int i,j,quantCiclistas=0,fase=0,OP; /*i e j variaveis auxiliares, fase é o controlador de status do programa, OP é a opção do status 0*/
    Vertex base=consultarBase(); /*a base recebe inicialmente o que está no arquivo base.txt*/
    Digrafo G = iniciarGrafo(33); /*Grafo no qual se baseia o mapa*/
    int **pontos ; /*Pontos onde o usuario clicou*/
    int *pontosIniciais = malloc(G->V*sizeof(int));
    int *distancias;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_DISPLAY *janela = NULL;
    ALLEGRO_BITMAP *imagem = NULL;
    ALLEGRO_BITMAP *backgroundMenu = NULL;
    ALLEGRO_BITMAP *pin = NULL;
    ALLEGRO_BITMAP *flag = NULL;
    ALLEGRO_BITMAP *menu = NULL;
    ALLEGRO_BITMAP *startFlag = NULL;
    ALLEGRO_EVENT_QUEUE *fila_de_eventos = NULL;
    ALLEGRO_EVENT evento;
    ALLEGRO_FONT *fonte = NULL;
    ALLEGRO_FONT *fonteMenu = NULL;
    ALLEGRO_FONT *fonteMenuLateral = NULL;
    ALLEGRO_COLOR *cor;
    iniciarMapaAugusta(G);
    dijkstra(G);

    if(!al_init()){
        fprintf(stderr, "Falha ao iniciar Allegro5!\n");
        return -1;
    }
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
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
    fonteMenu = al_load_font("Tabaquera.ttf", 48, 0);
    fonteMenuLateral = al_load_font("AlexandriaFLF.ttf", 24, 0);
    if(!fonte){
        fprintf(stderr, "Falha ao carregar fonte!\n");
        return -1;
    }

    imagem = al_load_bitmap("fundo.png");
    backgroundMenu = al_load_bitmap("augusta.jpg");

    al_draw_bitmap(backgroundMenu,0,0,0);
    pin = al_load_bitmap("pin.png");
    flag = al_load_bitmap("flag.png");
    startFlag = al_load_bitmap("racing-flags.png");
    menu = al_load_bitmap("menu.png");

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
                al_draw_textf(fonteMenu, al_map_rgb(0,140,140), (LARG)/2, ALT/2-50, ALLEGRO_ALIGN_CENTRE, "Carregar base (%d)",base);
            }
            al_draw_textf(fonteMenu, al_map_rgb(0,140,140), (LARG)/2, ALT/2, ALLEGRO_ALIGN_CENTRE, "Definir nova base");
            al_draw_textf(fonteMenu, al_map_rgb(0,140,140), (LARG)/2, ALT/2+50, ALLEGRO_ALIGN_CENTRE, "Sair");

            al_flip_display();
            if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) exit = true;
            }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                exit = true;
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(evento.mouse.x>=(LARG)/2-200 && evento.mouse.x<=(LARG)/2+200
                && evento.mouse.y>=ALT/2-40 && evento.mouse.y<=ALT/2+5 && consultarBase()!=0) OP = 1;
                if(evento.mouse.x>=(LARG)/2-200 && evento.mouse.x<=(LARG)/2+200
                && evento.mouse.y>=ALT/2+10 && evento.mouse.y<=ALT/2+55) OP = 2;
                if(evento.mouse.x>=(LARG)/2-200 && evento.mouse.x<=(LARG)/2+200
                && evento.mouse.y>=ALT/2+60 && evento.mouse.y<=ALT/2+105) OP = 3;
            }
            switch(OP){
            case 1:
                fase = 2;
                break;
            case 2:
                i=1;
                while(1){
                    al_wait_for_event(fila_de_eventos,&evento);
                    al_draw_rectangle(LARG-90,ALT-50,LARG-60,ALT-25,al_map_rgb(0,0,0),10);
                    if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                        if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) OP=0;break;
                    }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                        exit = true; break;
                    }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                        if(evento.mouse.x >= LARG-95 && evento.mouse.x <= LARG-55
                        && evento.mouse.y >= ALT-50 && evento.mouse.y <= ALT-25){
                            fase = 2;
                            base = i;
                            alterarBase(i);
                            break;
                        }
                        i = ondeClicou(G,evento.mouse.x,evento.mouse.y);
                    }
                    al_draw_bitmap(imagem,0,0,0);
                    desenharVertices(G);
                    al_draw_textf(fonte, al_map_rgb(0,0,0), LARG/2, 0, ALLEGRO_ALIGN_CENTRE, "Selecione no mapa o local da nova base");
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
                        OP=0;
                        fase = 0;
                        break;
                    }
                }else if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                    exit = true; break;
                }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                    if(evento.mouse.x>=LARG-290 && evento.mouse.x<=LARG-260
                    && evento.mouse.y>=ALT-230 && evento.mouse.y<=ALT-205){
                        fase=1;
                        quantCiclistas = i;
                        pontos = malloc(quantCiclistas * sizeof(Vertex *));
                        distancias = malloc(quantCiclistas * sizeof(int));
                        cor = malloc(quantCiclistas * sizeof(ALLEGRO_COLOR));
                        zerarVetor(distancias,quantCiclistas);
                        mostrarCiclista = malloc(quantCiclistas * sizeof(bool));
                        iniciarVetorBooleano(mostrarCiclista,quantCiclistas);
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
            if(timeClock) tempoInicial = clock();
            al_wait_for_event(fila_de_eventos,&evento);
            al_draw_bitmap(imagem,0,0,0);
            desenharVertices(G);
            al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            if (selecionouPontos && !vetorVazio(pontosIniciais,G->V)) {
                for(i=0;i<quantCiclistas;i++){
                    if(mostrarCiclista[i]) distancias[i] = desenharCaminhos(G, base, pontos[i],cor[i]);
                }
            }
            if(!vetorVazio(pontosIniciais,G->V) && !selecionouPontos && tamanhoVetor(pontosIniciais,G->V)>=quantCiclistas){
                al_draw_bitmap(startFlag,LARG-130,ALT-130,0);
            }
            if(timeClock) {
                tempoFinal = clock();
                timeClock = false;
            }
            al_draw_bitmap(pin,G->x[base]-16,G->y[base]-32,0);
            inserirPonto(G,pontosIniciais,flag);

            if(!menuEnabled){
                al_draw_bitmap(menu,LARG-40,20,0);
            }else{
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-220, 130+(i*50), 0, "%d",i+1);
                al_draw_filled_rectangle(LARG-245,0,LARG,ALT,al_map_rgb(235,231,220));
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-180, 10, 0, "Ciclistas: %d",quantCiclistas);
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-180, 40, 0, "Pontos: %d",tamanhoVetor(pontosIniciais,G->V));
                al_draw_line(LARG-245,70,LARG,70,al_map_rgb(0,0,0),3);
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-230, 80, 0, "Limpar Pontos");
                al_draw_line(LARG-245,120,LARG,120,al_map_rgb(0,0,0),1);
                for(i=0;i<quantCiclistas;i++){
                    al_draw_line(LARG-245,170+(i*50),LARG,170+(i*50),al_map_rgb(0,0,0),1);
                    if(mostrarCiclista[i]){
                        al_draw_rectangle(LARG-230,130+(i*50),LARG-200,160+(i*50),al_map_rgb(0,0,0),1);
                        al_draw_filled_rectangle(LARG-230,130+(i*50),LARG-200,160+(i*50),cor[i]);
                        al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-220, 130+(i*50), 0, "%d",i+1);
                        al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-190, 130+(i*50), 0, "Distancia: %d m",distancias[i]);
                    }else{
                        al_draw_rectangle(LARG-230,130+(i*50),LARG-200,160+(i*50),al_map_rgb(0,0,0),1);
                        al_draw_filled_rectangle(LARG-230,130+(i*50),LARG-200,160+(i*50),al_map_rgb(255,255,255));
                        al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-220, 130+(i*50), 0, "%d",i+1);
                        al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-190, 130+(i*50), 0, "Distancia: %d m",distancias[i]);
                    }
                }
                al_draw_line(LARG-245,170+(i*50),LARG,170+(i*50),al_map_rgb(0,0,0),1);
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-230, 130+(i*50), 0, "Redefinir Base");
                i++;
                al_draw_line(LARG-245,170+(i*50),LARG,170+(i*50),al_map_rgb(0,0,0),1);
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-230, 130+(i*50), 0, "Redefinir Ciclistas");
                i++;
                al_draw_line(LARG-245,170+(i*50),LARG,170+(i*50),al_map_rgb(0,0,0),3);
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-230, 130+(i*50), 0, "Sair");
                tempoGasto = (tempoFinal-tempoInicial)*1000/CLOCKS_PER_SEC;
                al_draw_textf(fonteMenuLateral, al_map_rgb(0,0,0), LARG-100, ALT - 40, 0, "%.0f ms",tempoGasto);
                al_draw_bitmap(menu,LARG-240,20,0);
            }
            al_flip_display();

            if(evento.type ==  ALLEGRO_EVENT_KEY_DOWN){
                if(evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    if(menuEnabled){
                        menuEnabled=false;
                    } else {
                        menuEnabled=true;
                    }
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
                if(!menuEnabled){
                    if(evento.mouse.x>=LARG-40 && evento.mouse.x <=LARG-8
                       && evento.mouse.y>=20 && evento.mouse.y <=56){
                        menuEnabled = true;
                    }else{
                        if(!vetorVazio(pontosIniciais,G->V)){
                            if(evento.mouse.x >= LARG-130 && evento.mouse.x <= LARG-65
                            && evento.mouse.y >= ALT-120 && evento.mouse.y <= ALT-80){
                                if(!selecionouPontos && tamanhoVetor(pontosIniciais,G->V)>=quantCiclistas){
                                    selecionouPontos = true;
                                    timeClock = true;
                                    distribuirPontos(G,pontos,pontosIniciais,quantCiclistas,G->V);
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
                    }
                }else{
                    if(evento.mouse.x>=LARG-240 && evento.mouse.x <=LARG-208
                       && evento.mouse.y>=20 && evento.mouse.y <=56){
                        menuEnabled = false;
                    }
                    if(evento.mouse.x>=LARG-245 && evento.mouse.x<=LARG
                    && evento.mouse.y>=70 && evento.mouse.y<=120){
                        zerarVetor(pontosIniciais,G->V);
                        zerarVetor(distancias,quantCiclistas);
                        for(i=0;i<quantCiclistas;i++){
                            zerarVetor(pontos[i],G->V);
                        }
                        selecionouPontos=false;
                        menuEnabled=false;
                    }
                    for(i=0;i<quantCiclistas;i++){
                        if(evento.mouse.x >=LARG-245 && evento.mouse.x <=LARG
                        && evento.mouse.y >=120+(i*50) && evento.mouse.y <= 170+(i*50)){
                            if(mostrarCiclista[i]) mostrarCiclista[i]=false;
                            else mostrarCiclista[i]=true;
                        }
                    }
                    if(evento.mouse.x >=LARG-245 && evento.mouse.x <=LARG
                    && evento.mouse.y >=120+(i*50) && evento.mouse.y <= 170+(i*50)){
                        fase = 0;
                        OP = 0;
                        selecionouPontos = false;
                        menuEnabled = false;
                        zerarVetor(pontosIniciais,G->V);
                        for(j=0;j<quantCiclistas;j++)
                            zerarVetor(pontos[j],G->V);
                    }
                    i++;
                    if(evento.mouse.x >=LARG-245 && evento.mouse.x <=LARG
                    && evento.mouse.y >=120+(i*50) && evento.mouse.y <= 170+(i*50)){
                        fase = 2;
                        OP = 0;
                        selecionouPontos = false;
                        menuEnabled = false;
                        zerarVetor(pontosIniciais,G->V);
                        for(j=0;j<quantCiclistas;j++)
                            zerarVetor(pontos[j],G->V);
                    }
                    i++;
                    if(evento.mouse.x >=LARG-245 && evento.mouse.x <=LARG
                    && evento.mouse.y >=120+(i*50) && evento.mouse.y <= 170+(i*50)){
                        exit=true;
                    }
                }
            }else if(evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){

            }
        break;
        }
    }
    return 0;
}
