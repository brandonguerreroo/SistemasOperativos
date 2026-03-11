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

struct PCB *crear_nodo(int pid, char nombre_proceso[], int PC, int estado, char IR[], int EAX, int EBX, int ECX, int EDX){

    struct PCB *nuevo = malloc(sizeof(struct PCB));
    nuevo->PID = pid;
    strcpy(nuevo->nombre_proceso, nombre_proceso); //Cambiar por strncpy
    nuevo->PC = PC;
    nuevo->estado = estado;
    strcpy(nuevo->IR, IR); //Cambiar por strncpy
    nuevo->EAX = EAX;
    nuevo->EBX = EBX;
    nuevo->ECX = ECX;
    nuevo->EDX = EDX;
    
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
            printf("%d\t%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\n",temp->PID,temp->nombre_proceso, temp->PC, temp->estado, temp->IR, temp->EAX, temp->EBX, temp->ECX, temp->EDX);
            if(temp->sig == NULL){
                break;
            }
            temp = temp->sig;
        }
        
    }
}
int main() {
    char nombre_proceso[50];
    struct PCB *listos = NULL; //cabeza
    struct PCB *ejecucion = NULL;
    struct PCB *terminados = NULL;

    //FILE *arc_instrucciones = fopen("hola.txt","r");

    for(int i = 0; i < 6 ; i++){
        printf("Ingrese el nombre del proceso: \n");
        scanf("%s", nombre_proceso);
        struct PCB *nuevo = crear_nodo(i,nombre_proceso,0,0,"ejecucion",0,0,0,0);
        insertar(&listos, nuevo);
        imprimir(&listos);
    }

    //imprimir(&listos);
    return 0;
}