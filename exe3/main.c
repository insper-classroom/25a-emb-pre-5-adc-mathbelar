#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void process_task(void *p) {
    int data = 0;

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            
            const int WINDOW_SIZE = 5;
            int buffer[WINDOW_SIZE] = {0};
            int index = 0;
            int sum = 0;
            int count = 0;

            while (true) {
                if (xQueueReceive(xQueueData, &data, 100)) {
                    // Remove valor mais antigo da soma
                    sum -= buffer[index];
        
                    // Adiciona novo valor no buffer e à soma
                    buffer[index] = data;
                    sum += data;
        
                    // Atualiza o índice circular
                    index = (index + 1) % WINDOW_SIZE;
        
                    // Garante que só divide pela quantidade certa até ter 5 amostras
                    if (count < WINDOW_SIZE) count++;
        
                    // Calcula média
                    int media = sum / count;
        
                    // Imprime na UART
                    printf("%d\n", media);
        
                    // Delay obrigatório
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
            }



            // deixar esse delay!
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
