#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <math.h>
#include <curses.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>

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
    return -1;
}

void cargar_a_memoria_RAM(FILE *SWAP, char RAM[], int TMM[], PCB *proceso, int pagina_instruccion, PCB *ejecucion, PCB *suspendidos){
    int marco_de_la_pagina_en_swap;
    int marcoLibre_RAM;
    char linea[64];
    marco_de_la_pagina_en_swap = proceso->paginas[pagina_instruccion][2];
    marcoLibre_RAM = buscarMarcoPaginaLibreRAM(TMM);
    if((marcoLibre_RAM >= 0)  && (marcoLibre_RAM <= 15)){
        fseek(SWAP, marco_de_la_pagina_en_swap * 4 * tam_linea, SEEK_SET);
        for(int i = 0; i < 4; i++){
            fread(linea, sizeof(char), 64, SWAP);
            memcpy(RAM + (marcoLibre_RAM * 4 * tam_linea) + (i * tam_linea), linea, 64);
        }
        TMM[marcoLibre_RAM] = proceso->PID;
        proceso->paginas[pagina_instruccion][0] = 1;
        proceso->paginas[pagina_instruccion][1] = marcoLibre_RAM;
    }
    /*mvprintw(numLineaErrorLista,4,"ERROR, no hay memoria RAM");
    refresh();
    sleep(1);
    limpiarLinea(numLineaErrorLista);*/
    
}

void guardarTiempos(PCB *procesoSuspendido){
    int numero_aleatorio = (rand() % 9) + 2;
    numero_aleatorio = 10;
    time(&procesoSuspendido->tiempo_de_salida);
    procesoSuspendido->espera = numero_aleatorio;
}

void imprimirTMM(int TMM[]){
    for(int i = 0; i < marcosRAM; i++){
        mvprintw(5,4 + i,"%d", TMM[i]);  //CAMBIAR
        refresh();
        usleep(500000);
    }
}


void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso){
    char linea[64];
    int marcoLibre_SWAP;
    int contador = 0;
    rewind(archivoOrigen); //Como contamos las lineas el puntero quedaba al final
    for(int i = 0; i < (numPaginas); i++){
        marcoLibre_SWAP = buscarMarcoPaginaLibreSWAP(TMS);
        fseek(archivoDestino, marcoLibre_SWAP * 4 * tam_linea, SEEK_SET); // Adelanta el archivo marcoLibre * 4 * 64 bytes desde el inicio
        while ((fgets(linea, sizeof(linea), archivoOrigen)) != NULL){
            if(i == (numPaginas -1)){   //revisa todo el ultimo marco en busca del END
                if(strcmp(linea, "END") == 0){
                    linea[3] = '\n';
                }
            }
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
        proceso->paginas[i][2] = marcoLibre_SWAP;
        
    }
}
void imprimirTMS(int TMS[]){
    for(int i = 0; i < marcosSWAP; i++){
        mvprintw(6,4 + i,"%d", TMS[i]);  //CAMBIAR
        refresh();
        sleep(1);
    }
}

void liberar_marcos_RAM_SWAP(PCB *proceso, int TMM[], int TMS[]){
    int marcoRAM;
    int marcoSWAP;
    int entradasTMP;
    entradasTMP = calcularNumPaginas(proceso->numInstrucciones); 
    for(int i = 0 ; i < entradasTMP ; i++){
        proceso->paginas[i][0] = 0;
        marcoRAM = proceso->paginas[i][1];
        marcoSWAP = proceso->paginas[i][2];
        TMM[marcoRAM] = 0;
        TMS[marcoSWAP] = 0;    
    }

}

int cargarNuevos(PCB *nuevos, PCB *listos, FILE *memoriaVirtual, int TMS[]){
    int paginasLibres;
    int numeroPaginas;
    bool memoriaSuficiente = true;

    while(memoriaSuficiente){
        if(nuevos->sig == NULL){
            return 0;
        }
        paginasLibres = calcularPaginasLibresSWAP(TMS);
        numeroPaginas = calcularNumPaginas((nuevos->sig)->numInstrucciones);

        if(paginasLibres >= numeroPaginas){
            PCB *nuevo = sacarFrente(nuevos);
            FILE *archivo = fopen(nuevo->nombre_proceso, "rb");
            cargar_a_memoria_virtual(archivo, memoriaVirtual,numeroPaginas,TMS,nuevo);
            insertar(listos, nuevo);
            if (archivo != NULL){
                fclose(archivo);
                archivo = NULL;
            }
        }
        else{
            memoriaSuficiente = false;
            mvprintw(numLineaErrorLista,4,"ERROR: memoria virtual insuficiente");
            refresh();
            sleep(1);
            limpiarLinea(numLineaErrorLista);
        }
    }
    return 1;
}

PCB *sacarSuspendidos(PCB *suspendidos, PCB *listos){
    
    time_t diferencia;
    time_t tiempo_actual;

    PCB *temp1 = suspendidos;
    PCB *temp2 = suspendidos->sig;

    while(temp1->sig != NULL){
        time(&tiempo_actual); 
        diferencia = tiempo_actual - temp2->tiempo_de_salida;
        if(diferencia >= (time_t)temp2->espera){
            temp1->sig = temp2->sig;
            temp2->sig = NULL;
            return temp2;
        }
        else{
            temp1= temp1->sig;
            temp2= temp2->sig;
        }
    }
    return NULL;
}