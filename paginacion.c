#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <math.h>
#include <curses.h>
#include <sys/select.h>
#include <unistd.h>
#include <time.h>

int reloj = 0;

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

int algoritmo_reloj(int TMM[][2], PCB *listos, PCB *ejecucion, PCB *suspendidos){
    int paginaLibre;
    int GID_proceso_desalojado;
    PCB *procesoDesalojado;
    bool actualizoPCB = false;
    while(1){
        if(TMM[reloj][1] == 0){
            GID_proceso_desalojado = TMM[reloj][0];
            if(GID_proceso_desalojado > 0){
                if( (procesoDesalojado = buscarPorGID(ejecucion, GID_proceso_desalojado)) == NULL ){
                    if( (procesoDesalojado = buscarPorGID(listos, GID_proceso_desalojado)) == NULL ){
                        if( (procesoDesalojado = buscarPorGID(suspendidos, GID_proceso_desalojado)) == NULL ){
                            mvprintw(numLineaErrorLista,4,"ERROR de TMM. No se encuentra en ninguna lista.");
                            refresh();
                            sleep(1);
                            limpiarLinea(numLineaErrorLista);

                            TMM[reloj][0] = 0; 
                            TMM[reloj][1] = 0;
                            continue;
                        }
                    }
                }
                for(int i = 0; i < procesoDesalojado->numPaginas; i++){
                    // Actualizar PCB del proceso cuyo marco es desalojado y comprobar que ese marco si este en RAM 
                    if (((procesoDesalojado->paginas[i][1]) == reloj) && (procesoDesalojado->paginas[i][0] == 1)){
                        procesoDesalojado->paginas[i][0] = 0;
                        procesoDesalojado->paginas[i][1] = 0;
                        actualizoPCB = true;
                        break;
                    }
                }
                if(actualizoPCB == false){
                    mvprintw(numLineaErrorLista,4,"ERROR de TMP. No se encontró ese marco de RAM en la TMP");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                }
                actualizoPCB = false;
            }
            TMM[reloj][1] = 1;
            paginaLibre = reloj;
            if(reloj == 15){
                reloj = 0;
            }
            else{
                reloj++;
            }
            return paginaLibre;
        }
        else if(TMM[reloj][1] == 1){
            TMM[reloj][1] = 0;
            if(reloj == 15){
                reloj = 0;
            }
            else{
                reloj++;
            }
        }
    }
}

void cargar_a_memoria_RAM(FILE *SWAP, char RAM[], int TMM[][2], PCB *proceso, int pagina_instruccion, PCB *listos, PCB *ejecucion, PCB *suspendidos){
    int marco_de_la_pagina_en_swap;
    int marcoLibre_RAM;
    char linea[64];
    marco_de_la_pagina_en_swap = proceso->paginas[pagina_instruccion][2];
    marcoLibre_RAM = algoritmo_reloj(TMM, listos, ejecucion, suspendidos);
    if((marcoLibre_RAM >= 0)  && (marcoLibre_RAM <= 15)){
        fseek(SWAP, marco_de_la_pagina_en_swap * 4 * tam_linea, SEEK_SET);
        for(int i = 0; i < 4; i++){
            fread(linea, sizeof(char), 64, SWAP);
            memcpy(RAM + (marcoLibre_RAM * 4 * tam_linea) + (i * tam_linea), linea, 64);
        }
        TMM[marcoLibre_RAM][0] = proceso->GID;
        proceso->paginas[pagina_instruccion][0] = 1;
        proceso->paginas[pagina_instruccion][1] = marcoLibre_RAM;
    }
}

void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso){
    char linea[64];
    int marcoLibre_SWAP;
    int contador = 0;
    for(int i = 0; i < (numPaginas); i++){
        marcoLibre_SWAP = buscarMarcoPaginaLibreSWAP(TMS);
        fseek(archivoDestino, marcoLibre_SWAP * 4 * tam_linea, SEEK_SET); // Adelanta el archivo marcoLibre * 4 * 64 bytes desde el inicio
        while (((fgets(linea, sizeof(linea), archivoOrigen)) != NULL)){
            if(i == (numPaginas -1)){   //revisa todo el ultimo marco en busca del END
                if(linea[0] == 'J'  &&  linea[1] == 'N'  && linea[2] == 'Z'){
                    size_t len_JNZ = strlen(linea);
                    if(len_JNZ < 63){
                        linea[len_JNZ] = '\n';
                        linea[len_JNZ + 1] = '\0';
                    }
                }
                if(strcmp(linea, "END") == 0){
                    linea[3] = '\n';
                    linea[4] = '\0';
                }
            }
            size_t len = strlen(linea);
            if (len < 64) {
                memset(linea + len, '-', 64 - len);
            }
            fwrite(linea, sizeof(char), 64, archivoDestino);
            contador++;
            memset(linea, ' ', 64);
            if(contador == 4){
                contador = 0;
                break;
            }
            
        }
        TMS[marcoLibre_SWAP] = proceso->GID;
        proceso->paginas[i][0] = 0;
        proceso->paginas[i][1] = 0;
        proceso->paginas[i][2] = marcoLibre_SWAP;
        
    }
}

