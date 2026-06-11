#ifndef LISTAS_H
#define LISTAS_H //sirve evita errores si el archivo se incluye varias veces

#include <stdbool.h> //debe ir aqui por el uso del bool
#include <time.h>

#define tamañoDePagina 4
#define marcosSWAP 32768
#define marcosRAM 16
#define numLineaErrorLista 5
#define numLineaComando 4
#define numFilaEjecucion 2
#define tam_linea 64

typedef struct PCB {
    int PID, GID;
    char nombre_proceso[50];
    int PC;
    char IR[64];
    int EAX,EBX,ECX,EDX;
    int CPU, GCPU, P; // P es Prioridad.
    int numInstrucciones;
    time_t tiempo_de_salida;
    int espera;
    int (*paginas)[3];
    struct PCB *sig;
} PCB;

void limpiarLinea(int num);

PCB *crear_nodo(int pid, int gid, char nombre_proceso[], int PC, int numeroPaginas, int numInstrucciones, PCB *nodoCopiar);

void insertar(PCB *lista, PCB *nuevo);

void imprimir(PCB *lista, int numLista, int *numLinea);

void mostrarEncabezados();

void mostrarPantalla(int TMS[], int TMM[][2], PCB *nodo_a_ejecutar);

PCB *sacarFrente(PCB *lista);

void actualizar_PCBs(PCB *lista, int GCPU_actual, int num_GID, float WK, int base);

PCB *buscar_por_prioridad(PCB *lista);

PCB *buscar_sacar(PCB *lista, int num_PID, bool condicion);

PCB *buscarPorGID(PCB *lista, int num_GID);

void imprimirListas(PCB *ejecucion, PCB *listos, PCB *nuevos, PCB *suspendidos, PCB *terminados, int *numLineaLista);

void verERROR();

int calcularPaginasLibresSWAP(int TMS[]);

int buscarMarcoPaginaLibreRAM(int TMM[][2]);

void cargar_a_memoria_RAM(FILE *SWAP, char RAM[], int TMM[][2], PCB *proceso, int pagina_instruccion, PCB *ejecucion, PCB *suspendidos);

void guardarTiempos(PCB *procesoSuspendido);

void imprimirTMM(int TMM[][2]);

int calcularInstrucciones(FILE *archivo);

int calcularNumPaginas(int numeroInstrucciones);

int buscarMarcoPaginaLibreSWAP(int TMS[]);

void cargar_a_memoria_virtual(FILE *archivoOrigen, FILE *archivoDestino, int numPaginas, int TMS[], PCB *proceso);

void imprimirTMS(int TMS[]);

void imprimirTMP(PCB *nodo_a_ejecutar);

void liberar_marcos_RAM_SWAP(PCB *proceso, int TMM[][2], int TMS[]);

int cargarNuevos(PCB *nuevos, PCB *listos, FILE *memoriaVirtual, int TMS[]);

PCB *sacarSuspendidos(PCB *suspendidos, PCB *listos);

#endif