#include <Arduino.h>
// Definição dos pinos utilizados
#define sensorDireita 8
#define sensorCentroDireita 7
#define sensorCentro 3
#define sensorCentroEsquerda 5
#define sensorEsquerda 4
#define motorEsquerdoRe 10
#define motorEsquerdoFrente 11
#define motorDireitoRe 9
#define motorDireitoFrente 6

// Variáveis globais
int contador = 0;
const int MIN_DELAY = 150;
const int ONE_SECOND = 1000;
const int TWO_SECONDS = 2000;
const int THREE_SECONDS = 3000;
const int FOUR_SECONDS = 4000;
const int FIVE_SECONDS = 5000;
const int VEL_ALTA = 225;
const int VEL_REDUZIDA = 175;
const int VEL_ZERO = 0;
boolean detectouViradaDeRotatoria = false;
boolean passouPelaRotatoria = false;

void setup()
{
  // Inicialização da comunicação serial e configuração dos pinos
  Serial.begin(9600);
  pinMode(sensorDireita, INPUT);
  pinMode(sensorCentroDireita, INPUT);
  pinMode(sensorCentro, INPUT);
  pinMode(sensorCentroEsquerda, INPUT);
  pinMode(sensorEsquerda, INPUT);
  pinMode(motorEsquerdoRe, OUTPUT);
  pinMode(motorEsquerdoFrente, OUTPUT);
  pinMode(motorDireitoRe, OUTPUT);
  pinMode(motorDireitoFrente, OUTPUT);
}

void loop()
{
  seguirLinha(VEL_ALTA);
  detectarVirada();
  detectarFaixaDePedestre();
  detectarFimDaPista();
}

/*
Recebe as velocidades dos motores direito e esquerdo e controla os
motores para mover o carrinho para frente.
Ela utiliza a função analogWrite() para enviar sinais PWM aos pinos
correspondentes aos motores.
*/
void controlarMotoresFrente(int velMotorDireito, int velMotorEsquerdo)
{
  analogWrite(motorDireitoFrente, velMotorDireito);
  analogWrite(motorEsquerdoFrente, velMotorEsquerdo);
}

/*
Funciona de maneira similar à função controlarMotoresFrente(), mas faz o
carrinho se mover para trás.
*/
void controlarMotoresRe(int velMotorDireito, int velMotorEsquerdo)
{
  analogWrite(motorDireitoRe, velMotorDireito);
  analogWrite(motorEsquerdoRe, velMotorEsquerdo);
}

/*
Para os motores, definindo a velocidade de ambos os motores como zero.
*/
void pararMotores()
{
  controlarMotoresFrente(VEL_ZERO, VEL_ZERO);
  controlarMotoresRe(VEL_ZERO, VEL_ZERO);
}

/*
Recebe a velocidade como parâmetro e utiliza os sensores para detectar a
posição da linha.
Dependendo da leitura dos sensores, o carrinho ajusta a velocidade dos
motores para manter-se alinhado com a linha.
*/
void seguirLinha(int velocidade)
{
  while (carrinhoEstaParado())
  {
    seguirReto(velocidade);
  }
  if (digitalRead(sensorCentroDireita) == LOW)
  {
    controlarMotoresFrente(VEL_ZERO, velocidade);
  }
  if (digitalRead(sensorCentroEsquerda) == LOW)
  {
    controlarMotoresFrente(velocidade, VEL_ZERO);
  }
}

/*
Esse método é semelhante ao método seguirLinha(), mas é usado quando o
carrinho está em cima de uma linha preta.
Ele ajusta a velocidade dos motores para manter o carrinho em cima da
linha preta.
*/
void seguirLinhaPreta(int velocidade)
{
  while (carrinhoEstaParadoEmLinhaPreta())
  {
    seguirReto(velocidade);
  }
  if (digitalRead(sensorCentroDireita) == HIGH)
  {
    controlarMotoresFrente(VEL_ZERO, velocidade);
  }
  if (digitalRead(sensorCentroEsquerda) == HIGH)
  {
    controlarMotoresFrente(velocidade, VEL_ZERO);
  }
}

