#include <iostream>
#include <cuda_runtime.h>

// Operación aritmética simple ejecutada en  GPU

__global__ void kernel_prueba(float* datos, int n) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        datos[i] = datos[i] * 2.0f;
    }
}

int main() {
    const int N = 10; // Tamaño del arreglo de prueba
    size_t size = N * sizeof(float);

    //  Reserva de Memoria en el Host
    float* h_datos = (float*)malloc(size);
    for (int i = 0; i < N; i++) {
        h_datos[i] = (float)i;
    }

    std::cout << "Datos originales en Host: ";
    for (int i = 0; i < N; i++) std::cout << h_datos[i] << " ";
    std::cout << std::endl;

    //  Reserva de Memoria en el Device (VRAM de la  gráfica)
    float* d_datos;
    cudaMalloc((void**)&d_datos, size);

    //  Transferencia de Memoria: Host a Device (H2D)
    cudaMemcpy(d_datos, h_datos, size, cudaMemcpyHostToDevice);

    //  Ejecución del Kernel (Prueba de procesamiento)
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    kernel_prueba<<<blocksPerGrid, threadsPerBlock>>>(d_datos, N);

    // Transferencia de Memoria: Device a Host
    // Retornamos los valores a la CPU para validar la transferencia
    cudaMemcpy(h_datos, d_datos, size, cudaMemcpyDeviceToHost);

    // Validación de resultados
    std::cout << "Datos procesados retornados al Host: ";
    for (int i = 0; i < N; i++) std::cout << h_datos[i] << " ";
    std::cout << std::endl;

    // Liberación de Memoria
    cudaFree(d_datos);
    free(h_datos);

    std::cout << "\nPrueba de gestion de memoria finalizada con exito" << std::endl;

    return 0;
}
