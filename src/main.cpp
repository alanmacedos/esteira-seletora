#include <Arduino.h>

// =====================================
// MAPEAMENTO DAS ENTRADAS (SENSORES)
// =====================================

#define SENSOR_1 27
#define SENSOR_2 26
#define SENSOR_3 25
#define SENSOR_START 14
#define SENSOR_RAMPA01 32
#define SENSOR_METAL 35
#define SENSOR_7 34

// =====================================
// MAPEAMENTO DAS SAIDAS (RELES)
// =====================================

#define RELE_1 4
#define RELE_2 16
#define RELE_3 17
#define RELE_4 18
#define RELE_PARA_PECA 19
#define RELE_MAO2 21
#define RELE_MAO1 22
#define RELE_ESTEIRA 23

// =====================================
// ARRAYS DE MONITORAMENTO
// =====================================

const int sensores[] = {
  SENSOR_1,
  SENSOR_2,
  SENSOR_3,
  SENSOR_START,
  SENSOR_RAMPA01,
  SENSOR_METAL,
  SENSOR_7
};

const char* nomesSensores[] = {
  "SENSOR_1",
  "SENSOR_2",
  "SENSOR_3",
  "SENSOR_START",
  "SENSOR_RAMPA01",
  "SENSOR_METAL",
  "SENSOR_7"
};

const int reles[] = {
  RELE_1,
  RELE_2,
  RELE_3,
  RELE_4,
  RELE_PARA_PECA,
  RELE_MAO2,
  RELE_MAO1,
  RELE_ESTEIRA
};

const int quantidadeSensores = 7;
const int quantidadeReles = 8;

int estadosSensores[quantidadeSensores];
int estadosReles[quantidadeReles];

// =====================================
// CONTROLE DA ESTEIRA SELETORA
// =====================================

enum EstadoProducao {
  PARADO,
  ESTEIRA_LIGADA,
  AGUARDANDO_PARA_PECA,
  AGUARDANDO_MAO1,
  MAO1_ATIVA,
  AGUARDANDO_RAMPA
};

EstadoProducao estadoProducao = PARADO;

// =====================================
// CONFIGURACAO DOS TEMPOS
// =====================================

// Tempo máximo para detectar metal
const unsigned long TEMPO_SEM_METAL = 3000;

// Tempo entre acionar PARA_PECA e MAO1
const unsigned long TEMPO_ATE_MAO1 = 800;

// =====================================
// CONTROLE DE TEMPO
// =====================================

unsigned long tempoEtapa = 0;

// =====================================
// ESTADOS ANTERIORES DOS SENSORES
// =====================================

int estadoAnteriorStart = LOW;
int estadoAnteriorMetal = LOW;
int estadoAnteriorRampa = LOW;

// =====================================
// SETUP
// =====================================

void setup() {

  Serial.begin(9600);

  // Configura entradas
  for (int i = 0; i < quantidadeSensores; i++) {
    pinMode(sensores[i], INPUT);
    estadosSensores[i] = digitalRead(sensores[i]);
  }

  // Configura todas as saídas em LOW
  for (int i = 0; i < quantidadeReles; i++) {
    pinMode(reles[i], OUTPUT);
    digitalWrite(reles[i], LOW);
    estadosReles[i] = LOW;
  }

  // Inicializa os estados anteriores
  estadoAnteriorStart = digitalRead(SENSOR_START);
  estadoAnteriorMetal = digitalRead(SENSOR_METAL);
  estadoAnteriorRampa = digitalRead(SENSOR_RAMPA01);

  Serial.println("\n=== SISTEMA DA LINHA DE PRODUCAO ===");
  Serial.println("Entradas configuradas.");
  Serial.println("Saidas inicializadas em LOW.");
  Serial.println("Sistema pronto.");
}

// =====================================
// MONITORAMENTO DAS ENTRADAS
// =====================================

