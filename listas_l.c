#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
                return temp2;
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

void matar(PCB *lista_listos, PCB *lista_ejecucion, PCB *lista_terminados, int num_PID){
    PCB *matar;
    if((matar = buscar_sacar(lista_listos, num_PID, 0)) != NULL){
        insertar(lista_terminados, matar);
    }
    else if((matar = buscar_sacar(lista_ejecucion, num_PID, 0)) != NULL){
        insertar(lista_terminados, matar);
    }
    else if((matar = buscar_sacar(lista_terminados, num_PID, 1)) != NULL){
        printf("\nEste proceso ya esta terminado\n");
    }
    else{
        printf("\nNo existe ese proceso\n");
    }
}

int main() {
    char nombre_proceso[50];
    PCB listos; //cabeza
    PCB ejecucion;
    PCB terminados;
    int num_PID = 4;

    listos.sig = NULL;
    ejecucion.sig = NULL;
    terminados.sig = NULL;

    //FILE *arc_instrucciones = fopen("hola.txt","r");
    for(int i = 1; i <= 4 ; i++){
        printf("Ingrese el nombre del proceso: \n");
        scanf("%s", nombre_proceso);
        PCB *nuevo = crear_nodo(i,nombre_proceso,0,"MOV",0,0,0,0);
        insertar(&listos, nuevo);
    }
    printf("LISTOS\n");
    imprimir(&listos);

    PCB *meterEjecucion = sacarFrente(&listos);  
    insertar(&ejecucion, meterEjecucion);
    printf("LISTOS\n");
    imprimir(&listos);
    printf("EJECUCION\n");
    imprimir(&ejecucion);

    matar(&listos, &ejecucion, &terminados, num_PID);

    printf("LISTOS\n");
    imprimir(&listos);
    printf("EJECUCION\n");
    imprimir(&ejecucion);
    printf("TERMINADOS\n");
    imprimir(&terminados);

    matar(&listos, &ejecucion, &terminados, 4);
    printf("TERMINADOS\n");
    imprimir(&terminados);

    //imprimir(&listos);
    return 0;
}        