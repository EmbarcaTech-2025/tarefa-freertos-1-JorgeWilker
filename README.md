👤 **Autor:** Jorge Wilker Mamede de Andrade

📍 **Data:** 13 de junho de 2025

🏫 **Curso:** Residência Tecnológica em Sistemas Embarcados

🏢 **Instituição:** EmbarcaTech - HBr

📍 **Localização:** Campinas

---

# 🔍 Tarefa 1: Sistema Multitarefa Avançado com FreeRTOS na BitDogLab - EmbarcaTech 2025

## 📋 Descrição da Tarefa

Este projeto implementa um sistema multitarefa embarcado avançado usando FreeRTOS para controlar periféricos da BitDogLab de forma concorrente. O sistema demonstra o uso de suspensão/retomada de tarefas, controle por interrupções de botões e gerenciamento de múltiplos periféricos simultaneamente.

## 🚀 Como Usar

1. Configure o ambiente de desenvolvimento com Pico SDK
2. Compile o projeto usando CMake
3. Transfira o arquivo .uf2 gerado para o Raspberry Pi Pico na BitDogLab
4. Observe o comportamento dos periféricos:
   - **LED RGB** alterna automaticamente entre vermelho, verde e azul
   - **Buzzer** emite bipes periódicos a cada 2 segundos
   - **Botão A** suspende/retoma a tarefa do LED RGB
   - **Botão B** suspende/retoma a tarefa do buzzer

## 🔧 Requisitos

- **Hardware:**
  - BitDogLab (ou Raspberry Pi Pico compatível)
  - LED RGB nos pinos GPIO 11, 12, 13
  - Buzzer no pino GPIO 21
  - Botões nos pinos GPIO 5 e 6

- **Software:**
  - Pico SDK instalado
  - CMake e compilador ARM GCC
  - FreeRTOS configurado

## 🎯 Funcionalidades Implementadas

### 🌈 **LED RGB (GPIOs 11, 12, 13)**
- Ciclo automático: Vermelho → Verde → Azul
- Transição a cada 1 segundo
- Controlado pelo Botão A (suspender/retomar)
- LEDs desligam automaticamente quando suspenso

### 🔊 **Buzzer (GPIO 21)**
- Bipes periódicos a cada 2 segundos
- Duração do bipe: 100ms com frequência de 500Hz
- Controlado pelo Botão B (suspender/retomar)
- Buzzer desliga automaticamente quando suspenso

### 🎮 **Controle por Botões**
- **Botão A (GPIO 5):** Suspende/retoma tarefa do LED RGB
- **Botão B (GPIO 6):** Suspende/retoma tarefa do buzzer
- Debounce implementado para evitar múltiplos acionamentos
- Pull-up interno habilitado

### 📊 **Sistema de Status**
- Tarefa dedicada para monitoramento do sistema
- Exibe status das tarefas a cada 5 segundos via serial
- Feedback visual e textual das operações

## ⚙️ Arquitetura do Sistema

### 📋 **Tarefas Implementadas:**

1. **`led_rgb_task`** (Prioridade 2)
   - Controla o ciclo de cores do LED RGB
   - Pode ser suspensa/retomada dinamicamente

2. **`buzzer_task`** (Prioridade 2)
   - Gerencia os bipes periódicos do buzzer
   - Pode ser suspensa/retomada dinamicamente

3. **`button_monitor_task`** (Prioridade 3 - Mais Alta)
   - Monitora constantemente os botões A e B
   - Implementa debounce e controle de tarefas

4. **`system_status_task`** (Prioridade 1 - Mais Baixa)
   - Exibe status do sistema periodicamente
   - Monitora o estado das tarefas

### 🔄 **Controle de Tarefas:**
- Uso de `TaskHandle_t` para controle direto das tarefas
- Funções `vTaskSuspend()` e `vTaskResume()` para controle dinâmico
- Estados persistentes para rastreamento do status

## 📈 Resultados Esperados

- **LED RGB** alternando cores suavemente
- **Buzzer** emitindo bipes regulares
- **Botões** respondendo imediatamente às pressões
- **Sistema** mantendo operação estável com multitarefas
- **Serial** exibindo logs de status e operações

## 🧠 Conceitos Aplicados

- **Multitarefas Preemptivas** com FreeRTOS
- **Suspensão/Retomada Dinâmica** de tarefas
- **Controle de Prioridades** para responsividade
- **Debounce** de botões por software
- **GPIO** para controle de periféricos
- **Monitoramento de Estado** em tempo real

---

## 📜 Licença

GNU GPL-3.0.
