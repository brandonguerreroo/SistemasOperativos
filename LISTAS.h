#ifndef LISTAS_H
#define LISTAS_H //sirve evita errores si el archivo se incluye varias veces

#include <stdbool.h> //debe ir aqui por el uso del bool
#define tamañoDePagina 4
#define marcosSWAP 32768
#define marcosRAM 16
typedef struct PCB {
    int PID, GID;
    char nombre_proceso[50];
    int PC;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    int CPU, GCPU, P; // P es Prioridad.
    int numInstrucciones;
    int (*paginas)[3];
    struct PCB *sig;
} PCB;

void limpiarLinea(int num);

PCB *crear_nodo(int pid, int gid, char nombre_proceso[], int PC, int numeroPaginas, PCB *nodoCopiar);

void insertar(PCB *lista, PCB *nuevo);

void imprimir(PCB *lista, int numLista, int *numLinea);

PCB *sacarFrente(PCB *lista);

void actualizar_PCBs(PCB *lista, int GCPU_actual, int num_GID, float WK, int base);

PCB *buscar_por_prioridad(PCB *lista);

PCB *buscar_sacar(PCB *lista, int num_PID, bool condicion);

PCB *buscarPorGID(PCB *lista, int num_GID);

int calcularPaginasLibresSWAP(int TMS[]);

int buscarMarcoPaginaLibreRAM(int TMM[]);

void cargar_a_memoria_RAM(FILE *SWAP, char RAM[], int TMM[], PCB *proceso, int pagina_instruccion);

void imprimirTMM(int TMM[]);

int calcularInstrucciones(FILE *archivo);

int calcularNumPaginas(int numeroInstrucciones);

int buscarMarcoPaginaLibreSWAP(int TMS[]);

void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso);

void imprimirTMS(int TMS[]);

#endif