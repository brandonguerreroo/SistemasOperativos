// git add lab_sis.c
// git commit -m "comentarios"
// git push -u

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>
#include <sys/select.h>
#include <unistd.h>
#include "LISTAS.h"
#include <time.h>
#include <limits.h>

FILE *memoriaVirtual;
int EAX = 0; 
int EBX = 0; 
int ECX = 0; 
int EDX = 0; 
int PC = 0;
int PID = 0;
int GID = 0;
float Wk = 0;
int numeroDeGrupos = 0;
int base = 60;
int CPU_temp = 0;
int GCPU_temp = 0;

PCB listos;
PCB ejecucion;
PCB terminados;
PCB nuevos;
PCB suspendidos;

int numLineaLista = 8;
char copiaNombre_archivo[50];
int Q = 3;
bool mataEjecucion = false;
char copiaLinea[65];
bool terminoProceso = false; // Se ocupa para verificar que un proceso va pasar a lista de terminados, nos sirve para el numeroDeGrupos
bool ultimoProcesoGrupo = false;
char RAM[4096]; // 64 * 64
int TMS[marcosSWAP] = {0};
int TMM[marcosRAM][2] = {0};
char nombreArchivoSWAP[] = "memoriavirtual.bin";

int kbhit(void);
void limpiarLinea(int num)
{
    mvprintw(num, 4, "                                                                                                                                  ");
    refresh();
}
int cerrarArch_error(int num){
   
    switch(num){
        case 1:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea invalida debido a numero mayor de argumentos", copiaNombre_archivo, PC); 
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 2:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea invalida debido a uno o varios argumentos nulos (revisa sintaxis)", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 3:
            mvprintw(numLineaErrorLista,4,"%s\t%d\ttercer argumento invalido, revisa sintaxis", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 4:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tel segundo argumento no corresponde a un registro valido", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 5:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tno se encontro una sentencia END", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 6:
            mvprintw(numLineaErrorLista,4,"%s\t%d\terror division por 0", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 7:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tinstruccion inicial no valida", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 8:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tdemasiados argumentos en sentencia END", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 9:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tsintaxis incorrecta en sentencias INC o DEC", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 10:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea de instruccion demasiado larga o falta END (revisar archivo)", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 11:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tel PC no es un numero en instruccion JNZ", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 12:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tsintaxis incorrecta en sentencia JNZ", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 13:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tel PC excede el numero de instrucciones del archivo en instruccion JNZ", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 14:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tse rebasaron los limites de instruccion del archivo", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 15:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tDesbordamiento en rv_token en instrucciones MOV,ADD,SUB,MUL,DIV", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 16:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tOverflow en numero de instruccion en JNZ", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
        case 17:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tDesbordamiento en sentencias INC o DEC", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
        case 18:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tDesbordamiento en operaciones ADD, SUB, MUL", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);

    }
    
    return 0;
}
// Recibe un string con el nombre del registro y devuelve un puntero al registro correspondiente, si no se encuentra el registro devuelve NULL
int *obtener_registro(char *reg){
    if(strcmp(reg,"EAX") == 0){ 
        return &EAX;
    }
    if(strcmp(reg,"EBX") == 0){ 
        return &EBX;
    }
    if(strcmp(reg,"ECX") == 0){
        return &ECX;
    }
    if(strcmp(reg,"EDX") == 0){
        return &EDX;
    }
    return NULL;
}

int MOV_ADD_SUB_MUL_DIV(char inst_to[], char reg_to[], char rv_to[]){
    
    int *destino, *origen;
    int valor;
    int len = strlen(rv_to);
    bool caracter = false;

    destino = obtener_registro(reg_to);
    
    if(destino == NULL){
        cerrarArch_error(4);
        return 1;
    }

    origen = obtener_registro(rv_to);

    if(origen != NULL){
        valor = *origen;
    }
    else{
        for(int i = 0; i < len; i++){   
            if (rv_to[i] < '0' || rv_to[i] > '9') {
                caracter = true;
                if(i == 0){
                    if(rv_to[i] == '-'){
                    caracter = false;
                    }
                }
            }
        }
        if(len == 1 && rv_to[0] == '-') // Caso para cuando solo haya un '-' sin numero
        {
            caracter = true;
        }
        
        if(caracter){
            cerrarArch_error(3);
            return 1;
        }
        else{
            if(verificarDesbordamiento(rv_to) == 1){
                cerrarArch_error(15);
                return 1;
            }
            valor = atoi(rv_to);
        }
    }

    if(strcmp(inst_to,"MOV") == 0){
        *destino = valor;
    }
    else if(strcmp(inst_to,"ADD") == 0){
        if(*destino == 2147483647 && valor >= 0){
            cerrarArch_error(18);
            return 1;
        }
        *destino = *destino + valor;
    }
    else if(strcmp(inst_to,"SUB") == 0){
        if(*destino == -2147483647 && valor >= 0){
            cerrarArch_error(18);
            return 1;
        }
        *destino = *destino - valor;
    }
    else if(strcmp(inst_to,"MUL") == 0){
        long long valorMaximo = (long long)(*destino) * valor;
        if(valorMaximo > 2147483647 || valorMaximo < -2147483647){
            cerrarArch_error(18);
            return 1;
        }
        *destino = *destino * valor;
    }
    else if(strcmp(inst_to,"DIV") == 0){
        if(valor == 0){
            cerrarArch_error(6);
            return 1;
        }
        *destino = *destino / valor;
    }
    return 0;
}

int INC_DEC(char inst_to[], char reg_to[]){
    
    int *destino;

    destino = obtener_registro(reg_to);
    
    if(destino == NULL){
        cerrarArch_error(4);
        return 1;
    }
    
    if(strcmp(inst_to,"INC") == 0){
        if(*destino == 2147483647){
            cerrarArch_error(17);
            return 1;
        }
        (*destino)++;
    }
    else if(strcmp(inst_to,"DEC") == 0){
        if(*destino == -2147483647){
            cerrarArch_error(17);
            return 1;
        }
        (*destino)--;
    }
    
    return 0;
}
int JNZ(char reg_to[], bool *instJNZ, PCB *proceso){
    int len = strlen(reg_to);
    for(int j = 0; j < len; j++){   
        if (reg_to[j] < '0' || reg_to[j] > '9'){
            cerrarArch_error(11);
            return 1;
        }
    }
    if(verificarDesbordamiento(reg_to) == 1){
        cerrarArch_error(16);
        return 1;
    }
    int valor = atoi(reg_to);

    if(ECX != 0){
        if(valor > (proceso->numInstrucciones -1)){
            cerrarArch_error(13);
            return 1;
        }
        PC = valor;
        *instJNZ = true;
    }
    return 0;
}

void limpiar(){  //Limpia la pantalla desde la linea 8 hasta el ultimo renglon que se imprimio
    for(int l = 8; l <= numLineaLista; l++){
        mvprintw(l, 4, "                                                                                                                                  ");
    }
    refresh();
    numLineaLista = 8;   
}
void guardarContexto(PCB *nodo, char linea[])
{
    nodo->EAX = EAX;
    nodo->EBX = EBX;
    nodo->ECX = ECX;
    nodo->EDX = EDX;
    strncpy(nodo->IR,linea, sizeof(nodo->IR) - 1);
    nodo->IR[sizeof(nodo->IR)-1] = '\0';
    nodo->PC = PC;
    nodo->CPU = CPU_temp / 2;  //Actualizar los valores para este nodo
    nodo->GCPU = GCPU_temp / 2;
    if(((buscarPorGID(&listos, nodo->GID)) == NULL) && (terminoProceso == true) && ((buscarPorGID(&suspendidos, nodo->GID)) == NULL)){
        numeroDeGrupos--;
        ultimoProcesoGrupo = true;
    }
    
    // Esto nos sirve para no hacer division sobre cero.
    if(numeroDeGrupos == 0){
        Wk = 1.0;
    }
    else if(numeroDeGrupos > 0){
        Wk = 1.0 / numeroDeGrupos;
    }
    nodo->P = base + ( nodo->CPU / 2 ) + ( nodo->GCPU / (4.0 * Wk) );
    actualizar_PCBs(&listos,GCPU_temp, nodo->GID, Wk, base); //actualizar los valores para los demas procesos del mismo grupo
    actualizar_PCBs(&suspendidos,GCPU_temp, nodo->GID, Wk, base);
}

void meterEnTerminados(char linea[]){
    PCB *nodo; 
    nodo = sacarFrente(&ejecucion);
    terminoProceso = true;
    guardarContexto(nodo, linea);
    terminoProceso = false;
    if(ultimoProcesoGrupo){
        liberar_marcos_RAM_SWAP(nodo,TMM,TMS);
        limpiarTMP(nodo);
        mostrarTablas(TMS,TMM,nodo);
        sleep(1);
        ultimoProcesoGrupo = false;
    }
    insertar(&terminados, nodo);
    mostrarTablas(TMS,TMM,nodo);
    cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS);
    
    limpiar();
    //Imprimir cada que cambie la lista de terminados
    imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
}
int matar(int num_PID){
    PCB *matar;
    if((matar = buscar_sacar(&listos, num_PID, 0)) != NULL){
        if(((buscarPorGID(&listos, matar->GID)) == NULL) && ((buscarPorGID(&ejecucion, matar->GID)) == NULL) && ((buscarPorGID(&suspendidos, matar->GID)) == NULL)){
            numeroDeGrupos--;
            ultimoProcesoGrupo = true;
        }
        if(ultimoProcesoGrupo){
            liberar_marcos_RAM_SWAP(matar,TMM,TMS);
            limpiarTMP(matar);
            mostrarTablas(TMS,TMM,matar);
            sleep(1);
            ultimoProcesoGrupo = false;
        }
        insertar(&terminados, matar);
        mostrarTablas(TMS,TMM,matar);
        cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS);
        
        return 0;
    }
    else if((matar = buscar_sacar(&ejecucion, num_PID, 0)) != NULL){
        terminoProceso = true;
        guardarContexto(matar, copiaLinea);
        terminoProceso = false;
        if(ultimoProcesoGrupo){
            liberar_marcos_RAM_SWAP(matar,TMM,TMS);
            limpiarTMP(matar);
            mostrarTablas(TMS,TMM,matar);
            sleep(1);
            ultimoProcesoGrupo = false;
        }
        insertar(&terminados, matar);
        mostrarTablas(TMS,TMM,matar);
        cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS);
        
        return 1;
    }
    else if((matar = buscar_sacar(&suspendidos, num_PID, 0)) != NULL){
        if(((buscarPorGID(&listos, matar->GID)) == NULL) && ((buscarPorGID(&ejecucion, matar->GID)) == NULL) && ((buscarPorGID(&suspendidos, matar->GID)) == NULL)){
            numeroDeGrupos--;
            ultimoProcesoGrupo = true;
        }
        
        if(ultimoProcesoGrupo){
            liberar_marcos_RAM_SWAP(matar,TMM,TMS);
            limpiarTMP(matar);
            mostrarTablas(TMS,TMM,matar);
            sleep(1);
            ultimoProcesoGrupo = false;
        }
        insertar(&terminados, matar);
        mostrarTablas(TMS,TMM,matar);
        cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS); // Intentar cargar nuevos
        return 0;
    }
    else if((matar = buscar_sacar(&nuevos, num_PID, 0)) != NULL){
        numeroDeGrupos--;
        insertar(&terminados, matar);
        mostrarTablas(TMS,TMM,matar);
        cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS); // Intentar cargar nuevos
        return 0;
    }
    else{
        mvprintw(5,4,"No existe ese proceso o ya se encuentra en terminados");
        refresh();
        sleep(1);
        limpiarLinea(numLineaErrorLista);
        return 0;
    }
}

