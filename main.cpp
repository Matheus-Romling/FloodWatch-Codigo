// ===== PINOS =====
const int buzzer = 7;
const int luzVermelha = 11;
const int luzAzul = 9;
const int luzVerde = 10;
const int botao = 2;
const int triggerPin = 4;
const int echoPin = 5;
const int sensorUmidade = A0;

// ===== VARIÁVEIS =====
int valorUmidade = 0;
unsigned long tempoInicial = 0;
unsigned long tempoFinal = 0;
bool contando = false;

// ===== CONTADOR DE CLIQUES =====
int contador = 0;
int estadoAtual = HIGH;
int estadoAnterior = HIGH;

// ===== MÉDIA MÓVEL =====
const int numLeituras = 10;
int leituras[numLeituras];
int indice = 0;
long soma = 0;
int mediaUmidade = 0;

// ======================================================
// SENSOR ULTRASSÔNICO
// ======================================================
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  return pulseIn(echoPin, HIGH);
}

// ======================================================
// RESET DO SISTEMA (NÃO DESLIGA O CONTADOR)
// ======================================================
void resetSistema()
{
  Serial.println("\n=== RESET DO SISTEMA ===");

  tempoInicial = 0;
  tempoFinal = 0;
  contando = false;

  soma = 0;
  indice = 0;
  valorUmidade = 0;
  mediaUmidade = 0;

  for (int i = 0; i < numLeituras; i++)
  {
    leituras[i] = 0;
  }

  digitalWrite(luzVermelha, LOW);
  digitalWrite(luzVerde, LOW);
  digitalWrite(luzAzul, LOW);
  digitalWrite(buzzer, LOW);

  Serial.println("Sistema reiniciado!\n");
}

// ======================================================
// ROTINA DE TESTE
// ======================================================
void rotinaTeste()
{
  Serial.println("\n=== INICIANDO TESTE ===");

  Serial.print("Botao: ");
  Serial.println(digitalRead(botao));

  Serial.print("Umidade: ");
  Serial.println(analogRead(sensorUmidade));

  digitalWrite(luzVermelha, HIGH); delay(1000); digitalWrite(luzVermelha, LOW);
  digitalWrite(luzVerde, HIGH); delay(1000); digitalWrite(luzVerde, LOW);
  digitalWrite(luzAzul, HIGH); delay(1000); digitalWrite(luzAzul, LOW);

  digitalWrite(buzzer, HIGH); delay(1000); digitalWrite(buzzer, LOW);

  Serial.println("=== TESTE FINALIZADO ===\n");
}

// ======================================================
// SETUP
// ======================================================
void setup()
{
  pinMode(luzVermelha, OUTPUT);
  pinMode(luzVerde, OUTPUT);
  pinMode(luzAzul, OUTPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(botao, INPUT_PULLUP);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

  for (int i = 0; i < numLeituras; i++)
    leituras[i] = 0;

  digitalWrite(luzVermelha, LOW);
  digitalWrite(luzVerde, LOW);
  digitalWrite(luzAzul, LOW);
  digitalWrite(buzzer, LOW);
}

// ======================================================
// LOOP
// ======================================================
void loop()
{
  // ===== SERIAL =====
  if (Serial.available() > 0)
  {
    char comando = Serial.read();
    if (comando == 't') rotinaTeste();
  }

  // ===== BOTÃO (TOGGLE + RESET) =====
  estadoAtual = digitalRead(botao);

  if (estadoAnterior == HIGH && estadoAtual == LOW)
  {
    contador++;

    Serial.print("Cliques: ");
    Serial.println(contador);

    if (contador % 2 != 0)
      Serial.println("Sistema ATIVADO");
    else
      Serial.println("Sistema DESATIVADO");

    resetSistema();  // <<< AQUI ESTÁ O RESET ADICIONADO

    delay(50);
  }

  estadoAnterior = estadoAtual;

  // ===== SISTEMA DESLIGADO =====
  if (contador % 2 == 0)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, LOW);
    contando = false;
    return;
  }

  // ===== MÉDIA MÓVEL =====
  soma -= leituras[indice];
  leituras[indice] = analogRead(sensorUmidade);
  soma += leituras[indice];

  indice++;
  if (indice >= numLeituras) indice = 0;

  mediaUmidade = soma / numLeituras;
  valorUmidade = mediaUmidade;

  // ===== UMIDADE BAIXA =====
  if (valorUmidade <= 0)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(luzVerde, HIGH);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzAzul, LOW);
    return;
  }

  // ===== ULTRASSÔNICO =====
  long duracao = readUltrasonicDistance(triggerPin, echoPin);
  float distanciaCm = duracao * 0.034 / 2.0;
  
  if(distanciaCm > 40){
    contando = false;

    digitalWrite(buzzer, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, LOW);
  }

  if (distanciaCm <= 40 && distanciaCm > 30 && !contando)
  {
    tempoInicial = millis();
    contando = true;

    digitalWrite(luzVerde, HIGH);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzAzul, LOW);

    Serial.println("Monitorando subida da agua...");
  }

  if (distanciaCm <= 30 && contando)
  {
    tempoFinal = millis();
    contando = false;

    digitalWrite(buzzer, HIGH);
    digitalWrite(luzVermelha, HIGH);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, LOW);

    float tempoSegundos = (tempoFinal - tempoInicial) / 1000.0;

    if (tempoSegundos > 0)
    {
      float velocidadeCms = 10.0 / tempoSegundos;
      float velocidadeCmm = velocidadeCms * 60.0;

      Serial.println("\n=== ALERTA DE ENCHENTE ===");
      Serial.print("Umidade: ");
      Serial.println(valorUmidade);
      Serial.print("Velocidade: ");
      Serial.print(velocidadeCmm);
      Serial.println(" cm/min");
      delay(15000);
    }
  }
}