/*
Faz o carrinho seguir reto com a velocidade especificada, controlando
ambos os motores com a mesma velocidade.
*/
void seguirReto(int velocidade)
{
  controlarMotoresFrente(velocidade, velocidade);
}

/*
Essa função é chamada quando uma curva para a esquerda é detectada.
Ela controla o carrinho para realizar a curva, ajustando a velocidade
dos motores e a direção do movimento.
*/
void realizarViraradaParaEsquerda()
{
  while (temSensorExtremoAtivado())
  {
    seguirReto(VEL_ALTA);
    delay(MIN_DELAY);
  }
  pararMotores();
  delay(ONE_SECOND);
  while (sensorCentroEstaAtivado())
  {
    controlarMotoresFrente(VEL_ALTA, VEL_ZERO);
    controlarMotoresRe(VEL_ZERO, VEL_ALTA);
  }
  while (sensorCentroEstaDesativado())
  {
    controlarMotoresFrente(VEL_ALTA, VEL_ZERO);
    controlarMotoresRe(VEL_ZERO, VEL_ALTA);
  }
  pararMotores();
  delay(ONE_SECOND);
  resetarValores();
  Serial.println("Realizou virada a esquerda");
}

/*
Funciona de maneira similar à função realizarViraradaParaEsquerda(), mas
é chamada quando uma curva para a direita é detectada.
*/
void realizarViradaParaDireita()
{
  while (temSensorExtremoAtivado())
  {
    seguirReto(VEL_ALTA);
    delay(MIN_DELAY);
  }
  pararMotores();
  delay(ONE_SECOND);
  while (sensorCentroEstaAtivado())
  {
    controlarMotoresFrente(VEL_ZERO, VEL_ALTA);
    // controlarMotoresRe(VEL_ALTA, VEL_ZERO);
  }
  while (sensorCentroEstaDesativado())
  {
    controlarMotoresFrente(VEL_ZERO, VEL_ALTA);
    // controlarMotoresRe(VEL_ALTA, VEL_ZERO);
  }
  pararMotores();
  delay(ONE_SECOND);
  Serial.println("realizou virada a direita");
}

/*
Essa função é chamada dentro do loop principal e é responsável por
detectar curvas no caminho.
Ela chama funções auxiliares, como ehQuadradoADireita() e
ehQuadradoAEsquerda(), para identificar o tipo de curva detectada.
*/
void detectarVirada()
{
  if (ehQuadradoADireita())
  {
    Serial.println("detectou virada a direita");
    identificarViradaADireita();
    Serial.println("encontrou a virada a direita");
    consertarContador();
    pararMotores();
    delay(ONE_SECOND);
    identificarTipoDeViradaADireita();
  }
  if (ehQuadradoAEsquerda())
  {
    Serial.println("detectou virada a esquerda");
    identificarViradaAEsquerda();
    Serial.println("encontrou virada a esquerda");
    consertarContador();
    pararMotores();
    delay(ONE_SECOND);
    realizarViraradaParaEsquerda();
  }
}

/*
Responsável por identificar uma virada à esquerda do carrinho, com base
na leitura dos sensores.
*/
void identificarViradaAEsquerda()
{
  while (!todosSensoresEstaoAtivados() ||
         !todosSensoresEstaoDesativados())
  {
    seguirLinha(VEL_REDUZIDA);
    if (sensorDireitoEstaAtivado())
    {
      Serial.println("encontrou direito 1");
      break;
    }
    if (sensorEsquerdoEstaAtivado())
    {
      Serial.println("entrou no quadrado a esquerda");
      contador++;
      while (sensorEsquerdoEstaAtivado())
      {
        seguirLinha(VEL_REDUZIDA);
        if (sensorDireitoEstaAtivado())
        {
          Serial.println("encontrou direito 2");
          break;
        }
      }
      if (sensorDireitoEstaAtivado())
      {
        Serial.println("encontrou direito 3");
        break;
      }
      contador++;
      Serial.println("saiu do quadrado a esquerda");
    }
  }
}

