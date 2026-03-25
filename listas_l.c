#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LISTAS.h"
#include <curses.h>

PCB *crear_nodo(int pid, char nombre_proceso[], int PC, char IR[], int EAX, int EBX, int ECX, int EDX){

    PCB *nuevo = malloc(sizeof(PCB));
    nuevo->PID = pid;
    strncpy(nuevo->nombre_proceso, nombre_proceso, sizeof(nuevo->nombre_proceso) - 1);
    nuevo->PC = PC;
    strncpy(nuevo->IR, IR, sizeof(nuevo->IR) - 1);
    nuevo->EAX = EAX;
    nuevo->EBX = EBX;
    nuevo->ECX = ECX;
    nuevo->EDX = EDX;
    
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
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Listo");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,72, "%d\t%d\t%d\t%d", temp->EAX, temp->EBX, temp->ECX, temp->EDX);
            refresh();
        }
        if(numLista == 2){
            mvprintw(*numLinea,4, "%d\t\t%s\t\t%s\t%s",temp->PID,temp->nombre_proceso, "Ejecucion", "---------------------------------------------------");
            refresh();
        }
        
        if(numLista == 3){
            mvprintw(*numLinea,4, "%d", temp->PID);
            mvprintw(*numLinea,16, "%s", temp->nombre_proceso);
            mvprintw(*numLinea,32, "%s","Terminado");
            mvprintw(*numLinea,48, "%d", temp->PC);
            mvprintw(*numLinea,56, "%s", temp->IR);
            mvprintw(*numLinea,72, "%d\t%d\t%d\t%d", temp->EAX, temp->EBX, temp->ECX, temp->EDX);
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

/*void matar(PCB *lista_listos, PCB *lista_ejecucion, PCB *lista_terminados, int num_PID){
    PCB *matar;
    if((matar = buscar_sacar(lista_listos, num_PID, 0)) != NULL){
        insertar(lista_terminados, matar);
    }
    else if((matar = buscar_sacar(lista_ejecucion, num_PID, 0)) != NULL){
        insertar(lista_terminados, matar);
    }
    else{
        mvprintw(5,4,"No existe ese proceso"); //mvprint
    }
}*/