void ciclo_kbhit(bool *cortar, char nombre_archivo[], bool *salir, bool *ejecuta, bool end, size_t tam_arch, int num_ciclo){ 

    char cad[50];
	char *comando = cad; 
	char *token_comandos;
    char comando_to[10];
    char archivo_to[50];
    char noinst[50];
    int procesoPID_mata, procesoPID_fork;
    bool noinst_no_number = false;
    int numeroDeInstruccion;
    int numeroPaginas;
    while(*cortar == false){    

        comando_to[0] = '\0';
        archivo_to[0] = '\0';
        noinst[0] = '\0';
        cad[0] = '\0';
        nombre_archivo[0] = '\0';

        if(kbhit()){
            limpiarLinea(numLineaComando);
            mvscanw(numLineaComando,4,"%49[^\n]",cad);
        }
        else{
            cad[0] = '\0';
        }
        comando = cad;
        token_comandos = strsep(&comando, " ");
        if(token_comandos != NULL){
            strncpy(comando_to, token_comandos, sizeof(comando_to) - 1);
            comando_to[sizeof(comando_to) - 1] = '\0';
        }

        token_comandos = strsep(&comando, " ");
        if(token_comandos != NULL){
            strncpy(archivo_to, token_comandos, sizeof(archivo_to) - 1);
            archivo_to[sizeof(archivo_to) - 1] = '\0';
        }

        token_comandos = strsep(&comando, "\n");
        if(token_comandos != NULL){
            strncpy(noinst, token_comandos, sizeof(noinst) - 1);
            noinst[sizeof(noinst) - 1] = '\0';
        }

        if((strcmp(comando_to,"mata") == 0) && (archivo_to[0] != '\0' && (noinst[0] == '\0'))){
            bool PID_no_number = false;
            int longitud = strlen(archivo_to);
            for(int i = 0; i < longitud; i++){   
                if (archivo_to[i] < '0' || archivo_to[i] > '9'){
                    mvprintw(numLineaErrorLista,4, "Error, el PID no es un numero");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    PID_no_number = true;
                    break;
                }
            }
            if(PID_no_number == false){
                if(verificarDesbordamiento(archivo_to) == 1){
                    mvprintw(numLineaErrorLista,4,"OVERFLOW en en PID de Mata");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    break;
                }
                procesoPID_mata = atoi(archivo_to);
                if(matar(procesoPID_mata) == 1){
                    mataEjecucion = true;
                }
            }
            limpiar();
            //Imprimir cada que se mate un proceso
            imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
        }
        else if( (strcmp(comando_to,"salir") == 0) && (archivo_to[0] == '\0') && (noinst[0] == '\0') ){
            *ejecuta = false;
            *salir = true;
            break;
        }
        else if((strcmp(comando_to,"ejecuta") == 0) && (archivo_to[0] != '\0') && (noinst[0] == '\0')){
            int numeroDeInstrucciones;
            limpiarLinea(numLineaErrorLista);
            FILE *archivo = fopen(archivo_to, "rb"); //Nos sirve para poder comprobar que el archivo exista
            if (archivo == NULL){
                mvprintw(numLineaErrorLista,4,"ERROR: archivo no encontrado."); //Si no existe, marcamos error
                refresh();
                sleep(1);
                limpiarLinea(numLineaErrorLista);
                continue;
            } 
            strncpy(nombre_archivo, archivo_to, tam_arch - 1);
            nombre_archivo[tam_arch - 1] = '\0';
            //for(int i = 0; i < 1; i++){
            numeroDeInstrucciones = calcularInstrucciones(archivo);
            numeroPaginas = calcularNumPaginas(numeroDeInstrucciones);
            
            if (archivo != NULL){
                fclose(archivo);
                archivo = NULL;
            }

            PCB *nuevo;
            if(numeroPaginas <= marcosSWAP){
                PID++;
                GID++; 
                numeroDeGrupos++;
                nuevo = crear_nodo(PID, GID, nombre_archivo,0,numeroPaginas,numeroDeInstrucciones, NULL);
                insertar(&nuevos, nuevo);
                limpiar();
                imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista); // Imprimir cada que se mande a nuevos
            }
            else{
                mvprintw(numLineaErrorLista,4, "ERROR. Archivo no cabe en el SWAP");
                refresh();
                sleep(1);
                limpiarLinea(numLineaErrorLista);
                continue;
            }

            if(cargarNuevos(&nuevos,&listos,memoriaVirtual,TMS) == 1){
                continue;
            }
            //}
            *ejecuta = true;
            limpiar();
            //Imprimir cada que cambie se agregue uno nuevo
            imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
            break;
        }
        else if((strcmp(comando_to, "fork") == 0) && (archivo_to[0] != '\0') && (noinst[0] != '\0')){
            char lineaFork[64];
            bool PID_no_number = false;
            int longitud = strlen(archivo_to);
            for(int i = 0; i < longitud; i++){   
                if (archivo_to[i] < '0' || archivo_to[i] > '9'){
                    mvprintw(numLineaErrorLista,4, "Error, el PID no es un numero");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    PID_no_number = true;
                    break;
                }
            }
            if(PID_no_number == false){
                if(verificarDesbordamiento(archivo_to) == 1){
                    mvprintw(numLineaErrorLista,4,"Error, OVERFLOW en en PID de Fork");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    break;
                }
                procesoPID_fork = atoi(archivo_to);
            }
            else{
                break;
            }
            longitud = strlen(noinst); // Reiniciamos
 
            for(int i = 0; i < longitud; i++){   
                if (noinst[i] < '0' || noinst[i] > '9'){
                    mvprintw(numLineaErrorLista,4, "Error, el numero de instruccion no es un numero");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    noinst_no_number = true;
                    break;
                }
            }
            if(noinst_no_number == false){
                if(verificarDesbordamiento(noinst) == 1){
                    mvprintw(numLineaErrorLista,4,"Error, OVERFLOW en numero de instruccion de Fork");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    break;
                }
                numeroDeInstruccion = atoi(noinst);
            }
            else{
                break;
            }
            PCB *nodoCopiar;
            if(((nodoCopiar = buscar_sacar(&ejecucion, procesoPID_fork, 1)) != NULL)){
                if(nodoCopiar->numInstrucciones > numeroDeInstruccion){
                    PID++;
                    PCB *nuevo = crear_nodo(PID, nodoCopiar->GID, nodoCopiar->nombre_proceso,numeroDeInstruccion, nodoCopiar->numPaginas, nodoCopiar->numInstrucciones, nodoCopiar); 
                    insertar(&listos, nuevo); 
                    //no se debe actualizar el gcpu porque al salir el proceso en ejecucion se va a guardar gcpu para todo el grupo
                }
                else{
                    mvprintw(numLineaErrorLista,4,"Error, numero de instruccion no existe en el archivo.");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                }
            }

            else if((nodoCopiar = buscar_sacar(&listos, procesoPID_fork, 1)) != NULL){
                if(nodoCopiar->numInstrucciones > numeroDeInstruccion){
                    PID++;
                    PCB *nuevo = crear_nodo(PID, nodoCopiar->GID, nodoCopiar->nombre_proceso,numeroDeInstruccion, nodoCopiar->numPaginas, nodoCopiar->numInstrucciones,nodoCopiar); 
                    nuevo->GCPU = nodoCopiar->GCPU;  //se debe copiar porque el nuevo proceso pertenece al mismo grupo
                    insertar(&listos, nuevo);
                }
                else{
                    mvprintw(numLineaErrorLista,4,"Error, numero de instruccion no existe en el archivo.");
                    refresh();
                    sleep(1); 
                    limpiarLinea(numLineaErrorLista);
                }
            }
            else if((nodoCopiar = buscar_sacar(&suspendidos, procesoPID_fork, 1)) != NULL){
                if(nodoCopiar->numInstrucciones > numeroDeInstruccion){
                    PID++;
                    PCB *nuevo = crear_nodo(PID, nodoCopiar->GID, nodoCopiar->nombre_proceso,numeroDeInstruccion, nodoCopiar->numPaginas, nodoCopiar->numInstrucciones,nodoCopiar); 
                    nuevo->GCPU = nodoCopiar->GCPU;  //se debe copiar porque el nuevo proceso pertenece al mismo grupo
                    insertar(&listos, nuevo);
                }
                else{
                    mvprintw(numLineaErrorLista,4,"Error, numero de instruccion no existe en el archivo.");
                    refresh();
                    sleep(1); 
                    limpiarLinea(numLineaErrorLista);
                }
            }
            else{
                mvprintw(numLineaErrorLista,4, "Error, ese PID NO existe.");
                refresh();
                sleep(1);
                limpiarLinea(numLineaErrorLista);        
            }
            
            limpiar();
            //Imprimir cada que se copie un proceso
            imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
            break;
        }
        else if (comando_to[0] != '\0' || (comando_to[0] == '\0' && archivo_to[0] != '\0')){
            mvprintw(numLineaErrorLista,4, "Error, comando de terminal no valido");
            refresh();
            sleep(1);
            limpiarLinea(numLineaErrorLista);
            continue;
        }
        
        if(num_ciclo == 1){  
            if((listos.sig != NULL) || (suspendidos.sig != NULL) || (nuevos.sig != NULL)){             
                break;
            }
            else if(comando_to[0] == '\0' || strcmp(comando_to,"mata") == 0){
                continue;
            }
        }
        if(num_ciclo == 2){
            if((listos.sig != NULL && end == true) || (suspendidos.sig != NULL  && end == true) && (nuevos.sig != NULL  && end == true)){ 
                *ejecuta = true;
                break;
            }
            else if(end == true){
                continue;
            }
        }
        *cortar = true;
    }
}

