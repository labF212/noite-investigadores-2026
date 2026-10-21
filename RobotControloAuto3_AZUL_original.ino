#include <Servo.h>

// --------------------
// Braço robotico azul
// --------------------

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// Pinos dos servos
const int servoPin1 = 6;
const int servoPin2 = 3;
const int servoPin3 = 10;
const int servoPin4 = 11;


// =========================
// TRAJETORIA
// =========================

struct Ponto {
  int servo1;
  int servo2;
  int servo3;
  int servo4;
};

Ponto traj[] = {
  // G, teta, Yy, Xx
  {10,   3, 80, 30},
  {10, 112, 80, 33},
  {10, 112, 80, 20},
  {10, 112, 45, 20},
  {10, 112, 45, 32},
  {62, 112, 45, 32},
  {62, 112, 90, 32},
  {62,   3, 90, 25},
  {62,   3, 35, 25},
  {62,   3, 35, 42},
  {10,   3, 35, 42},
  {10,   3, 55, 42}  
};

const int totalPontos = sizeof(traj) / sizeof(traj[0]);


// =========================
// POSICAO ATUAL
// =========================

float current1;
float current2;
float current3;
float current4;


// =========================
// VELOCIDADE ATUAL
// graus por segundo
// =========================

float vel1 = 0;
float vel2 = 0;
float vel3 = 0;
float vel4 = 0;


// =========================
// PARAMETROS DO MOVIMENTO
// =========================

// Velocidade máxima em graus/segundo
float velocidadeMax = 25.0;

// Aceleração em graus/segundo^2
float aceleracao = 30.0;

// Intervalo entre atualizações dos servos
const unsigned long periodoMovimento = 20; // ms

unsigned long ultimoMovimento = 0;

// Tempo que permanece parado em cada ponto
const unsigned long tempoEspera = 1000; // 1 segundo

unsigned long inicioEspera = 0;

bool emEspera = false;

int indexTraj = 0;


// =========================
// NOVO
// CONTROLO DA EXECUCAO
// =========================

// false = braço parado
// true  = trajetória em execução

bool executando = false;


// =========================
// FUNCAO MOVIMENTO COM RAMPA
// =========================

void moverServo(
  float &posicao,
  float &velocidade,
  float destino,
  float dt
) {

  float distancia = destino - posicao;

  // Já chegámos
  if (abs(distancia) < 0.1) {
    posicao = destino;
    velocidade = 0;
    return;
  }

  // Direção do movimento

  float direcao;

  if (distancia > 0)
    direcao = 1.0;
  else
    direcao = -1.0;


  // ==================================
  // Distância necessária para travar
  //
  // v^2 = 2*a*d
  // ==================================

  float distanciaTravagem =
      (velocidade * velocidade) /
      (2.0 * aceleracao);


  // ==================================
  // DECIDIR SE ACELERA OU DESACELERA
  // ==================================

  if (abs(distancia) <= distanciaTravagem) {

    // DESACELERAR

    if (velocidade > 0)
      velocidade -= aceleracao * dt;

    else if (velocidade < 0)
      velocidade += aceleracao * dt;

  }
  else {

    // ACELERAR

    velocidade += direcao * aceleracao * dt;
  }


  // =========================
  // LIMITAR VELOCIDADE
  // =========================

  velocidade = constrain(
      velocidade,
      -velocidadeMax,
       velocidadeMax
  );


  // =========================
  // CALCULAR NOVA POSICAO
  // =========================

  float novaPosicao =
      posicao + velocidade * dt;


  // Evitar ultrapassar o destino

  if ((direcao > 0 && novaPosicao >= destino) ||
      (direcao < 0 && novaPosicao <= destino)) {

    novaPosicao = destino;
    velocidade = 0;
  }

  posicao = novaPosicao;
}


// =========================
// VERIFICAR SE CHEGARAM
// =========================

bool chegouDestino() {

  const float tolerancia = 0.2;

  return
    abs(current1 - traj[indexTraj].servo1) < tolerancia &&
    abs(current2 - traj[indexTraj].servo2) < tolerancia &&
    abs(current3 - traj[indexTraj].servo3) < tolerancia &&
    abs(current4 - traj[indexTraj].servo4) < tolerancia;
}


// =========================
// SETUP
// =========================

void setup() {

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);


  // Posição inicial

  current1 = traj[0].servo1;
  current2 = traj[0].servo2;
  current3 = traj[0].servo3;
  current4 = traj[0].servo4;

  servo1.write(current1);
  servo2.write(current2);
  servo3.write(current3);
  servo4.write(current4);


  Serial.begin(9600);

  ultimoMovimento = millis();


  // =========================
  // NOVO
  // =========================

  Serial.println();
  Serial.println("Braco robotico pronto.");
  Serial.println("Envie 'a' para iniciar a trajetoria.");
}


// =========================
// LOOP
// =========================

void loop() {

  unsigned long agora = millis();


  // =====================================
  // NOVO
  // VERIFICAR TECLADO / PORTA SERIE
  // =====================================

  if (Serial.available() > 0) {

    char tecla = Serial.read();

    if (tecla == 'a' || tecla == 'A') {

      // Só iniciar se não estiver já em execução

      if (!executando) {

        Serial.println();
        Serial.println("A iniciar trajetoria...");

        indexTraj = 0;

        vel1 = 0;
        vel2 = 0;
        vel3 = 0;
        vel4 = 0;

        emEspera = false;

        executando = true;

        ultimoMovimento = agora;
      }
    }
  }


  // =====================================
  // SE NAO ESTIVER A EXECUTAR, NAO MOVE
  // =====================================

  if (!executando) {
    return;
  }


  // =========================
  // MOVIMENTO
  // =========================

  if (!emEspera &&
      agora - ultimoMovimento >= periodoMovimento) {

    float dt =
      (agora - ultimoMovimento) / 1000.0;

    ultimoMovimento = agora;


    // Executar rampas

    moverServo(
      current1,
      vel1,
      traj[indexTraj].servo1,
      dt
    );

    moverServo(
      current2,
      vel2,
      traj[indexTraj].servo2,
      dt
    );

    moverServo(
      current3,
      vel3,
      traj[indexTraj].servo3,
      dt
    );

    moverServo(
      current4,
      vel4,
      traj[indexTraj].servo4,
      dt
    );


    // Enviar posições aos servos

    servo1.write(current1);
    servo2.write(current2);
    servo3.write(current3);
    servo4.write(current4);


    // =========================
    // VERIFICAR DESTINO
    // =========================

    if (chegouDestino()) {

      vel1 = 0;
      vel2 = 0;
      vel3 = 0;
      vel4 = 0;


      // ====================================
      // NOVO
      // VERIFICAR SE E O ULTIMO PONTO
      // ====================================

      if (indexTraj == totalPontos - 1) {

        executando = false;

        Serial.println();
        Serial.println("Trajetoria terminada.");
        Serial.println("Envie 'a' para executar novamente.");

        return;
      }


      // Se não for o último ponto,
      // esperar normalmente

      inicioEspera = agora;

      emEspera = true;
    }
  }


  // =========================
  // TEMPO DE ESPERA
  // =========================

  if (emEspera &&
      agora - inicioEspera >= tempoEspera) {

    indexTraj++;

    emEspera = false;

    ultimoMovimento = agora;

    Serial.print("Novo ponto: ");
    Serial.println(indexTraj);
  }
}


