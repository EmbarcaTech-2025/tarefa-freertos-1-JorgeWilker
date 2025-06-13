👤 **Autor:** Jorge Wilker Mamede de Andrade

📍 **Data:** 13 de junho de 2025

🏫 **Curso:** Residência Tecnológica em Sistemas Embarcados

🏢 **Instituição:** EmbarcaTech - HBr

📍 **Localização:** Campinas

---

# 🔍 Tarefa 1: Atividade roteirizada com FreeRTOS na BitDogLab - EmbarcaTech 2025

## 📋 Descrição da Tarefa

Este projeto implementa um sistema FreeRTOS para Raspberry Pi Pico demonstrando o uso de tarefas concorrentes, filas de comunicação e software timers. O projeto controla três LEDs diferentes através de tarefas independentes e utiliza uma fila para comunicação entre tarefas.

## 🚀 Como Usar

1. Configure o ambiente de desenvolvimento com Pico SDK
2. Compile o projeto usando CMake
3. Transfira o arquivo .uf2 gerado para o Raspberry Pi Pico
4. Observe o comportamento dos LEDs conforme especificado

## 🔧 Requisitos

- Raspberry Pi Pico
- Pico SDK instalado
- CMake e compilador ARM GCC
- Três LEDs conectados nos GPIOs 11, 12 e 13

## 🎯 Funcionalidades

- **LED Verde (GPIO 11)**: Pisca a cada 3 segundos via tarefa dedicada
- **LED Azul (GPIO 12)**: Pisca quando dados são recebidos da fila
- **LED Vermelho (GPIO 13)**: Pisca a cada 2 segundos via software timer
- **Comunicação via Fila**: Transferência de dados entre tarefas sender e receiver

---

## 📜 Licença

GNU GPL-3.0.
