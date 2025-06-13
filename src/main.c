#include "FreeRTOS.h" // Biblioteca principal do FreeRTOS para gerenciamento de tarefas
#include "task.h" // Funcionalidades para criação e controle de tarefas
#include "queue.h" // Sistema de filas para comunicação inter-tarefas
#include "timers.h" // Software timers para controle temporal
#include <stdio.h> // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h" // Biblioteca padrão do Pico SDK
#include <stdint.h> // Definições de tipos de dados padronizados
#include <stdbool.h> // Definições para tipo booleano
#include "hardware/gpio.h" // Interface de controle GPIO
#include "hardware/pwm.h" // Interface PWM para buzzer

// Definições dos pinos da BitDogLab
#define LED_RGB_RED_PIN     11  // Pino do LED vermelho do RGB
#define LED_RGB_GREEN_PIN   12  // Pino do LED verde do RGB  
#define LED_RGB_BLUE_PIN    13  // Pino do LED azul do RGB
#define BUZZER_PIN          21  // Pino do buzzer
#define BUTTON_A_PIN        5   // Pino do botão A
#define BUTTON_B_PIN        6   // Pino do botão B

// Handles das tarefas para controle de suspensão/retomada
TaskHandle_t xLedRgbTaskHandle = NULL;
TaskHandle_t xBuzzerTaskHandle = NULL;

// Estados dos botões para debounce
static bool button_a_last_state = false;
static bool button_b_last_state = false;

// Estados das tarefas (suspensas ou não)
static bool led_task_suspended = false;
static bool buzzer_task_suspended = false;

