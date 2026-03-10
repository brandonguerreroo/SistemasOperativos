#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct PCB {
    int PID;
    char nombre_proceso[50];
    int PC;
    int estado;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    struct PCB *sig;
};

struct PCB *crear_nodo(int pid, char np[]){

    struct PCB *nuevo = malloc(sizeof(struct PCB));
    nuevo->PID = pid;
    strcpy(nuevo->nombre_proceso, "hola.txt");
    nuevo->sig = NULL; //el sig del nodo nuevo debe apuntar a nulo porque lo vamos a insertar al final

    return nuevo;
}

void insertar(struct PCB **lista, struct PCB *nuevo){

    if(*lista == NULL){
        *lista = nuevo; //la cabeza debe apuntar al nodo nuevo unicamente si la lista estaba vacia.
        return;
    }
    
    struct PCB *temp = *lista; //para insertar otro nodo creamos un temp que igualmente apunte a la cabeza de la lista
    
    while(temp->sig != NULL){ //recorremos la lista hasta que el sig de algun nodo sea igual a NULL
        temp = temp->sig;
    }
    temp->sig = nuevo;  //"metemos" el nuevo nodo haciendo que el sig del actual nodo final apunte a este

}

void imprimir(struct PCB **lista){

    if(*lista != NULL){

        struct PCB *temp = *lista; //para imprimir nodos creamos un temp que apunte a la cabeza de la lista
        
        while(1){ //recorremos la lista hasta que el sig de algun nodo sea igual a NULL
            printf("%d\t%s\n",temp->PID,temp->nombre_proceso);
            if(temp->sig == NULL){
                break;
            }
            temp = temp->sig;
        }
        
    }
}


int main() {
    struct PCB *listos = NULL; //cabeza
    struct PCB *ejecucion = NULL;
    struct PCB *terminados = NULL;

    for(int i = 0; i < 6 ; i++){
        struct PCB *nuevo = crear_nodo(i,"hola.txt");
        insertar(&listos, nuevo);
    }

    imprimir(&listos);

    return 0;
}
