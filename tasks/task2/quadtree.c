#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Estructura para guardar las coordenadas de una región
typedef struct {
    int x, y, width, height;
} Region;

// Estructura para simular una lista dinámica
typedef struct {
    Region* data;
    int size;
    int capacity;
} RegionList;

void initRegionList(RegionList* list) {
    list->capacity = 10;
    list->size = 0;
    list->data = (Region*)malloc(list->capacity * sizeof(Region));
}

void addRegion(RegionList* list, int x, int y, int w, int h) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->data = (Region*)realloc(list->data, list->capacity * sizeof(Region));
    }
    list->data[list->size].x = x;
    list->data[list->size].y = y;
    list->data[list->size].width = w;
    list->data[list->size].height = h;
    list->size++;
}

void freeRegionList(RegionList* list) {
    free(list->data);
}

// Función matemática para calcular la varianza de un sub-bloque
double calcularVarianza(int* imagen, int x, int y, int w, int h, int w_total) {
    if (w == 0 || h == 0) return 0.0;

    double suma = 0;
    int n = w * h;

    // Media
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            suma += imagen[i * w_total + j];
        }
    }
    double media = suma / n;

    // Varianza
    double varianza = 0;
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            double diff = imagen[i * w_total + j] - media;
            varianza += diff * diff;
        }
    }
    return varianza / n;
}

// Función Recursiva del Quadtree
void segmentarQuadtree(int* imagen, int x, int y, int w, int h, int w_total, double umbral, RegionList* resultados) {
    if (w <= 1 && h <= 1) {
        addRegion(resultados, x, y, w, h);
        return;
    }

    double varianza = calcularVarianza(imagen, x, y, w, h, w_total);

    if (varianza <= umbral) {
        addRegion(resultados, x, y, w, h);
    } else {
        int mitad_w = w / 2;
        int mitad_h = h / 2;
        int w2 = w - mitad_w;
        int h2 = h - mitad_h;

        // Superior Izquierdo
        if (mitad_w > 0 && mitad_h > 0)
            segmentarQuadtree(imagen, x, y, mitad_w, mitad_h, w_total, umbral, resultados);

        // Superior Derecho
        if (w2 > 0 && mitad_h > 0)
            segmentarQuadtree(imagen, x + mitad_w, y, w2, mitad_h, w_total, umbral, resultados);

        // Inferior Izquierdo
        if (mitad_w > 0 && h2 > 0)
            segmentarQuadtree(imagen, x, y + mitad_h, mitad_w, h2, w_total, umbral, resultados);

        // Inferior Derecho
        if (w2 > 0 && h2 > 0)
            segmentarQuadtree(imagen, x + mitad_w, y + mitad_h, w2, h2, w_total, umbral, resultados);
    }
}

int main() {
    int ancho = 10;
    int alto = 10;
    int* imagen = (int*)calloc(ancho * alto, sizeof(int));

    for (int i = 0; i < alto; i++) {
        for (int j = ancho / 2; j < ancho; j++) {
            imagen[i * ancho + j] = 255;
        }
    }

    RegionList regiones_finales;
    initRegionList(&regiones_finales);

    double umbral = 50.0; // Umbral de varianza

    // Iniciar el Quadtree
    segmentarQuadtree(imagen, 0, 0, ancho, alto, ancho, umbral, &regiones_finales);

    printf("Coordenadas resultantes (x, y, ancho, alto):\n");
    for (int i = 0; i < regiones_finales.size; i++) {
        printf("[%d, %d] - Tamaño: %dx%d\n",
               regiones_finales.data[i].x,
               regiones_finales.data[i].y,
               regiones_finales.data[i].width,
               regiones_finales.data[i].height);
    }

    free(imagen);
    freeRegionList(&regiones_finales);

    return 0;
}
