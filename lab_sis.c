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

FILE *arc_instrucciones;
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
int numLineaErrorLista = 5;
int numLineaComando = 4;
int numFilaEjecucion = 2;
int numLineaLista = 8;
char copiaNombre_archivo[50];
int Q = 3;
bool mataEjecucion = false;
char copiaLinea[64];

int kbhit(void);
void limpiarLinea(int num)
{
    move(num,4);
    clrtoeol();
    refresh();
}
int cerrarArch_error(int num){
    if (arc_instrucciones != NULL) {
        if(fclose(arc_instrucciones) != 0) {
            fprintf(stdout, "Error al cerrar el archivo.\n");
        }
        arc_instrucciones = NULL;
    }
    
    if (num == 0){
        return 0;
    }
    switch(num){
        case 1:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea invalida debido a numero mayor de argumentos", copiaNombre_archivo, PC);
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 2:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea invalida debido a uno o varios argumentos nulos (revisa sintaxis)", copiaNombre_archivo, PC);//
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 3:
            mvprintw(numLineaErrorLista,4,"%s\t%d\ttercer argumento invalido, revisa sintaxis", copiaNombre_archivo, PC);//
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
            mvprintw(numLineaErrorLista,4,"%s\t%d\tno se encontro una sentencia END", copiaNombre_archivo, PC);  //
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
            mvprintw(numLineaErrorLista,4,"%s\t%d\tdemasiados argumentos en sentencia END", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 9:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tsintaxis incorrecta en sentencias INC o DEC", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
        case 10:
            mvprintw(numLineaErrorLista,4,"%s\t%d\tlinea de instruccion demasiado larga (revisar archivo)", copiaNombre_archivo, PC); //
            refresh();
            sleep(2);
            limpiarLinea(numLineaErrorLista);
            break;
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
            mvprintw(6,4,"Hola");
            refresh();
            caracter = true;
        }
        
        if(caracter){
            cerrarArch_error(3);
            return 1;
        }
        else{
            valor = atoi(rv_to);
        }
    }

    if(strcmp(inst_to,"MOV") == 0){
        *destino = valor;
    }
    else if(strcmp(inst_to,"ADD") == 0){
        *destino = *destino + valor;
    }
    else if(strcmp(inst_to,"SUB") == 0){
        *destino = *destino - valor;
    }
    else if(strcmp(inst_to,"MUL") == 0){
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
        (*destino)++;
    }
    else if(strcmp(inst_to,"DEC") == 0){
        (*destino)--;
    }
    
    return 0;
}

