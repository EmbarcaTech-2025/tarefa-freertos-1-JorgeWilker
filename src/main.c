#include "FreeRTOS.h" // Biblioteca principal do FreeRTOS para gerenciamento de tarefas
#include "task.h" // Funcionalidades para criação e controle de tarefas
#include "queue.h" // Sistema de filas para comunicação inter-tarefas
#include "timers.h" // Software timers para controle temporal
#include <stdio.h> // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h" // Biblioteca padrão do Pico SDK
#include <stdint.h> // Definições de tipos de dados padronizados
#include <stdbool.h> // Definições para tipo booleano
#include "hardware/gpio.h" // Interface de controle GPIO

#include "hardware/i2c.h" // Interface de comunicação I2C

// Declaração global da fila para comunicação entre tarefas
QueueHandle_t xQueue = NULL;

// Declaração global do timer para controle do LED vermelho
TimerHandle_t xTimer = NULL;

// Implementação da tarefa de controle do LED verde
void led_task_green() {
    const uint LED_PIN = 11; // Definição do pino GPIO para LED verde
    gpio_init(LED_PIN); // Inicialização do pino GPIO
    gpio_set_dir(LED_PIN, GPIO_OUT); // Configuração como saída digital
    while (true) { // Loop de execução contínua
        gpio_put(LED_PIN, 0); // Estado baixo - LED desligado
        vTaskDelay(3000); // Suspensão da tarefa por 3000 ticks
        gpio_put(LED_PIN, 1); // Estado alto - LED ligado
        vTaskDelay(3000); // Suspensão da tarefa por 3000 ticks
    }
}

// Implementação da tarefa produtora para envio de dados
void sender_task() {
    uint32_t counter = 0; // Contador incremental para envio
    while (true) { // Loop de execução contínua
        if (xQueue != NULL) { // Verificação de inicialização da fila
            // Envio do valor para a fila com timeout de 100ms
            if (xQueueSend(xQueue, &counter, pdMS_TO_TICKS(100)) == pdPASS) {
                // Incremento apenas em caso de envio bem-sucedido
                counter++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500)); // Período de 500ms entre transmissões
    }
}

// Implementação da tarefa consumidora para recebimento de dados
void receiver_task() {
    const uint LED_BLUE_PIN = 12; // Definição do pino GPIO para LED azul
    gpio_init(LED_BLUE_PIN); // Inicialização do pino GPIO
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT); // Configuração como saída digital
    uint32_t receivedValue; // Buffer para dados recebidos
    while (true) { // Loop de execução contínua
        if (xQueue != NULL) { // Verificação de inicialização da fila
            // Recepção de dados da fila com timeout de 100ms
            if (xQueueReceive(xQueue, &receivedValue, pdMS_TO_TICKS(100)) == pdPASS) {
                // Sinalização visual de recepção de dados
                gpio_put(LED_BLUE_PIN, 1); // Ativação do LED azul
                vTaskDelay(pdMS_TO_TICKS(50)); // Duração do pulso luminoso
                gpio_put(LED_BLUE_PIN, 0); // Desativação do LED azul
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Período de polling de 100ms
    }
}

// Função de callback executada pelo timer para controle do LED vermelho
void vTimerCallback(TimerHandle_t xTimer) {
    const uint LED_RED_PIN = 13; // Definição do pino GPIO para LED vermelho
    static bool led_state = false; // Estado interno do LED

    // Configuração inicial do GPIO (executada apenas uma vez)
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);

    led_state = !led_state; // Inversão do estado do LED
    gpio_put(LED_RED_PIN, led_state); // Aplicação do novo estado
}

// Função principal do sistema
int main() {
    stdio_init_all(); // Inicialização das interfaces de entrada/saída

    // Criação da fila com capacidade para 10 elementos uint32_t
    xQueue = xQueueCreate(10, sizeof(uint32_t));
    if (xQueue == NULL) {
        // Tratamento de erro crítico - falha na criação da fila
        while(1){};
    }

    // Criação do timer de software com período de 2000ms
    xTimer = xTimerCreate(
        "LED_Timer",             // Identificador do timer
        pdMS_TO_TICKS(2000),     // Período de 2000ms
        pdTRUE,                  // Modo de auto-recarga ativado
        (void *)1,               // ID numérico do timer
        vTimerCallback           // Função de callback associada
    );

    if (xTimer == NULL) {
        // Tratamento de erro crítico - falha na criação do timer
        while(1){};
    }

    // Ativação do timer com início imediato
    if (xTimerStart(xTimer, pdMS_TO_TICKS(0)) != pdPASS) {
        // Tratamento de erro crítico - falha na inicialização do timer
        while(1){};
    }

    // Criação das tarefas do sistema com prioridade padrão
    xTaskCreate(led_task_green, "LED_Task_Green", 256, NULL, 1, NULL);
    xTaskCreate(sender_task, "Sender_Task", 256, NULL, 1, NULL);
    xTaskCreate(receiver_task, "Receiver_Task", 256, NULL, 1, NULL);
    
    // Inicialização do escalonador de tarefas
    vTaskStartScheduler();

    // Loop infinito de segurança (não deve ser alcançado)
    while(1){};
}