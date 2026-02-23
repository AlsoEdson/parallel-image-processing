//wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
//wget https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <stdio.h>
#include <stdlib.h>

// --- TU FUNCIÓN (La misma de antes) ---
void aplicar_filtro(unsigned char* imagen, int start_x, int start_y, int ancho_bloque, int alto_bloque, int ancho_total) {
    // 1. Gris
    for (int j = start_y; j < start_y + alto_bloque; ++j) {
        for (int i = start_x; i < start_x + ancho_bloque; ++i) {
            int idx = (j * ancho_total + i) * 3;
            unsigned char gris = (unsigned char)(0.299f * imagen[idx] + 0.587f * imagen[idx+1] + 0.114f * imagen[idx+2]);
            imagen[idx] = imagen[idx+1] = imagen[idx+2] = gris;
        }
    }
    // 2. Blur (Simplificado para la prueba)
    unsigned char* temp = (unsigned char*)malloc(ancho_bloque * alto_bloque);
    for (int j = start_y; j < start_y + alto_bloque; ++j) {
        for (int i = start_x; i < start_x + ancho_bloque; ++i) {
            int suma = 0, cont = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int ny = j + ky, nx = i + kx;
                    if (ny >= 0 && ny < alto_bloque && nx >= 0 && nx < ancho_total) {
                        suma += imagen[(ny * ancho_total + nx) * 3];
                        cont++;
                    }
                }
            }
            temp[(j-start_y)*ancho_bloque + (i-start_x)] = suma / cont;
        }
    }
    for (int j = 0; j < alto_bloque; j++) {
        for (int i = 0; i < ancho_bloque; i++) {
            int idx = ((j+start_y)*ancho_total + (i+start_x))*3;
            imagen[idx] = imagen[idx+1] = imagen[idx+2] = temp[j*ancho_bloque + i];
        }
    }
    free(temp);
}

int main() {
    int ancho, alto, canales;
    
    // 1. Cargar imagen
    unsigned char *img = stbi_load("entrada.jpg", &ancho, &alto, &canales, 3);
    if (img == NULL) {
        printf("Error: No se pudo cargar entrada.jpg. Asegúrate de que el archivo existe.\n");
        return 1;
    }

    printf("Imagen cargada: %dx%d con %d canales.\n", ancho, alto, canales);

    // 2. Aplicar filtro (A TODA la imagen)
    // Como eres Master, aquí simulas que el bloque es la imagen completa
    aplicar_filtro(img, 0, 0, ancho, alto, ancho);

    // 3. Guardar resultado
    stbi_write_jpg("salida.jpg", ancho, alto, 3, img, 100);
    
    printf("¡Éxito! Revisa el archivo salida.jpg\n");

    stbi_image_free(img);
    return 0;
}