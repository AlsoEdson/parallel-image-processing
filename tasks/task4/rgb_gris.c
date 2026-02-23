#include <stdio.h>
#include <stdlib.h>

// Firma solicitada
void aplicar_filtro(unsigned char* imagen, int start_x, int start_y, int ancho_bloque, int alto_bloque, int ancho_total) {
    
    // 1. CONVERSIÓN A GRIS (In-place)
    for (int j = start_y; j < start_y + alto_bloque; ++j) {
        for (int i = start_x; i < start_x + ancho_bloque; ++i) {
            int idx = (j * ancho_total + i) * 3;
            
            unsigned char r = imagen[idx];
            unsigned char g = imagen[idx + 1];
            unsigned char b = imagen[idx + 2];

            // Fórmula de luminancia (estándar ITU-R)
            unsigned char gris = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);

            // Guardamos el mismo valor en los 3 canales
            imagen[idx] = imagen[idx + 1] = imagen[idx + 2] = gris;
        }
    }

    // 2. FILTRO DE DESENFOQUE (Blur 3x3)
    // Reservamos memoria temporal para el bloque (solo 1 canal necesario porque ya es gris)
    unsigned char* temp_block = (unsigned char*)malloc(ancho_bloque * alto_bloque * sizeof(unsigned char));

    for (int j = start_y; j < start_y + alto_bloque; ++j) {
        for (int i = start_x; i < start_x + ancho_bloque; ++i) {
            int suma = 0;
            int contador = 0;

            // Revisar vecinos 3x3
            for (int ky = -1; ky <= 1; ++ky) {
                for (int kx = -1; kx <= 1; ++kx) {
                    int ny = j + ky;
                    int nx = i + kx;

                    // Verificar límites de la imagen total para evitar desbordamiento
                    if (ny >= 0 && ny < ancho_total && nx >= 0 && nx < ancho_total) {
                        int n_idx = (ny * ancho_total + nx) * 3;
                        suma += imagen[n_idx]; // Usamos el canal R (ya convertido a gris)
                        contador++;
                    }
                }
            }
            int temp_idx = (j - start_y) * ancho_bloque + (i - start_x);
            temp_block[temp_idx] = (unsigned char)(suma / contador);
        }
    }

    // 3. COPIAR RESULTADOS
    // Devolvemos los valores del buffer temporal a la imagen original
    for (int j = 0; j < alto_bloque; ++j) {
        for (int i = 0; i < ancho_bloque; ++i) {
            int final_idx = ((j + start_y) * ancho_total + (i + start_x)) * 3;
            unsigned char valor_final = temp_block[j * ancho_bloque + i];
            imagen[final_idx] = imagen[final_idx + 1] = imagen[final_idx + 2] = valor_final;
        }
    }

    free(temp_block); // Liberamos la memoria temporal
}

// PRUEBA INDEPENDIENTE
int main() {
    int ancho = 4;
    int alto = 4;
    
    // Matriz de prueba 4x4 RGB (16 píxeles * 3 canales = 48 bytes)
    unsigned char imagen[48] = {
        255,0,0,   0,255,0,   0,0,255,   255,255,255,
        255,0,0,   0,255,0,   0,0,255,   255,255,255,
        255,0,0,   0,255,0,   0,0,255,   255,255,255,
        255,0,0,   0,255,0,   0,0,255,   255,255,255
    };

    printf("--- Píxel Original (0,0): R=%d G=%d B=%d ---\n", imagen[0], imagen[1], imagen[2]);

    // Procesar toda la matriz como un solo bloque para la prueba
    aplicar_filtro(imagen, 0, 0, 4, 4, 4);

    printf("--- Después de Gris y Blur (0,0): R=%d G=%d B=%d ---\n", imagen[0], imagen[1], imagen[2]);
    
    if (imagen[0] == imagen[1] && imagen[1] == imagen[2]) {
        printf("Resultado: Prueba de canal GRIS exitosa.\n");
    }

    return 0;
}