// Implementação da tarefa de controle do LED RGB
void led_rgb_task(void *pvParameters) {
    // Inicialização dos pinos do LED RGB
    gpio_init(LED_RGB_RED_PIN);
    gpio_init(LED_RGB_GREEN_PIN);
    gpio_init(LED_RGB_BLUE_PIN);
    gpio_set_dir(LED_RGB_RED_PIN, GPIO_OUT);
    gpio_set_dir(LED_RGB_GREEN_PIN, GPIO_OUT);
    gpio_set_dir(LED_RGB_BLUE_PIN, GPIO_OUT);

    // Estados do ciclo RGB: 0=Vermelho, 1=Verde, 2=Azul
    uint8_t rgb_state = 0;

    while (true) {
        switch (rgb_state) {
            case 0: // Estado vermelho
                gpio_put(LED_RGB_RED_PIN, 1);
                gpio_put(LED_RGB_GREEN_PIN, 0);
                gpio_put(LED_RGB_BLUE_PIN, 0);
                break;
            case 1: // Estado verde
                gpio_put(LED_RGB_RED_PIN, 0);
                gpio_put(LED_RGB_GREEN_PIN, 1);
                gpio_put(LED_RGB_BLUE_PIN, 0);
                break;
            case 2: // Estado azul
                gpio_put(LED_RGB_RED_PIN, 0);
                gpio_put(LED_RGB_GREEN_PIN, 0);
                gpio_put(LED_RGB_BLUE_PIN, 1);
                break;
        }

        // Avança para o próximo estado do ciclo
        rgb_state = (rgb_state + 1) % 3;

        // Delay de 1 segundo entre as cores
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Implementação da tarefa de controle do buzzer
void buzzer_task(void *pvParameters) {
    // Inicialização do pino do buzzer
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    while (true) {
        // Emite bipe de 100ms
        for (int i = 0; i < 100; i++) {
            gpio_put(BUZZER_PIN, 1);
            vTaskDelay(pdMS_TO_TICKS(1)); // 1ms alto
            gpio_put(BUZZER_PIN, 0);
            vTaskDelay(pdMS_TO_TICKS(1)); // 1ms baixo
        }
        
        // Pausa de 2 segundos entre bipes
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Implementação da tarefa de monitoramento dos botões
void button_monitor_task(void *pvParameters) {
    // Inicialização dos pinos dos botões com pull-up interno
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);

    while (true) {
        // Leitura dos estados dos botões (invertidos devido ao pull-up)
        bool button_a_current = !gpio_get(BUTTON_A_PIN);
        bool button_b_current = !gpio_get(BUTTON_B_PIN);

        // Detecção de borda de subida no botão A (pressionado)
        if (button_a_current && !button_a_last_state) {
            if (led_task_suspended) {
                // Retoma a tarefa do LED RGB
                vTaskResume(xLedRgbTaskHandle);
                led_task_suspended = false;
                printf("LED RGB retomado\n");
            } else {
                // Suspende a tarefa do LED RGB
                vTaskSuspend(xLedRgbTaskHandle);
                led_task_suspended = true;
                // Desliga todos os LEDs ao suspender
                gpio_put(LED_RGB_RED_PIN, 0);
                gpio_put(LED_RGB_GREEN_PIN, 0);
                gpio_put(LED_RGB_BLUE_PIN, 0);
                printf("LED RGB suspenso\n");
            }
        }

        // Detecção de borda de subida no botão B (pressionado)
        if (button_b_current && !button_b_last_state) {
            if (buzzer_task_suspended) {
                // Retoma a tarefa do buzzer
                vTaskResume(xBuzzerTaskHandle);
                buzzer_task_suspended = false;
                printf("Buzzer retomado\n");
            } else {
                // Suspende a tarefa do buzzer
                vTaskSuspend(xBuzzerTaskHandle);
                buzzer_task_suspended = true;
                // Desliga o buzzer ao suspender
                gpio_put(BUZZER_PIN, 0);
                printf("Buzzer suspenso\n");
            }
        }

        // Atualiza os estados anteriores dos botões
        button_a_last_state = button_a_current;
        button_b_last_state = button_b_current;

        // Delay para debounce dos botões
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// Implementação da tarefa de status do sistema
void system_status_task(void *pvParameters) {
    while (true) {
        printf("=== Status do Sistema ===\n");
        printf("LED RGB: %s\n", led_task_suspended ? "SUSPENSO" : "ATIVO");
        printf("Buzzer: %s\n", buzzer_task_suspended ? "SUSPENSO" : "ATIVO");
        printf("Botão A: Controla LED RGB\n");
        printf("Botão B: Controla Buzzer\n");
        printf("========================\n");

        // Exibe status a cada 5 segundos
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

// Função principal do sistema
int main() {
    // Inicialização das interfaces de entrada/saída
    stdio_init_all();

    printf("\n=== Sistema Multitarefa BitDogLab ===\n");
    printf("Inicializando sistema FreeRTOS...\n");

    // Criação da tarefa do LED RGB com prioridade 2
    if (xTaskCreate(led_rgb_task, "LED_RGB_Task", 256, NULL, 2, &xLedRgbTaskHandle) != pdPASS) {
        printf("Erro: Falha na criação da tarefa LED RGB\n");
        while(1);
    }

    // Criação da tarefa do buzzer com prioridade 2
    if (xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 2, &xBuzzerTaskHandle) != pdPASS) {
        printf("Erro: Falha na criação da tarefa do buzzer\n");
        while(1);
    }

    // Criação da tarefa de monitoramento dos botões com prioridade 3 (mais alta)
    if (xTaskCreate(button_monitor_task, "Button_Monitor_Task", 256, NULL, 3, NULL) != pdPASS) {
        printf("Erro: Falha na criação da tarefa de monitoramento dos botões\n");
        while(1);
    }

    // Criação da tarefa de status do sistema com prioridade 1 (mais baixa)
    if (xTaskCreate(system_status_task, "System_Status_Task", 512, NULL, 1, NULL) != pdPASS) {
        printf("Erro: Falha na criação da tarefa de status do sistema\n");
        while(1);
    }

    printf("Todas as tarefas criadas com sucesso!\n");
    printf("Pressione Botão A para controlar LED RGB\n");
    printf("Pressione Botão B para controlar Buzzer\n");

    // Inicialização do escalonador de tarefas
    vTaskStartScheduler();

    // Loop infinito de segurança (não deve ser alcançado)
    printf("Erro: Escalonador parou inesperadamente\n");
    while(1);
}