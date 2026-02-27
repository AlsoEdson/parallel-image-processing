/**
 * ============================================================
 * TAREA 5 (S11) - Pruebas de transferencia y medición
 *
 * Este archivo contiene funciones para medir tiempos de
 * transferencia CPU-GPU y comparar con procesamiento en CPU.
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <omp.h>
#include <cuda_runtime.h>

// -----------------------------------------------------------------
// Kernel de prueba (el mismo de graf_cuda.cu)
// -----------------------------------------------------------------
__global__ void kernel_prueba(float *datos, int n)
{
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n)
    {
        datos[i] = datos[i] * 2.0f;
    }
}

// -----------------------------------------------------------------
// Medición de transferencia y ejecución de kernel en GPU
// -----------------------------------------------------------------
void medir_gpu(float *h_data, int n, bool ejecutar_kernel)
{
    float *d_data;
    size_t size = n * sizeof(float);
    cudaEvent_t start, stop;
    float tiempo_ms;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // 1. Transferencia H2D
    cudaEventRecord(start);
    cudaMalloc(&d_data, size);
    cudaMemcpy(d_data, h_data, size, cudaMemcpyHostToDevice);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&tiempo_ms, start, stop);
    printf("Transferencia H2D: %.3f ms\n", tiempo_ms);

    if (ejecutar_kernel)
    {
        // 2. Ejecución del kernel
        int threads = 256;
        int blocks = (n + threads - 1) / threads;
        cudaEventRecord(start);
        kernel_prueba<<<blocks, threads>>>(d_data, n);
        cudaEventRecord(stop);
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&tiempo_ms, start, stop);
        printf("Ejecución kernel:   %.3f ms\n", tiempo_ms);
    }

    // 3. Transferencia D2H
    cudaEventRecord(start);
    cudaMemcpy(h_data, d_data, size, cudaMemcpyDeviceToHost);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&tiempo_ms, start, stop);
    printf("Transferencia D2H: %.3f ms\n", tiempo_ms);

    cudaFree(d_data);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

// -----------------------------------------------------------------
// Simulación de procesamiento en CPU (usando OpenMP)
// -----------------------------------------------------------------
double procesar_cpu(float *data, int n)
{
    double t1 = omp_get_wtime();

// Operación similar a la del kernel (multiplicar por 2)
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        data[i] = data[i] * 2.0f;
    }

    double t2 = omp_get_wtime();
    return (t2 - t1) * 1000.0; // milisegundos
}

// -----------------------------------------------------------------
// PRUEBA INDEPENDIENTE (S11)
// -----------------------------------------------------------------
int main()
{
    printf("=== PRUEBA DE TRANSFERENCIA Y RENDIMIENTO (S11) ===\n\n");

    const int N = 1000000; // 1 millón de elementos (~4 MB)
    size_t size = N * sizeof(float);

    // Datos en host
    float *h_data = (float *)malloc(size);
    for (int i = 0; i < N; i++)
    {
        h_data[i] = (float)i;
    }

    // ---- Medición GPU ----
    printf("--- Medición en GPU ---\n");
    // Copia de los datos para la prueba GPU (para no modificarlos en la prueba CPU)
    float *h_data_gpu = (float *)malloc(size);
    memcpy(h_data_gpu, h_data, size);
    medir_gpu(h_data_gpu, N, true);
    free(h_data_gpu);

    // ---- Medición CPU ----
    printf("\n--- Medición en CPU (OpenMP) ---\n");
    float *h_data_cpu = (float *)malloc(size);
    memcpy(h_data_cpu, h_data, size);
    double tiempo_cpu = procesar_cpu(h_data_cpu, N);
    printf("Tiempo CPU: %.3f ms\n", tiempo_cpu);
    free(h_data_cpu);

    // ---- Comparación ----
    printf("\n--- Comparación ---\n");
    printf("Nota: Esta es una prueba simplificada. En el proyecto real se medirán\n");
    printf("transferencias de bloques de imagen y filtros más complejos.\n");

    free(h_data);
    return 0;
}