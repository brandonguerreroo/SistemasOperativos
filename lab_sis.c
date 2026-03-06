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

FILE *arc_instrucciones;
int EAX = 0; 
int EBX = 0; 
int ECX = 0; 
int EDX = 0; 
int PC = 0;

int kbhit(void);

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
            mvprintw(5,4,"%d\tlinea invalida debido a numero mayor de argumentos\n", PC);
            refresh();
            break;
        case 2:
            mvprintw(5,4,"%d\tlinea invalida debido a uno o varios argumentos nulos (revisa sintaxis)\n", PC);//
            refresh();
            break;
        case 3:
            mvprintw(5,4,"%d\ttercer argumento invalido, revisa sintaxis\n", PC);//
            refresh();
            break;
        case 4:
            mvprintw(5,4,"%d\tel segundo argumento no corresponde a un registro valido\n", PC); //
            refresh();
            break;
        case 5:
            mvprintw(5,4,"%d\tno se encontro una sentencia END", PC);//
            refresh();
            break;
        case 6:
            mvprintw(5,4,"%d\terror division por 0\n", PC); //
            refresh();
            break;
        case 7:
            mvprintw(5,4,"%d\tinstruccion inicial no valida\n", PC); //
            refresh();
            break;
        case 8:
            mvprintw(5,4,"%d\tdemasiados argumentos en sentencia END\n", PC);
            refresh();
            break;
        case 9:
            mvprintw(5,4,"%d\tsintaxis incorrecta en sentencias INC o DEC\n", PC);
            refresh();
            break;
    }
    
    return 0;
}

int* obtener_registro(char *reg){
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
                    if(rv_to[i] == '-')
                    caracter = false;
                }
                
            }
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

void ciclo_kbhit(bool *cortar, char nombre_archivo[], bool *salir, bool *ejecuta, bool end, size_t tam_arch, int num_ciclo){

    char cad[50];
	char *comando = cad; 
	char *token_comandos;
    char comando_to[10];
    char archivo_to[50];
    int fila = 2;

    while(*cortar == false){    

        comando_to[0] = '\0';
        archivo_to[0] = '\0';
        cad[0] = '\0';
        nombre_archivo[0] = '\0';

        if(kbhit()){
            move(10,0);
            clrtoeol();
            move(9,0);
            clrtoeol();
            refresh();
            mvscanw(10,4,"%49[^\n]",cad);
        }
        else {
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
        
        if( (strcmp(comando_to,"salir") == 0) && (archivo_to[0] == '\0') ){
            *salir = true;
            break;
        }
        else if((strcmp(comando_to,"ejecuta") == 0) && (archivo_to[0] != '\0')){
            move(5,0);
            clrtoeol();
            move(fila,0);
            clrtoeol();
            refresh();
            strncpy(nombre_archivo, archivo_to, tam_arch - 1);
            nombre_archivo[tam_arch - 1] = '\0';
            PC = 0;
            *ejecuta = true;
            if(num_ciclo == 2){
                *salir = true;            
            }
            break;
        }
        else if (comando_to[0] != '\0'){
            mvprintw(5,4, "Error, comando de terminal no valido");
            refresh();
            continue;
        }
        if(num_ciclo == 1){
            if(comando_to[0] == '\0'){
                continue;
            }
        }
        if(num_ciclo == 2){
            if(end == true){
                continue;
            }
        }
        *cortar = true;
    }

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

	//char cad[50];
	//char *comando = cad; 
	//char *token_comandos;
    //char comando_to[10];
    //char archivo_to[50];
    int fila = 2;
    bool salir = false; 
    bool ejecuta = false;
    bool cortar = false;
    bool error_archivo = false;
    
    
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
        
        mvprintw(0,4," ");
        refresh();

        if(ejecuta == false){

            ciclo_kbhit(&cortar, nombre_archivo, &salir, &ejecuta, end, sizeof(nombre_archivo), 1);
            if(salir == true){
                continue;
            }
            end = false;
            cortar = false;
        }
        ejecuta = false;

        arc_instrucciones = fopen(nombre_archivo, "r");

        if (arc_instrucciones == NULL){
            mvprintw(5,4,"ERROR: archivo no encontrado.");
            refresh();
            continue;
            
        }
        
        mvprintw(1,4,"PC\t\tIR\t\tEAX\tEBX\tECX\tEDX");
        refresh();

        while (((fgets(linea, sizeof(linea), arc_instrucciones)) != NULL)  && (salir == false)){
            st = linea;
            inst_to[0] = '\0';
            reg_to[0] = '\0';
            rv_to[0] = '\0';
            
            move(fila,0);
            clrtoeol();
            refresh();
            mvprintw(fila,4,"%d",PC);
            refresh();
        
            linea[strcspn(linea, "\n")] = '\0';  // Eliminar el salto de línea si existe 

            mvprintw(fila,16,"%s",linea);
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
        
            /*token = strsep(&st, "\n");
            if (token != NULL ) {
                cerrarArch_error(1);
                error_archivo = true;
                break;
                //return 1;
            }*/
                
            if((strcmp(inst_to,"MOV") == 0) || (strcmp(inst_to,"ADD") == 0) || (strcmp(inst_to,"SUB") == 0)|| (strcmp(inst_to,"MUL") == 0) || (strcmp(inst_to,"DIV") == 0)){
                if((reg_to[0] != '\0') && (rv_to[0] != '\0')){
                    if( MOV_ADD_SUB_MUL_DIV(inst_to,reg_to, rv_to) != 0){
                        //return 1;
                        error_archivo = true;
                        break;
                    }
                }
                else{
                    cerrarArch_error(2);
                    error_archivo = true;
                    break;
                    //return 1;
                }
            }
            else if( (strcmp(inst_to,"INC") == 0) || (strcmp(inst_to,"DEC") == 0) ){
                if((reg_to[0] != '\0') && (rv_to[0] == '\0') && (coma == false)){
                    
                    if( INC_DEC(inst_to,reg_to) != 0){
                        //return 1;
                        error_archivo = true;
                        break;
                    }
                }
                else{
                    cerrarArch_error(9);
                    error_archivo = true;
                    break;
                    //return 1;
                }
                
            }
            else if(strcmp(inst_to,"END") == 0){
                
                end = true;
                //mvprintw(9,4,"Archivo terminado");
                //refresh();
                
                if((reg_to[0] != '\0') || (rv_to[0] != '\0') || (espacio == true)){
                    cerrarArch_error(8);
                    error_archivo = true;
                    break;
                }
                else{
                    mvprintw(9,4,"Archivo terminado");
                    refresh();
                }
            }
            else{
                cerrarArch_error(7);
                error_archivo = true;
                break;
            }

            mvprintw(fila,33,"%d",EAX);
            refresh();
            mvprintw(fila,41,"%d",EBX);
            refresh();
            mvprintw(fila,48,"%d",ECX);
            refresh();
            mvprintw(fila,56,"%d",EDX);
            refresh();
            usleep(50000);
            PC++;
            coma = false; 
            espacio = false;

            ciclo_kbhit(&cortar, nombre_archivo, &salir, &ejecuta, end, sizeof(nombre_archivo), 2);

            end = false;
            cortar = false;
            
        }
        cerrarArch_error(0);
        if(ejecuta){
            salir = false;
            continue;
        }
        if(salir){
            continue;
        }

        if(end == false && error_archivo == false){ 
        
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