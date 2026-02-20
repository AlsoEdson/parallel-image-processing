/*
 * ============================================================
 *  TAREA 3 - Algoritmo de Detección de Bordes (Sobel)
 *  Integrante 3 | Proyecto MPI + OpenMP | S10
 * ============================================================
 *
 *  Compilar en WSL2:
 *      gcc -o sobel_filter sobel_filter.c -lm
 *  Ejecutar:
 *      ./sobel_filter
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ============================================================
 *  FUNCIÓN PRINCIPAL: aplicar_filtro
 *
 *  Parámetros:
 *    imagen       - puntero a la imagen en escala de grises (1 canal)
 *    x, y         - coordenada superior-izquierda del bloque a procesar
 *    ancho_bloque - ancho del bloque asignado a este proceso
 *    alto_bloque  - alto del bloque asignado a este proceso
 *    ancho_total  - ancho TOTAL de la imagen original (necesario para
 *                   saltar filas correctamente en memoria contigua)
 *
 *  El resultado se escribe en el mismo buffer (in-place).
 *  Los píxeles del borde del bloque quedan en 0 (sin suficientes vecinos).
 * ============================================================ */
void aplicar_filtro(unsigned char *imagen,
                    int x, int y,
                    int ancho_bloque, int alto_bloque,
                    int ancho_total)
{
    /*
     *  Kernels de Sobel:
     *
     *  Gx (gradiente horizontal)      Gy (gradiente vertical)
     *  -1   0  +1                     -1  -2  -1
     *  -2   0  +2                      0   0   0
     *  -1   0  +1                     +1  +2  +1
     *
     *  Piensa en estos kernels como "detectores de pendiente":
     *  Gx busca cambios de izquierda a derecha (bordes verticales),
     *  Gy busca cambios de arriba a abajo (bordes horizontales).
     */

    /* Buffer temporal para no sobreescribir valores que aún se necesitan */
    unsigned char *temp = (unsigned char *)malloc(ancho_total *
                          (y + alto_bloque) * sizeof(unsigned char));
    if (!temp) {
        fprintf(stderr, "Error: sin memoria para buffer temporal\n");
        return;
    }

    /* Copia del bloque relevante al temporal */
    for (int row = y; row < y + alto_bloque; row++) {
        for (int col = x; col < x + ancho_bloque; col++) {
            temp[row * ancho_total + col] = imagen[row * ancho_total + col];
        }
    }

    /* Recorre el bloque evitando el borde (necesita vecinos 3x3) */
    for (int row = y + 1; row < y + alto_bloque - 1; row++) {
        for (int col = x + 1; col < x + ancho_bloque - 1; col++) {

            /* Vecindad 3x3 usando el buffer temporal */
            int p00 = temp[(row-1)*ancho_total + (col-1)];
            int p01 = temp[(row-1)*ancho_total +  col   ];
            int p02 = temp[(row-1)*ancho_total + (col+1)];

            int p10 = temp[ row   *ancho_total + (col-1)];
         /* int p11 = centro, no se usa en Sobel */
            int p12 = temp[ row   *ancho_total + (col+1)];

            int p20 = temp[(row+1)*ancho_total + (col-1)];
            int p21 = temp[(row+1)*ancho_total +  col   ];
            int p22 = temp[(row+1)*ancho_total + (col+1)];

            /* Aplicar kernels */
            int Gx = (-1*p00) + (0*p01) + (1*p02)
                   + (-2*p10) + (0)      + (2*p12)
                   + (-1*p20) + (0*p21) + (1*p22);

            int Gy = (-1*p00) + (-2*p01) + (-1*p02)
                   + ( 0)     + ( 0)     + ( 0)
                   + ( 1*p20) + ( 2*p21) + ( 1*p22);

            /* Magnitud del gradiente: G = sqrt(Gx^2 + Gy^2) */
            double G = sqrt((double)(Gx*Gx + Gy*Gy));

            /* Clamp al rango [0, 255] */
            if (G > 255.0) G = 255.0;

            imagen[row * ancho_total + col] = (unsigned char)G;
        }
    }

    free(temp);
}


/* ============================================================
 *  PRUEBA INDEPENDIENTE
 *
 *  Matriz 5x5 con gradiente fuerte en el centro (izquierda=0, derecha=255).
 *  El pixel central (fila 2, col 2) debe mostrar alta magnitud de gradiente,
 *  confirmando que el filtro detecta el borde.
 * ============================================================ */
int main(void)
{
    printf("=== Prueba Independiente - Filtro Sobel ===\n\n");

    /*
     *  Imagen 5x5 (ancho_total = 5):
     *
     *    0    0  128  255  255
     *    0    0  128  255  255
     *    0    0  128  255  255   <-- fila 2, borde en col 2
     *    0    0  128  255  255
     *    0    0  128  255  255
     *
     *  Analogía: como una pared dividida a la mitad,
     *  blanca de un lado y negra del otro.
     *  Sobel debería "iluminar" la línea divisoria.
     */

    const int W = 5, H = 5;
    unsigned char imagen[5*5] = {
          0,   0, 128, 255, 255,
          0,   0, 128, 255, 255,
          0,   0, 128, 255, 255,
          0,   0, 128, 255, 255,
          0,   0, 128, 255, 255
    };

    printf("Imagen ANTES del filtro Sobel:\n");
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++)
            printf("%4d", imagen[r*W + c]);
        printf("\n");
    }

    /* Procesar el bloque completo (x=0, y=0, ancho=W, alto=H) */
    aplicar_filtro(imagen, 0, 0, W, H, W);

    printf("\nImagen DESPUÉS del filtro Sobel (magnitud del gradiente):\n");
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++)
            printf("%4d", imagen[r*W + c]);
        printf("\n");
    }

    printf("\n¿Resultado esperado?\n");
    printf("  - Col 0 y col 4 (bordes del bloque): 0 (sin vecinos suficientes)\n");
    printf("  - Columnas interiores sobre la transición: valores ALTOS (cerca de 255)\n");
    printf("  - Columnas lejos del borde: valores BAJOS (fondo uniforme)\n");

    /* Verificación automática */
    int centro = imagen[2*W + 2]; /* fila 2, col 2 -> zona de borde */
    int fondo  = imagen[2*W + 0]; /* fila 2, col 0 -> zona uniforme (borde del bloque=0) */
    printf("\n  Pixel central (borde real): %d\n", centro);
    printf("  Pixel fondo uniforme:       %d\n", fondo);

    if (centro > 50)
        printf("\n[OK] El filtro detectó el borde correctamente.\n");
    else
        printf("\n[FALLO] El filtro no detectó gradiente suficiente.\n");

    return 0;
}
