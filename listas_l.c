#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <curses.h>

PCB *crear_nodo(int pid, int gid, char nombre_proceso[], int PC, char IR[]){

    PCB *nuevo = malloc(sizeof(PCB));
    nuevo->PID = pid;
    nuevo->GID = gid;
    strncpy(nuevo->nombre_proceso, nombre_proceso, sizeof(nuevo->nombre_proceso) - 1);
    nuevo->PC = PC;
    strncpy(nuevo->IR, IR, sizeof(nuevo->IR) - 1);
    nuevo->EAX = 0;
    nuevo->EBX = 0;
    nuevo->ECX = 0;
    nuevo->EDX = 0;
    nuevo->CPU = 0;
    nuevo->GCPU = 0;
    nuevo->P = 60;
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
            mvprintw(*numLinea,32, "%s","Listo");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,80, "%d", temp->EAX);
            mvprintw(*numLinea,96, "%d", temp->EBX);
            mvprintw(*numLinea,112, "%d", temp->ECX);
            mvprintw(*numLinea,128, "%d", temp->EDX);
            mvprintw(*numLinea,140, "%d", temp->CPU);
            mvprintw(*numLinea,148, "%d", temp->GCPU);
            refresh();
        }
        if(numLista == 2){
            mvprintw(*numLinea,4, "%d     %d     %s\t\t%s\t%s",temp->PID,temp->GID,temp->nombre_proceso, "Ejecucion", "-----------------------------------------------------------------------------------");
            refresh();
        }
        
        if(numLista == 3){
            mvprintw(*numLinea,4, "%d", temp->PID);
             mvprintw(*numLinea,10, "%d", temp->GID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Terminado");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,80, "%d", temp->EAX);
            mvprintw(*numLinea,96, "%d", temp->EBX);
            mvprintw(*numLinea,112, "%d", temp->ECX);
            mvprintw(*numLinea,128, "%d", temp->EDX);
            mvprintw(*numLinea,140, "%d", temp->CPU);
            mvprintw(*numLinea,148, "%d", temp->GCPU);
            refresh();
        }
        (*numLinea)++;
    } 
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

void actualizar_PCB_del_grupo(PCB *lista, int GCPU_temp, int num_GID, float Wk, int base){
    PCB *nodo_a_actualizar;
    int prioridad;
    while(( nodo_a_actualizar = buscar_sacar(lista,num_GID,1) ) != NULL){
        nodo_a_actualizar->GCPU = GCPU_temp / 2;
        prioridad = base + ( nodo_a_actualizar->CPU / 2 ) + ( nodo_a_actualizar->GCPU / (4 * Wk) );
        nodo_a_actualizar->P = prioridad; 
    }
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