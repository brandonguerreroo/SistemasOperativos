#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct PCB {
    int PID;
    char nombre_proceso[50];
    int PC;
    int estado;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    struct PCB *sig;
}PCB;

PCB *crear_nodo(int pid, char nombre_proceso[], int PC, char IR[], int EAX, int EBX, int ECX, int EDX){

    PCB *nuevo = malloc(sizeof(PCB));
    nuevo->PID = pid;
    strcpy(nuevo->nombre_proceso, nombre_proceso); //Cambiar por strncpy
    nuevo->PC = PC;
    strcpy(nuevo->IR, IR); //Cambiar por strncpy
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

void imprimir(PCB *lista){

        PCB *temp = lista; //para imprimir nodos creamos un temp que apunte a la cabeza de la lista
        while(temp->sig != NULL){ //recorremos la lista hasta que el sig de algun nodo sea igual a NULL
            temp = temp->sig;
            printf("%d\t%s\t%d\t%d\t%s\t%d\t%d\t%d\t%d\n",temp->PID,temp->nombre_proceso, temp->PC, temp->estado, temp->IR, temp->EAX, temp->EBX, temp->ECX, temp->EDX);
        }
}
int main() {
    char nombre_proceso[50];
    PCB listos; //cabeza
    //struct PCB *ejecucion = NULL;
    //struct PCB *terminados = NULL;

    //FILE *arc_instrucciones = fopen("hola.txt","r");
    for(int i = 1; i <= 6 ; i++){
        printf("Ingrese el nombre del proceso: \n");
        scanf("%s", nombre_proceso);
        PCB *nuevo = crear_nodo(i,nombre_proceso,0,"MOV",0,0,0,0);
        insertar(&listos, nuevo);
        imprimir(&listos);
    }

    //imprimir(&listos);
    return 0;
}