/*
Responsável por identificar uma virada à direita do carrinho, com base
na leitura dos sensores.
*/
void identificarViradaADireita()
{
  while (!todosSensoresEstaoAtivados() ||
         !todosSensoresEstaoDesativados())
  {
    seguirLinha(VEL_REDUZIDA);
    if (sensorEsquerdoEstaAtivado())
    {
      Serial.println("Encontrou esquerdo 1");
      break;
    }
    if (sensorDireitoEstaAtivado())
    {
      Serial.println("Entrou no quadrado a direita");
      contador++;
      while (sensorDireitoEstaAtivado())
      {
        seguirLinha(VEL_REDUZIDA);
        if (sensorEsquerdoEstaAtivado())
        {
          Serial.println("Encontrou esquerdo 2");
          break;
        }
      }
      if (sensorEsquerdoEstaAtivado())
      {
        Serial.println("Encontrou esquerdo 3");
        break;
      }
      contador++;
      Serial.println("Saiu do quadrado a direita");
    }
  }
}

/*
Responsável por identificar o tipo de virada para a direita detectada
pelo carrinho. Ele chama outros métodos auxiliares,
como ehViradaSimples() e encontrarViradaDeRotatoria(), para realizar
essa identificação.
*/
void identificarTipoDeViradaADireita()
{
  if (ehViradaSimples())
  {
    Serial.println("eh virada simples para a direita");
    realizarViradaParaDireita();
    resetarValores();
  }
  else
  {
    Serial.println("nao eh virada simples - rotatoria");
    realizarViradaParaDireita();
    encontrarViradaDeRotatoria();
    resetarValores();
  }
}

/*
Responsável por corrigir o valor do contador caso ele seja ímpar.
*/
void consertarContador()
{
  Serial.println("contador impar, precisou ser arrumado");
  Serial.print("contador = ");
  if (ehImpar(contador))
  {
    contador--;
  }
  Serial.println(contador);
}

/*
Responsável por detectar a virada de rotatória.
O método segue uma lógica de detecção e controle dos motores para
realizar a virada correta.
*/
void encontrarViradaDeRotatoria()
{
  while (naoEhHoraDeVirar())
  {
    seguirLinha(VEL_ALTA);
    while (sensorDireitoEstaAtivado())
    {
      seguirLinha(VEL_ALTA);
      detectouViradaDeRotatoria = true;
    }
    if (detectouViradaDeRotatoria)
    {
      Serial.println("detectou virada de rotatoria");
      contador = contador - 2;
      Serial.print("contador = ");
      Serial.println(contador);
    }
    detectouViradaDeRotatoria = false;
  }
  realizarViradaParaDireita();
}

/*
Responsável por redefinir as variáveis globais utilizadas no programa
para os valores iniciais.
*/
void resetarValores()
{
  contador = 0;
  detectouViradaDeRotatoria = false;
}

/*
Responsável por detectar a presença de uma faixa de pedestres à frente
do carrinho.
Ele verifica os sensores e, caso detecte a presença da faixa, executa
uma sequência de ações para atravessá-la com segurança.
Isso inclui seguir uma linha preta até a faixa, aguardar a passagem do
pedestre, atravessar a faixa e indicar que o carrinho passou pela
rotatória.
*/
void detectarFaixaDePedestre()
{
  if (temFaixaDePedestreAFrente())
  {
    Serial.println("detectou faixa de pedestre a frente");
    while (temSensorExtremoAtivado())
    {
      seguirLinhaPreta(VEL_ALTA);
    }
    Serial.println("terminou de seguir linha preta");
    Serial.println("esperando pedestre passar");
    pararMotores();
    delay(FIVE_SECONDS);
    if (sensorCentroEstaDesativado())
    {
      Serial.println("procurando linha branca após pista branca");
      while (sensorCentroDireitaEstaDesativado())
      {
        seguirReto(VEL_ALTA);
      }
    }
    Serial.println("passando pela faixa de pedestre");
    seguirReto(VEL_REDUZIDA);
    delay(FIVE_SECONDS);
    Serial.println("parar na linha branca do outro lado da faixa");
    pararMotores();
    delay(ONE_SECOND);
    passouPelaRotatoria = true;
    Serial.println("passou pela rotatoria");
  }
}

