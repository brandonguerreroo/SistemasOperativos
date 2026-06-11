#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <curses.h>
#include <unistd.h>

PCB *crear_nodo(int pid, int gid, char nombre_proceso[], int PC, int numeroPaginas, int numInstrucciones, PCB *nodoCopiar){
    PCB *nuevo = malloc(sizeof(PCB));
    nuevo->PID = pid;
    nuevo->GID = gid;
    strncpy(nuevo->nombre_proceso, nombre_proceso, sizeof(nuevo->nombre_proceso) - 1);
    nuevo->nombre_proceso[sizeof(nuevo->nombre_proceso) - 1] = '\0';
    nuevo->PC = PC;
    strncpy(nuevo->IR,"", sizeof(nuevo->IR) - 1);
    nuevo->EAX = 0;
    nuevo->EBX = 0;
    nuevo->ECX = 0;
    nuevo->EDX = 0;
    nuevo->CPU = 0;
    nuevo->GCPU = 0;
    nuevo->P = 60;
    nuevo->numInstrucciones = numInstrucciones;
    if(nodoCopiar == NULL){
        nuevo->paginas = malloc(numeroPaginas * sizeof(*nuevo->paginas));
        if (nuevo->paginas == NULL) {
            mvprintw(numLineaErrorLista,4,"Error: Memoria insuficiente");
            refresh();
            sleep(1);
        }
    }else{
        nuevo->paginas = nodoCopiar->paginas;
    }
    nuevo->sig = NULL; //el sig del nodo nuevo debe apuntar a nulo porque lo vamos a insertar al final
    return nuevo;
}

void insertar(PCB *lista, PCB *nuevo){
    
    PCB *temp = lista; //para insertar otro nodo creamos un temp que igualmente apunte a la cabeza de la lista
    
    while(temp->sig != NULL){ //recorremos la lista hasta que el sig de algun nodo sea igual a NULL
        temp = temp->sig;
    }
    temp->sig = nuevo;  //"metemos" el nuevo nodo haciendo que el sig del actual nodo final apunte a este

}

void imprimir(PCB *lista, int numLista, int *numLinea){
    PCB *temp = lista; //para imprimir nodos creamos un temp que apunte a la cabeza de la lista
    while(temp->sig != NULL){ //recorremos la lista hasta que el sig de algun nodo sea igual a NULL
        temp = temp->sig;
        if(numLista == 1){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,24, "%d", temp->espera);
            mvprintw(*numLinea,32, "%s","Listo");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,76, "%d", temp->EAX);
            mvprintw(*numLinea,84, "%d", temp->EBX);
            mvprintw(*numLinea,92, "%d", temp->ECX);
            mvprintw(*numLinea,100, "%d", temp->EDX);
            mvprintw(*numLinea,108, "%d", temp->P);
            mvprintw(*numLinea,116, "%d", temp->CPU);
            mvprintw(*numLinea,124, "%d", temp->GCPU);
            refresh();
        }
        if(numLista == 2){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Ejecucion");
            mvprintw(*numLinea,48, "--------------------------------------------------------------------------------");
            refresh();
        }
        
        if(numLista == 3){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Terminado");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
           mvprintw(*numLinea,76, "%d", temp->EAX);
            mvprintw(*numLinea,84, "%d", temp->EBX);
            mvprintw(*numLinea,92, "%d", temp->ECX);
            mvprintw(*numLinea,100, "%d", temp->EDX);
            mvprintw(*numLinea,108, "%d", temp->P);
            mvprintw(*numLinea,116, "%d", temp->CPU);
            mvprintw(*numLinea,124, "%d", temp->GCPU);
            refresh();
        }
        if(numLista == 4){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Nuevos");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,76, "%d", temp->EAX);
            mvprintw(*numLinea,84, "%d", temp->EBX);
            mvprintw(*numLinea,92, "%d", temp->ECX);
            mvprintw(*numLinea,100, "%d", temp->EDX);
            mvprintw(*numLinea,108, "%d", temp->P);
            mvprintw(*numLinea,116, "%d", temp->CPU);
            mvprintw(*numLinea,124, "%d", temp->GCPU);
            refresh();
        }
        if(numLista == 5){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,24, "%d", temp->espera);
            mvprintw(*numLinea,32, "%s","Suspendidos");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,76, "%d", temp->EAX);
            mvprintw(*numLinea,84, "%d", temp->EBX);
            mvprintw(*numLinea,92, "%d", temp->ECX);
            mvprintw(*numLinea,100, "%d", temp->EDX);
            mvprintw(*numLinea,108, "%d", temp->P);
            mvprintw(*numLinea,116, "%d", temp->CPU);
            mvprintw(*numLinea,124, "%d", temp->GCPU);
            refresh();
        }
        (*numLinea)++;
    } 
}

