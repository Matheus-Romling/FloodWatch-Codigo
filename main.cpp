//pinos
const int buzzer = 7;
const int luzVermelha = 11;
const int luzAzul = 9;
const int luzVerde = 10;
const int botao = 2;
const int triggerPin = 4;
const int echoPin = 5;
const int sensorUmidade = A0;

//variaveis globais
int valorUmidade = 0;
unsigned long tempoInicial = 0;
unsigned long tempoFinal = 0;
bool contando = false;
bool alarmeAtivo = false;

//contador
int contador = 0;
int estadoAtual = HIGH;
int estadoAnterior = HIGH;

//media movel
const int numLeituras = 20; 
int leituras[numLeituras];
int indice = 0;
long soma = 0;
int mediaUmidade = 0;

//sensor ultrasonic
long readUltrasonicDistance(int triggerPin, int echoPin)
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

//evitar travar
  return pulseIn(echoPin, HIGH, 30000); 
}

//testes
void rotinaTeste()
{
  Serial.println("\n=== INICIANDO TESTE ===");
  Serial.print("Botao: ");
  Serial.println(digitalRead(botao));
  Serial.print("Umidade: ");
  Serial.println(analogRead(sensorUmidade));

  Serial.println("Testando LEDs...");
  digitalWrite(luzVermelha, HIGH);
  delay(1000);
  digitalWrite(luzVermelha, LOW);

  digitalWrite(luzVerde, HIGH);
  delay(1000);
  digitalWrite(luzVerde, LOW);

  digitalWrite(luzAzul, HIGH);
  delay(1000);
  digitalWrite(luzAzul, LOW);

  Serial.println("Testando buzzer...");
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);

  Serial.println("=== TESTE FINALIZADO ===\n");
}

//void setup
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

  //iniciar media movel
  for (int i=0; i<numLeituras; i++)
  {
    leituras[i] = 0;
  }

  //led e buzzer desligado
  digitalWrite(luzVermelha, LOW);
  digitalWrite(luzVerde, LOW);
  digitalWrite(luzAzul, LOW);
  digitalWrite(buzzer, LOW);

  //calibração inicial
  long duracao = readUltrasonicDistance(triggerPin, echoPin);
  float distanciaCm = duracao * 0.034 / 2.0;

  Serial.print("Distancia inicial: ");
  Serial.print(distanciaCm);
  Serial.println(" cm");
}

//void loop
void loop()
{
  //serial read da vida
  if (Serial.available() > 0)
  {
    char comando = Serial.read();
    if (comando == 't')
    {
      rotinaTeste();
    }
  }

  //contador de cliques
  estadoAtual = digitalRead(botao);

  if (estadoAnterior == HIGH && estadoAtual == LOW)
  {
    contador++;
    Serial.print("Numero de cliques: ");
    Serial.println(contador);

    if (contador % 2 != 0)
    {
      Serial.println("Sistema ATIVADO");
    }
    else
    {
      Serial.println("Sistema DESATIVADO");
    }
    delay(50); // debounce simples
  }
  estadoAnterior = estadoAtual;

  //media movel da unidade
  soma -= leituras[indice];
  leituras[indice] = analogRead(sensorUmidade);
  soma += leituras[indice];
  indice++;

  if (indice >= numLeituras)
  {
    indice = 0;
  }

  mediaUmidade = soma / numLeituras;
  valorUmidade = mediaUmidade;

  //sistema desligado
  if (contador % 2 == 0)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, LOW);
    contando = false;
    alarmeAtivo = false;
    delay(100); 
    return;
  }

  //sistema ligado
  if (valorUmidade <= 420)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzVerde, HIGH); // Verde indica tudo OK
    digitalWrite(luzAzul, LOW);
    
    contando = false;
    alarmeAtivo = false;
    
    delay(100);
    return;
  }

  //leitura sensor ultrasonic
  long duracao = readUltrasonicDistance(triggerPin, echoPin);
  float distanciaCm = duracao * 0.034 / 2.0;

  if (distanciaCm == 0) {
    delay(100);
    return;
  }

  if (distanciaCm > 40)
  {
    digitalWrite(buzzer, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, HIGH);
    
    contando = false;
    alarmeAtivo = false;
  }
 
  else if (distanciaCm <= 40 && distanciaCm > 30 && !contando)
  {
    tempoInicial = millis();
    contando = true;

    digitalWrite(buzzer, LOW);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzVermelha, LOW);
    digitalWrite(luzAzul, HIGH);

    Serial.println("Monitorando subida da agua...");
  }
  else if (distanciaCm <= 30 && !alarmeAtivo)
  {
    tempoFinal = millis();
    alarmeAtivo = true;

    digitalWrite(buzzer, HIGH);
    digitalWrite(luzVermelha, HIGH);
    digitalWrite(luzVerde, LOW);
    digitalWrite(luzAzul, LOW);

    if (contando) {
        float tempoSegundos = (tempoFinal - tempoInicial) / 1000.0;
        contando = false;

        if (tempoSegundos > 0)
        {
          float velocidadeCms = 10.0 / tempoSegundos;
          float velocidadeCmm = velocidadeCms * 60.0;

          //caso nao esteja tao grave
          Serial.println("\n=== ALERTA DE ENCHENTE ===");
          Serial.print("Umidade: ");
          Serial.println(valorUmidade);
          Serial.print("Velocidade da subida da agua: ");
          Serial.print(velocidadeCmm);
          Serial.println(" cm/min");
          
        }
    } else {
        //caso esteja grave
        Serial.println("\n=== ALERTA DE ENCHENTE IMEDIATO ===");
        Serial.print("Umidade: ");
        Serial.println(valorUmidade);
    }
  }
  delay(100);
}