void monitorarSensores() {

  for (int i = 0; i < quantidadeSensores; i++) {

    int estadoAtual = digitalRead(sensores[i]);

    if (estadoAtual != estadosSensores[i]) {

      Serial.printf(
        "%s | GPIO %d | %s | Estado: %d\n",
        nomesSensores[i],
        sensores[i],
        estadoAtual == HIGH ? "ATIVADO" : "DESATIVADO",
        estadoAtual
      );

      estadosSensores[i] = estadoAtual;
    }
  }
}

// =====================================
// LOGICA DA LINHA DE PRODUCAO
// =====================================

void executarLogicaProducao() {

  // Leitura dos sensores
  int start = digitalRead(SENSOR_START);
  int metal = digitalRead(SENSOR_METAL);
  int rampa = digitalRead(SENSOR_RAMPA01);

  // -------------------------------------
  // INICIAR ESTEIRA
  // -------------------------------------

  if (estadoProducao == PARADO &&
      start == HIGH &&
      estadoAnteriorStart == LOW) {

    digitalWrite(RELE_ESTEIRA, HIGH);

    tempoEtapa = millis();

    estadoProducao = ESTEIRA_LIGADA;

    Serial.println("Esteira ligada.");
  }

  // -------------------------------------
  // DETECTAR PECA METALICA
  // -------------------------------------

  if (estadoProducao == ESTEIRA_LIGADA &&
      metal == HIGH &&
      estadoAnteriorMetal == LOW) {

    estadoProducao = AGUARDANDO_PARA_PECA;

    Serial.println("Metal detectado.");
  }

  // -------------------------------------
  // TEMPO LIMITE SEM DETECCAO DE METAL
  // -------------------------------------

  if (estadoProducao == ESTEIRA_LIGADA &&
      millis() - tempoEtapa >= TEMPO_SEM_METAL) {

    digitalWrite(RELE_PARA_PECA, HIGH);

    estadoProducao = AGUARDANDO_RAMPA;

    Serial.println("Nenhum metal detectado.");
    Serial.println("Rele para peca acionado.");
    Serial.println("Esteira continua ligada.");
  }

  // -------------------------------------
  // ACIONAR RELE PARA PECA - METAL
  // -------------------------------------

  if (estadoProducao == AGUARDANDO_PARA_PECA) {

    digitalWrite(RELE_PARA_PECA, HIGH);

    tempoEtapa = millis();

    estadoProducao = AGUARDANDO_MAO1;

    Serial.println("Rele para peca acionado.");
  }

  // -------------------------------------
  // ACIONAR MAO 1
  // -------------------------------------

  if (estadoProducao == AGUARDANDO_MAO1 &&
      millis() - tempoEtapa >= TEMPO_ATE_MAO1) {

    digitalWrite(RELE_MAO1, HIGH);

    estadoProducao = MAO1_ATIVA;

    Serial.println("Mao 1 acionada.");
    Serial.println("Aguardando sensor da rampa.");
  }

  // -------------------------------------
  // CONFIRMAR PECA NA RAMPA
  // -------------------------------------

  if (rampa == HIGH &&
      estadoAnteriorRampa == LOW &&
      estadoProducao != PARADO) {

    digitalWrite(RELE_ESTEIRA, LOW);
    digitalWrite(RELE_PARA_PECA, LOW);
    digitalWrite(RELE_MAO1, LOW);

    estadoProducao = PARADO;

    Serial.println("1 Peca adicionada na rampa.");
    Serial.println("Esteira parada.");
  }

  // -------------------------------------
  // ATUALIZAR ESTADOS ANTERIORES
  // -------------------------------------

  estadoAnteriorStart = start;
  estadoAnteriorMetal = metal;
  estadoAnteriorRampa = rampa;
}

// =====================================
// LOOP PRINCIPAL
// =====================================

void loop() {

  monitorarSensores();

  executarLogicaProducao();

}
