
#include <Arduino.h>

// =====================================
// MAPEAMENTO DAS ENTRADAS (SENSORES)
// =====================================

#define SENSOR_1 27
#define SENSOR_2 26
#define SENSOR_3 25
#define SENSOR_4 14
#define SENSOR_5 32
#define SENSOR_6 35
#define SENSOR_7 34

// =====================================
// MAPEAMENTO DAS SAIDAS (RELES)
// =====================================

#define RELE_1 4
#define RELE_2 16
#define RELE_3 17
#define RELE_4 18
#define RELE_5 19
#define RELE_6 21
#define RELE_7 22
#define RELE_8 23

// =====================================
// ARRAYS DE ENTRADAS
// =====================================

const int sensores[] = {
  SENSOR_1,
  SENSOR_2,
  SENSOR_3,
  SENSOR_4,
  SENSOR_5,
  SENSOR_6,
  SENSOR_7
};

const char* nomesSensores[] = {
  "SENSOR_1",
  "SENSOR_2",
  "SENSOR_3",
  "SENSOR_4",
  "SENSOR_5",
  "SENSOR_6",
  "SENSOR_7"
};

const int quantidadeSensores = 7;

int estadosSensores[quantidadeSensores];

// =====================================
// ARRAYS DE SAIDAS
// =====================================

const int reles[] = {
  RELE_1,
  RELE_2,
  RELE_3,
  RELE_4,
  RELE_5,
  RELE_6,
  RELE_7,
  RELE_8
};

const char* nomesReles[] = {
  "RELE_1",
  "RELE_2",
  "RELE_3",
  "RELE_4",
  "RELE_5",
  "RELE_6",
  "RELE_7",
  "RELE_8"
};

const int quantidadeReles = 8;

// =====================================
// CONFIGURACAO DO TESTE
// =====================================

// Selecione aqui a saida que deseja testar
int teste = RELE_5;

// Intervalo de alternancia
const unsigned long INTERVALO_TESTE = 2000;

// Estado inicial
int estadoRele = LOW;

unsigned long tempoAnterior = 0;

// =====================================
// IDENTIFICAR NOME DO RELE
// =====================================

const char* identificarRele(int pino) {

  for (int i = 0; i < quantidadeReles; i++) {

    if (reles[i] == pino) {
      return nomesReles[i];
    }
  }

  return "RELE NAO IDENTIFICADO";
}

// =====================================
// SETUP
// =====================================

void setup() {

  Serial.begin(9600);

  // Configurar entradas
  for (int i = 0; i < quantidadeSensores; i++) {

    pinMode(sensores[i], INPUT);

    estadosSensores[i] = digitalRead(sensores[i]);
  }

  // Configurar todas as saidas
  for (int i = 0; i < quantidadeReles; i++) {

    pinMode(reles[i], OUTPUT);

    digitalWrite(reles[i], LOW);
  }

  Serial.println();
  Serial.println("=====================================");
  Serial.println("       MAPEAMENTO BANCADA MPS");
  Serial.println("=====================================");

  Serial.println("Entradas configuradas.");

  Serial.println("Saidas configuradas em LOW.");

  Serial.println("-------------------------------------");

  Serial.print("Rele selecionado: ");
  Serial.println(identificarRele(teste));

  Serial.print("GPIO selecionado: ");
  Serial.println(teste);

  Serial.println("Estado inicial: LOW");

  Serial.println("Iniciando teste HIGH/LOW...");
  Serial.println("-------------------------------------");
}

// =====================================
// MONITORAMENTO DOS SENSORES
// =====================================

void monitorarSensores() {

  for (int i = 0; i < quantidadeSensores; i++) {

    int estadoAtual = digitalRead(sensores[i]);

    if (estadoAtual != estadosSensores[i]) {

      Serial.print(nomesSensores[i]);
      Serial.print(" | GPIO: ");
      Serial.print(sensores[i]);

      Serial.print(" | Estado: ");

      if (estadoAtual == HIGH) {
        Serial.println("HIGH - ATIVADO");
      } else {
        Serial.println("LOW - DESATIVADO");
      }

      estadosSensores[i] = estadoAtual;
    }
  }
}

// =====================================
// TESTE DA SAIDA SELECIONADA
// =====================================

void testarSaida() {

  if (millis() - tempoAnterior >= INTERVALO_TESTE) {

    tempoAnterior = millis();

    // Alternar estado
    estadoRele = (estadoRele == LOW) ? HIGH : LOW;

    digitalWrite(teste, estadoRele);

    Serial.println("-------------------------------------");

    Serial.print("RELE: ");
    Serial.println(identificarRele(teste));

    Serial.print("GPIO: ");
    Serial.println(teste);

    Serial.print("ESTADO: ");

    if (estadoRele == HIGH) {
      Serial.println("HIGH - LIGADO");
    } else {
      Serial.println("LOW - DESLIGADO");
    }
  }
}

// =====================================
// LOOP PRINCIPAL
// =====================================

void loop() {

  monitorarSensores();

  testarSaida();

}
