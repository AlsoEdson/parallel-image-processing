/**
 * ============================================================
 * TAREA 5 (S10) - Orquestador Híbrido con OpenMP
 *
 * Este archivo contiene la función que recorre las regiones
 * del Quadtree y decide si procesarlas en GPU (si existe) o
 * en CPU usando OpenMP.
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <omp.h>

// Estructura de región (debe coincidir con la definida en quadtree.c)
typedef struct
{
    int x, y, width, height;
} Region;

// Estructura de lista de regiones (ídem)
typedef struct
{
    Region *data;
    int size;
    int capacity;
} RegionList;

/* -----------------------------------------------------------------
 * Prototipos de funciones de procesamiento (Tareas 3, 4 y 6)
 *
 * NOTA: Para la prueba independiente, estas funciones serán
 * reemplazadas por versiones dummy que solo imprimen.
 * En la integración real, deben estar implementadas en otros
 * archivos y enlazadas correctamente.
 * ----------------------------------------------------------------- */
void convertir_a_gris(unsigned char *img, int x, int y, int w, int h, int ancho_total);
void aplicar_blur(unsigned char *img, int x, int y, int w, int h, int ancho_total);
void aplicar_sobel(unsigned char *img, int x, int y, int w, int h, int ancho_total);
void procesar_region_cuda(unsigned char *img, int x, int y, int w, int h, int ancho_total);

// Versiones dummy para prueba (imprimen mensaje)
void dummy_convertir_a_gris(unsigned char *img, int x, int y, int w, int h, int ancho_total)
{
    printf("   [CPU] Gris en (%d,%d) tamaño %dx%d\n", x, y, w, h);
}

void dummy_aplicar_blur(unsigned char *img, int x, int y, int w, int h, int ancho_total)
{
    printf("   [CPU] Blur en (%d,%d) tamaño %dx%d\n", x, y, w, h);
}

void dummy_aplicar_sobel(unsigned char *img, int x, int y, int w, int h, int ancho_total)
{
    printf("   [CPU] Sobel en (%d,%d) tamaño %dx%d\n", x, y, w, h);
}

void dummy_procesar_region_cuda(unsigned char *img, int x, int y, int w, int h, int ancho_total)
{
    printf("   [GPU] Procesando región (%d,%d) tamaño %dx%d en CUDA\n", x, y, w, h);
}

/* -----------------------------------------------------------------
 * Función principal del orquestador
 *
 * Recorre la lista de regiones en paralelo con OpenMP y, según
 * el flag 'tiene_nvidia', llama a la versión GPU o a la secuencia
 * de filtros en CPU.
 * ----------------------------------------------------------------- */
void procesar_regiones(RegionList *regiones, unsigned char *imagen,
                       int ancho_total, bool tiene_nvidia)
{

#pragma omp parallel for
    for (int i = 0; i < regiones->size; i++)
    {
        Region r = regiones->data[i];

        if (tiene_nvidia)
        {
            // Ruta GPU: llamar a la función CUDA
            // (en la prueba usamos la dummy)
            dummy_procesar_region_cuda(imagen, r.x, r.y, r.width, r.height, ancho_total);
        }
        else
        {
            // Ruta CPU: aplicar filtros en orden (gris -> blur -> sobel)
            dummy_convertir_a_gris(imagen, r.x, r.y, r.width, r.height, ancho_total);
            dummy_aplicar_blur(imagen, r.x, r.y, r.width, r.height, ancho_total);
            dummy_aplicar_sobel(imagen, r.x, r.y, r.width, r.height, ancho_total);
        }
    }
}

/* -----------------------------------------------------------------
 * Función para generar una lista de regiones de prueba
 * (simula la salida del quadtree)
 * ----------------------------------------------------------------- */
RegionList generar_regiones_prueba()
{
    RegionList lista;
    lista.size = 0;
    lista.capacity = 10;
    lista.data = (Region *)malloc(lista.capacity * sizeof(Region));

    // Agregamos algunas regiones ficticias
    // (x, y, ancho, alto)
    Region r1 = {0, 0, 8, 8};
    Region r2 = {8, 0, 8, 8};
    Region r3 = {0, 8, 8, 8};
    Region r4 = {8, 8, 8, 8};
    Region r5 = {4, 4, 4, 4};

    lista.data[lista.size++] = r1;
    lista.data[lista.size++] = r2;
    lista.data[lista.size++] = r3;
    lista.data[lista.size++] = r4;
    lista.data[lista.size++] = r5;

    return lista;
}

/* -----------------------------------------------------------------
 * PRUEBA INDEPENDIENTE (S10)
 *
 * Ejecuta el orquestador con dos configuraciones: sin GPU y con GPU.
 * Debe mostrar en consola la ruta elegida para cada región.
 * ----------------------------------------------------------------- */
int main()
{
    printf("=== PRUEBA ORQUESTADOR HÍBRIDO (S10) ===\n\n");

    // Generar regiones de prueba
    RegionList regiones = generar_regiones_prueba();
    printf("Regiones generadas: %d\n", regiones.size);

    // Buffer de imagen ficticio (no se usa realmente)
    unsigned char *imagen_dummy = NULL;
    int ancho_total = 16; // ancho de la imagen completa

    // Prueba 1: sin GPU (tiene_nvidia = false)
    printf("\n--- Modo CPU (sin NVIDIA) ---\n");
    procesar_regiones(&regiones, imagen_dummy, ancho_total, false);

    // Prueba 2: con GPU (tiene_nvidia = true)
    printf("\n--- Modo GPU (con NVIDIA) ---\n");
    procesar_regiones(&regiones, imagen_dummy, ancho_total, true);

    // Liberar memoria
    free(regiones.data);

    printf("\n[Prueba completada]\n");
    return 0;
}