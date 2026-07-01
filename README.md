
# 🌊 FloodWatch

> Sistema embarcado de alerta precoce para enchentes, desenvolvido com Arduino.

---

## 📋 Sobre o Projeto

O **FloodWatch** é um sistema de monitoramento de enchentes em tempo real. Ele detecta o nível da água, calcula a velocidade de subida e emite alertas visuais e sonoros quando a situação atinge um nível crítico — dando tempo para que as pessoas se protejam antes que os danos se tornem irreversíveis.

O projeto foi desenvolvido como trabalho acadêmico do curso de **Ciência da Computação da PUC Minas**, e se relaciona com o **ODS 9 — Indústria, Inovação e Infraestrutura**.

---

## 👥 Integrantes

| Nome |
|------|
| Gabriela Schaper Soriano Veiga |
| Lucas Manata de Pinho |
| Matheus Romling Rotheia Almeida |
| Miguel Saraiva Ferreira |



**Professores:** Cristiano Neves Rodrigues · Ilo Amy Saldanha Rivero · Mariana de Oliveira Santos Silva



---

## ⚙️ Como Funciona

O sistema é ativado e desativado por **cliques alternados no botão** (toggle). Cliques ímpares ligam o sistema; cliques pares desligam.

Com o sistema ativo:

1. Se a umidade estiver **abaixo de 420**, o LED verde permanece aceso e nenhuma ação é tomada
2. Se a umidade **ultrapassar 420**, o sensor ultrassônico começa a monitorar o nível da água
3. Ao detectar água entre **40 cm e 30 cm**, o cronômetro é iniciado e o LED verde permanece aceso
4. Ao atingir **30 cm**, o cronômetro para, a **velocidade de subida** é calculada em cm/min e cm/s
5. O **LED vermelho** acende e o **buzzer** é ativado
6. Os dados de umidade, distância e velocidade são exibidos no Monitor Serial

Com o sistema desativado, todos os LEDs e o buzzer são apagados automaticamente.

---

## 🔌 Hardware Utilizado

### Entradas

| Componente | Tipo | Pino | Função |
|---|---|---|---|
| Sensor de umidade do solo* | Analógico | A0 | Detecta presença de água |
| Sensor ultrassônico (HC-SR04) | Digital | 4 (trigger) e 5 (echo) | Mede distância da água |
| Botão | Digital | 2 (INPUT_PULLUP) | Liga/desliga o sistema por clique |

*Utilizado para representar um sensor de umidade do ar.

### Saídas

| Componente | Tipo | Pino | Função |
|---|---|---|---|
| LED vermelho | Digital | 11 | Alerta de enchente crítica |
| LED verde | Digital | 10 | Sistema ativo e sem perigo |
| LED azul | Digital | 9 | Disponível para uso futuro |
| Buzzer | Digital | 7 | Alerta sonoro de enchente |
| Monitor Serial | — | — | Exibe dados em tempo real (9600 baud) |

---

## 🧠 Destaques Técnicos

**Toggle por clique:** O botão usa `INPUT_PULLUP` e conta os cliques. Cliques ímpares ativam o sistema; cliques pares desativam. Inclui debounce simples de 50ms.

**Filtro de média móvel:** O sensor de umidade realiza 10 leituras consecutivas e calcula a média, eliminando ruídos e interferências elétricas.

**Cálculo de velocidade:** Usando `millis()`, o sistema cronometra o intervalo entre as marcas de 40 cm e 30 cm e calcula a velocidade de subida em cm/s e cm/min.

**Fórmula do ultrassônico:**
```
distância (cm) = duração × 0.034 / 2
```

**Calibração inicial:** No `setup()`, o sistema faz uma leitura inicial do ultrassônico e exibe a distância no Monitor Serial antes de começar o monitoramento.

**Rotina de teste:** Ao pressionar `t` no Monitor Serial, o sistema testa cada componente individualmente — LED vermelho, verde, azul, buzzer e sensores — para validar o hardware antes do uso.

---

## 📟 Saídas no Monitor Serial

| Evento | Mensagem |
|---|---|
| Clique no botão | `Numero de cliques: X` + `Sistema ATIVADO/DESATIVADO` |
| Água entre 40–30 cm | `Monitorando subida da agua...` |
| Alerta disparado | `=== ALERTA DE ENCHENTE ===` + umidade, distância e velocidade |
| Rotina de teste | `=== INICIANDO TESTE ===` ... `=== TESTE FINALIZADO ===` |

---

## 🔗 Simulação

Acesse o circuito no Tinkercad: *[inserir link]*

---

## ⚠️ Limitações

- Dependência de energia elétrica contínua
- Alcance reduzido do sensor ultrassônico
- Sensível a interferências em ambientes externos (chuva intensa, sujeira)

---

## 🚀 Melhorias Futuras

- Alimentação por bateria solar
- Proteção física contra água e intempéries
- Conexão com internet (IoT)
- Envio de alertas para celular via SMS ou notificação
- Armazenamento de histórico para análise de padrões de enchente

---

## 📎 Referências

- G1 Minas Gerais — Enchentes em Juiz de Fora (2026)
- ODS 9 — Agenda 2030 da ONU

---

## Video Demonstrativo
- https://www.youtube.com/watch?v=TmTVx0oz63U&authuser=0