void limpiar(){  //Limpia la pantalla desde la linea 8 hasta el ultimo renglon que se imprimio
    for(int l = 8; l <= numLineaLista; l++){
        move(l,0);
        clrtoeol();
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
    Wk = 1 / numeroDeGrupos;
    nodo->P = base + ( nodo->CPU / 2 ) + ( nodo->GCPU / (4 * Wk) );
    actualizar_PCB_del_grupo(&listos,GCPU_temp, nodo->GID, Wk, base); //actualizar los valores para los demas procesos del mismo grupo
}

void meterEnTerminados(char linea[]){
    PCB *nodo; 
    nodo = sacarFrente(&ejecucion); 
    guardarContexto(nodo, linea);
    insertar(&terminados, nodo);
    limpiar();
    //Imprimir cada que cambie la lista de terminados
    imprimir(&ejecucion, 2, &numLineaLista);
    imprimir(&listos, 1, &numLineaLista);
    imprimir(&terminados, 3, &numLineaLista);
}
int matar(int num_PID){
    PCB *matar;
    if((matar = buscar_sacar(&listos, num_PID, 0)) != NULL){
        insertar(&terminados, matar);
        return 0;
    }
    else if((matar = buscar_sacar(&ejecucion, num_PID, 0)) != NULL){
        
        guardarContexto(matar, copiaLinea);
        insertar(&terminados, matar);
        return 1;
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
    int numFilaEjecucion = 2;
    int procesoPID;
    bool PID_no_number = false;
    while(*cortar == false){    

        comando_to[0] = '\0';
        archivo_to[0] = '\0';
        cad[0] = '\0';
        nombre_archivo[0] = '\0';

        if(kbhit()){
            move(numLineaComando,0);
            clrtoeol();
            refresh();
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

        token_comandos = strsep(&comando, "\n");
        if(token_comandos != NULL){
            strncpy(archivo_to, token_comandos, sizeof(archivo_to) - 1);
            archivo_to[sizeof(archivo_to) - 1] = '\0';
        }
        if((strcmp(comando_to,"mata") == 0) && (archivo_to[0] != '\0'))
        {
            int longitud = strlen(archivo_to);
            for(int i = 0; i < longitud; i++)
            {   
                if (archivo_to[i] < '0' || archivo_to[i] > '9') 
                {
                    mvprintw(numLineaErrorLista,4, "Error, el PID no es un numero");
                    refresh();
                    sleep(1);
                    limpiarLinea(numLineaErrorLista);
                    PID_no_number = true;
                    break;
                }
            }
            if(PID_no_number == false){
                procesoPID = atoi(archivo_to);
                if(matar(procesoPID) == 1)
                {
                    mataEjecucion = true;
                }
            }
            limpiar();
            PID_no_number = false;
            //Imprimir cada que se mate un proceso
            imprimir(&ejecucion, 2, &numLineaLista);  
            imprimir(&listos, 1, &numLineaLista);
            imprimir(&terminados, 3, &numLineaLista);
        }
        else if( (strcmp(comando_to,"salir") == 0) && (archivo_to[0] == '\0') ){
            *ejecuta = false;
            *salir = true;
            break;
        }
        else if((strcmp(comando_to,"ejecuta") == 0) && (archivo_to[0] != '\0')){
            move(5,0);
            clrtoeol();
            move(numFilaEjecucion,0);
            clrtoeol();
            refresh();
            FILE *archivo = fopen(archivo_to, "r"); //Nos sirve para poder comprobar que el archivo exista
            if (archivo == NULL) 
            {
                mvprintw(numLineaErrorLista,4,"ERROR: archivo no encontrado."); //Si no existe, marcamos error
                refresh();
                sleep(1);
                limpiarLinea(numLineaErrorLista);
                continue;
            } 
            else
            {
                fclose(archivo); //Si existe, cerramos archivo y seguimos con lo demas (crear_nodo)
            }
            strncpy(nombre_archivo, archivo_to, tam_arch - 1);
            nombre_archivo[tam_arch - 1] = '\0';
            PID++;
            GID++; 
            numeroDeGrupos ++;
            PCB *nuevo = crear_nodo(PID, GID, nombre_archivo,0,"0"); // Se agregó
            insertar(&listos, nuevo); 
            *ejecuta = true;
            limpiar();
            //Imprimir cada que cambie se agregue uno nuevo
            imprimir(&ejecucion, 2, &numLineaLista); 
            imprimir(&listos, 1, &numLineaLista);
            imprimir(&terminados, 3, &numLineaLista);
            break;
        }
        else if (comando_to[0] != '\0'){
            mvprintw(numLineaErrorLista,4, "Error, comando de terminal no valido");
            refresh();
            sleep(1);
            limpiarLinea(numLineaErrorLista);
            continue;
        }
        if(num_ciclo == 1){  
            if(listos.sig != NULL){            
                break;
            }
            else if(comando_to[0] == '\0' || strcmp(comando_to,"mata") == 0){
                continue;
            }
        }
        if(num_ciclo == 2){
            if(listos.sig != NULL && end == true){ 
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

void restaurarContexto(PCB *nodo, char linea[], size_t tam_linea)
{
    EAX = nodo->EAX;
    EBX = nodo->EBX;
    ECX = nodo->ECX;
    EDX = nodo->EDX;
    strncpy(linea,nodo->IR, tam_linea - 1);
    linea[tam_linea - 1] = '\0';
    PC = nodo->PC;
    CPU_temp = nodo->CPU;
    GCPU_temp = nodo->GCPU;

}

int main(){

    char linea[64];
    char *token;
    char inst_to[5];
    char reg_to[5];
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

    PCB *meterTerminados; 
    listos.sig = NULL;
    ejecucion.sig = NULL;
    terminados.sig = NULL;
    
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
            PCB *meterEjecucion = sacarFrente(&listos);
            // Si no hay nada en listos, no meter nada en ejecucion
            if(meterEjecucion == NULL){
                continue;
            } 
            insertar(&ejecucion, meterEjecucion); 
            
        }
        
        PCB *archivo = ejecucion.sig; 
        copiaLinea[0] = '\0';
        restaurarContexto(archivo, linea, sizeof(linea));
        strncpy(copiaNombre_archivo, archivo->nombre_proceso, sizeof(copiaNombre_archivo) - 1); // Para tener el nombre del archivo en global.
        copiaNombre_archivo[sizeof(copiaNombre_archivo)-1] = '\0';
        
        arc_instrucciones = fopen(archivo->nombre_proceso, "r");
        if (arc_instrucciones == NULL){
            mvprintw(numLineaErrorLista,4,"ERROR: archivo no encontrado.");
            meterEnTerminados(copiaLinea);
            refresh();
            sleep(1);
            limpiarLinea(numLineaErrorLista);
            continue;
        }

        limpiar();
        //Imprimir cada que cambie el que esta en ejecucion 
        imprimir(&ejecucion, 2, &numLineaLista);
        imprimir(&listos, 1, &numLineaLista);
        imprimir(&terminados, 3, &numLineaLista);

        mvprintw(1,4,"PC\t\tIR\t\tEAX\t\tEBX\t\tECX\t\tEDX");
        mvprintw(7,4,"PID   GID   Nombre\t\tEstado\t\tPC\tIR\t\t\tEAX\t\tEBX\t\tECX\t\tEDX         CPU     GCPU");
        refresh();
        int qua = 0;
        //CPU_temp = 0;    //no se debe reiniciar a 0 ya que establecemos el valor de estos dos al restaurar contexto
        //GCPU_temp = 0;
        int i = 0;
        entrar = false;
        mataEjecucion = false;
        while (((fgets(linea, sizeof(linea), arc_instrucciones)) != NULL)  && (salir == false)){
            if(entrar == false){
                if(i < PC){
                    i++;
                    continue;
                }
                entrar = true;
            }
            if(strchr(linea, '\n') == NULL){ //busca \n en linea si no lo encuentra la linea es mas larga de lo que se permite
                int c;
                if((c = fgetc(arc_instrucciones)) != EOF){ //por si es el caso de la linea END ya que no tiene \n al final
                    meterEnTerminados(copiaLinea);
                    cerrarArch_error(10);
                    error_archivo = true;
                    break;
                }
            }
            qua++;
            CPU_temp += 20;
            GCPU_temp += 20;

            st = linea;
            inst_to[0] = '\0';
            reg_to[0] = '\0';
            rv_to[0] = '\0';
            
            move(numFilaEjecucion,0);
            clrtoeol();
            refresh();
            mvprintw(numFilaEjecucion,4,"%d",PC);
            refresh();
        
            linea[strcspn(linea, "\n")] = '\0';  // Eliminar el salto de línea si existe
            strncpy(copiaLinea, linea, sizeof(copiaLinea) - 1);
            copiaLinea[sizeof(copiaLinea)-1] = '\0';

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
            else if(strcmp(inst_to,"END") == 0){
                end = true;
                if((reg_to[0] != '\0') || (rv_to[0] != '\0') || (espacio == true)){
                    meterEnTerminados(copiaLinea);
                    cerrarArch_error(8);
                    error_archivo = true;
                    break;
                }
                else{
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

            mvprintw(numFilaEjecucion,32,"%d",EAX);
            mvprintw(numFilaEjecucion,48,"%d",EBX);
            mvprintw(numFilaEjecucion,64,"%d",ECX);
            mvprintw(numFilaEjecucion,80,"%d",EDX);
            mvprintw(numFilaEjecucion,90,"%d",CPU_temp);
            mvprintw(numFilaEjecucion,100,"%d",GCPU_temp);
            refresh();
            usleep(500000);
            PC++;
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
                imprimir(&ejecucion, 2, &numLineaLista);
                imprimir(&listos, 1, &numLineaLista);
                imprimir(&terminados, 3, &numLineaLista);
                break;
            }
            
            ciclo_kbhit(&cortar, nombre_archivo, &salir, &ejecuta, end, sizeof(nombre_archivo), 2); 
    
            end = false;
            cortar = false;

            if(mataEjecucion){
                break;
            }
        }
        cerrarArch_error(0); // Este nada mas cierra el archivo
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
        if(end == false && error_archivo == false){
            meterEnTerminados(copiaLinea);
            cerrarArch_error(5);  
            continue;
        }
    }
    endwin();
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