void mostrarEncabezados(){
        mvprintw(1,4,"PC\t\tIR\t\tEAX\t\tEBX\t\tECX\t\tEDX\t  CPU\t    GCPU");
        mvprintw(7,4, "PID");
        mvprintw(7,10, "GID");
        mvprintw(7,16, "Nombre");
        mvprintw(7,32, "Estado");
        mvprintw(7,56, "IR");
        mvprintw(7,48, "PC");
        mvprintw(7,76, "EAX");
        mvprintw(7,84, "EBX");
        mvprintw(7,92, "ECX");
        mvprintw(7,100, "EDX");
        mvprintw(7,108, "P");
        mvprintw(7,116, "CPU");
        mvprintw(7,124, "GCPU");
        refresh();
}
void mostrarPantalla(int TMS[], int TMM[][2], PCB *nodo_a_ejecutar){
        mostrarEncabezados();
        mvprintw(1,151,"TMM");
        mvprintw(2,140,"Marco");
        mvprintw(2,150,"Dueño");
        mvprintw(2,160,"Reloj");
        refresh();
        imprimirTMM(TMM);
        mvprintw(1,181,"TMS");
        mvprintw(2,175,"Marco");
        mvprintw(2,185,"Dueño");
        imprimirTMS(TMS);
        refresh();
        mvprintw(22,160,"TMP");
        mvprintw(23,145,"Pagina");
        mvprintw(23,155,"Bit");
        mvprintw(23,165,"MarcoRAM");
        mvprintw(23,175,"MarcoSWAP");
        imprimirTMP(nodo_a_ejecutar);
        refresh();
}

PCB *sacarFrente(PCB *lista){  
    
    if(lista->sig == NULL){
        return NULL;
    }

    PCB *temp = lista->sig;
    lista->sig = temp->sig;
    temp->sig=NULL;
    return temp;
}   

void actualizar_PCBs(PCB *lista, int GCPU_temp, int num_GID, float Wk, int base){
    int prioridad;
    PCB *temp = lista; 
    while(temp->sig != NULL){ 
        temp = temp->sig;
        if(temp->GID == num_GID){
            temp->CPU = temp->CPU / 2;
            temp->GCPU = GCPU_temp / 2;
            prioridad = base + ( temp->CPU / 2 ) + ( temp->GCPU / (4.0 * Wk) );
            temp->P = prioridad; 
        }
        else{
            temp->CPU = temp->CPU / 2;
            temp->GCPU = temp->GCPU / 2;
            prioridad = base + ( temp->CPU / 2 ) + ( temp->GCPU / (4.0 * Wk) );
            temp->P = prioridad; 
        }
    }
};

PCB *buscar_por_prioridad(PCB *lista){
    PCB *temp = lista;
    PCB *temp2 = lista->sig;
    if(temp2 == NULL){
        return NULL;
    }
    PCB *nodoDeMayorPrioridad = temp2; // Es el primero de la lista
    PCB *anteriorNodoMayorPrioridad = temp;
    while(temp2->sig != NULL){ 
        temp2 = temp2->sig;
        temp = temp->sig;
        if(temp2->P < nodoDeMayorPrioridad->P){
            nodoDeMayorPrioridad = temp2;
            anteriorNodoMayorPrioridad = temp;
        }
    }
    anteriorNodoMayorPrioridad->sig = nodoDeMayorPrioridad->sig;
    nodoDeMayorPrioridad->sig = NULL;
    return nodoDeMayorPrioridad;
};


PCB *buscar_sacar(PCB *lista, int num_PID, bool condicion){  
    //si la condicion esta en 1 busca y regresa si lo encuentra
    //si no busca y desenlaza si lo encuentra
    
    if(lista->sig == NULL){
        return NULL;
    }

    PCB *temp1 = lista;
    PCB *temp2 = lista->sig;

    while(temp1->sig != NULL){
        if(temp2->PID == num_PID){
            if(condicion == 1){
                return temp2; //solo buscar
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

PCB *buscarPorGID(PCB *lista, int num_GID){  
    if(lista->sig == NULL){
        return NULL;
    }

    PCB *temp1 = lista;
    PCB *temp2 = lista->sig;

    while(temp1->sig != NULL){
        if(temp2->GID == num_GID){
            return temp2;
        }
        else{
            temp1= temp1->sig;
            temp2= temp2->sig;
        }
    }
    return NULL;
}

void imprimirListas(PCB *ejecucion, PCB *listos, PCB *nuevos, PCB *suspendidos, PCB *terminados, int *numLineaLista){
    imprimir(ejecucion, 2, numLineaLista);
    imprimir(listos, 1, numLineaLista);
    imprimir(nuevos, 4, numLineaLista);
    imprimir(suspendidos, 5, numLineaLista);
    imprimir(terminados, 3, numLineaLista);
}

void verERROR(){
    mvprintw(numLineaErrorLista,4,"AQUI");
    refresh();
    sleep(1);
    limpiarLinea(numLineaErrorLista);
}