# ☁️ MindCast: Bio-Atmospheric Sync

![MindCast Logo](assets/image_549822.jpg)

> **"Sincronize sua biologia com o ambiente ao seu redor."**

O **MindCast** é um sistema IoT de bio-feedback que monitora variáveis ambientais (Temperatura e Pressão Atmosférica) e utiliza **Inteligência Artificial Generativa (Google Gemini 2.0)** para fornecer insights de produtividade e bem-estar em tempo real.

Não é apenas uma estação meteorológica; é um **Coach Cognitivo** que traduz dados físicos em ações biológicas para combater a fadiga e aumentar a performance cognitiva.

---

## 🧠 O Problema vs. Solução

**O Problema:** Estudos indicam que variações de temperatura e pressão em ambientes fechados afetam diretamente a capacidade cognitiva humana. [cite_start]O calor excessivo (>25°C) reduz a produtividade e aumenta a taxa de erros[cite: 1].

**A Solução:** Um ecossistema Omni-channel (Web + Telegram) que cruza dados do sensor BMP280 com perfis de usuário, gerando recomendações personalizadas via IA.

---

## ⚙️ Como Funciona

![Arquitetura do Sistema](assets/image_0c4526.png)

1.  **Coleta:** O microcontrolador **ESP32** lê temperatura e pressão via sensor **BMP280** (Protocolo I2C).
2.  **Processamento:** Os dados são enviados via HTTPS para a **API do Google Gemini 2.0 Flash**.
3.  **Inteligência:** O algoritmo "Coach 3.0" analisa o impacto biológico dos dados no perfil do usuário (ex: "Estudante Cansado").
4.  **Interface:** O usuário recebe o insight via **Bot do Telegram** (com botões interativos) ou visualiza no **Dashboard Web**.

---

## 🚀 Funcionalidades

* **Monitoramento em Tempo Real:** Leitura precisa de temperatura (°C) e pressão (hPa).
* **AI Insights:** Integração com Google Gemini para análise de contexto e dicas de saúde.
* **Interface Omni-channel:**
    * 📱 **Telegram Bot:** Comandos `/agora` e `/perfil` com teclado interativo.
    * 💻 **Web Dashboard:** Servidor Web embarcado no ESP32 com design Glassmorphism e atualização automática via AJAX/Meta Refresh.
* **Multitarefa:** Arquitetura de código não-bloqueante (millis) permitindo que o Bot e o Site funcionem simultaneamente.

---

## 🛠️ Hardware Necessário

* Microcontrolador ESP32 (DevKit V1)
* Sensor de Pressão e Temperatura BMP280 (ou BME280)
* Cabos Jumpers e Protoboard (ou Case impressa em 3D)

---

## 📦 Instalação e Configuração

### 1. Pré-requisitos de Software (Arduino IDE)
Instale as seguintes bibliotecas através do Gerenciador de Bibliotecas:
* `UniversalTelegramBot` (Brian Lough)
* `ArduinoJson` (Benoit Blanchon)
* `Adafruit BMP280 Library` (Adafruit)
* `Adafruit Unified Sensor` (Adafruit)

### 2. Configuração do Código
Abra o arquivo `MindCast.ino` e preencha suas credenciais nas linhas iniciais:

```cpp
#define WIFI_SSID     "SUA_REDE_WIFI"
#define WIFI_PASSWORD "SUA_SENHA_WIFI"
#define BOT_TOKEN     "SEU_TOKEN_TELEGRAM" // Obtenha com o @BotFather
#define CHAT_ID       "SEU_CHAT_ID"        // Obtenha com o @IDBot
#define GEMINI_API_KEY "SUA_API_KEY_GOOGLE" // Obtenha no Google AI Studio