void restaurarContexto(PCB *nodo, char linea[], size_t size_linea)
{
    EAX = nodo->EAX;
    EBX = nodo->EBX;
    ECX = nodo->ECX;
    EDX = nodo->EDX;
    strncpy(linea,nodo->IR, size_linea - 1);
    linea[tam_linea - 1] = '\0';
    PC = nodo->PC;
    CPU_temp = nodo->CPU;
    GCPU_temp = nodo->GCPU;
}

int main(){
    char linea[65];
    char *token;
    char inst_to[5];
    char reg_to[16];
    char rv_to[16]; //register-valor
    bool end = false;
    char *st = linea; // strsep modifica este puntero
    bool coma = false;
    bool espacio = false;
    char nombre_archivo[50];
    bool salir = false;
    bool ejecuta = false;
    bool cortar = false;
    bool error_archivo = false;
    bool entrar = false;
    bool salidaPorQuantum = false;
    bool instJNZ = false;
    bool entroSuspendidos = false;
    listos.sig = NULL;
    ejecucion.sig = NULL;
    terminados.sig = NULL;
    nuevos.sig = NULL;
    suspendidos.sig = NULL;
    int pagina_instruccion;
    int desplazamiento;
    int marcoRAM;
    int direccionFisica;
    int bytesArchivo = 8388608; //2^17 instrucciones * 2^6 tamaño de IR.
    char basura = ' ';
    srand(time(NULL));

    memoriaVirtual = fopen(nombreArchivoSWAP,"r+b");
 
    if(memoriaVirtual == NULL) {
        printf("Error al crear el archivo.\n");
        return 1;
    }

    for (int i = 0; i < bytesArchivo; i++){
        fwrite(&basura, sizeof(char), 1, memoriaVirtual);
    }
    rewind(memoriaVirtual);
    
    initscr();
    while (salir == false){
        salir = false;
        EAX = 0;
        EBX = 0;
        ECX = 0;
        EDX = 0;
        error_archivo = false;
        coma = false;
        espacio = false;
        salidaPorQuantum = false;
        
        mvprintw(0,4," ");
        refresh();
        

        limpiar();
        imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
        if(ejecuta == false){
            ciclo_kbhit(&cortar, nombre_archivo, &salir, &ejecuta, end, sizeof(nombre_archivo), 1);
            if(salir == true){
                continue;
            }
        }
        ejecuta = false;
        end = false;        //debe volver a falso
        cortar = false;

        if(ejecucion.sig == NULL){
            while(suspendidos.sig != NULL){
                PCB *nodo = sacarSuspendidos(&suspendidos,&listos,&ejecucion,memoriaVirtual,RAM, TMM, TMS);
                
                if(nodo != NULL){
                    insertar(&listos, nodo);
                    mostrarTablas(TMS, TMM, nodo);
                }
                else{
                    break;
                }
            }

            //Comparar todas las prioridades de la lista de listos para meter a ejecucion (el de la prioridad mas alta que es el numero mas ). 
            PCB *meterEjecucion = buscar_por_prioridad(&listos);
            //PCB *meterEjecucion = sacarFrente(&listos);
            // Si no hay nada en listos, no meter nada en ejecucion
            if(meterEjecucion == NULL){
                continue;
            }
        
            insertar(&ejecucion, meterEjecucion); 

        }
        
        PCB *nodo_a_ejecutar = ejecucion.sig;
        copiaLinea[0] = '\0';
        restaurarContexto(nodo_a_ejecutar, linea, sizeof(linea));
        limpiarLinea(numLineaErrorLista);
        strncpy(copiaNombre_archivo, nodo_a_ejecutar->nombre_proceso, sizeof(copiaNombre_archivo) - 1); // Para tener el nombre del archivo en global.
        copiaNombre_archivo[sizeof(copiaNombre_archivo)-1] = '\0';
        mostrarTablas(TMS, TMM, nodo_a_ejecutar);
        calcularPorcentajes_RAM_SWAP(TMM,TMS);
        
        //Imprimir cada que cambie el que esta en ejecucion 
        limpiar();
        imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);

        int qua = 0;
        entrar = false;
        mataEjecucion = false;
        instJNZ = false;
        while (salir == false){
            if((PC > (nodo_a_ejecutar->numInstrucciones - 1))){
                cerrarArch_error(14);
                meterEnTerminados(copiaLinea);
                error_archivo = true;
                break;
            }

            pagina_instruccion = PC/4;
            desplazamiento = PC%4;
            if((nodo_a_ejecutar->paginas[pagina_instruccion][0]) == 0){
                PCB *procesoSuspendido = sacarFrente(&ejecucion);
                guardarContexto(procesoSuspendido, copiaLinea);
                insertar(&suspendidos, procesoSuspendido);
                guardarTiempos(procesoSuspendido);
                limpiar();
                imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista); // Imprimir cada que se mande a suspendidos
                entroSuspendidos = true;
                break;
            }
            else if((nodo_a_ejecutar->paginas[pagina_instruccion][0]) == 1){
                int indiceTMM = nodo_a_ejecutar->paginas[pagina_instruccion][1];
                TMM[indiceTMM][1] = 1;
                marcoRAM = nodo_a_ejecutar->paginas[pagina_instruccion][1];
                direccionFisica = (marcoRAM * 4 * tam_linea) + (desplazamiento * tam_linea);
                strncpy(linea, RAM + direccionFisica, 64);
                linea[64] = '\0';
            }

            if(strchr(linea, '\n') == NULL){ //busca \n en linea si no lo encuentra la linea es mas larga de lo que se permite
                meterEnTerminados(copiaLinea);
                cerrarArch_error(10);
                error_archivo = true;
                break;
            }

            coma = false;
            espacio = false;
            qua++;
            instJNZ = false;

            st = linea;
            inst_to[0] = '\0';
            reg_to[0] = '\0';
            rv_to[0] = '\0';
            limpiarLinea(numFilaEjecucion);
            refresh();
            mvprintw(numFilaEjecucion,4,"%d",PC);
            refresh();
        
            linea[strcspn(linea, "\n")] = '\0';  // Eliminar el salto de línea si existe
            strncpy(copiaLinea, linea, sizeof(copiaLinea) - 1);
            copiaLinea[sizeof(copiaLinea)-1] = '\0';
            //imprimirTMM(TMM);
            mvprintw(numFilaEjecucion,16,"%s",linea);
            refresh();

            token = strsep(&st, " "); //instruccion
            if(token != NULL){
                strncpy(inst_to, token, sizeof(inst_to) - 1);
                inst_to[sizeof(inst_to) - 1] = '\0';
            }

            if(st != NULL){ //si despues de haber pasado el delimitador " " el puntero no está en nulo significa que si se encontro uno
                espacio = true;
            }

            token = strsep(&st, ","); //registro
            if(token != NULL){
                strncpy(reg_to, token, sizeof(reg_to) - 1);
                reg_to[sizeof(reg_to) - 1] = '\0';
            }
            
            if(st != NULL){ //si despues de haber pasado el delimitador "," el puntero no está en nulo significa que si se encontro una
                coma = true;
            }
                
            token = strsep(&st, "\n"); //registro o valor
            if(token != NULL){
                strncpy(rv_to, token, sizeof(rv_to) - 1);
                rv_to[sizeof(rv_to) - 1] = '\0';
            }
                
            if((strcmp(inst_to,"MOV") == 0) || (strcmp(inst_to,"ADD") == 0) || (strcmp(inst_to,"SUB") == 0)|| (strcmp(inst_to,"MUL") == 0) || (strcmp(inst_to,"DIV") == 0)){
                if((reg_to[0] != '\0') && (rv_to[0] != '\0')){
                    if(MOV_ADD_SUB_MUL_DIV(inst_to,reg_to, rv_to) != 0){
                        meterEnTerminados(copiaLinea);
                        error_archivo = true;
                        break;
                    }
                }
                else{
                    meterEnTerminados(copiaLinea);
                    cerrarArch_error(2);
                    error_archivo = true;
                    break;
                }
            }
            else if( (strcmp(inst_to,"INC") == 0) || (strcmp(inst_to,"DEC") == 0) ){
                if((reg_to[0] != '\0') && (rv_to[0] == '\0') && (coma == false)){
                    
                    if(INC_DEC(inst_to,reg_to) != 0){
                        meterEnTerminados(copiaLinea);
                        error_archivo = true;
                        break;
                    }
                }
                else{
                    cerrarArch_error(9);
                    meterEnTerminados(copiaLinea);
                    error_archivo = true;
                    break;
                }       
            }
            else if(strcmp(inst_to,"JNZ") == 0 ){
                if((reg_to[0] != '\0') && (rv_to[0] == '\0') && (coma == false)){     
                    if(JNZ(reg_to, &instJNZ, nodo_a_ejecutar) != 0){
                        meterEnTerminados(copiaLinea);
                        error_archivo = true;
                        break;
                    }
                }
                else{
                    cerrarArch_error(12);
                    meterEnTerminados(copiaLinea);
                    error_archivo = true;
                    break;
                }       
            }
            else if(strcmp(inst_to,"END") == 0){
                end = true;
                if((reg_to[0] != '\0') || (rv_to[0] != '\0') || (espacio == true)){
                    meterEnTerminados(copiaLinea);
                    cerrarArch_error(8);
                    error_archivo = true;
                    break;
                }
                else{
                    CPU_temp += 20;
                    GCPU_temp += 20;
                    meterEnTerminados(copiaLinea);
                    break;
                }
            }
            else{
                meterEnTerminados(copiaLinea);
                cerrarArch_error(7);
                error_archivo = true;
                break;
            }

            CPU_temp += 20;
            GCPU_temp += 20;
            mvprintw(numFilaEjecucion,32,"%d",EAX);
            mvprintw(numFilaEjecucion,48,"%d",EBX);
            mvprintw(numFilaEjecucion,64,"%d",ECX);
            mvprintw(numFilaEjecucion,80,"%d",EDX);
            mvprintw(numFilaEjecucion,90,"%d",CPU_temp);
            mvprintw(numFilaEjecucion,100,"%d",GCPU_temp);
            //mvprintw(numFilaEjecucion,115, "%d", numeroDeGrupos);
            refresh();
            usleep(50000);
            if(instJNZ == false){
                PC++;
            }
            
            coma = false; 
            espacio = false;
            if(qua == Q && end == false){
                salidaPorQuantum = true;
                PCB *nodoEnEjecucion; 
                nodoEnEjecucion = sacarFrente(&ejecucion);
                guardarContexto(nodoEnEjecucion, copiaLinea);
                insertar(&listos, nodoEnEjecucion);
                limpiar();
                //Imprimir cada que cambie listos
                imprimirListas(&ejecucion, &listos, &nuevos, &suspendidos, &terminados, &numLineaLista);
                break;
            }
            
            ciclo_kbhit(&cortar, nombre_archivo, &salir, &ejecuta, end, sizeof(nombre_archivo), 2); 
    
            end = false;
            cortar = false;

            if(mataEjecucion){
                break;
            }
        }
        mostrarTablas(TMS, TMM, NULL);
        calcularPorcentajes_RAM_SWAP(TMM,TMS);
        if(mataEjecucion){
            continue;
        }
        if(ejecuta){
            salir = false;
            continue;
        }
        
        if(salir){
            continue;
        }
        if(salidaPorQuantum == true){
            continue;
        }
        if(entroSuspendidos == true){
            entroSuspendidos = false;
            continue;
        }
        if(end == false && error_archivo == false){
            meterEnTerminados(copiaLinea);
            cerrarArch_error(5);  
            continue;
        }
    }
    endwin();    
    if (memoriaVirtual != NULL){
        fclose(memoriaVirtual);
        memoriaVirtual = NULL;
    }
    return 0;
}

int kbhit(void) {

    struct timeval tv;
    fd_set read_fd;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&read_fd);
    FD_SET(0, &read_fd);

    if (select(1, &read_fd, NULL, NULL, &tv) == -1)
        return 0;

    if (FD_ISSET(0, &read_fd))
        return 1;

    return 0;
}