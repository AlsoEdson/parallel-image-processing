#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char* datos;
    int ancho;
    int alto;
    int canales;
} Fragmento;

/* Divide la imagen en franjas horizontales */
Fragmento* dividir_imagen(unsigned char* img, int ancho, int alto, int canales, int procesos) {

    Fragmento* partes = malloc(sizeof(Fragmento) * procesos);

    int filas_base = alto / procesos;
    int resto = alto % procesos;

    int fila_actual = 0;

    for (int i = 0; i < procesos; i++) {

        int filas = filas_base;
        if (i == procesos - 1)
            filas += resto;

        int bytes_por_fila = ancho * canales;
        int tam = filas * bytes_por_fila;

        partes[i].datos = malloc(tam);
        partes[i].ancho = ancho;
        partes[i].alto = filas;
        partes[i].canales = canales;

        memcpy(
            partes[i].datos,
            img + fila_actual * bytes_por_fila,
            tam
        );

        fila_actual += filas;
    }

    return partes;
}

int main() {

    int ancho, alto, canales;

    unsigned char* img = stbi_load("imagen.jpg", &ancho, &alto, &canales, 0);

    if (!img) {
        printf("No se pudo cargar la imagen\n");
        return 1;
    }

    int procesos = 4;

    Fragmento* partes = dividir_imagen(img, ancho, alto, canales, procesos);

    printf("Imagen original: %dx%d\n\n", ancho, alto);

    int suma = 0;

    for (int i = 0; i < procesos; i++) {
        printf("Tira %d -> alto: %d\n", i, partes[i].alto);
        suma += partes[i].alto;
    }

    printf("\nSuma alturas: %d\n", suma);

    if (suma == alto)
        printf("✔ División correcta\n");
    else
        printf("✘ Error en división\n");

    stbi_image_free(img);
    return 0;
}