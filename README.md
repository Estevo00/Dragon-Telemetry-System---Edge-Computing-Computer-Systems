# 🚀 Dragon Telemetry System - Edge Computing & Computer Systems

### 🔗 Link do Projeto no Wokwi:
👉 **[Clique aqui para acessar a simulação ao vivo](https://wokwi.com/projects/465937213607910401)**

---

## 📋 Descrição do Projeto
Este projeto consiste no desenvolvimento do sistema embarcado de borda (Edge Computing) para a cápsula **Crew Dragon da SpaceX**, integrado como parte fundamental da Global Solution 2026. O dispositivo utiliza um microcontrolador ESP32 para monitorar variáveis críticas de navegação e suporte à vida, transmitindo os dados em tempo real via rádio através do protocolo leve **MQTT** para o broker público da **EMQX**.

O sistema opera de forma autônoma: realiza a leitura dos sensores físicos e processa localmente regras de negócio para identificar anomalias em órbita. Caso alguma falha crítica seja detectada, um atuador visual (LED Master Caution) é acionado imediatamente em tempo de execução para alertar a tripulação.

---

## 🛠️ Arquitetura do Sistema

### 1. Hardware e Componentes (Simulados no Wokwi)
* **ESP32 (NodeMCU):** Unidade central de processamento com conectividade Wi-Fi nativa para transmissão de dados.
* **Sensor DHT22:** Monitora a temperatura interna da cabine, representando o Sistema de Suporte à Vida (ECLSS).
* **Sensor BMP180:** Sensor barométrico digital conectado via barramento I2C para monitorar a pressurização interna estrutural da cabine.
* **Potenciômetro Analógico:** Simula os computadores de bordo gerando o cálculo da Velocidade Orbital Dinâmica da espaçonave através da porta ADC (GPIO 34).
* **LED Vermelho:** Atuador físico local responsável pelo sistema visual de alertas intermitentes em caso de emergência.

### 2. Protocolo de Comunicação e Mensageria
* **Protocolo:** MQTT (Message Queuing Telemetry Transport) sobre TCP/IP, garantindo baixa latência e consumo mínimo de banda.
* **Broker Público:** `broker.emqx.io` (Porta WebSockets: `8084` / Porta MQTT Padrão: `1883`)
* **Tópico de Publicação (Payload JSON):** `dragon/telemetry/full`

---

## ⚙️ Funcionamento e Regras de Triggers (Anomalias)

O firmware do ESP32 realiza a leitura de todos os sensores e publica a telemetria a cada **2 segundos**. Conforme as diretrizes do projeto, o sistema monitora **3 parâmetros distintos**:

| Parâmetro Monitorado | Componente | Operação Nominal | Limite Crítico | Evento Espacial Simulado |
| :--- | :--- | :--- | :--- | :--- |
| **Temperatura da Cabine** | Sensor DHT22 | 20°C a 30°C | **> 80.0°C** | Sobrecarga térmica ou falha nos radiadores |
| **Pressão Interna** | Sensor BMP180 | 101.3 kPa | **< 90.0 kPa** | Despressurização / Vazamento estrutural |
| **Velocidade Orbital** | Potenciômetro | 7.600 a 7.800 km/h | **> 28.000 km/h** | Exceção de órbita / Afastamento da rota segura |

### Comportamento do Atuador (LED):
* **🟢 Status Nominal:** Condições de voo seguras. O LED permanece **desligado** e o payload JSON envia `"status": "OK"`.
* **🔴 Status Crítico:** Se qualquer um dos limites da tabela for violado, o sistema entra em estado de emergência. O LED começa a **piscar rapidamente (a cada 200ms)** e o payload JSON envia `"status": "CRITICAL"`.

---

## 🔌 Diagrama de Conexões (Pinagem do ESP32)

| Componente | Pino no Componente | Pino no ESP32 | Tipo de Sinal / Protocolo |
| :--- | :--- | :--- | :--- |
| **DHT22** | SDA / OUT | **GPIO 5** | Digital (Single-Wire) |
| **BMP180** | SDA | **GPIO 21** | Digital (I2C) |
| **BMP180** | SCL | **GPIO 22** | Digital (I2C) |
| **Potenciômetro** | SIG / OUT | **GPIO 34** | Analógico (Entrada ADC) |
| **LED Vermelho** | Anodo (+) | **GPIO 2** | Digital (Saída PWM) |
| **Todos** | VCC / VIN | **3V3** | Alimentação Positiva |
| **Todos** | GND | **GND** | Terra de Referência |

---

## 💻 Código Fonte (Firmware)
O código desenvolvido utiliza as bibliotecas oficiais da Adafruit e o cliente PubSubClient para a orquestração dos dados. O arquivo completo com tratamento de reconexão do Wi-Fi e do Broker MQTT encontra-se na raiz desta pasta sob o nome de `sketch.ino`.

---

## 📺 Demonstração de Funcionamento
Abaixo, é possível conferir o comportamento do hardware simulado no Wokwi, demonstrando a leitura dos sensores e o disparo dos alertas em tempo real:

https://youtu.be/n0TZ1mkNzdo

---

## 👥 Integrantes da Equipe
* **Felipe Estevo Santos** - RM567780
* **Cauã Pereira da Silva** - RM568143
* **Igor Grave Teixeira** - RM567663
* **Renan dos Reis Santos** - RM568540