void guardarTiempos(PCB *procesoSuspendido){
    int numero_aleatorio = (rand() % 9) + 2;
    numero_aleatorio = (rand() % 2);
    numero_aleatorio = 0;
    time(&procesoSuspendido->tiempo_de_salida);
    procesoSuspendido->espera = numero_aleatorio;
}

void imprimirTMM(int TMM[][2]){
    for(int i = 0; i < marcosRAM; i++){
        mvprintw(3+i,140,"%d",i);
        mvprintw(3+i,150,"%d", TMM[i][0]);
        mvprintw(3+i,160,"%d", TMM[i][1]);
    }
}

void imprimirTMS(int TMS[]){
    for(int i = 0; i < 16; i++){
        mvprintw(3+i,175, "%d", i);
        mvprintw(3+i,185, "%d", TMS[i]);
    }
}

void imprimirTMP(PCB *nodo_a_ejecutar){
    int paginas = nodo_a_ejecutar->numPaginas;
    if(paginas > 16){
        paginas = 16;
    }
    for(int i = 0; i < paginas; i++){
        mvprintw(24+i,140, "%d", i);
        mvprintw(24+i,150, "%d", nodo_a_ejecutar->paginas[i][0]);
        mvprintw(24+i,160, "%d", nodo_a_ejecutar->paginas[i][1]);
        mvprintw(24+i,170, "%d", nodo_a_ejecutar->paginas[i][2]);
    }
}   

void calcularPorcentajes_RAM_SWAP(int TMM[][2], int TMS[]){
    int cntRAM = 0;
    int cntSWAP = 0;
    double usoRAM = 0;
    double usoSWAP = 0;

    for(int i = 0; i < marcosRAM; i++){
        if(TMM[i][0] != 0){
            cntRAM++;
        }
    }
    usoRAM = (cntRAM/(double)marcosRAM) * 100.0;

    for(int i = 0; i < marcosSWAP; i++){
        if(TMS[i] != 0){
            cntSWAP++;
        }
    }
    usoSWAP = (cntSWAP/(double)marcosSWAP) * 100.0;

    mvprintw(24,190, "                            ");
    mvprintw(25,190, "                            ");

    mvprintw(24,190, "USO RAM:  %.3f %%", usoRAM);
    mvprintw(25,190, "USO SWAP: %.3f %%", usoSWAP);
    refresh();
}

void liberar_marcos_RAM_SWAP(PCB *proceso, int TMM[][2], int TMS[]){
    int marcoRAM;
    int marcoSWAP;
    int entradasTMP;
    entradasTMP = proceso->numPaginas; 
    for(int i = 0 ; i < entradasTMP ; i++){
        if(proceso->paginas[i][0] == 1){
            marcoRAM = proceso->paginas[i][1];
            TMM[marcoRAM][0] = 0;
            TMM[marcoRAM][1] = 0;
        }
        proceso->paginas[i][0] = 0;
        marcoSWAP = proceso->paginas[i][2]; 
        TMS[marcoSWAP] = 0;  
    }
    calcularPorcentajes_RAM_SWAP(TMM,TMS);
}

void limpiarTMP(PCB *proceso){
    for(int i = 0; i < (proceso->numPaginas); i++){
        proceso->paginas[i][0] = 0;
        proceso->paginas[i][1] = 0;
        proceso->paginas[i][2] = 0;
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
        numeroPaginas = (nuevos->sig)->numPaginas;

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
            mvprintw(numLineaErrorLista,4,"Proceso a nuevos debido a memoria virtual insuficiente");
            refresh();
            usleep(500000);
            limpiarLinea(numLineaErrorLista);
        }
    }
    return 1;
}

PCB *sacarSuspendidos(PCB *suspendidos, PCB *listos, PCB *ejecucion, FILE *memoriaVirtual, char RAM[], int TMM[][2], int TMS[]){
    
    time_t diferencia;
    time_t tiempo_actual;
    int pagina_instruccion;
    PCB *temp1 = suspendidos;
    PCB *temp2 = suspendidos->sig;

    while(temp1->sig != NULL){
        time(&tiempo_actual); 
        diferencia = tiempo_actual - temp2->tiempo_de_salida;
        if(diferencia >= (time_t)temp2->espera){
            pagina_instruccion = temp2->PC/4;
            if((temp2->paginas[pagina_instruccion][0]) == 0){
                cargar_a_memoria_RAM(memoriaVirtual, RAM, TMM, temp2, pagina_instruccion, listos, ejecucion, suspendidos);
                calcularPorcentajes_RAM_SWAP(TMM,TMS);
            }
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