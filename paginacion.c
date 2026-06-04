#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <math.h>
#include <curses.h>
#include <sys/select.h>
#include <unistd.h>

int calcularPaginasLibresSWAP(int TMS[]){
    int paginasLibresSWAP = 0;
    for(int i = 0; i < marcosSWAP; i++){
        if(TMS[i] == 0){
            paginasLibresSWAP++;
        }
    }
    return paginasLibresSWAP;
}
int calcularInstrucciones(FILE *archivo){
    char linea[64];
    int numeroInstrucciones = 0;
    while(((fgets(linea, sizeof(linea), archivo)) != NULL)){
        numeroInstrucciones++;
    }
    return numeroInstrucciones;
}
int calcularNumPaginas(int numeroInstrucciones){
    int numeroDePaginas;
    numeroDePaginas = ceil(numeroInstrucciones/(float)tamañoDePagina);
    return numeroDePaginas;
}
int buscarMarcoPaginaLibreSWAP(int TMS[]){
    for(int i = 0; i < marcosSWAP; i++){
        if(TMS[i] == 0){
            return i;
        }
    }
    return -1; //Nunca llega aqui
}

int buscarMarcoPaginaLibreRAM(int TMM[]){
    for(int i = 0; i < marcosRAM; i++){
        if(TMM[i] == 0){
            return i;
        }
    }
    return -1; //Nunca llega aqui
}

void cargar_a_memoria_RAM(FILE *SWAP, char RAM[], int TMM[], PCB *proceso, int pagina_instruccion){
    int marco_de_la_pagina_en_swap;
    int marcoLibre_RAM;
    char linea[64];
    marco_de_la_pagina_en_swap = proceso->paginas[pagina_instruccion][3];
    marcoLibre_RAM = buscarMarcoPaginaLibreRAM(TMM);
    fseek(SWAP, marco_de_la_pagina_en_swap * 4 * 64, SEEK_SET);
    for(int i = 0; i < 4; i++){
        fread(linea, sizeof(char), 64, SWAP);
        memcpy(RAM + (marcoLibre_RAM*4*64) + (i*64), linea, 64);
    }
    TMM[marcoLibre_RAM] = proceso->PID;
    proceso->paginas[pagina_instruccion][0] = 1;
    
}

void imprimirTMM(int TMM[]){
    for(int i = 0; i < marcosRAM - 9; i++){
        mvprintw(5,4 + i,"%d", TMM[i]);  //CAMBIAR
        refresh();
        usleep(50000);
    }
}


void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso){
    char linea[64];
    int marcoLibre_SWAP;
    int contador = 0;
    rewind(archivoOrigen); //Como contamos las lineas el puntero quedaba al final
    for(int i = 0; i < (numPaginas); i++){
        marcoLibre_SWAP = buscarMarcoPaginaLibreSWAP(TMS);
        fseek(archivoDestino, marcoLibre_SWAP * 4 * 64, SEEK_SET); // Adelanta el archivo marcoLibre * 4 * 64 bytes desde el inicio
        while ((fgets(linea, sizeof(linea), archivoOrigen)) != NULL){
            size_t len = strlen(linea);
            if (len < 64) {
                memset(linea + len, '0', 64 - len);
            }
            fwrite(linea, sizeof(char), 64, archivoDestino);
            contador++;
            if(contador == 4){
                contador = 0;
                break;
            }
            
        }
        TMS[marcoLibre_SWAP] = proceso->PID;
        proceso->paginas[i][0] = 0;
        proceso->paginas[i][1] = 0;
        proceso->paginas[i][3] = marcoLibre_SWAP;
    }
    fclose(archivoDestino);
    archivoDestino = NULL;
}
void imprimirTMS(int TMS[]){
    for(int i = 0; i < marcosSWAP; i++){
        mvprintw(5,4 + i,"%d", TMS[i]);  //CAMBIAR
        refresh();
        sleep(1);
    }
}