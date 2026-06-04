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
void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso){
    char linea[64];
    int marcoLibre;
    int contador = 0;
    rewind(archivoOrigen); //Como contamos las lineas el puntero quedaba al final
    for(int i = 0; i < (numPaginas); i++){
        marcoLibre = buscarMarcoPaginaLibreSWAP(TMS);
        fseek(archivoDestino, marcoLibre * 4 * 64, SEEK_SET); // Adelanta el archivo marcoLibre * 4 * 64 bytes desde el inicio
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
        TMS[marcoLibre] = proceso->PID;
        proceso->paginas[i][0] = 0;
        proceso->paginas[i][1] = 0;
        proceso->paginas[i][3] = marcoLibre;
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
