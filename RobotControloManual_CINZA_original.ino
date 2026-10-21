#include <Servo.h>

// Criar objetos servo
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// Pinos dos servos
int servoPin1 = 3;
int servoPin2 = 6;
int servoPin3 = 10;
int servoPin4 = 11;

// Pinos dos potenciômetros
//int potPin1 = A0;
int potPin2 = A2;
int potPin3 = A0;
int potPin4 = A1;

// Variáveis para guardar valores
int potVal1 = 0;
int potVal2 = 0;
int potVal3 = 0;
int potVal4 = 0;

// Variáveis para guardar valores
int angle1 = 70;    // sem significado
int angle2 = 80;    // sem significado
int angle3 = 105;   // posição inicial segura
int angle4 = 170;   // posição inicial segura

// Garra
bool EstadoGarra = LOW;       // Guarda se o LED está ON ou OFF
bool botaoPressionado = false; 

// Passo incremental
int passo = 2;    //originalmente 5 passou para 3

// Limites
int min3 = 10;
int max3 = 160;
int min4 = 80;
int max4 = 180;

void setup() {

  pinMode(7, INPUT); // Sets pin 7 as input

  // Ligar os servos aos pinos
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);  

  Serial.begin(9600);
}

void loop() {

  // =========================
  // CONTROLO GARRA (toggle)
  // =========================

  if (digitalRead(7) == LOW && !botaoPressionado) {
    EstadoGarra = !EstadoGarra;          // Inverte o estado (se era LOW vira HIGH)
    if (EstadoGarra == 1) {
      angle1 = 70;    // 70 para robot azul
    } else {
      angle1 = 10;
    }
    botaoPressionado = true;         // Marca que o botão ainda está sendo segurado
  }
  // Reseta a trava quando o dedo sai do botão
  if (digitalRead(7) == HIGH) {
    botaoPressionado = false;
  }

  // =========================
  // LEITURA DOS POTENCIÓMETROS
  // =========================

  potVal2 = analogRead(potPin2);
  potVal3 = analogRead(potPin3);
  potVal4 = analogRead(potPin4);

  // =========================
  // BASE (controlo direto)
  // =========================

  int angle2 = map(potVal2, 0, 1023,  0,  180);    // -120- base 0 a 60 (azul) 0 a 180 (cinza)

  // =========================
  // CONTROLO INCREMENTAL
  // =========================

  // ---- SERVO 3 ----
  if (potVal3 > 580) {    //800
    angle3 += passo;
  }
  else if (potVal3 < 450) {    //400
    angle3 -= passo;
  }

  // ---- SERVO 4 ----
  if (potVal4 > 580) {    //800
    angle4 += passo;
  }
  else if (potVal4 < 450) {    //400
    angle4 -= passo;
  }

  // =========================
  // LIMITES DE SEGURANÇA
  // =========================

  angle3 = constrain(angle3, min3, max3);
  angle4 = constrain(angle4, min4, max4);

  // =========================
  // ESCREVER SERVOS
  // =========================

  // Mover os servos
  servo1.write(angle1);
  servo2.write(angle2);
  servo3.write(angle3);
  servo4.write(angle4);

  // Mostrar valores no monitor serial (opcional)
  Serial.print("Servo1: ");
  Serial.print(angle1);
  Serial.print(" <- ");  
  Serial.print(potVal1);

  Serial.print(" | Servo2: ");
  Serial.print(angle2);
  Serial.print(" <- ");  
  Serial.print(potVal2);

  Serial.print(" | Servo3: ");
  Serial.print(angle3);
  Serial.print(" <- ");
  Serial.print(potVal3);

  Serial.print(" | Servo4: ");
  Serial.print(angle4);
  Serial.print(" <- ");
  Serial.print(potVal4);
  Serial.println("");

  delay(100);
}