/*
Responsável por detectar o fim da pista em que o carrinho está
percorrendo.
Se for detectado que o carrinho chegou ao fim da pista, o carrinho será
estacionado e o programa será encerrado.
*/
void detectarFimDaPista()
{
  if (chegouNoFinalDaPista())
  {
    Serial.println("estacionando carrinho");
    seguirReto(VEL_REDUZIDA);
    delay(TWO_SECONDS);
    resetarValores();
    while (true)
    {
      pararMotores();
      Serial.println("circuito encerrado");
    }
  }
}

boolean ehImpar(int numero)
{
  return numero % 2 != 0;
}

boolean ehQuadradoADireita()
{
  return digitalRead(sensorDireita) == LOW && digitalRead(sensorEsquerda) == HIGH;
}

boolean ehQuadradoAEsquerda()
{
  return digitalRead(sensorEsquerda) == LOW && digitalRead(sensorDireita) == HIGH;
}

boolean ehViradaSimples()
{
  return contador <= 2;
}

boolean naoEhHoraDeVirar()
{
  return contador != 2;
}

boolean carrinhoEstaParado()
{
  return digitalRead(sensorCentroDireita) == HIGH && digitalRead(sensorCentroEsquerda) == HIGH;
}

boolean carrinhoEstaParadoEmLinhaPreta()
{
  return digitalRead(sensorCentroDireita) == LOW && digitalRead(sensorCentroEsquerda) == LOW;
}

boolean sensorEsquerdoEstaDesativado()
{
  return digitalRead(sensorEsquerda) == HIGH;
}

boolean sensorEsquerdoEstaAtivado()
{
  return digitalRead(sensorEsquerda) == LOW;
}

boolean sensorCentroEsquerdaEstaDesativado()
{
  return digitalRead(sensorCentroEsquerda) == HIGH;
}

boolean sensorCentroEsquerdaEstaAtivado()
{
  return digitalRead(sensorCentroEsquerda) == LOW;
}

boolean sensorCentroEstaDesativado()
{
  return digitalRead(sensorCentro) == HIGH;
}

boolean sensorCentroEstaAtivado()
{
  return digitalRead(sensorCentro) == LOW;
}

boolean sensorCentroDireitaEstaDesativado()
{
  return digitalRead(sensorCentroDireita) == HIGH;
}

boolean sensorCentroDireitaEstaAtivado()
{
  return digitalRead(sensorCentroDireita) == LOW;
}

boolean sensorDireitoEstaDesativado()
{
  return digitalRead(sensorDireita) == HIGH;
}

boolean sensorDireitoEstaAtivado()
{
  return digitalRead(sensorDireita) == LOW;
}

boolean temFaixaDePedestreAFrente()
{
  return digitalRead(sensorDireita) == LOW &&
         digitalRead(sensorCentro) == HIGH &&
         digitalRead(sensorEsquerda) == LOW;
}

boolean chegouNoFinalDaPista()
{
  return passouPelaRotatoria && todosSensoresEstaoDesativados();
}

boolean temSensorExtremoAtivado()
{
  return digitalRead(sensorDireita) == LOW || digitalRead(sensorEsquerda) == LOW;
}

boolean todosSensoresEstaoAtivados()
{
  return digitalRead(sensorDireita) == LOW &&
         digitalRead(sensorCentroDireita) == LOW &&
         digitalRead(sensorCentro) == LOW &&
         digitalRead(sensorCentroEsquerda) == LOW &&
         digitalRead(sensorEsquerda) == LOW;
}

boolean todosSensoresEstaoDesativados()
{
  return digitalRead(sensorDireita) == HIGH &&
         digitalRead(sensorCentroDireita) == HIGH &&
         digitalRead(sensorCentro) == HIGH &&
         digitalRead(sensorCentroEsquerda) == HIGH &&
         digitalRead(sensorEsquerda) == HIGH;
}
